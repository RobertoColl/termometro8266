#ifndef _publica_h_
#define _publica_h_

#include <Arduino.h>
#include "mqtt.h"

/*======================================[macros y definiciones]============================================*/
#define INTERV_PUB                  600                 //Intervalo de publicacion en segundos

void send_pub(void);
void publica_medicion(void);
void publica_canales(uint8_t canal);

#endif