/*****************************************************************************
 * 
 *   T E R M O M E T R O    M O N I T O R    D E     T E M P E R A T U R A   *
 * 
 * ***************************************************************************/

/*==================[ file header ]==========================================*/
// File:    main.cpp
// Author:  Marcelo Castello (https://github.com/mcastellogh)
// Licence: GPLV3+
// Version: 2.0.0
// Date:    Diciembre 2020

// TODO: Subscripción a más de un tópico.
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

/*===================[macros y definiciones]=================================*/
#define CANAL1                   12    // pin salida canal 1
#define CANAL2                   13    // pin salida canal 2
#define LED_PULSO                14    // led de pulso
#define LED_WIFI                 2
#define LED_RANGO                16
#define NUMPIXELS                103
#define SERIAL_BAUDRATE          19200
#define PORT_WEB_SERVER          80
#define MAX_BYTE_EEPROM          512
#define SLOT_EEPROM_VARS         25    //no implementado
#define LM35                     1
#define TC1047                   2

/*==================[Variables de Factory reset]============================*/
String device                =   "terinfssp01";
String mqtt_server           =   "192.168.1.45";
String mqtt_tcp_str          =   "8883";
String passwd_AP             =   "Lancast3r";
String ssid                  =   "MAC";
String ssid_pass             =   "wpaadministrator08";
String area                  =   "Informatica";
String ubicacion             =   "Sala Servidores";
uint8_t sensor_type          =   TC1047;
float tmax                   =   10;
float tmin                   =   2;
float offset                 =   0;
float gain                   =   1;


/*=================[Variables harcodeadas]==================================*/
String www_username           =   "admin";
String hardware               =   "ter8266V1";
String tipo_device            =   "Termómetro";
const char * will_mess        =   "down";
int tls                       =   0;
const char * topic_telemetry  =   "v1/devices/me/telemtry";
const char * topic_attributes =   "v1/devices/me/attributes";
const char * topic_rpc        =   "v1/devices/me/rpc/request/+";


//--Variables globales
//String topic_sub;
String wifi_ssid;
String wifi_passwd;
String topic_dev_status;
//String topic_rpc;

uint16_t mqtt_tcp;

float Ti;           //Temp instantánea
float temperatura;  //promedio de temperatura

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
uint8_t intervalo_pub =10;       //intervalo de promediado y publicacion
uint8_t flag_publica =0;

//-- Versiones
String fversion="2.0.0";          
String hversion="1.0.0";      //en PCB Termometro 1.0

//--Instancias
MCC_wifi wifi;
MCC_mqtt broker;
ESP8266WebServer web_server (PORT_WEB_SERVER);



void setup() {
  Serial.begin(SERIAL_BAUDRATE);
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
  read_vars();  

  //--Pines
  pinMode(LED_WIFI,OUTPUT);
  pinMode(LED_PULSO,OUTPUT);
  pinMode(LED_RANGO,OUTPUT);
  pinMode(CANAL1, OUTPUT);
  pinMode(CANAL2, OUTPUT);
  //--Pone en 0 los pines de los canales
  digitalWrite(CANAL1, LOW);
  digitalWrite(CANAL2, LOW);
  
  //--Configuración interrupción por timer
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer0_ISR);
  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
  interrupts();

  bienvenida(LED_WIFI, LED_PULSO, LED_RANGO);

  //--Tópicos
  //topic_rpc="v1/devices/me/rpc/request/+";
  topic_dev_status=device+"/status/me";

  //--Inicializacion WIFi
  //Serial.println(ssid);
  //Serial.println(ssid_pass);

  wifi.init(device.c_str(), passwd_AP.c_str(), ssid.c_str(), ssid_pass.c_str(),LED_WIFI);//"hola","wpaadministrator08","MAC","wpaadministrator08",2);
  //flag_wifi_conn=wifi.conn();   //Si lo dejo bool se resetea (!!??)
  wifi.conn();
  //check_update();
  
  
  //ls();
  //--Inicializacion mqtt
  broker.init(mqtt_server.c_str(), mqtt_tcp, device.c_str(), topic_dev_status.c_str(), will_mess, topic_rpc, LED_WIFI);
  broker.setOnMess(rpc_proc);
  //while (!broker.conn()){
     //delay(3000);
     broker.conn();
  //}
  
  /*if (flag_wifi_conn){
    flag_mqtt_conn=broker.conn();
    //broker.sub(topic_sub);  //si se suscribe a otros tópicos (ojo, si se desconecta deberá volver a suscribirse!!)
  }else{
    Serial.println("No se puede conectar al broker.");
    flag_mqtt_conn=0;
  }*/

  
  //--Webserver y FS
  init_webserver();

   
}

void loop() {
  /*if(flag_mqtt_conn){
    wifi.control();
    broker.control();
    wifi.setLed();
    flag_seg == 0 ? digitalWrite(LED_PULSO,HIGH):digitalWrite(LED_PULSO,LOW);//digitalWrite(LED, !digitalRead(LED));
    broker.loop();
  }
  temp_inst();  
  temp_promedio();
  if (flag_publica){
    flag_publica=0;
    //--publica temperatura
    
  }*/
  web_server.handleClient();
  delay(100);
}

