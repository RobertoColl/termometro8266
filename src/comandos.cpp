#include "comandos.h"

//--Objetos externos
extern MCC_mqtt broker;
extern MCC_mqtt wifi;

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
extern uint32_t fuota_version_eeprom_pos;
extern uint32_t flag_update_eeprom_pos;
extern uint8_t canal1_status;
extern uint8_t canal2_status;
extern uint8_t flag_push;
extern const char * topic_attributes;
extern uint8_t flag_push_att;
extern float tmax;
extern float tmin;
extern float offset;
extern float gain;
extern uint8_t sensor;
extern float tempinst;
extern float temperatura;
extern uint8_t flag_rango;


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



void rpc_proc(char* topic, byte* payload, unsigned int length){
    payload[length] = '\0';
    topic_rpc_req = String((char*)topic);
    msg_rpc_req = String((char*)payload);
    //--Debug de mensaje de entrada
    //Serial.print("Topico de pregunta:");Serial.println(topic_rpc_req);
    //Serial.print("Mensaje de pregunta:");Serial.println(msg_rpc_req);
    //Serial.print("Topico de respuesta:");Serial.println(topic_rpc_rta);

    //--parseo de la payload (msg_rpc_req)
    StaticJsonDocument<200> parse_payload;
    DeserializationError error = deserializeJson(parse_payload, msg_rpc_req);
    if (error) {
        Serial.print("[DEBUG] deserializeJson() fallo: ");
        Serial.println(error.f_str());
        return;
    }

    //--Gestion de atributos
    //--Acá entran los cambios realizados desde widget update shared attributes
    if (topic_rpc_req==topic_attributes){
        //Serial.print("Mensaje de pregunta:");Serial.println(msg_rpc_req);
        if (parse_payload.containsKey("Tmax")) {
            tmax=parse_payload["Tmax"];
            //Serial.println(tmax);
        }else if (parse_payload.containsKey("Tmin")) {
            tmin=parse_payload["Tmin"];
            //Serial.println(tmin);
        }
        //--Grabar en eeprom 
        write_vars();
        //--Publicar atributos 
        flag_push_att=1;
        publica_atributos();
        return;
    }

    //--Gestion de RPC
    topic_rpc_rta="v1/devices/me/rpc/response/"+topic_rpc_req.substring(26);

    //--comando
    comando_rpc=parse_payload["method"];

    //--parametos y valores
    parametro_rpc=parse_payload["params"];
    ind1=((String)parametro_rpc).indexOf(" ");
    parametro=((String)parametro_rpc).substring(0,ind1);
    valor=((String)parametro_rpc).substring(ind1+1);

    //--Debug del parseo
    //Serial.print("Comando:");Serial.println(comando_rpc);
    //Serial.print("Parametro:");Serial.println(parametro);
    //Serial.print("Valor:");Serial.println(valor);

    comando=(String)comando_rpc;
    out.clear();

    //--Switch de los comandos
    if (comando=="?"){
        rpc_help();
    }
    else if (comando=="read"){
        rpc_read();
    }
    else if (comando=="update"){
        if (parametro==""){
            rpc_update();
        }
        else{
            rpc_update(parametro);
        }
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
    else if(comando=="ident"){
        rpc_ident();
    }
    else if(comando=="ip"){
        rpc_ip();
    }
    else if(comando=="push"){
        rpc_push();
    }
    else if(comando=="read_att"){//todo: discriminar entre att comunes y de tipo de dispositivo
        rpc_readatt();
    }
    else if (comando=="checkRango"){
        rpc_check_rango();
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
    out["comandos"]="read, read_att, param, set, update, reset, ident, ip, ver";
    send_rpc_rta();
}

void rpc_read(void){
    out["Ti:"]=tempinst;
    out["Tp:"]=temperatura;
    send_rpc_rta();
}

void rpc_version(void){
    out["Version Hardware"]=hversion;
    out["Version Firmware"]=fversion;
    send_rpc_rta();
}

void rpc_update(String version){
    write_StringEE(fuota_version_eeprom_pos,version);//470
    write_StringEE(flag_update_eeprom_pos, "1");//500
    out["Actualizando a:"]="V"+version;
    send_rpc_rta();
    noInterrupts();
    EEPROM.commit();
    interrupts();
    delay(100);
    ESP.reset();
}

void rpc_update(void){
    //Serial.print("sin param->");
    //Serial.println("V"+fversion);
    write_StringEE(fuota_version_eeprom_pos,"V"+fversion);//470
    write_StringEE(flag_update_eeprom_pos, "1");//500
    out["Actualizando a:"]="V"+fversion;
    send_rpc_rta();
    noInterrupts();
    EEPROM.commit();
    interrupts();
    delay(100);
    ESP.reset();
}


//--Muestra las variables de EEPROM y otros parámetros
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
    else if (parametro=="Tmax"){
        out["Tmax"]=tmax;
    }
    else if (parametro=="Tmin"){
        out["Tmin"]=tmin;
    }
    else if (parametro=="Offset"){
        out["Offset"]=offset;
    }    
    else if (parametro=="Gain"){
        out["Gain"]=gain;
    }
    else if (parametro=="Sensor"){
        out["Sensor"]=sensor;
    }
    else if (parametro=="canal1"){
        out["Canal 1"]=canal1_status==1 ? "Encendido":"Apagado";
    }
    else if (parametro=="canal2"){
        out["Canal 2"]=canal2_status==1 ? "Encendido":"Apagado";
    }

    send_rpc_rta();
}

//--Comandos set
void rpc_set(String parametro,String valor){
    //--Cambia las variables de EEPROM
    if (parametro=="ssid"){
        ssid=valor;
        out["ssid"]=valor;
        flag_push_att=1;
        publica_atributos();
    }
    else if (parametro=="ssid_pass"){
        ssid_pass=valor;
        out["ssid passwd"]=valor;
    }
    else if (parametro=="device"){
        device=valor;
        out["Nombre"]=valor;
    }
    else if (parametro=="mqtt_server"){
        mqtt_server=valor;
        out["mqtt_server"]=valor;
    }
    else if (parametro=="mqtt_port"){
        mqtt_tcp_str=valor;
        out["mqtt_port"]=valor;
    }
    else if (parametro=="ubicacion"){
        ubicacion=valor;
        out["Ubicacion"]=valor;
        flag_push_att=1;
        publica_atributos();
    }
    else if (parametro=="fuota"){
        fuota_server=valor;
        out["fuota_server"]=valor;
    }
    else if (parametro=="Area"){
        area=valor;
        out["Area"]=valor;
        flag_push_att=1;
        publica_atributos();
    }
    else if (parametro=="Tmax"){
        tmax=valor.toFloat();
        out["Tmax"]=valor;
        flag_push_att=1;
        publica_atributos();
    }
    else if (parametro=="Tmin"){
        tmin=valor.toFloat();
        out["Tmin"]=valor;
        flag_push_att=1;
        publica_atributos();
    }
    else if (parametro=="Offset"){
        offset=valor.toFloat();
        out["Offset"]=valor;
        flag_push_att=1;
        publica_atributos();
    }
    else if (parametro=="Gain"){
        gain=valor.toFloat();
        out["Gain"]=valor;
        flag_push_att=1;
        publica_atributos();
    }
    else if (parametro=="Sensor"){
        sensor=valor.toFloat();
        out["Sensor"]=valor;
        flag_push_att=1;
        publica_atributos();
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
    //--TODO: validar parametro
    write_vars();
    read_vars(0);
    send_rpc_rta();
}

void rpc_ident(void){
    out["Identificando"]="...";
    send_rpc_rta();
    for(int i=0;i<5;i++){
       bienvenida(14, 2, 16);
    }
}

void rpc_ip(void){
    out["IP Wlan"]=String(WiFi.localIP()[0])+"."+String(WiFi.localIP()[1])+"."+String(WiFi.localIP()[2])+"."+String(WiFi.localIP()[3]);
    out["IP AP"]=String(WiFi.softAPIP()[0])+"."+String(WiFi.softAPIP()[1])+"."+String(WiFi.softAPIP()[2])+"."+String(WiFi.softAPIP()[3]);
    send_rpc_rta();
}

void rpc_push(void){
    flag_push=1;
    out["Push"]="Enviando Telemetria";
    //out["Valor"]=0;
    send_rpc_rta();
    publica_medicion();
}

void rpc_readatt(){
    out["ssid"]=ssid;
    out["Area"]=area;
    out["Ubicacion"]=ubicacion;
    out["Version firmware"]=fversion;
    send_rpc_rta();
    //publica_atributos();
}

void rpc_check_rango(void){
    out["outRange"]=flag_rango;
    send_rpc_rta();
    publica_rango();
}

void rpc_unknown(String comando){
    out["Desconocido"]=comando;
    send_rpc_rta();
}

