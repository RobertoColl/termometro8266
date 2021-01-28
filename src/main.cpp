/*******************************************************************************
 * 
 *        T E M P L A T E   S I S T E M A   D E   M O N I T O R E O            *
 * 
 * *****************************************************************************/

/*==================[ file header ]==========================================*/
// File:    main.cpp
// Author:  Marcelo Castello (https://github.com/mcastellogh)
// Licence: GPLV3+
// Version: 2.0.0
// Date:    Diciembre 2020
// Info:    Template para sistema de monitoreo sobre hardware Termometro_8266 V1.0
//            
//          
// TODO: 
//
// Para subir FileSystem: platformio run --target uploadfs

/*==================[inclusiones]============================================*/
//#include <TimerOne.h>
#include "wifi.h"
#include "interrupciones.h"
#include "global_vars.h"
#include "mqtt.h"
#include "funciones.h"
#include "comandos.h"
#include "eeprom.h"
#include "frontend.h"
#include "backend.h"
#include "medicion.h"

/*===================[Definiciones de hardware]]=================================*/
#define CANAL1                   12    // pin salida canal 1
#define CANAL2                   13    // pin salida canal 2
#define LED_PULSO                14    // led de pulso
#define LED_WIFI                 2
#define LED_RANGO                16

/*===================[Definiciones de software]===============================*/
#define SERIAL_BAUDRATE          19200
#define PORT_WEB_SERVER          80
#define MAX_BYTE_EEPROM          512
#define SLOT_EEPROM_VARS         30    //no implementado
           
/*==================[Posicion de variables EEPROM]============================*/
uint32_t device_eeprom_pos=0;
uint32_t mqtt_server_eeprom_pos=30;
uint32_t mqtt_tcp_str_eeprom_pos=60;
uint32_t passwd_AP_eeprom_pos=90;
uint32_t ssid_eeprom_pos=120;
uint32_t ssid_passwd_eeprom_pos=150;
uint32_t ubicacion_eeprom_pos=180;
uint32_t fuota_eeprom_pos=210;
uint32_t area_eeprom_pos=240;

/*====================[Variables de Factory reset]============================*/
String device                =   "terinfssp01";
String mqtt_server           =   "192.168.1.45";
String fuota_server          =   "192.168.1.17";
String mqtt_tcp_str          =   "8883";
String ssid                  =   "MAC";
String ssid_pass             =   "wpaadministrator08";
String passwd_AP             =   "Lancast3r";
String area                  =   "Informatica";
String ubicacion             =   "Sala Servidores";


/*=================[Variables harcodeadas]==================================*/
String www_username           =   "admin";
String hardware               =   "ter8266V1";
String tipo_device            =   "Termómetro";
const char * will_mess        =   "down";
uint8_t tls                   =   1;
const char * topic_telemetry  =   "v1/devices/me/telemtry";
const char * topic_attributes =   "v1/devices/me/attributes";
const char * topic_rpc        =   "v1/devices/me/rpc/request/+";


//--Variables globales
String topic_dev_status;
uint16_t mqtt_tcp;
uint8_t MAX_mqtt_conn_timeout=10; //timeout en segundos para la conexion al broker
uint8_t MAX_live_timeout_mqtt=5;  //timeout en minutos de desaparicion de broker
uint8_t mqtt_conn_timeout=0;      //timeout de la conexion al broker en segundos
uint8_t wifi_conn_timeout=0;      //timeout de la conexion al broker en segundos
//unsigned long live_timeout_mqtt=0;      //timeout de la conexion en funcionamiento de broker en minutos
uint8_t flag_seg=0;
uint8_t flag_mqtt=0;
uint8_t flag_wifi=0;
uint8_t flag_mqtt_conn=0;
uint8_t flag_wifi_conn=0;
uint8_t flag_publica=0;

//-- Versiones
String fversion="2.0.0";          
String hversion="1.0.0";      //en PCB Termometro 1.0

//--Instancias
MCC_wifi wifi;
MCC_mqtt broker;
ESP8266WebServer web_server (PORT_WEB_SERVER);



void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  delay(100);
  //--Configuración interrupción por timer
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer0_ISR);
  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
  interrupts();
  delay(100);

  Serial.println("\r\nTermómetro 8266");
  Serial.print("Version:");
  Serial.print(fversion);
  EEPROM.begin(MAX_BYTE_EEPROM);
  LittleFS.begin();

  //--Factory reset
  //pinMode(LED_PULSO,INPUT);
  /*if (!digitalRead(LED_PULSO)){
    Serial.println("\r\nFactory reset!!");
    factory_reset();
  }*/
  //factory_reset();
  read_vars(1);  
  

  //--Pines
  pinMode(LED_WIFI,OUTPUT);
  pinMode(LED_PULSO,OUTPUT);
  pinMode(LED_RANGO,OUTPUT);
  pinMode(CANAL1, OUTPUT);
  pinMode(CANAL2, OUTPUT);
  //--Pone en 0 los pines de los canales
  digitalWrite(CANAL1, LOW);
  digitalWrite(CANAL2, LOW);
  

//int i;while(1){delay (1000);Serial.println(i++);}
  bienvenida(LED_WIFI, LED_PULSO, LED_RANGO);

  //--Tópicos
  topic_dev_status=device+"/status/me";

  //--Inicializacion WIFi
  //Serial.println(ssid);
  //Serial.println(ssid_pass);
  wifi.init(device.c_str(), passwd_AP.c_str(), ssid.c_str(), ssid_pass.c_str(),LED_WIFI);//"hola","wpaadministrator08","MAC","wpaadministrator08",2);
  //flag_wifi_conn=wifi.conn();   //Si lo dejo bool se resetea (!!??)
  wifi.conn();
  //check_update();
  
  
  //--Inicializacion mqtt
  broker.init(mqtt_server.c_str(), mqtt_tcp, device.c_str(), topic_dev_status.c_str(), will_mess, topic_rpc, LED_WIFI);
  broker.setOnMess(rpc_proc);
  //while (!broker.conn()){
     //delay(3000);
     broker.conn();
  //}
  
  /*if (flag_wifi_conn){
    flag_mqtt_conn=broker.conn();
    //broker.sub(topic_rpc);  //usar si se suscribe a otros tópicos (ojo, si se desconecta deberá volver a suscribirse!!)
  }else{
    Serial.println("No se puede conectar al broker.");
    flag_mqtt_conn=0;
  }*/

  
  //--Webserver y FS
  init_webserver();


}

void loop() {
  //Serial.println(flag_mqtt_conn);
  //if(flag_mqtt_conn){
    wifi.control();
    broker.control();
    wifi.setLed();
    flag_seg == 0 ? digitalWrite(LED_PULSO,HIGH):digitalWrite(LED_PULSO,LOW);//digitalWrite(LED, !digitalRead(LED));
    broker.loop();
  //}

  web_server.handleClient();
  delay(100);
}
