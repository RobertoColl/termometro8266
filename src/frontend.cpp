#include "frontend.h"

//--Locales
String header;

//--Externas
extern ESP8266WebServer web_server;
extern void auth(void);

extern String hversion;
extern String fversion;
extern String device;
extern String mqtt_server;
extern String mqtt_tcp_str;
extern String ubicacion;
extern String ssid;
extern String ssid_pass;
extern String passwd_AP;
  

//------Páginas web-------------------------------------------------------

//--No existe la pagina
void handleNotFound(){
    web_server.send ( 404, "text/html", "El recurso solicitado no existe" );
}

//--Home
void handle_root() {
 const char* Home;
 auth();
 header="<!DOCTYPE html>\
 <html lang='en'>\
  <head>\
   <title>Lámpara ws2812</title>\
    <meta charset='utf-8'>\
    <meta http-equiv='X-UA-Compatible' content='IE=edge'>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link href='/css/bootstrap.min.css.gz' rel='stylesheet'>\
    <script src='/js/jquery.min.js.gz'></script>\
    <script src='/js/bootstrap.min.js.gz'></script>\
    <style>\
     @font-face {\
      font-family: 'Montserrat';\
      font-style: normal;\
      font-weight: 400;\
      src: local('Montserrat Regular'), local('Montserrat-Regular'), url(/fonts/montserrat-font.woff2) format('woff2');\
      unicode-range: U+0000-00FF, U+0131, U+0152-0153, U+02BB-02BC, U+02C6, U+02DA, U+02DC, U+2000-206F, U+2074, U+20AC, U+2122, U+2191, U+2193, U+2212, U+2215, U+FEFF, U+FFFD;\
     }\
     body{\
      font: 18px Montserrat, sans-serif;\
      line-height: 1.8;\
      color: #2f2f2f;\
     }\
     p {font-size: 16px;}\
     table {font-size: 12px;}\
     thead {\
       background-color: #c1c4c6;\
       color: #555555;\
     };\
     .margin {margin-bottom: 45px;}\
     tbody{\
      background-color: #f0f2f4;\
      color: #1e1e1e;\
     }\
     .bg-2 {\
      background-color: #f2f1f0;\
      color: #555555;\
     }\
    </style>\
   </head>\
   <body>\
    <header>\
     <nav class='navbar navbar-inverse navbar-fixed-top' role='navigation'>\
      <div class='container'>\
       <div class='navbar-header'>\
        <button type='button' class='navbar-toggle' data-toggle='collapse' data-target='#navbar'>\
         <span class='icon-bar'></span>\
         <span class='icon-bar'></span>\
         <span class='icon-bar'></span>\
        </button>\
        <a class='navbar-brand' href='#'>Lámpara ws2812</a>\
       </div>\
       <div class='collapse navbar-collapse' id='navbar'>\
        <ul class='nav navbar-nav navbar-right'>\
         <li><a href='#' id='home'>Home</a></li>\
         <li><a href='#' id='config'>Configuración</a></li>\
         <li><a href='#' id='info'>Información</a></li>\
        </ul>\
       </div> \
      </div>\
     </nav>\
    </header>";
 Home="<div id='contenedor'></div>\
      <script>\
        setInterval(function(){\
         refresh();\
        },10000);\
        $(function() {\
         $('.navbar-nav').on('click', function(){\
          if($('.navbar-header .navbar-toggle').css('display') !='none'){\
           $('.navbar-header .navbar-toggle').trigger( 'click' );\
          }\
         });\
        });\
        $(document).ready(function() {\
         $('#home').on('click',function(){\
          $('#contenedor').load('estado.html');\
         });\
         $('#config').on('click',function(){\
          $('#contenedor').load('config.html');\
         });\
         $('#info').on('click',function(){\
          $('#contenedor').load('info.html');\
         });\
         $('#contenedor').load('estado.html');\
         refresh();\
        });\
        function valida(par){\
         switch (par){\
          case 0:\
           $('#contenedor').load('estado.html');\
           break;\
          case 1:   \
           document.forms['actualiza'].submit();\
           break;\
         }\
        }\
        function valida_timers(par){\
         if (par==1){\
          document.forms['actualiza_timers'].submit();\
         }else{\
          $('#contenedor').load('estado.html');\
         }\
        }\
        function refresh(){\
         var xhttp = new XMLHttpRequest();\
         xhttp.onreadystatechange = function(){\
          if (this.readyState == 4 && this.status == 200){\
           state=this.responseText.split(',');\
           if (state[0]=='1'){ document.getElementById('bt1_state').innerHTML='Llamado!';}\
           if (state[0]=='0'){document.getElementById('bt1_state').innerHTML='Inactivo';}\
          }\
         };\
         xhttp.open('GET', 'refresh', true);\
         xhttp.send(); \
        }\
      </script>\
   </body>\
</html>";
  web_server.send ( 200, "text/html",header+Home);
}

//--Pagina de estado
void handle_estado() {
  String estado;
  auth();
  estado="<!DOCTYPE HTML>\
<html>\
 <head>\
  <script>\
   $(document).ready(function() {\
   refresh();\
   });\
   function color_picker(){\
    var xhttp = new XMLHttpRequest();\
    ch=document.getElementById('color_p').value;\
    ch=ch.split('#')[1];\
    console.log(ch);\
    xhttp.open('GET', 'setcolor?color='+ch, true);\
    xhttp.send();\
   }\
   function onchangech(ch){\
    var xhttp = new XMLHttpRequest();\
    xhttp.onreadystatechange = function(){\
     if (this.readyState == 4 && this.status == 200){\
      if(ch=='11'){\
       document.getElementById('calido').innerHTML = 'Encendido!';\
       document.getElementById('natural').innerHTML = 'Natural';\
       document.getElementById('blanco').innerHTML = 'Blanco';\
      }\
     if(ch=='12'){\
       document.getElementById('calido').innerHTML = 'Cálido';\
       document.getElementById('natural').innerHTML = 'Encendido!';\
       document.getElementById('blanco').innerHTML = 'Blanco';\
      }\
     if(ch=='13'){\
       document.getElementById('calido').innerHTML = 'Cálido';\
       document.getElementById('natural').innerHTML = 'Natural';\
       document.getElementById('blanco').innerHTML = 'Encendido!';\
      }\
     if(ch=='10'){\
       document.getElementById('calido').innerHTML = 'Cálido';\
       document.getElementById('natural').innerHTML = 'Natural';\
       document.getElementById('blanco').innerHTML = 'Blanco';\
      }\
     }\
    };\
    xhttp.open('GET', 'setfixcolor?fixcolor='+ch, true);\
    xhttp.send(); \
    }\
  </script>\
 </head>\
 <body><br><br>\
  <div class='panel panel-primary bg-2' id='home'>\
   <div class='panel-heading'>" + device + " " + ubicacion + "</div>\
   <div class='panel-body'>\
    <form>\
     <div class='well'>\
      <button type='button' class='btn btn-warning btn-lg btn-block' name='calido' id='calido' value='1' onclick='onchangech(11)'>Cálido</button>\
     </div>\
     <div class='well'>\
      <button type='button' class='btn btn-info btn-lg btn-block' name='natural' id='natural' value='1' onclick='onchangech(12)'>Natural</button>\
     </div>\
     <div class='well'>\
      <button type='button' class='btn btn-primary btn-lg btn-block' name='blanco' id='blanco' value='1' onclick='onchangech(13)'>Blanco</button>\
     </div>\
     <div class='well'>\
      <button type='button' class='btn btn-danger btn-lg btn-block' name='apaga' id='apaga' value='1' onclick='onchangech(10)'>Apaga</button>\
     </div>\
     <div class='well'>\
      <input type='color' value='#ff0000' id='color_p' class='' onchange='color_picker()'>\
     </div>\
    </form>\
   </div>\
  </div>\
 </body>\
</html>";
  web_server.send ( 200, "text/html",estado);
}

//--Pagina de configuracion
void handle_config() {
  String conf;
  auth();
  conf="<!DOCTYPE html>\
<html lang='en'>\
 <body>\
  <br><br>\
  <div id='config' class='panel panel-primary bg-2'>\
   <div class='panel-heading'>Configuración</div>\
   <div class='panel-body'>\
    <form name='actualiza' action='/update_settings' method='post'>\
     <div class='well'><h2 class='text-center'>Dispositivo</h2>\
      <div class='form-group'>\
       <label for='mod'>Red</label>\
       <input type='text' class='form-control' name='ssid' id='ssid' value='"+ ssid +"'>\
      </div>\
      <div class='form-group'>\
       <label for='mod'>Passwd WiFi</label>\
       <input type='text' class='form-control' name='ssid_pass' id='ssid_pass' value='"+ ssid_pass +"'>\
      </div>\
      <div class='form-group'>\
       <label for='mod'>Passwd AP</label>\
       <input type='text' class='form-control' name='passwd_AP' id='passwd_AP' value='"+ passwd_AP +"'>\
      </div>\
      <div class='form-group'>\
       <label for='dev'>Nombre</label>\
       <input type='text' class='form-control' name='device' id='device' value='" + device + "'>\
      </div>\
      <div class='form-group'>\
       <label for='mqtt_server'>Broker</label>\
       <input type='text' class='form-control' name='mqtt_server' id='mqtt_server' value='" + mqtt_server + "'>\
      </div>\
      <div class='form-group'>\
       <label for='mqtt_server'>Puerto Broker</label>\
       <input type='text' class='form-control' name='mqtt_tcp_str' id='mqtt_tcp_str' value='" + mqtt_tcp_str + "'>\
      </div>\
      <div class='form-group'>\
       <label for='ubic'>Ubicacion</label>\
       <input type='text' class='form-control' name='ubic' id='ubic' value='" + ubicacion + "'>\
      </div>\
     </div>\
     <div class='form-group'>\
      <div class='col-xs-4'>\
       <button type='button' onclick='valida(1)' class='btn btn-success'>\
        Guardar\
       </button>\
      </div>\
     </div>\
    </form>\
   </div>\
  </div>\
 </body>\
</html>";
  web_server.send ( 200, "text/html",conf);
}

//--Pagina de info
void handle_info() {
  String info;
  auth();
  info="<!DOCTYPE html>\
<html lang='en'>\
 <body><br><br>\
  <div id='info' class='panel panel-primary bg-2 text-center'>\
   <div class='panel-heading'>Información</div>\
   <div class='panel-body'>\
    <div class='form-group'>\
     <p>Lámpara con WS2812</p>\
     <p>mcastello ingeniería</p>\
     <p>Versiones: hardware "+hversion+", firmware "+fversion+"</p>\
    </div>\
   </div>\
  </div>\
 </body>\
</html>";
web_server.send ( 200, "text/html",info);
}