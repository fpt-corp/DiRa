#ifndef HAL_H
#define HAL_H

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include <poll.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef HAL_QUIET
#define HAL_INFO(m) { printf("%s", m); fflush(stdout); }
#define HAL_INFO1(m, x) { printf(m, x); fflush(stdout); }
#define HAL_INFO2(m, x, y) { printf(m, x, y); fflush(stdout); }
#define HAL_INFO3(m, x, y, z) { printf(m, x, y, z); fflush(stdout); }
#define HAL_INFO4(m, x, y, z, a) { printf(m, x, y, z, a); fflush(stdout); }
#define HAL_INFO5(m, x, y, z, a, b) { printf(m, x, y, z, a, b); fflush(stdout); }
#define HAL_ERROR(m)    fprintf(stderr, m);
#define HAL_ERROR1(m, x)    fprintf(stderr, m, x);
#define HAL_ERROR2(m, x, y)    fprintf(stderr, m, x, y);
#define HAL_ERROR3(m, x, y, z)    fprintf(stderr, m, x, y, z);
#define HAL_ERROR4(m, x, y, z, a)    fprintf(stderr, m, x, y, z, a);

#else

#define HAL_INFO(m)
#define HAL_INFO1(m, x)
#define HAL_INFO2(m, x, y)
#define HAL_INFO3(m, x, y, z)
#define HAL_INFO4(m, x, y, z, a)
#define HAL_INFO5(m, x, y, z, a, b)
#define HAL_ERROR(m)
#define HAL_ERROR1(m, x)
#define HAL_ERROR2(m, x, y)
#define HAL_ERROR3(m, x, y, z)
#define HAL_ERROR4(m, x, y, z, a)

#endif

#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

#define MAX_WRITE_LEN 255
#define MAX_READ_LEN 255
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
#define MAX_BUF 64
typedef unsigned int PinDirection ;
typedef unsigned int PinValue ;

enum PinDirections {
    INPUT = 0,
    OUTPUT = 1
};

enum PinValues {
    LOW = 0,
    HIGH = 1,
    OFF = 0,
    ON = 1
};

namespace EmbeddedFramework {
class GPIO {
public:
    GPIO();
    virtual ~GPIO();

    int gpioExport(unsigned int gpio);
    int gpioUnexport(unsigned int gpio);
    int gpioSetDirection(unsigned int gpio, PinDirection out_flag);
    int gpioSetValue( unsigned int gpio, unsigned int value);
    int gpioGetValue( unsigned int gpio, unsigned int *value);
    int gpioSetEdge(unsigned int gpio, char *edge);
    int gpioOpen(unsigned int gpio);
    int gpioClose(int fileDescriptor);
    int gpioActiveLow(unsigned int gpio, unsigned int value);
   // int getFileDescriptor(int &fd);
private:
   // int fd;
};

class Comm {
public:
    Comm();
    virtual ~Comm() = 0;

    virtual bool HALOpen() = 0;
    virtual bool HALClose() = 0;
    virtual bool HALRead(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data, const char *error_msg) = 0;
    virtual bool HALRead(unsigned char slave_addr, unsigned char length, unsigned char *data, const char *error_msg) = 0;
    virtual bool HALWrite(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char const *data, const char *error_msg) = 0;
    virtual bool HALWrite(unsigned char slave_addr, unsigned char reg_addr, unsigned char const data, const char *error_msg) = 0;

    void delayMs(int milliseconds);

protected:
    virtual bool ifWrite(unsigned char *data, unsigned char length) = 0;
};

class I2C: public Comm
{
public:
    I2C();
    virtual ~I2C();

    bool HALOpen();
    bool HALClose();
    bool HALRead(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data, const char *error_msg);
    bool HALRead(unsigned char slave_addr, unsigned char length, unsigned char *data, const char *error_msg);
    bool HALWrite(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char const *data, const char *error_msg);
    bool HALWrite(unsigned char slave_addr, unsigned char reg_addr, unsigned char const data, const char *error_msg);

    bool selectSlave(unsigned char slave_addr, const char *error_msg);

    int m_i2c_bus; // I2C bus of the imu (eg 1 for Raspberry Pi usually)
private:
    int m_i2c;
    unsigned char m_current_slave;
protected:
    bool ifWrite(unsigned char *data, unsigned char length);
};

class SPI: public Comm {
public:
    SPI();
    virtual ~SPI();

    bool HALOpen();
    bool HALClose();
    bool HALRead(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data, const char *error_msg);
    bool HALRead(unsigned char slave_addr, unsigned char length, unsigned char *data, const char *error_msg);
    bool HALWrite(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char const *data, const char *error_msg);
    bool HALWrite(unsigned char slave_addr, unsigned char reg_addr, unsigned char const data, const char *error_msg);

private:
    int m_spi;
public:
    unsigned char m_spi_bus;
    unsigned char m_spi_select;
    unsigned int m_spi_speed;

protected:
    bool ifWrite(unsigned char *data, unsigned char length);
};
}

#endif // HAL_H
