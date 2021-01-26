#ifndef _ws2812_h_
#define _ws2812_h_

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class MCC_ws2812{
    private:
        int _desde;
        int _hasta;
    public:
        void act(void);
        void oneColor(int red, int green, int blue, int desde, int hasta);
        void off();
        void init();
};

#endif