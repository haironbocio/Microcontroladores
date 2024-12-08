#include "MQTT\MQTT.h"
#include "esp_mac.h"
typedefFLAGS Flag;
esp_mqtt_client_handle_t client;
static const char *TAG = "MQTT_TCP";
char listen_topic[32]="Prueba_Hairon";
char talk_topic[32];
extern QueueHandle_t MQTT_Queue;
extern char Data[100];
esp_mqtt_client_handle_t  Get_client(void)
{
return client; 
}
void clean(int len,char **buff)
{

    for (int i = 0; i <len; i++)
    {
       *buff[i]='\0';
    }
    
}
void Decoder (char *string)
{
    if(strstr(string,"encender cuarto"))
    {
        esp_mqtt_client_publish(client, talk_topic, "Cuarto encendido", 0, 1, 0);
        gpio_set_level(LED3,1);   
    }
    else if(strstr(string,"apagar cuarto"))
    {
        esp_mqtt_client_publish(client, talk_topic, "Cuarto Apagado", 0, 1, 0);
        gpio_set_level(LED3,0);   
    }
    else if(strstr(string,"encender cocina"))
    {
        esp_mqtt_client_publish(client, talk_topic, "Cocina encendida", 0, 1, 0);
        gpio_set_level(LED4,1);   
    }
    else if(strstr(string,"apagar cocina"))
    {
        esp_mqtt_client_publish(client, talk_topic, "Cocina Apagado", 0, 1, 0);
        gpio_set_level(LED4,0);   
    }
    else if(strstr(string,"on"))
    {
        gpio_set_level(LED3,1);
        gpio_set_level(LED4,1);   
    }
    else if(strstr(string,"of"))
    {
        gpio_set_level(LED3,0);
        gpio_set_level(LED4,0);   
    }
        else if(strstr(string,"apagar cocina"))
    {
        esp_mqtt_client_publish(client, talk_topic, "Cocina Apagado", 0, 1, 0);
        gpio_set_level(LED4,0);   
    }
    else if(strstr(string,"abrir"))
    {
        esp_mqtt_client_publish(client, talk_topic, "Puerta abierta", 0, 1, 0);
        gpio_set_level(Buzzer,1);   
        vTaskDelay(10000/portTICK_PERIOD_MS); 
        esp_mqtt_client_publish(client, talk_topic, "Puerta Cerrada", 0, 1, 0);
        gpio_set_level(Buzzer,0); 
    }
    
}

void Init_ESP32(char *talk,char *listen)
{
        uint8_t mac[6];
        esp_err_t ret = esp_efuse_mac_get_default(mac);
        char mac_str[18];  // 6 bytes * 2 caracteres por byte + 5 separadores de dos puntos + 1 carácter nulo
        snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    if (ret == ESP_OK)
    {
        // Imprimir la dirección MAC en formato hexadecimal
        printf("Dirección MAC predeterminada: %02x:%02x:%02x:%02x:%02x:%02x\n",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
        sprintf(talk,"%s%s",mac_str,"talk");
        printf("%s",talk);
        sprintf(listen,"%s%s",mac_str,"listen");
        
}
esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        Flag.MQTT_READY=1;
        gpio_set_level(LED1,1);
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        Init_ESP32(talk_topic,listen_topic);
        esp_mqtt_client_publish(client,talk_topic,"READY",strlen("READY"),2,0);
        esp_mqtt_client_subscribe(client,"Harion_prueba",2);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        gpio_set_level(LED2,0);
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        //printf("\nTOPIC=%.*s\r\n", event->topic_len, event->topic);
        //printf("DATA=%*s\r\n",event->data_len,event->data);
        if (xQueueSend(MQTT_Queue, event->data, portMAX_DELAY) != pdTRUE) {
            printf("Error enviando a queue_task1\n");
        }
        
        for (int i = 0; i <event->data_len; i++)  {event->data[i]='\0';}
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
    return ESP_OK;
}

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%ld", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
         .broker.address.uri = "mqtt://mqtt-dashboard.com",
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}