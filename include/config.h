#ifndef _config_h_
#define _config_h_

#include <Arduino.h>

/*===================[Definiciones de hardware]==================================*/
#define CANAL1                        12    // pin salida canal 1
#define CANAL2                        13    // pin salida canal 2
#define LED_PULSO                     14    // led de pulso
#define LED_WIFI                      2
#define LED_RANGO                     16
#define FRESET                        14    // pin para factory reset
#define CS_PIN                        4    // pin para CS de la SD

/*===================[Definiciones de software]==================================*/
#define SERIAL_BAUDRATE               19200
#define PORT_WEB_SERVER               80
#define MAX_BYTE_EEPROM               512
#define SLOT_EEPROM_VARS              30    //no implementado 
         





#endif