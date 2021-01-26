//--Proceso de medicion
#include "medicion.h"

//--Variables extenas
extern uint8_t sensor_type;
extern float tmax;
extern float tmin;
extern float offset;
extern float gain;
extern float Ti;
extern float temperatura;
extern uint8_t intervalo_pub;
extern uint8_t flag_publica;

//--Variables locales
uint8_t flag_read_analog;
uint16_t Vtemp;
float sumtemp;
uint8_t minu_ip;




void temp_inst(void){
    if (flag_read_analog){
        flag_read_analog=0;
        Vtemp = analogRead(A0);
        switch (sensor_type){
            case 1:
                Ti=((float)Vtemp*100/1024)*gain+offset;              //para LM35
                break;
            case 2:
                Ti=((((float)Vtemp/1024)-.5)/.01)*gain+offset;       //para TC1047
                break;
        }
        //Serial.println(Vtemp);
        //Serial.println((float)Vtemp/1024);
        sumtemp=sumtemp+Vtemp;
    }
}

void temp_promedio(void){
    if (minu_ip==intervalo_pub){
        switch (sensor_type){
                case 1:
                    temperatura=((sumtemp/(intervalo_pub*60))*100/1024)*gain+offset;       //para LM35
                    break;
                case 2:
                    temperatura=((((sumtemp/(intervalo_pub*60))/1024)-.5)/.01)*gain+offset; //para TC1047
                    break;  
            }
            sumtemp=0;
            minu_ip=0;
            flag_publica=1;
        }
}
