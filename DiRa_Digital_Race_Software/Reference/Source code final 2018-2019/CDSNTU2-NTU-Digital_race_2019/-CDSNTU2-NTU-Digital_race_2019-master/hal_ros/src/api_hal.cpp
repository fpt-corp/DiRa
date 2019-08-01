#include "api_hal.h"

ApiHAL::ApiHAL()
{
    gpio = new GPIO();
    i2c_device = new I2C();
    lcd = new LCDI2C();
}

ApiHAL::~ApiHAL()
{
    gpio->gpioUnexport(SW1_PIN);
    gpio->gpioUnexport(SW2_PIN);
    gpio->gpioUnexport(SW3_PIN);
    gpio->gpioUnexport(SW4_PIN);
    gpio->gpioUnexport(SS1_PIN);
    gpio->gpioUnexport(SS2_PIN);
    gpio->gpioUnexport(LED_PIN);
}

void ApiHAL::initPin(unsigned char addr)
{
    gpio->gpioExport(SW1_PIN);
    gpio->gpioExport(SW2_PIN);
    gpio->gpioExport(SW3_PIN);
    gpio->gpioExport(SW4_PIN);
    gpio->gpioExport(SS1_PIN);
    gpio->gpioExport(SS2_PIN);
    gpio->gpioExport(LED_PIN);

    gpio->gpioSetDirection(SW1_PIN, INPUT);
    gpio->gpioSetDirection(SW2_PIN, INPUT);
    gpio->gpioSetDirection(SW3_PIN, INPUT);
    gpio->gpioSetDirection(SW4_PIN, INPUT);
    gpio->gpioSetDirection(SS1_PIN, INPUT);
    gpio->gpioSetDirection(SS2_PIN, INPUT);
    gpio->gpioSetDirection(LED_PIN, OUTPUT);

    i2c_device->m_i2c_bus = 1;

    if (!i2c_device->HALOpen()) {
        printf("Cannot open I2C peripheral\n");
        exit(-1);
    } else printf("I2C peripheral is opened\n");

    unsigned char data;
    if (!i2c_device->HALRead(0x3f, 0xFF, 0, &data, "")) {
        printf("LCD is not found!\n");
        exit(-1);
    } else printf ("LCD is connected\n");

    usleep(10000);

    lcd->LCDInit(i2c_device, addr, 20, 4);
    lcd->LCDBacklightOn();
    lcd->LCDCursorOn();
}

void ApiHAL::setCursor(int x, int y)
{
    lcd->LCDSetCursor(x, y);
}

void ApiHAL::print(const char *c)
{
    lcd->LCDPrintStr(c);
}
int ApiHAL::gpioGetValue(unsigned int pin, unsigned int *value)
{
    gpio->gpioGetValue(pin, value);
}

int ApiHAL::gpioSetValue(unsigned int pin, unsigned int value)
{
    gpio->gpioSetValue(pin, value);
}
