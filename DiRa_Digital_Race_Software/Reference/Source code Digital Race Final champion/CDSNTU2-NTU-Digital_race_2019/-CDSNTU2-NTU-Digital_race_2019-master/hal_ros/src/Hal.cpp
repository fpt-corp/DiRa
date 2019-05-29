#include "Hal.h"

using namespace EmbeddedFramework;
//----GPIO----------------------------------------------------------------------------------------------------//
GPIO::GPIO() {

}

GPIO::~GPIO() {

}

/**
 * @brief GPIO::gpioExport : Export the given gpio to userspace;
 * @param fd =  0 ; otherwise open file error
 * @param gpio
 */
int GPIO::gpioExport(unsigned int gpio) {
    int length, fileDescriptor;
    char cmd_buff[MAX_BUF];
    fileDescriptor = open(SYSFS_GPIO_DIR"/export", O_WRONLY);
    if (fileDescriptor < 0) {
        char error_buff[128];
        snprintf(error_buff, sizeof(error_buff), "gpioExport unable to open gpio%d", gpio) ;
        perror(error_buff);
        return fileDescriptor;
    }

    length = snprintf(cmd_buff, sizeof(cmd_buff), "%d", gpio);
    if (write(fileDescriptor, cmd_buff, length) != length) {
        perror("gpioExport");
        return fileDescriptor;

    }
    close(fileDescriptor);
	return 0;
    //fd = 0;
}

/**
 * @brief GPIO::gpioUnexport
 * @details Unexport the given gpio from userspace
 * @param fd = 0 ; otherwise open file error
 * @param gpio
 */
int GPIO::gpioUnexport(unsigned int gpio) {
    int length, fileDescriptor;
    char cmd_buff[MAX_BUF];

    fileDescriptor = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
    if (fileDescriptor < 0) {
        char error_buff[128] ;
        snprintf(error_buff, sizeof(error_buff), "gpioUnexport unable to open gpio%d", gpio) ;
        perror(error_buff);
        return fileDescriptor;
    }

    length = snprintf(cmd_buff, sizeof(cmd_buff), "%d", gpio);
    if (write(fileDescriptor, cmd_buff, length) != length) {
        perror("gpioUnexport") ;
        return fileDescriptor;
    }
    close(fileDescriptor);
	return 0;
    //fd = 0;
}

/**
 * @brief GPIO::gpioSetDirection
 * @details Set the direction of the GPIO pin
 * @param fd = 0 ; otherwise open file error
 * @param gpio
 * @param out_flag
 */
int GPIO::gpioSetDirection(unsigned int gpio, PinDirection out_flag) {
    char cmd_buff[MAX_BUF];
    int fileDescriptor;
    snprintf(cmd_buff, sizeof(cmd_buff), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);

    fileDescriptor = open(cmd_buff, O_WRONLY);
    if (fileDescriptor < 0) {
        char error_buff[128] ;
        snprintf(error_buff, sizeof(error_buff), "gpioSetDirection unable to open gpio%d", gpio) ;
        perror(error_buff);
        return fileDescriptor;
    }

    if (out_flag) {
        if (write(fileDescriptor, "out", 4) != 4) {
            perror("gpioSetDirection") ;
            return fileDescriptor;
        }
    } else {
        if (write(fileDescriptor, "in", 3) != 3) {
            perror("gpioSetDirection") ;
            return fileDescriptor;
        }
    }
    close(fileDescriptor);
	return 0;
   // fd = 0;
}

/**
 * @brief GPIO::gpioSetValue
 * @details Set the value of the GPIO pin to 1 or 0
 * @param fd = 0 ; otherwise open file error
 * @param gpio
 */
int GPIO::gpioSetValue(unsigned int gpio, unsigned int value) {
    char cmd_buff[MAX_BUF];
	int fileDescriptor;
    snprintf(cmd_buff, sizeof(cmd_buff), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fileDescriptor = open(cmd_buff, O_WRONLY);
    if (fileDescriptor < 0) {
        char error_buff[128] ;
        snprintf(error_buff, sizeof(error_buff), "gpioSetValue unable to open gpio%d",gpio) ;
        perror(error_buff);
        return fileDescriptor;
    }

    if (value) {
        if (write(fileDescriptor, "1", 2) != 2) {
            perror("gpioSetValue") ;
            return fileDescriptor;
        }
    } else {
        if (write(fileDescriptor, "0", 2) != 2) {
            perror("gpioSetValue") ;
            return fileDescriptor;
        }
    }
    close(fileDescriptor);
    return 0;
}

/**
 * @brief GPIO::gpioGetValue
 * @details Get the value of the requested GPIO pin
 * @param fd = 0 ; otherwise open file error
 * @param gpio
 * @param value return is 0 or 1
 */
int GPIO::gpioGetValue(unsigned int gpio, unsigned int *value) {
    char cmd_buff[MAX_BUF];
    char ch;
	int fileDescriptor;
    snprintf(cmd_buff, sizeof(cmd_buff), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fileDescriptor = open(cmd_buff, O_RDONLY);
    if (fileDescriptor < 0) {
        char error_buff[128] ;
        snprintf(error_buff, sizeof(error_buff), "gpioGetValue unable to open gpio%d", gpio) ;
        perror(error_buff);
        return fileDescriptor;
    }

    if (read(fileDescriptor, &ch, 1) != 1) {
        perror("gpioGetValue") ;
        return fileDescriptor;
     }

    if (ch != '0') {
        *value = 1;
    } else {
        *value = 0;
    }

    close(fileDescriptor);
    return 0;
}
/**
 * @brief GPIO::gpioSetEdge
 * @details Set the edge of the GPIO pin
 * @param fd = 0 ; otherwise open file error
 * @param gpio
 * @param edge : Valid edges: 'none' 'rising' 'falling' 'both'
 */
int GPIO::gpioSetEdge(unsigned int gpio, char *edge) {
    char cmd_buff[MAX_BUF];
	int fileDescriptor;
    snprintf(cmd_buff, sizeof(cmd_buff), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);

    fileDescriptor = open(cmd_buff, O_WRONLY);
    if (fileDescriptor < 0) {
        char error_buff[128] ;
        snprintf(error_buff,sizeof(error_buff), "gpioSetEdge unable to open gpio%d",gpio) ;
        perror(error_buff);
        return fileDescriptor;
    }

    if (write(fileDescriptor, edge, strlen(edge) + 1) != ((int)(strlen(edge) + 1))) {
        perror("gpioSetEdge") ;
        return fileDescriptor;
    }
    close(fileDescriptor);
    return 0;
}

/**
 * @brief GPIO::gpioOpen
 * @details Open the given pin for reading
 * @param fd : the file descriptor of the named pin
 * @param gpio
 */
int GPIO::gpioOpen( unsigned int gpio) {
    char cmd_buff[MAX_BUF];
	int fileDescriptor;
    snprintf(cmd_buff, sizeof(cmd_buff), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fileDescriptor = open(cmd_buff, O_RDONLY | O_NONBLOCK );
    if (fileDescriptor < 0) {
        char error_buff[128] ;
        snprintf(error_buff,sizeof(error_buff), "gpioOpen unable to open gpio%d", gpio) ;
        perror(error_buff);
    }
    return fileDescriptor;
}

/**
 * @brief GPIO::gpioClose
 * @details Close the given file descriptor
 * @param res
 * @param fd
 */
int GPIO::gpioClose(int fileDescriptor) {
    return close(fileDescriptor);
}

int GPIO::gpioActiveLow(unsigned int gpio, unsigned int value) {
    char cmd_buff[MAX_BUF];
	int fileDescriptor;
    snprintf(cmd_buff, sizeof(cmd_buff), SYSFS_GPIO_DIR "/gpio%d/active_low", gpio);

    fileDescriptor = open(cmd_buff, O_WRONLY);
    if (fileDescriptor < 0) {
        char error_buff[128] ;
        snprintf(error_buff, sizeof(error_buff), "gpioActiveLow unable to open gpio%d", gpio) ;
        perror(error_buff);
        return fileDescriptor;
    }

    if (value) {
        if (write(fileDescriptor, "1", 2) != 2) {
            perror("gpioActiveLow") ;
            return fileDescriptor;
        }
    }
    else {
        if (write(fileDescriptor, "0", 2) != 2) {
            perror("gpioActiveLow") ;
            return fileDescriptor;
        }
    }
    close(fileDescriptor);
    return 0;
}

//----Comm----------------------------------------------------------------------------------------------------//
Comm::Comm() {

}

Comm::~Comm() {

}

void Comm::delayMs(int milliseconds) {

}

//----I2C-----------------------------------------------------------------------------------------------------//
I2C::I2C() : m_i2c(-1), m_current_slave(255) {

}

I2C::~I2C() {
    this->HALClose();
}

/**
 * @brief I2C::HALOpen
 * @details Open I2C device with default information
 * @return
 */
bool I2C::HALOpen() {
    char buf[32];
    if (m_i2c >= 0) return true;
    if (m_i2c_bus == 255) {
        HAL_ERROR("No I2C bus has been set\n");
        return false;
    }
    sprintf(buf, "/dev/i2c-%d", m_i2c_bus);
    m_i2c = open(buf, O_RDWR);
    if (m_i2c < 0) {
        HAL_ERROR1("Failed to open I2C bus %d\n", m_i2c_bus);
        m_i2c = -1;
        return false;
    }
    return true;
}

/**
 * @brief I2C::HALClose
 * @details Close I2C device
 * @return
 */
bool I2C::HALClose() {
    if (m_i2c >= 0) {
        close(m_i2c);
        m_i2c = -1;
        m_current_slave = 255;
    }
    return true;
}

/**
 * @brief I2C::HALRead
 * @details Read data from I2C device
 * @param slave_addr
 * @param reg_addr
 * @param length
 * @param data
 * @param error_msg
 * @return
 */
bool I2C::HALRead(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data, const char *error_msg) {
    int tries, total, result;
    if (!this->HALWrite(slave_addr, reg_addr, 0, NULL, error_msg))
        return false;

    total = 0;
    tries = 0;

    while ((total < length) && (tries < 5)) {
        result = read(m_i2c, data + total, length - total);

        if (result < 0) {
            if (strlen(error_msg) > 0)
                HAL_ERROR3("I2C read error from %d, %d - %s\n", slave_addr, reg_addr, error_msg);
            return false;
        }

        total += result;

        if (total == length)
            break;

        this->delayMs(10);
        tries++;
    }

    if (total < length) {
        if (strlen(error_msg) > 0)
            HAL_ERROR3("I2C read from %d, %d failed - %s\n", slave_addr, reg_addr, error_msg);
        return false;
    }
    return true;
}

/**
 * @brief I2C::HALRead
 * @param slave_addr
 * @param length
 * @param data
 * @param error_msg
 * @return
 */
bool I2C::HALRead(unsigned char slave_addr, unsigned char length, unsigned char *data, const char *error_msg) {
    int tries, result, total;
    if (!this->selectSlave(slave_addr, error_msg)) {
        return false;
    }

    total = 0;
    tries = 0;

    while ((total < length) && (tries < 5)) {
        result = read(m_i2c, data + total, length - total);

        if (result < 0) {
            if (strlen(error_msg) > 0)
                HAL_ERROR2("I2C read error from %d - %s\n", slave_addr, error_msg);
            return false;
        }

        total += result;

        if (total == length)
            break;

        this->delayMs(10);
        tries++;
    }

    if (total < length) {
        if (strlen(error_msg) > 0)
            HAL_ERROR2("I2C read from %d failed - %s\n", slave_addr, error_msg);
        return false;
    }
    return true;
}

/**
 * @brief I2C::selectSlave
 * @param slave_addr
 * @param error_msg
 * @return
 */
bool I2C::selectSlave(unsigned char slave_addr, const char *error_msg) {
    if (m_current_slave == slave_addr)
        return true;

    if (!this->HALOpen()) {
        HAL_ERROR1("Failed to open I2C port - %s\n", error_msg);
        return false;
    }

    if (ioctl(m_i2c, I2C_SLAVE, slave_addr) < 0) {
        HAL_ERROR2("I2C slave select %d failed - %s\n", slave_addr, error_msg);
        return false;
    }

    m_current_slave = slave_addr;

    return true;
}

/**
 * @brief I2C::HALWrite
 * @param slave_addr
 * @param reg_addr
 * @param data
 * @param error_msg
 * @return
 */
bool I2C::HALWrite(unsigned char slave_addr, unsigned char reg_addr, const unsigned char data, const char *error_msg) {
    return this->HALWrite(slave_addr, reg_addr, 1, &data, error_msg);
}

/**
 * @brief I2C::HALWrite
 * @param slave_addr
 * @param reg_addr
 * @param length
 * @param data
 * @param error_msg
 * @return
 */
bool I2C::HALWrite(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, const unsigned char *data, const char *error_msg) {
    int result;
    char *if_type;
    unsigned char tx_buff[MAX_WRITE_LEN + 1];

    if (!this->selectSlave(slave_addr, error_msg)) return false;
    if_type = (char *)"I2C";

    if (length == 0) {
        char len = 1;
        result = this->ifWrite(&reg_addr, len);

        if (result < 0) {
            if (strlen(error_msg) > 0)
                HAL_ERROR2("%s write of regAddr failed - %s\n", if_type, error_msg);
            return false;
        } else if (result != 1) {
            if (strlen(error_msg) > 0)
                HAL_ERROR2("%s write of regAddr failed (nothing written) - %s\n", if_type, error_msg);
            return false;
        }
    } else {
        tx_buff[0] = reg_addr;
        memcpy(tx_buff + 1, data, length);
        char len = length + 1;
        result = this->ifWrite(tx_buff, len);

        if (result < 0) {
            if (strlen(error_msg) > 0)
                HAL_ERROR3("%s data write of %d bytes failed - %s\n", if_type, length, error_msg);
            return false;
        } else if (result < (int)length) {
            if (strlen(error_msg) > 0)
                HAL_ERROR4("%s data write of %d bytes failed, only %d written - %s\n", if_type, length, result, error_msg);
            return false;
        }
    }
    return true;
}

/**
 * @brief I2C::ifWrite
 * @param data
 * @param length
 * @return
 */
bool I2C::ifWrite(unsigned char *data, unsigned char length) {
    return write(m_i2c, data, length);
}

//----SPI-----------------------------------------------------------------------------------------------------//

SPI::SPI() : m_spi(-1), m_spi_select(255), m_spi_speed(500000), m_spi_bus(255){

}

SPI::~SPI() {
    this->HALClose();
}

/**
 * @brief SPI::HALOpen
 * @return
 */
bool SPI::HALOpen() {
    char buf[32];
    unsigned char spi_mode = SPI_MODE_0;
    unsigned char spi_bits = 8;
    uint32_t spi_speed = m_spi_speed;
    if (m_spi_bus == 255) {
        HAL_ERROR("No SPI bus has been set\n");
        return false;
    }

    sprintf(buf, "/dev/spidev%d.%d", m_spi_bus, m_spi_select);
    m_spi = open(buf, O_RDWR);
    if (m_spi < 0) {
        HAL_ERROR2("Failed to open SPI bus %d, select %d\n", m_spi_bus, m_spi_select);
        m_spi = -1;
        return false;
    }

    if (ioctl(m_spi, SPI_IOC_WR_MODE, &spi_mode) < 0) {
        HAL_ERROR1("Failed to set WR SPI_MODE0 on bus %d", m_spi_bus);
        close(m_spi_bus);
        return false;
    }

    if (ioctl(m_spi, SPI_IOC_RD_MODE, &spi_mode) < 0) {
        HAL_ERROR1("Failed to set RD SPI_MODE0 on bus %d", m_spi_bus);
        close(m_spi_bus);
        return false;
    }

    if (ioctl(m_spi, SPI_IOC_WR_BITS_PER_WORD, &spi_bits) < 0) {
        HAL_ERROR1("Failed to set WR 8 bit mode on bus %d", m_spi_bus);
        close(m_spi_bus);
        return false;
    }

    if (ioctl(m_spi, SPI_IOC_RD_BITS_PER_WORD, &spi_bits) < 0) {
        HAL_ERROR1("Failed to set RD 8 bit mode on bus %d", m_spi_bus);
        close(m_spi_bus);
        return false;
    }

    if (ioctl(m_spi, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed) < 0) {
         HAL_ERROR2("Failed to set WR %dHz on bus %d", spi_speed, m_spi_bus);
         close(m_spi_bus);
         return false;
    }

    if (ioctl(m_spi, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed) < 0) {
         HAL_ERROR2("Failed to set RD %dHz on bus %d", spi_speed, m_spi_bus);
         close(m_spi_bus);
         return false;
    }
    return true;
}

/**
 * @brief SPI::HALClose
 * @return
 */
bool SPI::HALClose() {
    if (m_spi >= 0) {
        close(m_spi);
        m_spi = -1;
    }
    return true;
}

/**
 * @brief SPI::HALRead
 * @param slave_addr
 * @param reg_addr
 * @param length
 * @param data
 * @param error_msg
 * @return
 */
bool SPI::HALRead(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data, const char *error_msg) {
    unsigned char rx_buff[MAX_READ_LEN + 1];
    struct spi_ioc_transfer rdIOC;
    rx_buff[0] = reg_addr | 0x80;
    memcpy(rx_buff + 1, data, length);
    memset(&rdIOC, 0, sizeof(rdIOC));
    rdIOC.tx_buf = (unsigned long) rx_buff;
    rdIOC.rx_buf = (unsigned long) rx_buff;
    rdIOC.len = length + 1;

    if (ioctl(m_spi, SPI_IOC_MESSAGE(1), &rdIOC) < 0) {
        if (strlen(error_msg) > 0)
            HAL_ERROR2("SPI read error from %d - %s\n", reg_addr, error_msg);
        return false;
    }
    memcpy(data, rx_buff + 1, length);
    return true;
}

/**
 * @brief SPI::HALRead
 * @param slave_addr
 * @param length
 * @param data
 * @param error_msg
 * @return
 */
bool SPI::HALRead(unsigned char slave_addr, unsigned char length, unsigned char *data, const char *error_msg) {
    unsigned char rx_buff[MAX_READ_LEN + 1];
    struct spi_ioc_transfer rdIOC;

    memset(&rdIOC, 0, sizeof(rdIOC));
    rdIOC.tx_buf = 0;
    rdIOC.rx_buf = (unsigned long) rx_buff;
    rdIOC.len = length;

    if (ioctl(m_spi, SPI_IOC_MESSAGE(1), &rdIOC) < 0) {
        if (strlen(error_msg) > 0)
            HAL_ERROR1("SPI read error from - %s\n", error_msg);
        return false;
    }
    memcpy(data, rx_buff, length);
    return true;
}

/**
 * @brief SPI::HALWrite
 * @param slave_addr
 * @param reg_addr
 * @param length
 * @param data
 * @param error_msg
 * @return
 */
bool SPI::HALWrite(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, const unsigned char *data, const char *error_msg) {
    int result;
    unsigned char tx_buff[MAX_WRITE_LEN + 1];
    char *if_type;
    if_type = (char *)"SPI";
    if (length == 0) {
        char len = 1;
        result = this->ifWrite(&reg_addr, len);

        if (result < 0) {
            if (strlen(error_msg) > 0)
                HAL_ERROR2("%s write of regAddr failed - %s\n", if_type, error_msg);
            return false;
        } else if (result != 1) {
            if (strlen(error_msg) > 0)
                HAL_ERROR2("%s write of regAddr failed (nothing written) - %s\n", if_type, error_msg);
            return false;
        }
    } else {
        tx_buff[0] = reg_addr;
        memcpy(tx_buff + 1, data, length);
        char len = length + 1;
        result = this->ifWrite(tx_buff, len);

        if (result < 0) {
            if (strlen(error_msg) > 0)
                HAL_ERROR3("%s data write of %d bytes failed - %s\n", if_type, length, error_msg);
            return false;
        } else if (result < (int)length) {
            if (strlen(error_msg) > 0)
                HAL_ERROR4("%s data write of %d bytes failed, only %d written - %s\n", if_type, length, result, error_msg);
            return false;
        }
    }
    return true;
}

/**
 * @brief SPI::HALWrite
 * @param slave_addr
 * @param reg_addr
 * @param data
 * @param error_msg
 * @return
 */
bool SPI::HALWrite(unsigned char slave_addr, unsigned char reg_addr, const unsigned char data, const char *error_msg) {
    return this->HALWrite(slave_addr, reg_addr, 1, &data, error_msg);
}

/**
 * @brief SPI::ifWrite
 * @param data
 * @param length
 * @return
 */
bool SPI::ifWrite(unsigned char *data, unsigned char length) {
    struct spi_ioc_transfer wrIOC;
    memset(&wrIOC, 0, sizeof(wrIOC));
    wrIOC.tx_buf = (unsigned long) data;
    wrIOC.rx_buf = 0;
    wrIOC.len = length;
    return ioctl(m_spi, SPI_IOC_MESSAGE(1), &wrIOC);
}
