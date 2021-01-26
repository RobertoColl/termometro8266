#include "ws2812.h"

//--Instancias externas
extern Adafruit_NeoPixel pixels;

/**
 * @brief inicializa la clase. Crea el objeto que proviene de la clase Adafruit_NeoPixel
 * 
 */
void MCC_ws2812::init(){
    pixels.begin();
}

/**
 * @brief 
 * 
 */
void MCC_ws2812::act(void){
    pixels.clear(); // Set all pixel colors to 'off'
    delay(1000);
    for(int i=0; i<=32; i++) { // For each pixel...
        Serial.println(i);
        pixels.setPixelColor(i, pixels.Color(100, 35, 205));
    }
    pixels.show();
    delay(500);
    for(int i=32; i<=70; i++) {
        Serial.println(i); // For each pixel...
        pixels.setPixelColor(i, pixels.Color(10, 150, 100));
    }
    pixels.show();
    delay(500);
    for(int i=71; i<103; i++) {
        Serial.println(i); // For each pixel...
        pixels.setPixelColor(i, pixels.Color(50, 100, 150));
    }
    pixels.show();   // Send the updated pixel colors to the hardware.
}

/**
 * @brief cambia los colores de los pixeles desde, hasta con el color pasado en los argumentos
 * 
 * @param r 
 * @param g 
 * @param b 
 * @param desde 
 * @param hasta 
 */
void MCC_ws2812::oneColor(int r, int g, int b, int desde, int hasta){
    this->_desde=desde;
    this->_hasta=hasta;
    for(int i=_desde; i<_hasta ; i++) {
        pixels.setPixelColor(i, pixels.Color(r, g, b));
    }
    pixels.show();
}
void MCC_ws2812::off(){
    pixels.clear();
    pixels.show();
}

