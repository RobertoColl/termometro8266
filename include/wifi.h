#ifndef _wifi_h_
#define _wifi_h_

#include <Arduino.h>

//--Necesario para usar las librerías anteriores y que se conecte a mqtt/TLS!!!!
//-- Info de: /home/mcastello/.platformio/packages/framework-arduinoespressif8266/libraries/ESP8266WiFi/src/WiFiClientSecure.h
#define USING_AXTLS
#include <ESP8266WiFi.h>
//#include <WiFiClientSecure.h>
#include "WiFiClientSecureAxTLS.h"
using namespace axTLS;
//------------------------------------------------------------------------------


class MCC_wifi{
    private:    
        void blinkLed(int code);
        String _device;
        String _passwd_AP;
        String _ssid;
        String _ssid_pass;
        int _led;
        int MAX_wifi_conn_timeout=50;               //timeout en milisegundos*200 para la conexion wifi
        unsigned long MAX_live_timeout_wifi=300000; //timeout en milisegundos de desaparicion de wifi
        int wifi_conn_timeout=0;                    //timeout de la conexion a wifi en segundos
        unsigned long live_timeout_wifi;            //timeout de la conexion en funcionamiento de wifi en minutos
        
    public:
        void init(String device,String passwd_AP, String ssid, String ssid_pass, int led);
        uint8_t conn(void);
        int control(void);
        void setLed(void);
};

#endif