#include <stdbool.h>
#include "USI_TWI_Master.h"
#include "USI_TWI.h"

uint8_t USI_Buf[USI_BUF_SIZE];             // holds I2C send and receive data
uint8_t USI_BufIdx = 0;                    // current number of bytes in the send buff
uint8_t USI_LastRead = 0;                  // number of bytes read so far
uint8_t USI_BytesAvail = 0;                // number of bytes requested but not read

void TWI_begin(){ // initialize I2C lib
  USI_TWI_Master_Initialise();          
}

void TWI_beginTransmission(uint8_t slaveAddr){ // setup address & write bit
  USI_BufIdx = 0; 
  USI_Buf[USI_BufIdx] = (slaveAddr<<TWI_ADR_BITS) | USI_SEND; 
}

void TWI_send(uint8_t data){ // buffers up data to send
  if (USI_BufIdx >= USI_BUF_SIZE) return;         // dont blow out the buffer
  USI_BufIdx++;                                   // inc for next byte in buffer
  USI_Buf[USI_BufIdx] = data;
}

uint8_t TWI_endTransmission(){ // actually sends the buffer
  bool xferOK = false;
  uint8_t errorCode = 0;
  xferOK = USI_TWI_Start_Read_Write(USI_Buf,USI_BufIdx+1); // core func that does the work
  USI_BufIdx = 0;
  if (xferOK) return 0;
  else {                                  // there was an error
    errorCode = USI_TWI_Get_State_Info(); // this function returns the error number
    return errorCode;
  }
}

uint8_t TWI_requestFrom(uint8_t slaveAddr, uint8_t numBytes){ // setup for receiving from slave
  bool xferOK = false;
  uint8_t errorCode = 0;
  USI_LastRead = 0;
  USI_BytesAvail = numBytes; // save this off in a global
  numBytes++;                // add extra byte to transmit header
  USI_Buf[0] = (slaveAddr<<TWI_ADR_BITS) | USI_RCVE;   // setup address & Rcve bit
  xferOK = USI_TWI_Start_Read_Write(USI_Buf,numBytes); // core func that does the work
  // USI_Buf now holds the data read
  if (xferOK) return 0;
  else {                                  // there was an error
    errorCode = USI_TWI_Get_State_Info(); // this function returns the error number
    return errorCode;
  }
}

uint8_t TWI_receive(void){ // returns the bytes received one at a time
  USI_LastRead++;     // inc first since first uint8_t read is in USI_Buf[1]
  return USI_Buf[USI_LastRead];
}

uint8_t TWI_available(){ // the bytes available that haven't been read yet
  return USI_BytesAvail - (USI_LastRead); 
}

