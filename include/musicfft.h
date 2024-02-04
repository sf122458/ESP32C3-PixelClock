#include <Adafruit_NeoMatrix.h>
#include "arduinoFFT.h"

#define CHANNEL 1  //音频输入引脚
#define xres 32   
#define yres 8            

arduinoFFT FFT = arduinoFFT(); //创建FFT对象

const uint16_t SAMPLES = 64; //采样点数，必须为2的整数次幂
const double samplingFrequency = 4000; //Hz, 声音采样频率

unsigned int sampling_period_us;
unsigned long microseconds;
unsigned long lastTime = 0;
unsigned long fallingTime = 0;

double vReal[SAMPLES]; //FFT采样输入样本数组
double vImag[SAMPLES]; //FFT运算输出数组
int freq_gain2[xres] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
int Intensity[xres] = {}; // initialize Frequency Intensity to zero
int FallingPoint[xres] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int Displacement = 1;          // Create LED Object


void getSamples(){
  microseconds = micros();
  for(int i = 0; i < SAMPLES; i++){
    vReal[i] = analogRead(CHANNEL);
    vImag[i] = 0;
    microseconds += sampling_period_us;
  }
  bool reduce = false;
  if ((millis() - lastTime) > 16) {
    lastTime = millis();
    reduce = true;
  }
  //FFT
  FFT.Windowing(vReal, 1, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
  //Update Intensity Array  
  int t = 16;
  for(int i = t; i < (xres*Displacement)+t; i+=Displacement){
    vReal[i] = constrain(vReal[i], 0 ,3596);            // set max value for input data
    vReal[i] = map(vReal[i], freq_gain2[(i-t)/Displacement], 1548, 0, yres);        // map data to fit our display
    if(reduce){
      Intensity[(i-t)/Displacement] --;                      // Decrease displayed value
    }
    if (vReal[i] > Intensity[(i-t)/Displacement])          // Match displayed value to measured value
      Intensity[(i-t)/Displacement] = vReal[i];
  }
}

void drawYLine(Adafruit_NeoMatrix *matrix, int16_t x, int16_t y, int16_t h, int16_t c){
  for(int i=y;i<y+h;i++){
    matrix->drawPixel(x,7 - i,c);
  }
}

uint16_t hsv2rgb2(Adafruit_NeoMatrix *matrix, uint16_t hue, uint8_t saturation, uint8_t value)
{
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    uint16_t hi = (hue / 60) % 6;
    uint16_t F = 100 * hue / 60 - 100 * hi;
    uint16_t P = value * (100 - saturation) / 100;
    uint16_t Q = value * (10000 - F * saturation) / 10000;
    uint16_t T = value * (10000 - saturation * (100 - F)) / 10000;

    switch (hi)
    {
    case 0:
        red = value;
        green = T;
        blue = P;
        break;
    case 1:
        red = Q;
        green = value;
        blue = P;
        break;
    case 2:
        red = P;
        green = value;
        blue = T;
        break;
    case 3:
        red = P;
        green = Q;
        blue = value;
        break;
    case 4:
        red = T;
        green = P;
        blue = value;
        break;
    case 5:
        red = value;
        green = P;
        blue = Q;
        break;
    default:
        return matrix->Color(255, 0, 0);
    }
    red = red * 255 / 100;
    green = green * 255 / 100;
    blue = blue * 255 / 100;
    return matrix->Color(red, green, blue);
}

void displayUpdate(Adafruit_NeoMatrix *matrix, int displayPattern){
  int color = 0;
  switch(displayPattern){
    case 0:
      for(int i = 0; i < xres; i++){
        drawYLine(matrix,i,yres-Intensity[i],Intensity[i],hsv2rgb2(matrix, color, 80, 80 ));
        drawYLine(matrix,i,0,yres-1-Intensity[i],hsv2rgb2(matrix, color, 80, 80 ));
        color += 360/xres;
      }
      break;
    case 1:
      if ((millis() - fallingTime) > 130) {
        for(int i = 0; i < xres; i++){
          if(FallingPoint[i]>0){
            FallingPoint[i]--;
          }
        }
        fallingTime = millis();
      }
      for(int i = 0; i < xres; i++){
        drawYLine(matrix,i,0,yres-1,matrix->Color(0,0,0));
        if(FallingPoint[i]<Intensity[i]){
          FallingPoint[i] = Intensity[i];
        }
        drawYLine(matrix,i,yres-Intensity[i]+1,Intensity[i]-1,hsv2rgb2(matrix, color, 80, 80 ));
        if(FallingPoint[i]>0){
          matrix->drawPixel(i,yres-FallingPoint[i],matrix->Color(255,255,255));
        }
        color += 360/xres;
      }
      break;
    case 2:
      for(int i = 0; i < xres; i++){
        drawYLine(matrix,i,0,yres,matrix->Color(0,0,0));
        drawYLine(matrix,i,0,Intensity[i]+1,hsv2rgb2(matrix, color, 80, 80 ));
        color += 360/xres;
      }
      break;
  }
}
