#include "comandos.h"

//--Objetos externos
extern MCC_mqtt broker;

//--Variables externas
extern String hardware;
extern String fversion;          
extern String hversion;
extern String device;
extern String mqtt_server;
extern uint16_t mqtt_tcp;
extern String mqtt_tcp_str;
extern String passwd_AP;
extern String ssid;
extern String ssid_pass;
extern String ubicacion;
extern String area;
extern String tipo_device;
extern String fuota_server;
extern uint32_t device_eeprom_pos;
extern uint32_t mqtt_server_eeprom_pos;
extern uint32_t mqtt_tcp_str_eeprom_pos;
extern uint32_t passwd_AP_eeprom_pos;
extern uint32_t ssid_eeprom_pos;
extern uint32_t ssid_pass_eeprom_pos;
extern uint32_t ubicacion_eeprom_pos;
extern uint32_t fuota_eeprom_pos;
extern uint32_t area_eeprom_pos;
extern uint8_t canal1;
extern uint8_t canal2;


//--Variables locales
String topic_rpc_rta;
String topic_rpc_req;
String msg_rpc_req;
const char* comando_rpc;
const char* parametro_rpc;
String valor;
String parametro;
String comando;
int ind1;
StaticJsonDocument<200> out;
char json_out[200];
uint8_t flag_canal1=0;
uint8_t flag_canal2=0;


void rpc_proc(char* topic, byte* payload, unsigned int length){
    //Serial.println("Comando!");
    payload[length] = '\0';
    topic_rpc_req = String((char*)topic);
    msg_rpc_req = String((char*)payload);
    topic_rpc_rta="v1/devices/me/rpc/response/"+topic_rpc_req.substring(26);
    broker.shortBlinkMqttLed();
    Serial.print("Topico de pregunta:");Serial.println(topic_rpc_req);
    Serial.print("Mensaje de pregunta:");Serial.println(msg_rpc_req);
    Serial.print("Topico de respuesta:");Serial.println(topic_rpc_rta);

    //--parseo de la payload (msg_rpc_req)
    StaticJsonDocument<200> parse_payload;
    DeserializationError error = deserializeJson(parse_payload, msg_rpc_req);
    if (error) {
        Serial.print("[DEBUG] deserializeJson() fallo: ");
        Serial.println(error.f_str());
        return;
    }
    //--comando
    comando_rpc=parse_payload["method"];

    //--parametos y valores
    parametro_rpc=parse_payload["params"];
    ind1=((String)parametro_rpc).indexOf(" ");
    parametro=((String)parametro_rpc).substring(0,ind1);
    valor=((String)parametro_rpc).substring(ind1+1);

    //--Debug del parseo
    Serial.print("Comando:");Serial.println(comando_rpc);
    Serial.print("Parametro:");Serial.println(parametro);
    Serial.print("Valor:");Serial.println(valor);

    comando=(String)comando_rpc;
    //json_out[0]='\0';
    out.clear();

    //--Switch de los comandos
    if (comando=="?"){
        rpc_help();
    }
    else if (comando=="read"){
        rpc_read();
    }
    else if (comando=="update"){
        rpc_update(parametro);
    }
    else if (comando=="reset"){
        ESP.reset();
    }
    else if(comando=="ver"){
        rpc_version();
    }
    else if(comando=="param"){
        rpc_param(parametro);
    }
    else if(comando=="set"){
        rpc_set(parametro,valor);
    }
    else{
        rpc_unknown(comando);
    }
}

void send_rpc_rta(void){
    serializeJson(out, json_out);
    broker.pub(topic_rpc_rta,json_out);
}

//------------------------------Comandos RPC----------------------------------------
void rpc_help(void){
    out["comandos"]="read, param, set, update, reset, ver";
    send_rpc_rta();
}

void rpc_read(void){
    out["Valor"]=0;
    send_rpc_rta();
}

void rpc_version(void){
    out["Version Hardware"]=hversion;
    out["Version Firmware"]=fversion;
    send_rpc_rta();
}

void rpc_update(String version){
    write_StringEE(470,version);
    write_StringEE(500, "1");
    noInterrupts();
    EEPROM.commit();
    interrupts();
    ESP.reset();
}

void rpc_update(void){
    //TODO: ver la version
    //write_StringEE(470,version);
    write_StringEE(500, "1");
    noInterrupts();
    EEPROM.commit();
    interrupts();
    ESP.reset();
}


//--Muestra las variables de EEPROM (parámetros)
void rpc_param(String parametro){
    if (parametro=="ssid"){
        out["ssid"]=ssid;
    }
    else if (parametro=="ssid_pass"){
        out["ssid passwd"]=ssid_pass;
    }
    else if (parametro=="device"){
        out["Nombre"]=device;
    }
    else if (parametro=="mqtt_server"){
        out["mqtt_server"]=mqtt_server;
    }
    else if (parametro=="mqtt_port"){
        out["mqtt_port"]=mqtt_tcp_str;
    }
    else if (parametro=="ubicacion"){
        out["Ubicacion"]=ubicacion;
    }
    else if (parametro=="fuota"){
        out["fuota_server"]=fuota_server;
    }
    else if (parametro=="area"){
        out["Area"]=area;
    }
    send_rpc_rta();
}

//--Comandos set
void rpc_set(String parametro,String valor){
    //--Cambia las variables de EEPROM
    if (parametro=="ssid"){
        ssid=valor;
        //write_StringEE(ssid_eeprom_pos, valor);
        out["ssid"]=valor;
    }
    else if (parametro=="ssid_pass"){
        ssid_pass=valor;
        //write_StringEE(ssid_pass_eeprom_pos, valor);
        out["ssid passwd"]=valor;
    }
    else if (parametro=="device"){
        device=valor;
        //write_StringEE(device_eeprom_pos, valor);
        out["Nombre"]=valor;
    }
    else if (parametro=="mqtt_server"){
        mqtt_server=valor;
        //write_StringEE(mqtt_server_eeprom_pos, valor);
        out["mqtt_server"]=valor;
    }
    else if (parametro=="mqtt_port"){
        mqtt_tcp_str=valor;
        //write_StringEE(mqtt_tcp_str_eeprom_pos, valor);
        out["mqtt_port"]=valor;
    }
    else if (parametro=="ubicacion"){
        ubicacion=valor;
        //write_StringEE(ubicacion_eeprom_pos, valor);
        out["Ubicacion"]=valor;
    }
    else if (parametro=="fuota"){
        fuota_server=valor;
        //write_StringEE(fuota_eeprom_pos, valor);
        out["fuota_server"]=valor;
    }
    else if (parametro=="area"){
        area=valor;
        //write_StringEE(area_eeprom_pos, valor);
        out["Area"]=valor;
    }
    //Cambia estado de canales
    else if (parametro=="canal1"){
        if (valor=="1"){
            canal1_on();
            out["Canal 1"]="Encendido";
            
        }
        else if (valor=="0"){
            canal1_off();
            out["Canal 1"]="Apagado";
        }
    }
    else if (parametro=="canal2"){
        if (valor=="1"){
            canal2_on();
            out["Canal 2"]="Encendido";
        }
        else if (valor=="0"){
            canal2_off();
            out["Canal 2"]="Apagado";
        }
    }
    write_vars();
    read_vars(0);
    send_rpc_rta();
}

void rpc_unknown(String comando){
    out["Desconocido"]=comando;
    send_rpc_rta();
}

