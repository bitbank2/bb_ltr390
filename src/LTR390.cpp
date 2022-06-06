#include <Arduino.h>
#include <BitBang_I2C.h>
#include "LTR390.h"

//
// Written by Larry Bank - 6/6/2022
// Copyright (c) 2022 BitBank Software, Inc.
// bitbank@pobox.com
//

void LTR390::getSample()
{
uint8_t ucTemp[16];

    I2CReadRegister(&_bbi2c, _iAddr, LTR390_ALS_DATA_0, ucTemp, 6); // read ALS and UVS data together
    _iVisible = ucTemp[0] | (ucTemp[1] << 8) | (ucTemp[2] << 16); // 20-bits of ALS data
    _iUV = ucTemp[3] | (ucTemp[4] << 8) | (ucTemp[5] << 16); // 20-bits of UVS data

} /* getSample() */

int LTR390::stop()
{
     ucTemp[0] = LTR390_MAIN_CTRL;
     ucTemp[1] = 0x00; // de-activate visible or UV mode
     I2CWrite(&_bbi2c, _iAddr, ucTemp, 2);
     return LTR390_SUCCESS;
} /* stop() */

int LTR390::start(boolean bUV)
{
uint8_t ucTemp[4];

     ucTemp[0] = LTR390_MAIN_CTRL;
     ucTemp[1] = 0x02 | (bUV) ? 0x08 : 0x00; // activate visible or UV mode
     I2CWrite(&_bbi2c, _iAddr, ucTemp, 2);
     delay(10); // allow time to start
     return LTR390_SUCCESS;
} /* start() */

int LTR390::init(int iSDA, int iSCL, bool bBitBang, int32_t iSpeed)
{
uint8_t map[16];
int i, iDev;
uint32_t u32Capabilities;

//Serial.println("Entering init");
	_bbi2c.bWire = !bBitBang; // use bit bang?
	_bbi2c.iSDA = iSDA;
	_bbi2c.iSCL = iSCL;
	I2CInit(&_bbi2c, iSpeed);
	// Scan the I2C bus and see if there are any supported devices connected
	I2CScan(&_bbi2c, map);
	_iAddr = -1;
	for (i=1; i<128 && _iAddr < 0; i++) // skip address 0 (general call address) since more than 1 device can respond
	{
		if (map[i>>3] & (1 << (i & 7))) // device found
		{
//Serial.print("Found device at 0x"); Serial.println(i, HEX);
        		iDev = I2CDiscoverDevice(&_bbi2c, i, &u32Capabilities);
			if (iDev == DEVICE_LTR390) // found one
			{
                                //Serial.println("found an LTR390!");
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

