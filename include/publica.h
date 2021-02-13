#ifndef _publica_h_
#define _publica_h_

#include <Arduino.h>
#include "mqtt.h"

/*======================================[macros y definiciones]============================================*/
//#define INTERV_PUB                  600                 //Intervalo de publicacion en segundos
#define TELEMETRY                       1
#define ATTRIBUTES                      2

void send_pub(int topico);
void publica_medicion(void);
void publica_canales(uint8_t canal);
void publica_atributos(void);
void publica_rango(void);

#endif