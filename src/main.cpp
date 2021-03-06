/*********************************************************************************
 * 
 *          T E M P L A T E   S I S T E M A   D E   M O N I T O R E O            *
 * 
 * *******************************************************************************/

/*===================[File header]===============================================*/
// File:                main.cpp
// Author:              Marcelo Castello (https://github.com/mcastellogh)
// Licence:             GPLV3+
// Version:             2.0.0
// Date:                Diciembre 2020
// Info:                Template para sistema de monitoreo sobre hardware Termometro 8266 V1.0

//=============== [Datos para almacenamiento] ======================================
// PCB:                 Termometro V1.0
// Plataforma:          ESP8266  
// Uso:                 Template
// Carpeta:             termometro_V1_8266
// Nombre de Proyecto:  template

// TODO:    
//        
//
// BUGS:    
//          - No graba nombre desde portal cautivo=>Sí graba, pero cuando cambio nombre debe hacer nuevamente broker.init para que tome el nuevo valor
//            Solucion temporal: resetear luego de cambiar nombre.
//
//          - Factory reset cuando tiene fuente externa. 
//
//          - Al crear un nuevo dispositivo desde TB: No enciende led online, no publica attributos Tmax y Tmin (probar nuevamente)
//                   
//
// COMMIT:  
//
// Para subir FileSystem: platformio run --target uploadfs

/*===================[Inclusiones]===============================================*/
#include "wifi.h"
#include "interrupciones.h"
#include "global_vars.h"
#include "mqtt.h"
#include "funciones.h"
#include "comandos.h"
#include "eeprom.h"
#include "frontend.h"
#include "backend.h"
#include "mediciones.h"
#include "publica.h"
#include "config.h"

/*===================[Posicion de variables EEPROM]==============================*/
uint32_t device_eeprom_pos        =   0;
uint32_t mqtt_server_eeprom_pos   =   30;
uint32_t mqtt_tcp_str_eeprom_pos  =   60;
uint32_t passwd_AP_eeprom_pos     =   90;
uint32_t ssid_eeprom_pos          =   120;
uint32_t ssid_passwd_eeprom_pos   =   150;
uint32_t ubicacion_eeprom_pos     =   180;
uint32_t fuota_eeprom_pos         =   210;
uint32_t area_eeprom_pos          =   240;
uint32_t canal1_eeprom_pos        =   270;
uint32_t canal2_eeprom_pos        =   300;
uint32_t tmax_eeprom_pos          =   330;
uint32_t tmin_eeprom_pos          =   360;
uint32_t offset_eeprom_pos        =   390;
uint32_t gain_eeprom_pos          =   420;
uint32_t sensor_eeprom_pos        =   450;
uint32_t fuota_version_eeprom_pos =   480;
uint32_t flag_update_eeprom_pos   =   500;

/*===================[Variables de Factory reset]================================*/
String device                     =   "term";               //usado como template
String mqtt_server                =   "192.168.1.45";
String fuota_server               =   "192.168.1.17";
String mqtt_tcp_str               =   "8883";
String ssid                       =   "MAC";
String ssid_pass                  =   "wpaadministrator08";
String passwd_AP                  =   "Lancast3r";
String area                       =   "Informatica";
String ubicacion                  =   "Sala Servidores";
uint8_t canal1_status             =   0;
uint8_t canal2_status             =   0;
float tmax                        =   10;
float tmin                        =   2;
float offset                      =   0;
float gain                        =   1;
uint8_t sensor                    =   1;

/*===================[Variables harcodeadas]=====================================*/
String www_username               =   "admin";
String hardware                   =   "ter8266V1";
String tipo_device                =   "Termometro";
const char * will_mess            =   "down";
uint8_t tls                       =   1;
const char * topic_telemetry      =   "v1/devices/me/telemetry";
const char * topic_attributes     =   "v1/devices/me/attributes";
const char * topic_rpc            =   "v1/devices/me/rpc/request/+";
uint8_t MAX_mqtt_conn_timeout     =   10;       //timeout en segundos para la conexion al broker
uint8_t MAX_live_timeout_mqtt     =   5;        //timeout en minutos de desaparicion de broker
float  hist_rango                 =   0.5;      //histeresis en grados para la determinacion del rango
//-- Versiones
String fversion                   =   "2.0.1";          
String hversion                   =   "1.0.0";  //en PCB Termometro 1.0

/*===================[Variables globales]========================================*/
String topic_dev_status;
uint16_t mqtt_tcp;
//uint8_t mqtt_conn_timeout=0;      //timeout de la conexion al broker en segundos
uint8_t wifi_conn_timeout=0;      //timeout de la conexion al broker en segundos
uint8_t flag_seg=0;
uint8_t flag_wifi=0;
uint8_t flag_publica=0;
uint8_t led_pulso=LED_PULSO;
uint8_t canal1_pin=CANAL1;
uint8_t canal2_pin=CANAL2;

//===================[Instancias]================================================*/
MCC_wifi wifi;
MCC_mqtt broker;
ESP8266WebServer web_server (PORT_WEB_SERVER);



void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  //Serial.setDebugOutput(true);
  delay(100);


  

  Serial.println(tipo_device);
  Serial.print("Version:");
  Serial.print(fversion);
  EEPROM.begin(MAX_BYTE_EEPROM);
  LittleFS.begin();
  
  //--Pines
  pinMode(LED_WIFI,OUTPUT);
  pinMode(LED_PULSO,OUTPUT);
  pinMode(LED_RANGO,OUTPUT);
  pinMode(CANAL1, OUTPUT);
  pinMode(CANAL2, OUTPUT);
  //--Pone en 0 los pines de los canales
  digitalWrite(CANAL1, LOW);
  digitalWrite(CANAL2, LOW);

  factory_reset();
  read_vars(1);  
  bienvenida();

  //--Tópicos
  topic_dev_status=device+"/status/me";

  //--Inicializacion WIFi
  wifi.init(device.c_str(), passwd_AP.c_str(), ssid.c_str(), ssid_pass.c_str(),LED_WIFI);
  wifi.conn();
  check_update();
  
  //--Inicializacion mqtt
  broker.init(mqtt_server.c_str(), mqtt_tcp, device.c_str(), topic_dev_status.c_str(), will_mess, topic_rpc, LED_PULSO);
  broker.setOnMess(rpc_proc);
  broker.conn();
  broker.sub(topic_attributes);

  //--Inicializacion de webserver
  init_webserver();

  //--Configuración interrupción por timer
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer0_ISR);
  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
  interrupts();
  delay(100);
}

void loop() {
  flag_wifi=wifi.control();
  if (flag_wifi){
    broker.control();
    broker.loop();
    publica_medicion();
    publica_atributos();
  }
  web_server.handleClient();
  medicion();
  rango();
  delay(100);
}

