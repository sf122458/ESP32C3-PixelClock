基于ESP32-C3的仿AWTRIX像素时钟
--------------------------------------------------

### 1.介绍
AWTRIX开源项目：[AWTRIX2.0-Controller](https://github.com/awtrix/AWTRIX2.0-Controller)
AWTRIX因为ESP8266性能限制，在2.0版本采取了部署服务器并让时钟作为接收端的解决方案，这一方案赋予了AWTRIX显示内容的多样性。AWTRIX2.0可支持固件烧录，对于初学者友好，时钟的硬件只需要一个ESP8266以及WS2812B的灯带。此外需要一个服务器全天候将数据传递给AWTRIX。

然而服务器的部署也造成了限制，可以尝试添加一些模块实现可不依赖服务器工作的像素时钟。

### 2.材料
- ESP32-C3，项目中使用的是合宙的ESP32C3开发板 **(9块9包邮)**
- WS2812B 32*8 LED矩阵
- DHT11温湿度传感器
- DS1302时钟模块
- MAX4466麦克风模块
- 按键
- 外壳以及格栅可使用AWTRIX提供的3D文件打印，**其中格栅是形成像素风格的关键部分**，但是以我的踩坑经验，外发3D打印的成品质量参差不齐，同时价格高昂。解决方案之一是外壳使用微积木拼搭节约成本 **(但是废手)**，以及格栅可以上咸鱼低价淘一个。
- 硫酸纸或者灯箱片，搭配格栅实现像素风格
- 透明PVC薄片或亚克力板，作为最外层面板

### 3.编程环境
VSCode、PlatformIO插件，使用Arduino程序框架。如果采用合宙ESP32C3开发板可在PlatformIO的boards中选择AirM2M CORE ESP32C3。

依赖库包含：
```
adafruit/Adafruit NeoMatrix@^1.3.2
adafruit/Adafruit GFX Library@^1.11.9
adafruit/DHT sensor library@^1.4.6
adafruit/Adafruit Unified Sensor@^1.1.14
adafruit/Adafruit NeoPixel@^1.12.0
bblanchon/ArduinoJson@^7.0.2
thomasfredericks/Bounce2@^2.72
kosme/arduinoFFT@^1.6.2
```
此外还使用了GitHub上的DS1302库、uzlib库。
[arduino-ds1302](https://github.com/msparks/arduino-ds1302)
[uzlib](https://github.com/pfalcon/uzlib)


### 4.功能
通过按键进行功能切换：
- 时间显示：每次启动时使用NTP更新DS1302内部时间，后续读取DS1302内部时间并显示。由于DS1302的精度较高，只要经过一次校正，在无WIFI环境下仍可读取DS1302实现准确的时间显示。
- 天气预报：基于和风天气API，解析出今明两天天气与温度并显示，可自行增加`weather.h`文件中的`weather`类定义获得更多数据
- 温湿度显示：读取DHT11传感器数据，显示室内温湿度
- 音乐频谱：读取MAX4466麦克风数据，进行FFT处理后显示频谱图(ESP32C3不支持A2DP，无法实现蓝牙音箱功能)

### 5.发现的bug以及解决方法
- 当WIFI打开时，ESP32-C3驱动WS2812B的LED矩阵时，会出现偶尔的非正常闪烁现象。后来查找发现ESP32-C3开启WIFI时，驱动WS2812B的时序会产生毛刺造成干扰，具体可见[ESP32 corrupted data within rmt device](https://github.com/FastLED/FastLED/issues/719)。结合实现功能，因此只在获取天气数据时打开WIFI，其余情况下关闭。也算一种解决方法了。

### 6.部分代码参考
https://gitee.com/adamhxx/arduino-open-source/tree/master/ESP32C3-CLOCK
https://wokwi.com/projects/383363216212925441