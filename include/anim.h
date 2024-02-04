#ifndef _ANIM_H_
#define _ANIM_H_

#include <Adafruit_NeoMatrix.h>
#include "Arduino.h"

//显示消息
void ShowMessage(Adafruit_NeoMatrix *matrix, char *message, int len, uint32_t color);

//WIFI状态动画
void WifiSearchAnim(Adafruit_NeoMatrix *matrix, int x, int y);
void WifiCheckAnim(Adafruit_NeoMatrix *matrix, int x, int y);

//天气图标
void WeatherIcon(Adafruit_NeoMatrix *matrix, int x, int y, int weatherid);

//数字动画
void movingPixel(Adafruit_NeoMatrix *matrix, int x, int y, int colorh);
void showRainingCode(Adafruit_NeoMatrix *matrix);

//温度图标
void TempIcon(Adafruit_NeoMatrix *matrix, int x, int y);

//湿度图标
void HumidIcon(Adafruit_NeoMatrix *matrix, int x, int y);

#endif