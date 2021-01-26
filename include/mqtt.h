#ifndef _mqtt_h_
#define _mqtt_h_

#include <Arduino.h>
//#include <WiFiClientSecure.h>                       //Librería para TLS
#include <PubSubClient.h>
#include <LittleFS.h>
#include "wifi.h"
#include "comandos.h"

/*======================================[macros y definiciones]============================================*/
#define WILL_RETAIN_MESSAGE                 false
#define WILL_QOS                            2
//#define MAX_MQTT_CONNECTION_TIMEOUT         50          //timeout en milisegundos*200 para la conexion mqtt
#define MAX_MQTT_CONNECTION_RETRYS          20          //cantidad de reintentos de conexion mqtt
#define MAX_MQTT_LIVE_TIMEOUT               300000      //timeout en milisegundos de desaparicion de mqtt 
#define SHORT_DELAY_MQTT_LED                50


class MCC_mqtt{
    private:
        //int MAX_mqtt_conn_timeout=50;               //timeout en milisegundos*200 para la conexion mqtt
        //unsigned long MAX_live_timeout_mqtt=300000; //timeout en milisegundos de desaparicion de mqtt
        int mqtt_conn_timeout=0;                    //timeout de la conexion a mqtt en segundos
        unsigned long live_timeout_mqtt;            //timeout de la conexion en funcionamiento de mqtt en minutos
        const char * _server;
        uint16_t _port_server;
        const char * _device;
        const char * _topic_status;
        const char * _will_mess;
        bool _flag_mqtt;
        bool _conn_mqtt_status;
        const char * _topic_sub;
        int _led;
        uint8_t _mqtt_conn_retrys;
        const char* _ca_cert;

    public:
        bool conn(void);
        void init(const char * server, uint16_t port_server, const char * device, const char * topic_status, const char * will_mess, const char * topic_sub, int led);
        bool pub(String topic, String message);
        void on_msg(char* topic, byte* payload, unsigned int length);
        bool sub(String topic);
        bool loop(void);
        bool control(void);
        void setOnMess(std::function<void (char *, uint8_t *, unsigned int)> callb);
        void shortBlinkMqttLed();
};
#endif