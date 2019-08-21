#ifndef LCDI2C_H
#define LCDI2C_H

#include "Hal.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define EN 0x04  // Enable bit
#define RW 0x02  // Read/Write bit
#define RS 0x01  // Register select bit

namespace EmbeddedFramework {
class LCDI2C
{
public:
    LCDI2C();
    virtual ~LCDI2C();

    void LCDInit(I2C *i2c_dev, unsigned char lcd_addr, unsigned char lcd_cols, unsigned char lcd_rows);
    void LCDBegin(unsigned char cols, unsigned char rows, unsigned char char_size = LCD_5x8DOTS);
    void LCDClear();
    void LCDHome();
    void LCDOff();
    void LCDOn();
    void LCDBlinkOn();
    void LCDBlinkOff();
    void LCDCursorOn();
    void LCDCursorOff();
    void LCDScrollLeft();
    void LCDScrollRight();
    void LCDPrintLeft();
    void LCDPrintRight();
    void LCDLeftToRight();
    void LCDRightToLeft();
    void LCDShiftIncrement();
    void LCDShiftDecrement();
    void LCDBacklightOn();
    void LCDBacklightOff();
    void LCDAutoscroll();
    void LCDNoAutoscroll();
    void LCDCreateChar(unsigned char location, unsigned char *char_map);
    void LCDSetCursor(unsigned char col, unsigned char row);
    void LCDPrintStr(const char *c);
    void LCDCommand(unsigned char val);
    void LCDWrite(unsigned char val);

private:
    I2C *i2c_device;

    unsigned char addr;
    unsigned char display_function;
    unsigned char display_control;
    unsigned char display_mode;
    unsigned char num_lines;
    unsigned char cols;
    unsigned char rows;
    unsigned char backlight;

private:
    void send(unsigned char value, unsigned char mode);
    void write4Bits(unsigned char val);
    void expanderWrite(unsigned char data);
    void pulseEnable(unsigned char data);
};
}

#endif // LCDI2C_H
