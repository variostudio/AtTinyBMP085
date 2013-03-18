#include <inttypes.h>

#define USI_SEND         0              // indicates sending to TWI
#define USI_RCVE         1              // indicates receiving from TWI
#define USI_BUF_SIZE    16              // bytes in message buffer

void TWI_begin();
void TWI_beginTransmission(uint8_t slaveAddr);
void TWI_send(uint8_t);
uint8_t TWI_endTransmission();
uint8_t TWI_requestFrom(uint8_t, uint8_t);
uint8_t TWI_receive(); 
uint8_t TWI_available(); 

