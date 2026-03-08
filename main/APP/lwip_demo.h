/**
 ****************************************************************************************************
 * @file        udp.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-26
 * @brief       LWIP实验（适配华为云IoTDA 1883端口）
 * @license     Copyright (c) 2020-2032,广州市星翼电子科技有限公司
 ****************************************************************************************************
 */

#ifndef __LWIP_DEMO_H
#define __LWIP_DEMO_H

#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "lwip/apps/mqtt.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "mqtt_client.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_netif.h"
#include "led.h"
#include "spilcd.h"

/* ==================== 华为云IoTDA核心参数（仅改端口，其他不变） ==================== */
#define HUAWEI_DEVICE_ID     "69abad11cbb0cf6bb944a507_Smartcar"
#define HUAWEI_PRODUCT_ID    "Smartcar"
#define HUAWEI_DEVICE_SECRET "76fbdbae37022b8252b8859a8c0a4ba1cfd7b66774c06cde9a220f1b8cc0f568"

// 关键修改：从 8883 改为 1883（非加密 MQTT 端口）
#define HOST_NAME           "0b07ca584e.st1.iotda-device.cn-north-4.myhuaweicloud.com"
#define HOST_PORT           1883               

#define CLIENT_ID           "69abad11cbb0cf6bb944a507_Smartcar_0_0_2026030704"
#define USER_NAME           "69abad11cbb0cf6bb944a507_Smartcar"
#define PASSWORD            HUAWEI_DEVICE_SECRET

#define DEVICE_PUBLISH      "$oc/devices/"HUAWEI_DEVICE_ID"/sys/properties/report"
#define DEVICE_SUBSCRIBE    "$oc/devices/"HUAWEI_DEVICE_ID"/sys/commands/#"

void lwip_demo(void);

#endif