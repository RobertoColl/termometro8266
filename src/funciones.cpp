#include "funciones.h"

extern String device;
extern String mqtt_server;
extern uint16_t mqtt_tcp;
extern String mqtt_tcp_str;
extern String passwd_AP;
extern String ssid;
extern String ssid_pass;
extern String ubicacion;
extern String tipo_device;


//--Muestra los archivos en el FS
void ls(void){
   Dir dir = LittleFS.openDir("/");
   while (dir.next()) {
      Serial.print(dir.fileName());
      Serial.print(" ");
      if(dir.fileSize()) {
         File f = dir.openFile("r");
         Serial.println(f.size());
      }
   }
}

void bienvenida(int led1,int led2,int led3){
  for(int i=0;i<5;i++){
    digitalWrite(led1,HIGH);
    delay(50);
    digitalWrite(led1,LOW);
    delay(50);
    digitalWrite(led2,HIGH);
    delay(50);
    digitalWrite(led2,LOW);
    delay(50);
    digitalWrite(led3,HIGH);
    delay(50);
    digitalWrite(led3,LOW);
    delay(50);
  }
}
void factory_reset(void){
  write_StringEE(0, device);
  write_StringEE(30, mqtt_server);
  write_StringEE(60, mqtt_tcp_str);
  write_StringEE(90, passwd_AP);
  write_StringEE(120, ssid);
  write_StringEE(150, ssid_pass);
  write_StringEE(180, ubicacion);
  noInterrupts();
  EEPROM.commit();
  interrupts();
}

void read_vars(void){
  device=read_StringEE(0,25);
  mqtt_server=read_StringEE(30,25);
  mqtt_tcp_str=read_StringEE(60,25);
  passwd_AP=read_StringEE(90,25);
  ssid=read_StringEE(120,25); 
  ssid_pass=read_StringEE(150,25); 
  ubicacion=read_StringEE(180,25); 
  mqtt_tcp=mqtt_tcp_str.toInt();
  Serial.println("");
  Serial.print("Nombre:");Serial.println(device);
  Serial.print("Broker:");Serial.println(mqtt_server);
  Serial.print("Puerto mqtt:");Serial.println(mqtt_tcp);
  Serial.print("Passwd AP:");Serial.println(passwd_AP);
  Serial.print("SSID:");Serial.println(ssid);
  Serial.print("Passwd SSID:");Serial.println(ssid_pass);
  Serial.print("Ubicacion:");Serial.println(ubicacion);
}

unsigned int hexToDec(String hexString) {
  
  unsigned int decValue = 0;
  int nextInt;
  
  for (int i = 0; i < hexString.length(); i++) {
    
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    
    decValue = (decValue * 16) + nextInt;
  }
  
  return decValue;
}


void check_update(void){
  String flag_update=read_StringEE(500,1);
  String version=read_StringEE(470,25);
  Serial.print("Se actualizará a la version:");
  Serial.println(version);
  if(flag_update=="1"){
    write_StringEE(500, "0");
    noInterrupts();
    EEPROM.commit();
    interrupts();
    Serial.println("Actualizando.....");
    t_httpUpdate_return ret=ESPhttpUpdate.update("http://192.168.1.17/updates/"+tipo_device+"/"+version+"/firmware.bin");
    switch(ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("[update] Update FAILED (%d): %s\r\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;
      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[update] Update no Update.");
        break;
      case HTTP_UPDATE_OK:
        Serial.println("[update] Update ok."); // may not called we reboot the ESP
        break;
    }
  }
}