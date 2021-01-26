#include "comandos.h"

//--Variables externas
extern MCC_mqtt broker;
extern String ubicacion;
extern String device;
extern String hardware;

void rpc_proc(char* topic, byte* payload, unsigned int length){
    payload[length] = '\0';
    String strTopic = String((char*)topic);
    String msg = String((char*)payload);
    broker.shortBlinkMqttLed();
    //Serial.println(strTopic);
    //Serial.println(msg);

}

void update(String msg){
    write_StringEE(470,msg);
    write_StringEE(500, "1");
    noInterrupts();
    EEPROM.commit();
    interrupts();
    ESP.reset();
}

/*void on_off(String msg){
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, msg);
    if (error) {
        Serial.print(F("[DEBUG] deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    int red = doc["r"];
    int green = doc["g"];
    int blue = doc["b"];
    if (red==0 && green==0 && blue==0){
        ws2812.off();
    } else
    {
        ws2812.oneColor(red,green,blue,0,103,1);
    }
    
}*/

