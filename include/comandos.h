#ifndef _comandos_h_
#define _comandos_h_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "mqtt.h"
#include "eeprom.h"
#include "funciones.h"
#include "actuacion.h"
#include "config.h"

void rpc_proc(char* topic, byte* payload, unsigned int length);
void send_rpc_rta(void);
void rpc_read(void);
void rpc_update(void);
void rpc_update(String msg);
void rpc_help(void);
void rpc_version(void);
void rpc_param(String parametro);
void rpc_set(String parametro,String valor);
void rpc_ident(void);
void rpc_ip(void);
void rpc_push(void);
void rpc_readatt(void);
void rpc_unknown(String comando);
void rpc_check_rango(void);

#endif