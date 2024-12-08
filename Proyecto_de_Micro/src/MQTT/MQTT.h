#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
//#include "my_data.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "driver/gpio.h"
#include<GPIO/GPIO.h>

typedef struct FLAGS_STRUCT
{
    unsigned int MQTT_READY:1;
    unsigned int Data_Ready:1;
}typedefFLAGS;
void mqtt_app_start(void);
esp_mqtt_client_handle_t  Get_client(void);
void Decoder (char *string);
void clean(int len,char **buff);
void Init_ESP32(char *talk,char *listen);