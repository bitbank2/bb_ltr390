//
// LTR390 - Lite-On Visible/UV light sensor library
//
// Written by Larry Bank - 6/6/2022
// Copyright (c) 2022 BitBank Software, Inc.
// bitbank@pobox.com
//
//
#ifndef __LTR390__
#define __LTR390__

#include <BitBang_I2C.h>

#define LTR390_SUCCESS 0
#define LTR390_ERROR -1

// Register definitions
#define LTR390_MAIN_CTRL 0x00
#define LTR390_MEAS_RATE 0x04
#define LTR390_ALS_UVS_GAIN 0x05
#define LTR390_MAIN_STATUS 0x07
#define LTR390_ALS_DATA_0 0x0D
#define LTR390_UVS_DATA_0 0x10
class LTR390
{
  public:
    int init(int iSDA=-1, int iSCL=-1, bool bBitBang=false, int32_t iSpeed=100000L);
    int start(boolean bUV);
    int stop();
    void getSample(); // trigger + read the latest data
    int visible();
    int uv();
    uint8_t computeCRC8(uint8_t data[], uint8_t len);

  private:
    uint32_t _iVisible, _iUV;
    int _iAddr; // I2C address of device
    BBI2C _bbi2c;
};

#endif // __LTR390__
