#include "backend.h"


//--Objetos externos
extern ESP8266WebServer web_server;

//--Variables externas
extern String passwd_AP;
extern String www_username;
extern String device;
extern String mqtt_server;
extern String mqtt_tcp_str;
extern String ubicacion;
extern String area;
extern String ssid;
extern String ssid_pass;
extern String header;
extern uint8_t canal1_status;
extern uint8_t canal2_status;
extern uint8_t flag_push_att;
extern float tmax;
extern float tmin; 
extern float offset;
extern float gain;
extern uint8_t sensor;
extern float temperatura;
extern float tempinst;

//--Variables locales
//int trys_load_files=3;   //Cantidad de intentos de carga de archivos pedidos por el browser
File file_funciones,file_bs_css,file_bs_js,file_jq,file_font,file_estilo;


void init_webserver(void){
  //--Configura los recursos web del server
  //--Archivos
  web_server.on("/css/bootstrap.min.css.gz", handle_bootstrap_css);
  web_server.on("/js/bootstrap.min.js.gz", handle_bootstrap_js);
  web_server.on("/js/jquery.min.js.gz", handle_jquery);
  web_server.on("/fonts/montserrat-font.woff2", handle_font);
  web_server.on("/js/funciones.js", handle_funciones);
  web_server.on("/css/miestilo.css", handle_estilo);
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
  web_server.on("/canal",HTTP_GET,handle_canal);
  //--Inicia webserver
  web_server.begin();
}


void auth(void){
  if (!web_server.authenticate(www_username.c_str(), passwd_AP.c_str())){
    return web_server.requestAuthentication();
  }
}

void handleUpdateSettings(){
  String body;
  device=web_server.arg("device");
  mqtt_server=web_server.arg("mqtt_server");
  mqtt_tcp_str=web_server.arg("mqtt_tcp_str");
  passwd_AP=web_server.arg("passwd_AP");
  ssid=web_server.arg("ssid");
  ssid_pass=web_server.arg("ssid_pass");
  ubicacion=web_server.arg("ubic");
  area=web_server.arg("area");
  tmax=(web_server.arg("tmax")).toFloat();
  tmin=(web_server.arg("tmin")).toFloat();
  offset=(web_server.arg("offset")).toFloat();
  gain=(web_server.arg("gain")).toFloat();
  sensor=(web_server.arg("sensor")).toInt();

  write_vars();
  flag_push_att=1;

  //--Envia OK
   body="<body>\
     <div class='panel panel-primary bg-2'>\
      <div class='panel-heading'>Configuraci??n</div>\
      <div class='panel-body'>\
      <form action='/'>\
       <div id='config' class='panel panel-primary bg-2 text-center'>\
        <div class='panel-heading'>Informaci??n</div>\
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
}

void handle_refresh(){
  String estados;
  auth();
  estados=String(canal1_status)+","+String(canal2_status)+","+String(tempinst)+","+String(temperatura);
  web_server.send(200,"text/html",estados);
}

void handle_canal(void){
   int accion=web_server.arg("accion").toInt();
   switch (accion){
      case 11:
         canal1_on();
         break;
      case 10:
         canal1_off();
         break;
      case 21:
         canal2_on();
         break;
      case 20:
         canal2_off();
         break;
   }
   //--Env??a respuesta al cliente
   web_server.send ( 200, "text/html", "OK" );
}

//-- Archivos servidos

void handle_funciones(void){
  for (int idx=1;idx<=TRYS_LOAD_FILES;idx++){
     file_funciones = LittleFS.open("/js/funciones.js", "r");
     if (!file_funciones) {
        Serial.print("Fallo carga funciones! ");
        Serial.println(idx);
        delay(200);
     }
     else{
        web_server.streamFile(file_funciones, "text/javascript");
        return;
     }
  } 
}

void handle_estilo(void){
  for (int idx=1;idx<=TRYS_LOAD_FILES;idx++){
     file_estilo = LittleFS.open("/css/miestilo.css", "r");
     if (!file_estilo) {
        Serial.print("Fallo carga estilo CSS! ");
        Serial.println(idx);
        delay(200);
     }
     else{
        web_server.streamFile(file_estilo, "text/css");
        return;
     }
  } 
}

void handle_bootstrap_css(void){
  for (int idx=1;idx<=TRYS_LOAD_FILES;idx++){
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
  for (int idx=1;idx<=TRYS_LOAD_FILES;idx++){
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
  for (int idx=1;idx<=TRYS_LOAD_FILES;idx++){
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
  for (int idx=1;idx<=TRYS_LOAD_FILES;idx++){
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
  for (int idx=1;idx<=TRYS_LOAD_FILES;idx++){
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