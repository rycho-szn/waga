#include "HX711.h"
#include <iostream>
#include <numeric>

HX711::HX711(int dout, int pd_sck) : offset(0), scale(1.0) {
    PD_SCK = pd_sck;
    DOUT = dout;

    wiringPiSetupGpio(); // Użycie numeracji GPIO
    pinMode(PD_SCK, OUTPUT);
    pinMode(DOUT, INPUT);
}

bool HX711::is_ready() {
    return digitalRead(DOUT) == LOW;
}

void HX711::pulse() {
    digitalWrite(PD_SCK, HIGH);
    delayMicroseconds(1);
    digitalWrite(PD_SCK, LOW);
    delayMicroseconds(1);
}

long HX711::read() {
    while (!is_ready()) {
        // Czekaj, aż HX711 będzie gotowy
    }

    unsigned long count = 0;
    for (int i = 0; i < 24; ++i) {
        pulse();
        count = count << 1;
        if (digitalRead(DOUT) == HIGH) {
            count++;
        }
    }

    // Puls dodatkowy
    pulse();
    count ^= 0x800000; // Konwersja 2-komplementu

    return static_cast<long>(count);
}

void HX711::tare() {
    long sum = 0;
    for (int i = 0; i < 10; ++i) {
        sum += read();
    }
    offset = sum / 10;
}

void HX711::set_scale(float scale) {
    this->scale = scale;
}

float HX711::get_units(int times) {
    long sum = 0;
    for (int i = 0; i < times; ++i) {
        sum += read();
    }
    return (sum / times - offset) / scale;
}
