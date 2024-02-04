#include <Arduino.h>
#include <WiFi.h>
#include <esp_sntp.h> //用于设定NTP成功回调函数
#include <HTTPClient.h> 

#include <Adafruit_NeoMatrix.h> //点亮LED矩阵需要的库
#include "my_font.h" //引用文字库头文件
#include <DS1302.h> //DS1302驱动库
#include <DHT.h> //温湿度传感器驱动库
#include "Bounce2.h"  //按键库
#include "weather.h"  //定义Weather类，用于解析Json数据并返回天气数据
#include "anim.h"   //包含部分动画以及图标
#include "musicfft.h" //音乐频谱库

//WIFI
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"

//计时模块DS1302相关定义
#define RST 10
#define DATA 3
#define CLK 2
DS1302 rtc(RST, DATA, CLK); //初始化DS1302模块

//DHT11温湿度传感器相关定义
#define DHTPIN 4 //传感器引脚连接4
#define DHTTYPE DHT11 //传感器支持DHT11, DHT12, DHT21, DHT22, AM2301
#define TIMEDHT 1000  //每秒钟更新一次温湿度

DHT dht(DHTPIN, DHTTYPE); //初始化温湿度传感器
uint32_t timerDHT = TIMEDHT; 
float humidity, temperature;

//像素阵列定义
#define kMatrixWidth   32             //宽度
#define kMatrixHeight  8              //高度
#define BRIGHTNESS     150            //默认亮度
#define BRIGHTNESS_INTERVAL 30        //亮度调节间隔
#define LED_PIN        8              //像素阵列引脚
uint8_t brightness = BRIGHTNESS;      //亮度
bool change = false;                  //亮度调节标志位,为True时才能改变亮度
bool dir = 0;

Adafruit_NeoMatrix *matrix;     //LED矩阵类指针

//和风天气预报
String UserKey = "xxxxx";       //密钥
String Location = "101020200";  //地点标识

Weather weather(UserKey, Location);
int weatherid;                //根据和风天气API返回的天气ID显示对应天气
bool is_day = 1;              //白天为1，夜晚为0
bool is_today = 1;            //今天为1，明天为0
bool getweather = false;      //为True时才能获取天气

//NTP相关定义
#define NTP_SERVER     "pool.ntp.org" //使用ntp.org作为NTP服务器，用以实现自动对时
#define UTC_OFFSET     3600 * 8   // 确定时区UTC+8
#define UTC_OFFSET_DST 0          // 用于设置夏时制
//WIFI连接重试数
#define WIFI_RETRY_COUNT  20

//时间相关定义
uint8_t hour, minute, second, week, month, day;
uint16_t year;
bool need_set_time = false; //是否需要重新设定时间，用于NTP同步成功时触发
struct tm tm_info;  //NTP设定时间值
int anim_index = 0; //用于动画显示

//界面内容显示
enum displaytype{timedisplay, weatherdisplay, roomparameter, raindisplay, music_fft}; //用于显示内容选择
displaytype display = music_fft; //初始化显示时间界面
uint8_t rain_count = 0;

//按键引脚定义
#define BUTTONPIN 19
Bounce button = Bounce();

void on_time_sync(struct timeval *tv)//NTP时间同步函数，在NTP同步时间时会被调用
{
  localtime_r(&tv->tv_sec, &tm_info);
  need_set_time = true;
}

void readRtc()//读取DS1302 RTC寄存器，获取准确时间
{
  Time tim = rtc.time();  //读取时间
  second = tim.sec;
  minute = tim.min;
  hour = tim.hr;
  week = tim.day;
  day = tim.date;
  month = tim.mon;
  year = tim.yr;
}

void readDHT() //温湿度传感器数值读取
{
  if ((millis() - timerDHT) > TIMEDHT) {
    timerDHT = millis();

    humidity = dht.readHumidity();

    temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
}

void InitLED_Matrix()//像素矩阵初始化
{
  //设置像素矩阵的方向以及排列方式
  matrix = new Adafruit_NeoMatrix(32, 8, LED_PIN, NEO_MATRIX_TOP + NEO_MATRIX_LEFT+
    NEO_MATRIX_COLUMNS +  NEO_MATRIX_ZIGZAG,
    NEO_GRB + NEO_KHZ800);
  matrix->setTextWrap(false); //设置文字是否自动换行
  matrix->setFont(&Picopixel);
  matrix->clear();
  matrix->setBrightness(BRIGHTNESS);
}

void weekBarDisplay(int week) //日期显示
{
  for (int i = 0; i < 7; i++)
  {
    if (i == 0 || i== 6)
      matrix->drawFastHLine(12 + 3 * i, 7, 2, matrix->Color(127, 0, 0));
    else
      matrix->drawFastHLine(12 + 3 * i, 7, 2, matrix->Color(127, 127, 127));
  }
  if (week == 0 || week == 6)  //周六、日显示为红色
    matrix->drawFastHLine(12 + 3 * week, 7, 2, matrix->Color(255, 0, 0));
  else
    matrix->drawFastHLine(12 + 3 * week, 7, 2, matrix->Color(255, 255, 255));
}

void dayBarDisplay(bool is_today) //今明日
{
  if(is_today)
  { 
    matrix->drawFastHLine(9, 7, 7, matrix->Color(255, 255, 255));
    matrix->drawFastHLine(22, 7, 7, matrix->Color(127, 127, 127));
  }
  else
  {
    matrix->drawFastHLine(22, 7, 7, matrix->Color(255, 255, 255));
    matrix->drawFastHLine(9, 7, 7, matrix->Color(127, 127, 127));
  }
}

void InitDS1302() //DS1302初始化
{
  rtc.writeProtect(false);  //关闭写保护
  rtc.halt(false);      //清除时钟停止标志
  Time t(tm_info.tm_year + 12, tm_info.tm_mon + 1, tm_info.tm_mday, tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec, static_cast<Time::Day>(tm_info.tm_wday));
  rtc.time(t);
}

void showFFT(){
    //进行采样
    getSamples();
    //更新频谱柱
    displayUpdate(matrix, 2);
}

void setup() 
{
  int count = 0;
  Serial.begin(115200);
  button.attach(BUTTONPIN, INPUT);  //按键按下高电平 
  button.interval(5);               //5ms周期消抖
  InitLED_Matrix();                 //初始化LED矩阵显示
  ShowMessage(matrix, "Hello!", 6, matrix->Color(255, 0, 255));
  delay(500);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  //尝试连接WIFI网络
  delay(500);
  //尝试连接直至超时
  while (WiFi.status() != WL_CONNECTED) 
  {
    count++;
    if (count > WIFI_RETRY_COUNT)
      break;
    WifiSearchAnim(matrix, 24, 0);
  }

  //如果WIFI正确连接，则从NTP服务器尝试自动对时
  if (WiFi.status() == WL_CONNECTED)
  {
    configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
    sntp_set_time_sync_notification_cb(on_time_sync);
    WifiCheckAnim(matrix, 27, 1);
    weather.update();
    weatherid = weather.getWeather(1,1);
  }
  else
  {
    ShowMessage(matrix, "OFFLINE", 7, matrix->Color(255, 255, 255));
  }
  
  WiFi.disconnect();
  delay(5);
}

void loop() 
{
  char disp_buf[40];

  button.update();    //更新按键状态

  if (need_set_time) //如果需要对时
  {
    need_set_time = false; //反转对时状态，并从ntp取回来时间并更新DS1302的时间
    InitDS1302();
  }

  if(minute % 20 == 19)
    getweather = 1;

  if(minute % 20 == 0 && getweather)  //getweather标志位是为了在每次到达20min时只更新一次天气数据，否则会导致内存溢出而重启
  {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(500);
    if(WiFi.status() == WL_CONNECTED)
    {
      weather.update();
      weatherid = weather.getWeather(is_today, is_day);
    }
    WiFi.disconnect();
    getweather = 0;
  }

  readRtc(); //将时间从DS1302中取出并显示
  readDHT(); //将温湿度参数取出并显示
  
  switch (display)//切换展示内容
  {
    case timedisplay://时间与今日天气
      //夜晚展示夜晚天气图标
      if(hour < 6 || hour >= 18)
        is_day = 0;
      else
        is_day = 1;
      //时间页展示今日天气
      is_today = 1;

      matrix->clear();
      matrix->setTextColor(matrix->Color(0, 0, 255));
      matrix->setCursor(13, 5);
      sprintf(disp_buf, "%02d:%02d\0", hour, minute);
      matrix->print(disp_buf);
      weekBarDisplay(week);
      WeatherIcon(matrix, 2, 0, weatherid);
      matrix->show();
      break;

    case weatherdisplay://今明日天气
      if(hour < 6 || hour >= 18)
        is_day = 0;
      else
        is_day = 1;
      matrix->clear();
      //显示最低与最高温度
      matrix->setTextColor(matrix->Color(255,140,0));
      matrix->setCursor(9,5);
      sprintf(disp_buf, "%02d", weather.getTempMin(is_today));
      matrix->print(disp_buf);
      matrix->setCursor(22,5);
      sprintf(disp_buf, "%02d", weather.getTempMax(is_today));
      matrix->print(disp_buf);

      //显示部分图标与文字
      matrix->drawPixel(17,3,matrix->Color(255,140,0));
      matrix->drawPixel(18,2,matrix->Color(255,140,0));
      matrix->drawPixel(19,3,matrix->Color(255,140,0));
      matrix->drawPixel(20,2,matrix->Color(255,140,0));

      matrix->drawPixel(30,3,matrix->Color(255,140,0));
      matrix->drawPixel(30,4,matrix->Color(255,140,0));
      matrix->drawPixel(30,5,matrix->Color(255,140,0));
      matrix->drawPixel(31,3,matrix->Color(255,140,0));
      matrix->drawPixel(31,5,matrix->Color(255,140,0));
      matrix->drawPixel(31,1,matrix->Color(255,140,0));
      
      //显示今明日标识
      dayBarDisplay(is_today);
      WeatherIcon(matrix, 0, 0, weather.getWeather(is_today, is_day));
      
      //在切换今明日天气显示时实现渐变效果
      if(second%20 == 0)
        change = 1;
      
      if(change)
      {
        switch(dir)
        {
          case 0:
            brightness -= BRIGHTNESS_INTERVAL;
            matrix->setBrightness(brightness);
            break;
          case 1:
            brightness += BRIGHTNESS_INTERVAL;
            matrix->setBrightness(brightness);
            break;
        }
        if(brightness == 0)
        {
          dir = 1;
          is_today = 1 - is_today;
        }
        else if(brightness == BRIGHTNESS)
        {
          dir = 0;
          change = 0;
          delay(1000);  //保证代码执行完不在每20s的条件内
        }
      }  
      matrix->show();
      break;

    case roomparameter://室内温湿度,由DHT11读取
      matrix->clear();

      //显示温湿度图标
      TempIcon(matrix,1,0);
      HumidIcon(matrix,17,0);

      //显示DHT11记录的温湿度
      matrix->setCursor(9,5);
      sprintf(disp_buf, "%02.0d", (int)temperature);
      matrix->print(disp_buf);
      matrix->setCursor(23,5);
      sprintf(disp_buf, "%02.0d", (int)humidity);
      matrix->print(disp_buf);
      matrix->show();
      break;

    case raindisplay://数字雨动画
      ++rain_count;
      if(rain_count >= 5)
      {
        matrix->clear();
        showRainingCode(matrix);
        matrix->show();
        rain_count = 0;
      }
      break;

    case music_fft://音乐频谱,由MAX4466接收声音并进行FFT显示
      matrix->clear();
      showFFT();
      matrix->show();
      break;
  }

  if(button.rose())//根据按键是否释放确定状态转换
  {
    switch(display)
    {
      case timedisplay:
        display = weatherdisplay;
        break;

      case weatherdisplay:
        display = roomparameter;
        is_today = 1;
        matrix->setBrightness(BRIGHTNESS);
        dir = 0;
        change = 0;
        break;

      case roomparameter:
        display = raindisplay;
        break;

      case raindisplay:
        display = music_fft;
        break;

      case music_fft:
        display = timedisplay;
        break;
    }
  }

  delay(50);
}
