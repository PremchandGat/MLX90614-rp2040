/*
    Written By Premchand Gat
    Github: PremchandGat
    Email: Premchandg278@gmail.com
*/

#pragma once
#include <stdio.h>
#include "hardware/i2c.h"
#include <cstddef>
/* --------------------------------------------------------- MLX90614 Register Map ------------------------------------------------------*/
#define MLX90614_I2CADDR 0x5A
// RAM
#define MLX90614_RAWIR1 0x04
#define MLX90614_RAWIR2 0x05
#define MLX90614_TA 0x06
#define MLX90614_TOBJ1 0x07
#define MLX90614_TOBJ2 0x08
// EEPROM
#define MLX90614_TOMAX 0x20
#define MLX90614_TOMIN 0x21
#define MLX90614_PWMCTRL 0x22
#define MLX90614_TARANGE 0x23
#define MLX90614_EMISS 0x24
#define MLX90614_CONFIG 0x25
#define MLX90614_ADDR 0x2E
#define MLX90614_ID1 0x3C
#define MLX90614_ID2 0x3D
#define MLX90614_ID3 0x3E
#define MLX90614_ID4 0x3F
/*----------------------------------------------------------------END-----------------------------------------------------------------*/

#define INT_MASK 0x2

class MLX90614
{
public:
    MLX90614(i2c_inst_t *i2c);
    double readObjectTempC(void);
    double readAmbientTempC(void);
    double readObjectTempF(void);
    double readAmbientTempF(void);
    uint16_t readEmissivityReg(void);
    void writeEmissivityReg(uint16_t ereg);
    double readEmissivity(void);
    void writeEmissivity(double emissivity);

private:
    float readTemp(uint8_t reg);
    uint16_t read16(uint8_t addr);
    void write16(uint8_t addr, uint16_t data);
    uint8_t crc8(uint8_t *addr, uint8_t len);
    void read_addr(uint8_t reg, uint8_t *buf, int len);
    void write_byte(uint8_t *buffer, int len);
    i2c_inst_t *_i2c;
};