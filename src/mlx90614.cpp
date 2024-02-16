/*
    Written By Premchand Gat
    Github: PremchandGat
    Email: Premchandg278@gmail.com
*/

#include "mlx90614.h"

MLX90614::MLX90614(i2c_inst_t *i2c)
{
    _i2c = i2c;
}

/* Read Address */
void MLX90614::read_addr(uint8_t reg, uint8_t *buf, int len)
{
    if (i2c_write_blocking(_i2c, MLX90614_I2CADDR, &reg, 1, true) /*true to keep master control of bus*/ == PICO_ERROR_GENERIC)
    {
        /* Device not found */
        printf("MLX90614 Not found\n");
    };
    if (i2c_read_blocking(_i2c, MLX90614_I2CADDR, buf, len, false) /* false - finished with bus */ == PICO_ERROR_GENERIC)
    {
        /* Device not found */
        printf("MLX90614 Not found\n");
    }
}

/* Write Data */
void MLX90614::write_byte(uint8_t *buffer, int len)
{
    /*  buffer = {ADDRESS, value1, value2, ..... valueN }
        len = Elements in buffer
    */
    i2c_write_blocking(_i2c, MLX90614_I2CADDR, buffer, len, false);
}

/**
 * @brief Read the raw value from the emissivity register
 *
 * @return uint16_t The unscaled emissivity value or '0' if reading failed
 */
uint16_t MLX90614::readEmissivityReg(void)
{
    return read16(MLX90614_EMISS);
}
/**
 * @brief Write the raw unscaled emissivity value to the emissivity register
 *
 * @param ereg The unscaled emissivity value
 */
void MLX90614::writeEmissivityReg(uint16_t ereg)
{
    write16(MLX90614_EMISS, 0); // erase
    sleep_ms(10);
    write16(MLX90614_EMISS, ereg);
    sleep_ms(10);
}
/**
 * @brief Read the emissivity value from the sensor's register and scale
 *
 * @return double The emissivity value, ranging from 0.1 - 1.0 or NAN if reading
 * failed
 */
double MLX90614::readEmissivity(void)
{
    uint16_t ereg = read16(MLX90614_EMISS);
    if (ereg == 0)
        return 0;
    return ((double)ereg) / 65535.0;
}
/**
 * @brief Set the emissivity value
 *
 * @param emissivity The emissivity value to use, between 0.1 and 1.0
 */
void MLX90614::writeEmissivity(double emissivity)
{
    uint16_t ereg = (uint16_t)(0xffff * emissivity);

    writeEmissivityReg(ereg);
}

/**
 * @brief Get the current temperature of an object in degrees Farenheit
 *
 * @return double The temperature in degrees Farenheit or NAN if reading failed
 */
double MLX90614::readObjectTempF(void)
{
    return (readTemp(MLX90614_TOBJ1) * 9 / 5) + 32;
}
/**
 * @brief Get the current ambient temperature in degrees Farenheit
 *
 * @return double The temperature in degrees Farenheit or NAN if reading failed
 */
double MLX90614::readAmbientTempF(void)
{
    return (readTemp(MLX90614_TA) * 9 / 5) + 32;
}

/**
 * @brief Get the current temperature of an object in degrees Celcius
 *
 * @return double The temperature in degrees Celcius or NAN if reading failed
 */
double MLX90614::readObjectTempC(void)
{
    return readTemp(MLX90614_TOBJ1);
}

/**
 * @brief Get the current ambient temperature in degrees Celcius
 *
 * @return double The temperature in degrees Celcius or NAN if reading failed
 */
double MLX90614::readAmbientTempC(void)
{
    return readTemp(MLX90614_TA);
}

float MLX90614::readTemp(uint8_t reg)
{
    float temp;
    temp = read16(reg);
    if (temp == 0)
        return 0;
    temp *= .02;
    temp -= 273.15;
    return temp;
}

uint16_t MLX90614::read16(uint8_t reg)
{
    uint8_t buffer[3];
    read_addr(reg, buffer , 3);
    // return data, ignore pec
    return uint16_t(buffer[0]) | (uint16_t(buffer[1]) << 8);
}

uint8_t MLX90614::crc8(uint8_t *addr, uint8_t len)
// The PEC calculation includes all bits except the START, REPEATED START, STOP,
// ACK, and NACK bits. The PEC is a CRC-8 with polynomial X8+X2+X1+1.
{
    uint8_t crc = 0;
    while (len--)
    {
        uint8_t inbyte = *addr++;
        for (uint8_t i = 8; i; i--)
        {
            uint8_t carry = (crc ^ inbyte) & 0x80;
            crc <<= 1;
            if (carry)
                crc ^= 0x7;
            inbyte <<= 1;
        }
    }
    return crc;
}

void MLX90614::write16(uint8_t a, uint16_t v)
{
    uint8_t buffer[4];

    buffer[0] = MLX90614_I2CADDR << 1;
    buffer[1] = a;
    buffer[2] = v & 0xff;
    buffer[3] = v >> 8;

    uint8_t pec = crc8(buffer, 4);

    buffer[0] = buffer[1];
    buffer[1] = buffer[2];
    buffer[2] = buffer[3];
    buffer[3] = pec;
    i2c_write_blocking(_i2c, MLX90614_I2CADDR, buffer, 4, false);
}