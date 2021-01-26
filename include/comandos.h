#ifndef _comandos_h_
#define _comandos_h_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "mqtt.h"
#include "eeprom.h"

void rpc_proc(char* topic, byte* payload, unsigned int length);
void on_off(String msg);
void color_capa(String msg,int desde, int hasta);
void update(String msg);

#endif