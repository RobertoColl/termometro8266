#include "wifi.h"

//--Instancias
IPAddress ip_wlan;
IPAddress myIP;
//WiFiClient wifi_client;           //Sin TLS
//WiFiClientSecure wifi_client;       //Con TLS

/**
 * @brief Sólo inicializa las variables de la clase
 * 
 * @param device 
 * @param passwd_AP 
 * @param ssid 
 * @param ssid_pass 
 * @param led 
 */
void MCC_wifi::init(String device,String passwd_AP, String ssid, String ssid_pass, int led){
   this->_device=device;
   this->_passwd_AP=passwd_AP;
   this->_ssid=ssid;
   this->_ssid_pass=ssid_pass;
   this->_led=led;
}

/**
 * @brief Pone al micro en modo AP y WiFI. Conecta a la WiFi.
 *        Si el tiempo de conexión supera MAX_wifi_conn_timeout (definido en wifi.h),
 *        pasa a modo AP y sale.
 * 
 * @return int 
 *        Devuelve el resultado de la conexión: 1 conexion exitosa, 0 no se pudo realizar la conexión.
 */
uint8_t MCC_wifi::conn(void){

   //Serial.println(_ssid);
   //Serial.println(_ssid_pass);

   WiFi.mode(WIFI_AP_STA);
   WiFi.disconnect() ;
   WiFi.persistent(false);
   //--Access Point 
   Serial.println(WiFi.softAP(_device,_passwd_AP) ? "AP Ready" : "AP Failed!");
   myIP = WiFi.softAPIP();
   Serial.print("AP IP address: ");
   Serial.println(myIP);

   //--Conexion a la red WiFi 
   WiFi.begin(_ssid, _ssid_pass);
   Serial.print("Intentando conexión a WiFi: ");
   Serial.print(_ssid);
   wifi_conn_timeout=0;
   while (WiFi.status() != WL_CONNECTED) {
      wifi_conn_timeout++;
      delay(200);
      Serial.print(".");
      if (wifi_conn_timeout>=MAX_wifi_conn_timeout){
         WiFi.mode(WIFI_AP);  //--Necesario para la configuración cuando no se conecta a wifi
         Serial.print("\r\nTimeout. Falló conexióm WiFi, pasa a modo AP");
         return 0;
         //break;
      }  
   }
   if(WiFi.status()==3){
      Serial.print("\r\nConectado con IP:");
      ip_wlan=WiFi.localIP();
      Serial.println(ip_wlan);
      digitalWrite(_led,HIGH);
      return 1;
   }
   return 0;
}

/**
 * @brief Controla la conexión a wifi. Si la desconexión perdura por un 
 *        tiempo superior a MAX_live_timeout_wifi (definido en wifi.h), 
 *        resetea el microcontrolador.
 * 
 * @return int 
 *        Devuelve 1 si la conexión existe, 0 si no existe.
 */
int MCC_wifi::control(void){
   if (!WiFi.status()==3){
      live_timeout_wifi=millis();
      return 0;
   }
   //Serial.println(live_timeout_wifi - millis());
   //Serial.println(MAX_live_timeout_wifi);
   if (live_timeout_wifi - millis() <= MAX_live_timeout_wifi) {
      Serial.println("########################");
      Serial.println("Reset por WIFI Timeout");
      Serial.println("########################");
      ESP.reset(); 
   }
   return 1;
}

/**
 * @brief Blink del led de wifi según el estado de la conexión
 * 
 */
void MCC_wifi::setLed(void){
    //--Controla el estado de WiFi
    switch(WiFi.status()){
        case 0:
           Serial.println("WL_IDLE_STATUS Codigo:0");
           digitalWrite(_led, LOW);
           //delay(500);
           break;
        case 1:
           Serial.println("WL_NO_SSID_AVAIL Codigo:1");
           blinkLed(1);
           //delay(500);
           break;
        case 2:
           Serial.println("WL_SCAN_COMPLETED Codigo:2");
           blinkLed(2);
           break;
        case 3:
           // Todo bien WL_CONNECTED
           //digitalWrite(led_wifi, HIGH);
           break;
        case 4:
           Serial.println("WL_CONNECT_FAILED Codigo:4");
           blinkLed(4);
           //delay(500);
           break;
        case 5:
           Serial.println("WL_CONNECTION_LOST Codigo:5");
           blinkLed(5);
           //delay(500);
           break;
        case 6:
           Serial.println("WL_DISCONNECTED Codigo:6");
           blinkLed(6);
           //delay(500);
           break;  
        default:
            break;    
    }  
}

/**
 * @brief Función interna para blink led de wifi
 * 
 * @param code 
 */
void MCC_wifi::blinkLed(int code){
    for (int i=0;i<code;i++){
      digitalWrite(_led, HIGH);
      delay(80);
      digitalWrite(_led, LOW);
      delay(250);
    }
    delay(500);
}
