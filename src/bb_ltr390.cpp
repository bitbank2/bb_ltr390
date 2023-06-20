//
// bb_ltr390 - Lite-On Visible/UV light sensor library
//
// Written by Larry Bank - 6/6/2022
// Copyright (c) 2022-2023 BitBank Software, Inc.
// email: bitbank@pobox.com
//
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include <Arduino.h>
#include <BitBang_I2C.h>
#include "bb_ltr390.h"

void LTR390::getSample()
{
uint8_t ucTemp[8];

    for (int i=0; i<6; i++) { // need to read them one by one
       I2CReadRegister(&_bbi2c, _iAddr, LTR390_ALS_DATA_0+i, &ucTemp[i], 1); // read ALS and UVS data together
    }
    ucTemp[2] &= 0xf; // trim to 4-bits
    ucTemp[5] &= 0xf;
    _iVisible = ucTemp[0] | (ucTemp[1] << 8) | (ucTemp[2] << 16); // 20-bits of ALS data
    _iUV = ucTemp[3] | (ucTemp[4] << 8) | (ucTemp[5] << 16); // 20-bits of UVS data

} /* getSample() */
//
// Valid range for gain: 1,3,6,9,18
//
int LTR390::setGain(int iGain)
{
uint8_t ucTemp[4];
const uint8_t ucGain[] = {0,1,2,3,0,0,4};
   if (iGain != 1 && iGain != 3 && iGain != 6 && iGain != 9 && iGain != 18)
      return LTR390_BAD_PARAMETER;
   _iGain = iGain;
   ucTemp[0] = LTR390_ALS_UVS_GAIN;
   ucTemp[1] = ucGain[iGain];
   I2CWrite(&_bbi2c, _iAddr, ucTemp, 2);
   return LTR390_SUCCESS;
} /* setGain() */

int LTR390::getGain()
{
   return _iGain;
} /* getGain() */
//
// Set the bit resolution of the samples
// valid range is 13 to 20
//
int LTR390::setResolution(int iRes)
{
uint8_t ucTemp[4];
const uint8_t ucBits[8] = {5,0,0,4,3,2,1,0};

  if (iRes != 20 && iRes != 19 && iRes != 18 && iRes != 17 && iRes != 16 && iRes != 13) return LTR390_BAD_PARAMETER;
  _iResolution = iRes;
  ucTemp[0] = LTR390_MEAS_RATE;
  ucTemp[1] = 0x2 | (ucBits[iRes-13] << 4); // default 100ms capture rate
  I2CWrite(&_bbi2c, _iAddr, ucTemp, 2);
  return LTR390_SUCCESS;  
} /* setResolution() */
//
// Returns the bit resolution of the sensor
// possible values are 13-20
//
int LTR390::getResolution()
{
   return _iResolution;
} /* getResolution() */

//
// Calculat the UV Index (UVI) based upon the rated sensitivity
// of 1 UVI per 2300 counts at 18X gain factor and 20-bit resolution.
//
float LTR390::getUVI()
{
float fUVI;
float fGain;

    fUVI  = (float)(_iUV << (20 - _iResolution)); // scale up to 20-bits of res
    fGain = (float)_iGain / 18.0f;
    fUVI  = fUVI / (fGain * 2300.0f);
    return fUVI;
} /* getUVI() */

float LTR390::getLux()
{
float lux;
const float res_factor[] = {0.03125f, 0,0,0,0.25f, 0.5f, 1.0f, 2.0f, 4.0f};

    lux = 0.6f * (float)(_iVisible) / ((float)_iGain * res_factor[_iResolution-13]);
  return lux;
} /* getLux() */

int LTR390::reset()
{
uint8_t ucTemp[4];

     ucTemp[0] = LTR390_MAIN_CTRL;
     ucTemp[1] = 0x10; // assert reset
     I2CWrite(&_bbi2c, _iAddr, ucTemp, 2);
     delay(10);
     return LTR390_SUCCESS;
}
int LTR390::stop()
{
uint8_t ucTemp[4];

     ucTemp[0] = LTR390_MAIN_CTRL;
     ucTemp[1] = 0x00; // de-activate visible or UV mode
     I2CWrite(&_bbi2c, _iAddr, ucTemp, 2);
     return LTR390_SUCCESS;
} /* stop() */

uint8_t LTR390::status()
{
uint8_t ucStatus;
   I2CReadRegister(&_bbi2c, _iAddr, LTR390_MAIN_STATUS, &ucStatus, 1);
   return ucStatus;
} /* status() */

int LTR390::start(boolean bUV)
{
uint8_t ucReg, ucTemp[4];

    // reset();
     I2CReadRegister(&_bbi2c, _iAddr, LTR390_MAIN_CTRL, &ucReg, 1);
     ucTemp[0] = LTR390_MAIN_CTRL;
     if (bUV)
        ucReg |= 0x08;
     else
        ucReg &= ~0x08;
     ucTemp[1] = ucReg | 0x02;
     I2CWrite(&_bbi2c, _iAddr, ucTemp, 2);
     delay(10); // allow time to start
     //setGain(3); // default
     //setResolution(18); // default
     return LTR390_SUCCESS;
} /* start() */

int LTR390::init(int iSDA, int iSCL, bool bBitBang, int32_t iSpeed)
{
uint8_t map[16];
int i, iDev;
uint32_t u32Capabilities;

    if (_bbi2c.iSDA == 0 && _bbi2c.iSCL == 0) { // need to init
	_bbi2c.bWire = !bBitBang; // use bit bang?
	_bbi2c.iSDA = iSDA;
	_bbi2c.iSCL = iSCL;
	I2CInit(&_bbi2c, iSpeed);
    }
	// Scan the I2C bus and see if there are any supported devices connected
	I2CScan(&_bbi2c, map);
	_iAddr = -1;
	for (i=1; i<128 && _iAddr < 0; i++) // skip address 0 (general call address) since more than 1 device can respond
	{
		if (map[i>>3] & (1 << (i & 7))) // device found
		{
        		iDev = I2CDiscoverDevice(&_bbi2c, i, &u32Capabilities);
			if (iDev == DEVICE_LTR390) // found one
			{
				_iAddr = i;
				break;
			}
		} // if address responded
	} // for i
	if (_iAddr == -1) return LTR390_ERROR;
     return LTR390_SUCCESS;
} /* init() */

int LTR390::visible()
{
    return _iVisible;
} /* visible() */

int LTR390::uv()
{
    return _iUV;
} /* uv() */

