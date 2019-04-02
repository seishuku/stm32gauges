#ifndef __I2C_H__
#define __I2C_H__

void I2C_Init(void);
void I2C_WriteData(uint8_t Addr, uint8_t Reg, uint8_t Value);
void I2C_WriteBuffer(uint8_t Addr, uint8_t Reg,  uint8_t *pBuffer, uint16_t Length);
uint8_t I2C_ReadData(uint8_t Addr, uint8_t Reg);
uint8_t I2C_ReadBuffer(uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length);
void I2C_ITConfig(void);

#endif
