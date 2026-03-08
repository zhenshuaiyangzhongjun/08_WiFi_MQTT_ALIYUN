#include "lwip_demo.h"

int g_publish_flag = 0;
static const char *TAG = "MQTT_EXAMPLE";
char g_lcd_buff[100] = {0};

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "✅ MQTT_EVENT_CONNECTED (已连接华为云IoTDA 1883)");
            g_publish_flag = 1;
            msg_id = esp_mqtt_client_subscribe(client, DEVICE_SUBSCRIBE, 0);
            ESP_LOGI(TAG, "订阅主题成功, msg_id=%d", msg_id);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "❌ MQTT_EVENT_DISCONNECTED (与华为云IoTDA断开连接)");
            g_publish_flag = 0;
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "📤 MQTT_EVENT_PUBLISHED, msg_id=%d (数据上报成功)", event->msg_id);
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "📥 收到云端数据：");
            printf("  TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("  DATA=%.*s\r\n", event->data_len, event->data);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "❌ MQTT_EVENT_ERROR (连接错误)");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
            {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;

        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

static void pack_huawei_json(char *buff, int len, int led_state)
{
    int timestamp = (int)(esp_timer_get_time() / 1000000);
    snprintf(buff, len, 
             "{\"services\":[{\"service_id\":\"Light\",\"properties\":{\"LED_State\":%d},\"event_time\":\"%d\"}]}",
             led_state, timestamp);
}

void lwip_demo(void)
{
    char mqtt_publish_data[256] = {0};
    int led_state = 0;

    /* 1883 非加密 MQTT 配置，彻底解决 0x8017 错误 */
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.hostname = HOST_NAME,
        .broker.address.port = 1883,
        .broker.address.transport = MQTT_TRANSPORT_OVER_TCP, // 关键：改为 TCP
        .credentials.client_id = CLIENT_ID,
        .credentials.username = (char*)USER_NAME,
        .credentials.authentication.password = PASSWORD,
        // 不需要任何证书验证，直接留空
        .session.keepalive = 60,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    ESP_LOGI(TAG, "华为云MQTT客户端启动成功，等待连接...");

    while(1)
    {
        if (g_publish_flag == 1)
        {
            led_state = !led_state;
            pack_huawei_json(mqtt_publish_data, sizeof(mqtt_publish_data), led_state);
            esp_mqtt_client_publish(client, DEVICE_PUBLISH, mqtt_publish_data, strlen(mqtt_publish_data), 1, 0);
            ESP_LOGI(TAG, "上报数据：%s", mqtt_publish_data);
        }
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}