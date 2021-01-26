#ifndef _backend_h_
#define _backend_h_

#include <Arduino.h>
#include <LittleFS.h>
#include <ESP8266WebServer.h>
#include "eeprom.h"
#include "frontend.h"
#include "funciones.h"

void init_webserver(void);
void auth(void);
void handle_setfixcolor(void);
void handle_setcolor(void);
void handleUpdateSettings();
void handle_refresh();
void handle_bootstrap_css(void);
void handle_bootstrap_js(void);
void handle_jquery(void);
void handle_font(void);
#endif

