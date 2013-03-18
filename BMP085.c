#define F_CPU 1000000UL
#include <util/delay.h>
#include <math.h>
#include "BMP085.h"
#include "USI_TWI.h"

int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
uint16_t ac4, ac5, ac6;
uint8_t oversampling;


uint8_t read8(uint8_t a) {
  uint8_t ret;

  TWI_beginTransmission(BMP085_I2CADDR); // start transmission to device
  TWI_send(a); // sends register address to read from
  TWI_endTransmission(); // end transmission
  
  TWI_beginTransmission(BMP085_I2CADDR); // start transmission to device
  TWI_requestFrom(BMP085_I2CADDR, 1);// send data n-bytes read
  ret = TWI_receive(); // receive DATA
  //TWI_endTransmission(); // end transmission

  return ret;
}

uint16_t read16(uint8_t a) {
  uint16_t ret;

  TWI_beginTransmission(BMP085_I2CADDR); // start transmission to device
  TWI_send(a); // sends register address to read from
  TWI_endTransmission(); // end transmission
  
  TWI_beginTransmission(BMP085_I2CADDR); // start transmission to device
  TWI_requestFrom(BMP085_I2CADDR, 2);// send data n-bytes read
  ret = TWI_receive(); // receive DATA
  ret <<= 8;
  ret |= TWI_receive(); // receive DATA
  //TWI_endTransmission(); // end transmission


  return ret;
}

void write8(uint8_t a, uint8_t d) {
  TWI_beginTransmission(BMP085_I2CADDR); // start transmission to device
  TWI_send(a); // sends register address to read from
  TWI_send(d); // write data
  TWI_endTransmission(); // end transmission
}


void BMP085_begin(uint8_t mode) {
  if (mode > BMP085_ULTRAHIGHRES)
    mode = BMP085_ULTRAHIGHRES;
  oversampling = mode;

  TWI_begin();

  /* read calibration data */
  ac1 = read16(BMP085_CAL_AC1);
  ac2 = read16(BMP085_CAL_AC2);
  ac3 = read16(BMP085_CAL_AC3);
  ac4 = read16(BMP085_CAL_AC4);
  ac5 = read16(BMP085_CAL_AC5);
  ac6 = read16(BMP085_CAL_AC6);

  b1 = read16(BMP085_CAL_B1);
  b2 = read16(BMP085_CAL_B2);

  mb = read16(BMP085_CAL_MB);
  mc = read16(BMP085_CAL_MC);
  md = read16(BMP085_CAL_MD);
}

uint16_t readRawTemperature(void) {
  write8(BMP085_CONTROL, BMP085_READTEMPCMD);
  _delay_ms(5);

  return read16(BMP085_TEMPDATA);
}

uint32_t readRawPressure(void) {
  uint32_t raw;

  write8(BMP085_CONTROL, BMP085_READPRESSURECMD + (oversampling << 6));

  if (oversampling == BMP085_ULTRALOWPOWER)
    _delay_ms(5);
  else if (oversampling == BMP085_STANDARD)
    _delay_ms(8);
  else if (oversampling == BMP085_HIGHRES)
    _delay_ms(14);
  else
    _delay_ms(26);

  raw = read16(BMP085_PRESSUREDATA);
  raw <<= 8;
  raw |= read8(BMP085_PRESSUREDATA+2);
  raw >>= (8 - oversampling);

  return raw;
}


int32_t BMP085_readPressure(void) {
  int32_t UT, UP, B3, B5, B6, X1, X2, X3, p;
  uint32_t B4, B7;

  UT = readRawTemperature();
  UP = readRawPressure();

  // do temperature calculations
  X1 = ((UT - (int32_t)ac6) * (int32_t)ac5) >> 15;
  X2 = ((int32_t)mc << 11) - (X1 + md)/2; // round up
  X2 /= (X1 + md);
  B5 = X1 + X2;

  // do pressure calcs
  B6 = B5 - 4000;
  X1 = ((int32_t)b2 * ( (B6 * B6)>>12 )) >> 11;
  X2 = ((int32_t)ac2 * B6) >> 11;
  X3 = X1 + X2;
  B3 = ((((int32_t)ac1*4 + X3) << oversampling) + 2) / 4;

  X1 = ((int32_t)ac3 * B6) >> 13;
  X2 = ((int32_t)b1 * ((B6 * B6) >> 12)) >> 16;
  X3 = ((X1 + X2) + 2) >> 2;
  B4 = ((uint32_t)ac4 * (uint32_t)(X3 + 32768)) >> 15;
  B7 = ((uint32_t)UP - B3) * (uint32_t)( 50000UL >> oversampling );

  if (B7 < 0x80000000) {
    p = (B7 * 2) / B4;
  } else {
    p = (B7 * 2) / B3;
  }
  X1 = (p >> 8) * (p >> 8);
  X1 = (X1 * 3038) >> 16;
  X2 = (-7357 * p) >> 16;

  p = p + ((X1 + X2 + (int32_t)3791)>>4);

  return p;
}


int32_t BMP085_readTemperature(void) {

  int32_t UT, X1, X2, B5; // following ds convention
  //float temp;
  int32_t temp;

  UT = readRawTemperature();

  // step 1
  X1 = ((UT - (int32_t)ac6) * (int32_t)ac5) >> 15;
  X2 = ((int32_t)mc << 11) / (X1 + (int32_t)md);
  B5 = X1 + X2;
  temp = (B5 + 8) >> 4;
  //temp /= 10;

  //return (int32_t)temp;
  return temp;
}


/************************************************************************
Uncomment the function below to make use of the readAltitude function.
This function invokes the math.h library for the "pow()" function, and
as such takes up considerable program space. If you don't need to
calculate altitude, leave it out!
*************************************************************************/


float BMP085_readAltitude(float sealevelPressure) {
	float altitude;

	float pressure = BMP085_readPressure();

	altitude = 44330 * (1.0 - pow(pressure/sealevelPressure, 0.1903));

	return altitude;
}

