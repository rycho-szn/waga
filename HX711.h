#ifndef HX711_H
#define HX711_H

#include <wiringPi.h>

class HX711 {
public:
    HX711(int dout, int pd_sck);
    long read();
    bool is_ready();
    void tare();
    void set_scale(float scale);
    float get_units(int times = 10);

private:
    int PD_SCK;
    int DOUT;
    long offset;
    float scale;

    void pulse();
};

#endif
