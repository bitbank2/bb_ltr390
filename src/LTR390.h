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
#define LTR390_BAD_PARAMETER -2


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
    LTR390() {_iGain = 3; _iResolution = 18; memset(&_bbi2c,0,sizeof(_bbi2c));}
    ~LTR390() {}
    int init(int iSDA=-1, int iSCL=-1, bool bBitBang=false, int32_t iSpeed=100000L);
    int start(boolean bUV);
    int stop();
    int reset();
    BBI2C *getBB();
    void setBB(BBI2C *pBB);
    uint8_t status();
    void getSample(); // trigger + read the latest data
    int setResolution(int iRes);
    int getResolution();
    int setGain(int iGain);
    int getGain();
    int visible();
    int uv();
    float getLux();
    float getUVI();

  private:
    uint32_t _iVisible, _iUV;
    int _iGain, _iResolution;
    int _iAddr; // I2C address of device
    BBI2C _bbi2c;
};

#endif // __LTR390__
