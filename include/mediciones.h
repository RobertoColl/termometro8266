#ifndef _mediciones_h_
#define _mediciones_h_

#include <Arduino.h>
/*======================================[macros y definiciones]============================================*/
#define TC1047                  1
#define LM35                    2
#define DHT22                   3
//#define SENSOR                  LM35
#define MAX_SEG_PROM_AN0        600            //Cantidad de segundos para hacer promediado y publicacion
#define MAX_SEG_MUEST_AN0       1              //Cantidad de segundos para muestreo

//--Prototipos
void medicion(void);
void read_analog(void);
void promedia_analog(void);
float calc_unidad(uint16_t valor);




#endif