#include "mqtt.h"

//--Instancias
//extern  WiFiClient wifi_client;              //Sin TLS
WiFiClientSecure wifi_client;           //Con TLS  
PubSubClient mqtt(wifi_client);


/**
 * @brief Inicializa la clase: setea el servidor mqtt y el puerto
 * 
 * @param server ip del broker
 * @param port_server puerto tcp del broker
 * @param device nombre del dispositivo (se usa como id de sesión mqtt)
 * @param topic_status topic del status de la conexión
 * @param will_mess mensaje de desconexión
 * @param topic_sub 1er topico de subscripcion
 * @param led led de estado mqtt
 */
void MCC_mqtt::init(const char * server, uint16_t port_server, const char * device, const char * topic_status, const char * will_mess, const char * topic_sub, int led){
    this->_server=server;
    this->_port_server=port_server;
    this->_device=device;
    this->_topic_status=topic_status;
    this->_will_mess=will_mess;
    this->_topic_sub=topic_sub;
    this->_led=led;
    this->_ca_cert=\
                    "-----BEGIN CERTIFICATE-----\n" \
                    "MIIDtjCCAp6gAwIBAgIEJX4o1jANBgkqhkiG9w0BAQsFADB4MQswCQYDVQQGEwJB\n" \
                    "UjEQMA4GA1UECBMHU2FudGFGZTEQMA4GA1UEBxMHUm9zYXJpbzEWMBQGA1UEChMN\n" \
                    "TVVOSUNJUEFMSURBRDEWMBQGA1UECxMNTVVOSUNJUEFMSURBRDEVMBMGA1UEAxMM\n" \
                    "MTkyLjE2OC4xLjI1MB4XDTE5MDIxODE1MjcxOVoXDTQ2MDcwNTE1MjcxOVoweDEL\n" \
                    "MAkGA1UEBhMCQVIxEDAOBgNVBAgTB1NhbnRhRmUxEDAOBgNVBAcTB1Jvc2FyaW8x\n" \
                    "FjAUBgNVBAoTDU1VTklDSVBBTElEQUQxFjAUBgNVBAsTDU1VTklDSVBBTElEQUQx\n" \
                    "FTATBgNVBAMTDDE5Mi4xNjguMS4yNTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC\n" \
                    "AQoCggEBAJrHDfpvlhkGsRaMeGHY/4QBYhrN9M6I6FBOCicJz4nu0UZYqCryoONT\n" \
                    "uT5iLraLT0a26FtIVZNw74hgXyGhpg+llwL4pf6MciN7ExhNQfw37iC3K9gNpPTr\n" \
                    "QKYY4ZqPfHvl0uva5bgd2SIbADuA/mUnGQToSY9DPOqAxitqD5JGOcyE7owBTCwd\n" \
                    "etk9relkSSQZ3Dww65ZuTvKBP++X4v8U46DNaDtCrx4cCMrb0Qzc2YGJo7Ufhvvo\n" \
                    "vF9CYazJY+GTDlEfLMsyOk8yhg6J8nSEaYGKbAL2nR5SEnsrCPsqeL/vG45vOHaY\n" \
                    "6HAOYiY+VJ71Ll7vle1gr81VunA4YusCAwEAAaNIMEYwJQYDVR0RBB4wHIIUbWNh\n" \
                    "c3RlbGxvLmR5bmRucy5vcmeHBMCoARkwHQYDVR0OBBYEFGjNNy+2fRdSB7Btn95F\n" \
                    "uBnu9O8VMA0GCSqGSIb3DQEBCwUAA4IBAQCB3mrGDRUFNybpEmMbc1BjjCimDJ32\n" \
                    "HOdodKNx6lf5YSCrqfpOcCprKVfsgx0bSCIdraVIZRA8+9fTd8b+/Si/qWqyxRA2\n" \
                    "DgQwHmpMvbCG4eV1AnxMgab/k0oBo8xcdKNRRfQ90AgHmmsB3hWH2qAlYnWd8IH4\n" \
                    "0yZqJGLMjGsXQ79P2lyNhLOIi1ZpLXFimPd9IiaUjQTQ8CfMhtMyYQojCAYEdWT+\n" \
                    "BGSMw5+j1fSip8MGCQuX3DnPGMNF3ZDVpNeGp93b06PPt/Dt5R1991s6Hsy+5cWN\n" \
                    "4ZjfMf4ltJXv624Q3bpkA/LDtZ75NQUaSazHQJL3YeLlQvnQqBhTLpL/\n" \
                    "-----END CERTIFICATE-----\n";
    //LittleFS.begin();
    File cert = LittleFS.open("/mqttserver.der", "r");
    if (!cert) {
        Serial.println("Fallo apertura del certificado");
    }
    else{
        Serial.println("Certificado abierto");
    }
    delay(300);
    if (wifi_client.loadCertificate(cert))
        Serial.println("Certificado cargado");
    else
        Serial.println("Certificado NO cargado");
    //wifi_client.loadCertificate(cert);
    mqtt.setServer(_server, _port_server);
}

/**
 * @brief Genera la función callback para la entrada de mensajes
 * 
 * @param callb 
 */
void MCC_mqtt::setOnMess(std::function<void (char *, uint8_t *, unsigned int)> callb){
    mqtt.setCallback(callb);
}

/**
 * @brief Conecta al broker. Si el tiempo de conexión supera MAX_MQTT_CONNECTION_TIMEOUT (definido en mqtt.h),
 *        sale.
 * 
 * @return bool 
 *        Devuelve el resultado de la conexión: true conexion exitosa, false no se pudo realizar la conexión.
 */
bool MCC_mqtt::conn(void){
    _flag_mqtt=false;
    mqtt_conn_timeout=0;

/*
      if (!mqtt.connected()) {
        if (mqtt.connect(_device,_device,"")) {
            Serial.println("===> mqtt conectado");
            mqtt.subscribe("v1/devices/me/rpc/request/+");
            mqtt.subscribe("v1/devices/me/attributes");
            _flag_mqtt=true;
        } 
        else{
            Serial.print("---> mqtt fallo, rc=");
            Serial.println(mqtt.state());
            _flag_mqtt=false;
        }
      }*/


    while (!mqtt.connected()) {
        Serial.print("Intentando conexión a Broker MQTT ");
        Serial.print(_server);
        Serial.print(":");
        Serial.print(_port_server);
        Serial.print(" - ");
        Serial.print(_device);
        Serial.println("...");
        //_conn_mqtt_status=mqtt.connect(_device,_topic_status,WILL_QOS, WILL_RETAIN_MESSAGE, _will_mess); 
        _conn_mqtt_status=mqtt.connect(_device,_device,""); 
        _mqtt_conn_retrys++;
        delay(5000);
        Serial.println(_conn_mqtt_status);
        delay(1000);
        if (_conn_mqtt_status) {
            _flag_mqtt=true;
            Serial.println("Conectado a MQTT");
            this->sub(_topic_sub);
            this->pub(_topic_status,"up");
            return _flag_mqtt;
            //break;
        }
        delay(300);
        Serial.print("Reintento:");Serial.println(_mqtt_conn_retrys);
        if (_mqtt_conn_retrys>=MAX_MQTT_CONNECTION_RETRYS){
            _flag_mqtt=false;
            Serial.print("MAX Retrys. Falló conexióm MQTT, rc=");
            Serial.println(mqtt.state());
            return _flag_mqtt;
            //break;
        }
    }
    return _flag_mqtt;
}

/**
 * @brief Subscribe a un tópico. Si no está conectado, va a conexión
 * 
 * @param topic 
 * 
 * @return bool 
 *        Devuelve el resultado de la suscripción: true exitosa, false no se suscribió
 */
bool MCC_mqtt::sub(String topic){
    if (!mqtt.connected()){
        if (this->conn()){
            mqtt.subscribe(topic.c_str());
            return true;
        }else{
            Serial.println("No se pudo suscribir al topico");
            return false;
        }
    }
    mqtt.subscribe(topic.c_str());
    return true;
}

/**
 * @brief Loop principal del mqtt. Si no está conectado, va a conexión
 * 
 * @return bool
 *        Devuelve resultado del loop: true se pudo hacer, false no se pudo hacer
 */
bool MCC_mqtt::loop(void){
    if (!mqtt.connected()){
        if (this->conn()){
            mqtt.loop();
            return true;
        }else{
            Serial.println("No se pudo hacer loop");
            return false;
        }
    }
    mqtt.loop();
    return true;
}


/**
 * @brief Publica un mensaje en el broker. Si no está conectado, va a conexión
 * 
 * @param topic 
 * @param message 
 */
bool MCC_mqtt::pub(String topic, String message){
    if (mqtt.connected()) {
        if (mqtt.publish(topic.c_str(),message.c_str())){
            this->shortBlinkMqttLed();
            return true;
        }else{
            Serial.println("Falló la publicación");
            return false;
        }
    }else{
        this->conn();
    }
    return false;
}

/**
 * @brief Controla la conexión al broker. Si la desconexión perdura por un 
 *        tiempo superior a MAX_MQTT_LIVE_TIMEOUT (definido en mqtt.h), 
 *        resetea el microcontrolador.
 * 
 * @return int 
 *        Devuelve el estado de la conexión: 1 conectado, 0 no conectado
 */
bool MCC_mqtt::control(void){
    if (!mqtt.connected()){
         live_timeout_mqtt=millis();
      return false;
    }
    if (live_timeout_mqtt - millis() <= MAX_MQTT_LIVE_TIMEOUT) {
        Serial.println("########################");
        Serial.println("Reset por MQTT Timeout");
        Serial.println("########################");
        ESP.reset(); 
    }
    return true;
}

void MCC_mqtt::shortBlinkMqttLed(void){
    digitalWrite(_led, HIGH);
    delay(SHORT_DELAY_MQTT_LED);
    digitalWrite(_led, LOW);

}