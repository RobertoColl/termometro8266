#ifndef _funciones_h_
#define _funciones_h_

#include <ESP8266httpUpdate.h>
#include <LittleFS.h>
#include <Arduino.h>
#include "eeprom.h"
#include "config.h"

void ls(void);
void bienvenida (void);
void read_vars(bool ver);
void write_vars(void);
void factory_reset(void);
//unsigned int hexToDec(String hexString);
void check_update(void);

#endif