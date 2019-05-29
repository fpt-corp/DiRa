#ifndef APIHAL_H
#define APIHAL_H

#include "Hal.h"
#include "LCDI2C.h"

#define SW1_PIN 395
#define SW2_PIN	394
#define SW3_PIN	393
#define SW4_PIN 392

#define SS1_PIN 398
#define SS2_PIN 396
#define LED_PIN 481
using namespace EmbeddedFramework;

class ApiHAL {

public:
    ApiHAL();
    ~ApiHAL();
    void initPin(unsigned char addr);

    void setCursor(int x, int y);
    void print(const char *c);

    int gpioGetValue(unsigned int gpio, unsigned int *value);
    int gpioSetValue(unsigned int gpio, unsigned int value);

private:
    GPIO *gpio;
    I2C *i2c_device;
    LCDI2C *lcd;
};

#endif
