#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <lwip/api.h>
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs.h"
#include "driver/gpio.h"
#include"GPIO/GPIO.h"
#include"WIFI/wifi1.h"
#include"MQTT/MQTT.h"
#include "driver/timer.h"

#define State_0 0
#define State_1 1
#define State_2 2
#define State_3 3
#define State_4 4

int States[]={State_0,State_1,State_2,State_3,State_4};
int Frecuency[]={0,500,100,1000,2000};
int Actual_state=State_0;

QueueHandle_t queue_task1;
QueueHandle_t queue_task2;
QueueHandle_t MQTT_Queue;


TimerHandle_t xTimers;

int interval=1000;
int timerId=1;

char SSID[32]="ClaroB550";
char PASSWORD[32]="santodomingo0901!";
char NombreEquipo[32]="\0";

extern typedefFLAGS Flag;
esp_mqtt_client_handle_t Client;


esp_err_t change_timer_interval(TickType_t new_interval) {
    interval = new_interval;  // Actualizar el intervalo global

    // Cambiar el periodo del timer en tiempo real
    if (xTimerChangePeriod(xTimers, interval, 0) != pdPASS) {
        // No se pudo cambiar el intervalo
        return ESP_FAIL;
    }

    
    return ESP_OK;
}
void vTimerCallback(TimerHandle_t pxTimer)
{
static int level=0;
if(Actual_state!=State_0)
{
level=!level;
gpio_set_level(LED1,level);
}
else
{
gpio_set_level(LED1,0);
}
}
esp_err_t set_timer(void)
{
    xTimers = xTimerCreate("Timer",       
                           (pdMS_TO_TICKS(interval)),      
                           pdTRUE,         
                           (void *)timerId,      
                           vTimerCallback  
    );

    if (xTimers == NULL)
    {
        
    }
    else
    {
        if (xTimerStart(xTimers, 0) != pdPASS)
        {
            
        }
    }

    return ESP_OK;
}


void main_task(void *arg) {
    
 char received_data[20];
 static int State=0;
    while (1) {
        if (xQueueReceive(MQTT_Queue, &received_data, portMAX_DELAY) == pdTRUE) {
            if(State<4)
            {
            State+=1;
            Actual_state=State;
            }
            else{
                State=0;
                Actual_state=0;
            }         
        }
        // Enviar datos a la primera cola
        if (xQueueSend(queue_task1, &State, portMAX_DELAY) != pdTRUE) {
            printf("Error enviando a queue_task1\n");
        }

        // Enviar datos a la segunda cola
        if (xQueueSend(queue_task2, &State, portMAX_DELAY) != pdTRUE) {
            printf("Error enviando a queue_task2\n");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));  // Retraso de 1 segundo
    }
}
void secondary_task1(void *arg) {
    int received_data;
    while (1) {
        if (xQueueReceive(queue_task1, &received_data, portMAX_DELAY) == pdTRUE) {
            if(States[received_data]==State_0)
            {    
            //change_timer_interval(pdMS_TO_TICKS(0));
            }
            else if(States[received_data]==State_1)
            {
            change_timer_interval(pdMS_TO_TICKS(Frecuency[received_data]));
            }
            else if(States[received_data]==State_2)
            {
            change_timer_interval(pdMS_TO_TICKS(Frecuency[received_data]));
            }
            else if(States[received_data]==State_3)
            {
            change_timer_interval(pdMS_TO_TICKS(Frecuency[received_data]));
            }
            else if(States[received_data]==State_4)
            {
            change_timer_interval(pdMS_TO_TICKS(Frecuency[received_data]));
            }
            
        }
    }
}
void secondary_task2(void *arg) {
    int received_data;

    while (1) {
        if (xQueueReceive(queue_task2, &received_data, portMAX_DELAY) == pdTRUE) 
        {
            printf("Estado actual %d; La frecuencia es:%d\n",States[received_data],Frecuency[received_data]);
        }
    }
}

void app_main(void) {
////////////////////
esp_err_t ret = nvs_flash_init();
if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
{
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
}
ESP_ERROR_CHECK(ret);
wifi_init_sta(SSID,PASSWORD);
while(!Flag.MQTT_READY);
Client=Get_client();
////////////////////

    GPIO_INIT();
    set_timer();
    // Crear las colas
    queue_task1 = xQueueCreate(10, sizeof(int));
    queue_task2 = xQueueCreate(10, sizeof(int));
    MQTT_Queue = xQueueCreate(10, sizeof(char) * 20);

    if (queue_task1 == NULL || queue_task2 == NULL) {
        printf("Error creando las colas\n");
        return;
    }
    xTaskCreate(main_task, "Main Task", 2048, NULL, 1, NULL);
    xTaskCreate(secondary_task1, "Secondary Task 1", 2048, NULL, 1, NULL);
    xTaskCreate(secondary_task2, "Secondary Task 2", 2048, NULL, 1, NULL);
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));  // Retraso de 1 segundo
        
    }

}
