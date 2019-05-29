#include "LCDI2C.h"
using namespace EmbeddedFramework;
LCDI2C::LCDI2C()
{

}

LCDI2C::~LCDI2C() {

}

void LCDI2C::LCDInit(I2C *i2c_dev, unsigned char lcd_addr, unsigned char lcd_cols, unsigned char lcd_rows) {
    addr = lcd_addr;
    cols = lcd_cols;
    rows = lcd_rows;
    backlight = LCD_BACKLIGHT;
    i2c_device = i2c_dev;
    this->LCDBegin(lcd_cols, lcd_rows);
}

void LCDI2C::LCDBegin(unsigned char cols, unsigned char rows, unsigned char char_size) {
    display_function = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
    num_lines = rows;
    usleep(50000);
    this->expanderWrite(backlight);
    usleep(1000000);

    this->write4Bits(0x03 << 4);
    usleep(4500);
    this->write4Bits(0x03 << 4);
    usleep(4500);
    this->write4Bits(0x03 << 4);
    usleep(150);

    this->write4Bits(0x02 << 4);

    this->LCDCommand(LCD_FUNCTIONSET | display_function);

    display_control = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    this->LCDOn();

    this->LCDClear();

    display_mode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

    this->LCDCommand(LCD_ENTRYMODESET | display_mode);

    this->LCDHome();
}

void LCDI2C::LCDClear() {
    this->LCDCommand(LCD_CLEARDISPLAY);
    usleep(2000);
}

void LCDI2C::LCDHome() {
    this->LCDCommand(LCD_RETURNHOME);
    usleep(2000);
}

void LCDI2C::LCDOff() {
    display_control &= ~LCD_DISPLAYON;
    this->LCDCommand(LCD_DISPLAYCONTROL | display_control);
}

void LCDI2C::LCDOn() {
    display_control |= LCD_DISPLAYON;
    this->LCDCommand(LCD_DISPLAYCONTROL | display_control);
}

void LCDI2C::LCDBlinkOn() {
    display_control |= LCD_BLINKON;
    this->LCDCommand(LCD_DISPLAYCONTROL | display_control);
}

void LCDI2C::LCDBlinkOff() {
    display_control &= ~LCD_BLINKON;
    this->LCDCommand(LCD_DISPLAYCONTROL | display_control);
}

void LCDI2C::LCDCursorOn() {
    display_control |= LCD_CURSORON;
    this->LCDCommand(LCD_DISPLAYCONTROL | display_control);
}

void LCDI2C::LCDCursorOff() {
    display_control &= ~LCD_CURSORON;
    this->LCDCommand(LCD_DISPLAYCONTROL | display_control);
}

void LCDI2C::LCDScrollLeft() {
    this->LCDCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LCDI2C::LCDScrollRight() {
    this->LCDCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void LCDI2C::LCDPrintLeft() {

}

void LCDI2C::LCDPrintRight() {

}

void LCDI2C::LCDLeftToRight() {
    display_mode |= LCD_ENTRYLEFT;
    this->LCDCommand(LCD_ENTRYMODESET | display_mode);
}

void LCDI2C::LCDRightToLeft() {
    display_mode &= ~LCD_ENTRYLEFT;
    this->LCDCommand(LCD_ENTRYMODESET | display_mode);
}

void LCDI2C::LCDShiftIncrement() {

}

void LCDI2C::LCDShiftDecrement() {

}

void LCDI2C::LCDBacklightOn() {
    backlight = LCD_BACKLIGHT;
    this->expanderWrite(0);
}

void LCDI2C::LCDBacklightOff() {
    backlight= LCD_NOBACKLIGHT;
    this->expanderWrite(0);
}

void LCDI2C::LCDAutoscroll() {
    display_mode |= LCD_ENTRYSHIFTINCREMENT;
    this->LCDCommand(LCD_ENTRYMODESET | display_mode);
}

void LCDI2C::LCDNoAutoscroll() {
    display_mode &= ~LCD_ENTRYSHIFTINCREMENT;
    this->LCDCommand(LCD_ENTRYMODESET | display_mode);
}

void LCDI2C::LCDCreateChar(unsigned char location, unsigned char *char_map) {
    location &= 0x07;
    this->LCDCommand(LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; i++) {
        (char_map[i]);
    }
}

void LCDI2C::LCDSetCursor(unsigned char col, unsigned char row) {
    int row_offset[] = { 0x00, 0x40, 0x14, 0x54 };
    if (row > num_lines) {
        row = num_lines - 1;
    }
    this->LCDCommand(LCD_SETDDRAMADDR |(col + row_offset[row]));
}

void LCDI2C::LCDPrintStr(const char *c) {
    while (*c) {
        this->LCDWrite(*c);
        c++;
    }
}

void LCDI2C::LCDCommand(unsigned char val) {
    this->send(val, 0);
}

void LCDI2C::LCDWrite(unsigned char val) {
    send(val, RS);
}

void LCDI2C::send(unsigned char value, unsigned char mode) {
    unsigned char h = value & 0xf0;
    unsigned char l = (value << 4) & 0xf0;
    this->write4Bits((h) | mode);
    this->write4Bits((l) | mode);
}

void LCDI2C::write4Bits(unsigned char val) {
    this->expanderWrite(val);
    this->pulseEnable(val);
}

void LCDI2C::expanderWrite(unsigned char data) {
    i2c_device->HALWrite(addr, 0x00, data | backlight, "");
}

void LCDI2C::pulseEnable(unsigned char data) {
    this->expanderWrite(data | EN);
    usleep(2);

    this->expanderWrite(data & ~EN);
    usleep(70);
}
