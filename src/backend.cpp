#include "backend.h"


//--Externas
extern ESP8266WebServer web_server;
extern File file_bs_css,file_bs_js,file_jq,file_font;


extern String passwd_AP;
extern String www_username;
//extern String hversion;
//extern String fversion;
extern String device;
extern String mqtt_server;
extern String mqtt_tcp_str;
extern String ubicacion;
extern String area;
extern String ssid;
extern String ssid_pass;
extern String header;
/*extern uint32_t device_eeprom_pos;
extern uint32_t mqtt_server_eeprom_pos;
extern uint32_t mqtt_tcp_str_eeprom_pos;
extern uint32_t passwd_AP_eeprom_pos;
extern uint32_t ssid_eeprom_pos;
extern uint32_t ssid_pass_eeprom_pos;
extern uint32_t ubicacion_eeprom_pos;
extern uint32_t fuota_eeprom_pos;
extern uint32_t area_eeprom_pos;*/

int trys_load_files=3;   //Cantidad de intentos de carga de archivos pedidos por el browser
File file_bs_css,file_bs_js,file_jq,file_font;

void init_webserver(void){
  //--Configura los recursos web del server
  //--Archivos
  web_server.on("/css/bootstrap.min.css.gz", handle_bootstrap_css);
  web_server.on("/js/bootstrap.min.js.gz", handle_bootstrap_js);
  web_server.on("/js/jquery.min.js.gz", handle_jquery);
  web_server.on("/fonts/montserrat-font.woff2", handle_font);
  //web_server.on("/fonts/glyphicons-halflings-regular.woff2", handle_glicons);
  //--Paginas web
  web_server.on ( "/", handle_root);
  web_server.on ( "/estado.html", handle_estado);
  web_server.on ( "/config.html", handle_config);
  web_server.on ( "/info.html", handle_info);
  web_server.onNotFound (handleNotFound);
  //--AJAX
  //web_server.on ("/setfixcolor",HTTP_GET, handle_setfixcolor);
  //web_server.on ("/setcolor",HTTP_GET,handle_setcolor);
  web_server.on ("/refresh",HTTP_GET, handle_refresh);
  web_server.on ("/update_settings", HTTP_POST, handleUpdateSettings);
  //--Inicia webserver
  web_server.begin();
}


void auth(void){
  if (!web_server.authenticate(www_username.c_str(), passwd_AP.c_str())){
    return web_server.requestAuthentication();
  }
}

void handleUpdateSettings(){
  //bool res;
  String body;
  //String rq_reset=web_server.arg("reset");
  //Serial.print(rq_reset);
  
  device=web_server.arg("device");
  mqtt_server=web_server.arg("mqtt_server");
  mqtt_tcp_str=web_server.arg("mqtt_tcp_str");
  passwd_AP=web_server.arg("passwd_AP");
  ssid=web_server.arg("ssid");
  ssid_pass=web_server.arg("ssid_pass");
  ubicacion=web_server.arg("ubic");
  area=web_server.arg("area");

  write_vars();


/*
  res = write_StringEE(device_eeprom_pos, web_server.arg("device"));
  check_error_updating(res);
  device=read_StringEE(device_eeprom_pos,25);

  res = write_StringEE(mqtt_server_eeprom_pos, web_server.arg("mqtt_server"));
  check_error_updating(res);
  mqtt_server=read_StringEE(mqtt_server_eeprom_pos,25);

  res = write_StringEE(mqtt_tcp_str_eeprom_pos, web_server.arg("mqtt_tcp_str"));
  check_error_updating(res);
  mqtt_tcp_str=read_StringEE(mqtt_tcp_str_eeprom_pos,25); 

  res = write_StringEE(passwd_AP_eeprom_pos, web_server.arg("passwd_AP"));
  check_error_updating(res);
  passwd_AP=read_StringEE(passwd_AP_eeprom_pos,25); 

  res = write_StringEE(ssid_eeprom_pos, web_server.arg("ssid"));
  check_error_updating(res);
  ssid=read_StringEE(ssid_eeprom_pos,25); 

  res = write_StringEE(ssid_pass_eeprom_pos, web_server.arg("ssid_pass"));
  check_error_updating(res);
  ssid_pass=read_StringEE(ssid_pass_eeprom_pos,25); 

  res = write_StringEE(ubicacion_eeprom_pos, web_server.arg("ubic"));
  check_error_updating(res);
  ubicacion=read_StringEE(ubicacion_eeprom_pos,25);  

  res = write_StringEE(area_eeprom_pos, web_server.arg("area"));
  check_error_updating(res);
  area=read_StringEE(area_eeprom_pos,25); 

  noInterrupts();
  EEPROM.commit();
  interrupts();*/
  //Envia OK sin reset
    body="<body>\
     <div class='panel panel-primary bg-2'>\
      <div class='panel-heading'>Configuración</div>\
      <div class='panel-body'>\
      <form action='/'>\
       <div id='config' class='panel panel-primary bg-2 text-center'>\
        <div class='panel-heading'>Información</div>\
        <div class='panel-body'>\
         <div class='form-group'>\
          <h3>Grabado!!</h3>\
          <p></p>\
          <button type='submit' class='btn btn-primary center-block'>Volver</button>\
         </div>\
        </div>\
       </div>\
      </form>\
     </div>\
    </div>\
   </body>\
  </html>";
   web_server.send ( 200, "text/html", header+body);
  //}
}

void handle_refresh(){
  String estados;
  auth();
  //estados+="</td></tr>";
  //estados=String(flag_panico1)+","+String(flag_panico2);
  
  web_server.send(200,"text/html",estados);
}

//-- Archivos servidos
void handle_bootstrap_css(void){
  for (int idx=1;idx<=trys_load_files;idx++){
     file_bs_css = LittleFS.open("/css/bootstrap.min.css.gz", "r");
     if (!file_bs_css) {
        Serial.print("Fallo carga Booststrap CSS! ");
        Serial.println(idx);
        delay(200);
     }
     else{
        web_server.streamFile(file_bs_css, "text/css");
        return;
     }
  } 
}

void handle_bootstrap_js(void){
  for (int idx=1;idx<=trys_load_files;idx++){
     file_bs_js = LittleFS.open("/js/bootstrap.min.js.gz", "r");
     if (!file_bs_js) {
        Serial.print("Fallo carga Booststrap JS! ");
        Serial.println(idx);
        delay(200);
     }
     else{
        web_server.streamFile(file_bs_js, "application/javascript");
        return;
     }
  }
}

void handle_jquery(void){
  for (int idx=1;idx<=trys_load_files;idx++){
     file_jq = LittleFS.open("/js/jquery.min.js.gz", "r");
     if (!file_jq) {
        Serial.print("Fallo carga jquery! ");
        Serial.println(idx);
        delay(200);
     }
     else{
      web_server.streamFile(file_jq, "application/javascript");
      return;
     }
  } 
}
/*
void handle_glicons(){
  for (int idx=1;idx<=trys_load_files;idx++){
     file_gli = SPIFFS.open("/fonts/glyphicons.woff2", "r");
     if (!file_gli) {
        Serial.print("Fallo carga de los iconos! ");
        Serial.println(idx);        
        delay(200);
     }
     else{
        size_t sent = web_server.streamFile(file_gli, "application/octet-stream");
        return;
     }
  } 
}*/

void handle_font(void){
  for (int idx=1;idx<=trys_load_files;idx++){
     file_font = LittleFS.open("/fonts/montserrat.woff2", "r");
     if (!file_font) {
        Serial.print("Fallo carga de la fuente! ");
        Serial.println(idx);        
        delay(200);
     }
     else{
        web_server.streamFile(file_font, "application/octet-stream");
        return;
     }
  } 
}