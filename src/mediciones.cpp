//--Proceso de medicion
#include "mediciones.h"

//--Variables locales
uint32_t an0=0;
uint32_t an0_prom=0;
uint32_t an0_sum=0;
float temperatura=0;
float tempinst=0;
uint16_t seg_prom_an0=0;
uint8_t seg_muest_an0=0;

//--Variables externas
extern uint8_t flag_publica;
extern float offset;
extern float gain;
extern uint8_t sensor;


//--Prototipos
void medicion(void){
    if(seg_muest_an0>=MAX_SEG_MUEST_AN0){
        read_analog();
        seg_muest_an0=0;
    }
    if (seg_prom_an0>=MAX_SEG_PROM_AN0){
        promedia_analog();
        seg_prom_an0=0;
        flag_publica=1;
    }
}

void read_analog(void){
    an0 = analogRead(A0);
    tempinst=calc_unidad(an0);
    an0_sum=an0_sum+an0;
}

void promedia_analog(void){
    an0_prom=an0_sum/(MAX_SEG_PROM_AN0/MAX_SEG_MUEST_AN0);
    temperatura=calc_unidad(an0_prom);
    an0_sum=0;
    seg_muest_an0=0;
}

float calc_unidad(uint16_t an0_valor){
    switch (sensor){
        case LM35:
            return ((float)an0_valor*100/1024)*gain+offset;              //para LM35
            break;
        case TC1047:
            return ((((float)an0_valor/1024)-.5)/.01)*gain+offset;       //para TC1047
            break;
    }
}