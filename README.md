bb_ltr390 (Visible+UV I2C Sensor library)<br>
-----------------------------------------
Project started 6/22/2022<br>
Copyright (c) 2022-2023 BitBank Software, Inc.<br>
Written by Larry Bank<br>
email: bitbank@pobox.com<br>
<br>
The purpose of this code is to easily read data from the Lite-On LTR390 sensor. This sensor is low power, tiny and relatively easy to work with. It's sensitive to 2 relatively narrow bands of light (500-600nm and 300-350nm). It can give you an indication of the intensity (accumulated photons over a time period).
<br>
As has become common with my Arduino libraries, I use my BitBang_I2C library (https://github.com/bitbank2/BitBang_I2C) to simplify communication with I2C devices. It provides a wrapper layer for hardware I2C and, using the same API, can bit bang the protocol on any GPIO pins. This allows this library to work on a wide variety of MCUs in an almost infinite set of connected configurations.
<br>
See the Wiki for help getting started<br>
https://github.com/bitbank2/bb_ltr390/wiki <br>
<br>
<br>

If you find this code useful, please consider sending a donation or becomming a Github sponsor.

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=SR4F44J2UR8S4)

