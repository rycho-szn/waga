#ifndef HX711_H
#define HX711_H

class HX711 {
private:
    int PD_SCK;  // Pin PD_SCK
    int DOUT;    // Pin DOUT
    long offset; // Wartość offsetu (tara)
    float scale; // Skala (współczynnik kalibracji)
    int gain;    // Gain (wzmocnienie)

    void pulse();
    bool is_ready();

public:
    HX711(int dout, int pd_sck);

    long read();
    void tare();
    void set_scale(float scale);
    float get_units(int times = 1);
    void set_gain(int gain);
};

#endif // HX711_H
