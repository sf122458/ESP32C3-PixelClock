#include "anim.h"

//数字雨
int columnIndex[8];
int stepIndex[8];
int glow[8];
bool Running[8]={false, false, false, false, false, false, false, false};
int stepsTillNext[8] = {0, 6, 2, 11, 4, 10, 7, 1};

//温度计动画
uint8_t tempindex = 0;

//显示信息
void ShowMessage(Adafruit_NeoMatrix *matrix, char *message, int len, uint32_t color)
{
  matrix->clear();
  matrix->setTextColor(color);
  if (len < 1 || len > 8)
    return;
  matrix->setCursor(1 + (8 - len) * 2, 5); //设置鼠标光标的位置用以确定在哪里显示文字

  matrix->print(message);
  matrix->show();
}

//显示Wifi搜索动画
void WifiSearchAnim(Adafruit_NeoMatrix *matrix, int x, int y)
{
  for (int i = 0; i < 4; i++)
  {
    ShowMessage(matrix, "WiFi", 5, matrix->Color(255, 255, 255));
    switch (i)
    {
      case 3:
        matrix->drawPixel(x, y, (uint32_t)0x22ff);
        matrix->drawPixel(x + 1, y + 1, (uint32_t)0x22ff);
        matrix->drawPixel(x + 2, y + 2, (uint32_t)0x22ff);
        matrix->drawPixel(x + 3, y + 3, (uint32_t)0x22ff);
        matrix->drawPixel(x + 2, y + 4, (uint32_t)0x22ff);
        matrix->drawPixel(x + 1, y + 5, (uint32_t)0x22ff);
        matrix->drawPixel(x, y + 6, (uint32_t)0x22ff);
      case 2:
        matrix->drawPixel(x - 1, y + 2, (uint32_t)0x22ff);
        matrix->drawPixel(x, y + 3, (uint32_t)0x22ff);
        matrix->drawPixel(x - 1, y + 4, (uint32_t)0x22ff);
      case 1:
        matrix->drawPixel(x - 3, y + 3, (uint32_t)0x22ff);
      case 0:
        break;
    }
    matrix->show();
    delay(100);
    matrix->clear();
  }
}

//显示Wifi连接成功动画
void WifiCheckAnim(Adafruit_NeoMatrix *matrix, int x, int y)
{
  int wifiCheckTime = millis();
  int wifiCheckPoints = 0;
  matrix->setTextColor(matrix->Color(255, 255, 255));

  while (millis() - wifiCheckTime < 2000)
  {
    while (wifiCheckPoints < 7)
    {
      ShowMessage(matrix, "WiFi", 5, matrix->Color(255, 255, 255));
      switch (wifiCheckPoints)
      {
        case 6:
          matrix->drawPixel(x, y, matrix->Color(0, 0, 255));
        case 5:
          matrix->drawPixel(x - 1, y + 1, matrix->Color(0, 0, 255));
        case 4:
          matrix->drawPixel(x - 2, y + 2, matrix->Color(0, 0, 255));
        case 3:
          matrix->drawPixel(x - 3, y + 3, matrix->Color(0, 0, 255));
        case 2:
          matrix->drawPixel(x - 4, y + 4, matrix->Color(0, 0, 255));
        case 1:
          matrix->drawPixel(x - 5, y + 3, matrix->Color(0, 0, 255));
        case 0:
          matrix->drawPixel(x - 6, y + 2, matrix->Color(0, 0, 255));
          break;
      }
      wifiCheckPoints++;
      matrix->show();
      delay(100);
    }
  }
}

//根据和风天气API返回天气标识
void WeatherIcon(Adafruit_NeoMatrix *matrix, int x, int y, int weatherid)
{
    switch(weatherid)
    {
        case 100://白天晴
            matrix->drawPixel(x+2, y, matrix->Color(248,213,104));
            matrix->drawPixel(x+5, y, matrix->Color(248,213,104));
            matrix->drawPixel(x, y+2, matrix->Color(248,213,104));
            matrix->drawPixel(x, y+5, matrix->Color(248,213,104));
            matrix->drawPixel(x+2, y+7, matrix->Color(248,213,104));
            matrix->drawPixel(x+5, y+7, matrix->Color(248,213,104));
            matrix->drawPixel(x+7, y+2, matrix->Color(248,213,104));
            matrix->drawPixel(x+7, y+5, matrix->Color(248,213,104));
            for(int i = 2; i < 6; i++)
            {
                for(int j = 2; j < 6; j++)
                {
                    matrix->drawPixel(x+i, y+j, matrix->Color(248,213,104));
                }
            }
            matrix->drawPixel(x+2, y+2, matrix->Color(200,200,200));
            matrix->drawPixel(x+5, y+2, matrix->Color(200,200,200));
            matrix->drawPixel(x+2, y+5, matrix->Color(200,200,200));
            matrix->drawPixel(x+5, y+5, matrix->Color(200,200,200));
            break;
        case 101://多云
        case 102://少云
        case 151://夜间多云
        case 152://夜间少云
        case 103://晴间多云
            matrix->drawPixel(x+2, y, matrix->Color(248,213,104));
            matrix->drawPixel(x+5, y, matrix->Color(248,213,104));
            matrix->drawPixel(x, y+2, matrix->Color(248,213,104));
            matrix->drawPixel(x+7, y+2, matrix->Color(248,213,104));
            matrix->drawPixel(x+2, y+2, matrix->Color(127,127,127));
            matrix->drawPixel(x+5, y+2, matrix->Color(127,127,127));
            matrix->drawPixel(x+3, y+2, matrix->Color(248,213,104));
            matrix->drawPixel(x+4, y+2, matrix->Color(248,213,104));
            matrix->drawPixel(x+2, y+3, matrix->Color(248,213,104));
            matrix->drawPixel(x+3, y+3, matrix->Color(248,213,104));
            matrix->drawPixel(x+4, y+3, matrix->Color(248,213,104));
            matrix->drawPixel(x+3, y+4, matrix->Color(248,213,104));
            matrix->drawPixel(x+3, y+5, matrix->Color(248,213,104));

            matrix->drawPixel(x+5, y+3, matrix->Color(200,200,200));
            matrix->drawPixel(x+6, y+3, matrix->Color(200,200,200));
            for(int i = 0; i < 8; i++)
            {
                if(i != 3)
                {
                    matrix->drawPixel(x+i, y+4, matrix->Color(200,200,200));
                    matrix->drawPixel(x+i, y+5, matrix->Color(200,200,200));
                }
                matrix->drawPixel(x+i, y+6, matrix->Color(200,200,200));
                if(i != 0 && i != 7)
                {
                    matrix->drawPixel(x+i, y+7, matrix->Color(200,200,200));
                }
            }
            break;
        case 104://阴天
            matrix->drawPixel(x+5, y, matrix->Color(200,200,200));
            matrix->drawPixel(x+6, y, matrix->Color(200,200,200));
            for(int i = 0; i < 8; i++)
            {
                if(i != 3)
                {
                    matrix->drawPixel(x+i, y+1, matrix->Color(200,200,200));
                    matrix->drawPixel(x+i, y+2, matrix->Color(200,200,200));
                }
                matrix->drawPixel(x+i, y+3, matrix->Color(200,200,200));
                if(i != 0 && i != 7)
                {
                    matrix->drawPixel(x+i, y+4, matrix->Color(200,200,200));
                }
            }
            break;
        case 150://夜间晴朗
            matrix->drawPixel(x+2, y, matrix->Color(248,213,104));
            matrix->drawPixel(x+1, y+1, matrix->Color(248,213,104));
            matrix->drawPixel(x+2, y+1, matrix->Color(248,213,104));
            //matrix->drawPixel(x, y+3, matrix->Color(248,213,104));
            matrix->drawPixel(x+1, y+2, matrix->Color(248,213,104));
            matrix->drawPixel(x+2, y+2, matrix->Color(248,213,104));
            matrix->drawPixel(x+1, y+3, matrix->Color(248,213,104));
            matrix->drawPixel(x+2, y+3, matrix->Color(248,213,104));
            matrix->drawPixel(x+3, y+3, matrix->Color(248,213,104));
            matrix->drawPixel(x+1, y+4, matrix->Color(248,213,104));
            matrix->drawPixel(x+2, y+4, matrix->Color(248,213,104));
            matrix->drawPixel(x+3, y+4, matrix->Color(248,213,104));
            matrix->drawPixel(x+4, y+4, matrix->Color(248,213,104));
            matrix->drawPixel(x+2, y+5, matrix->Color(248,213,104));
            matrix->drawPixel(x+3, y+5, matrix->Color(248,213,104));
            matrix->drawPixel(x+4, y+5, matrix->Color(248,213,104));
            matrix->drawPixel(x+5, y+5, matrix->Color(248,213,104));
            matrix->drawPixel(x+6, y+5, matrix->Color(248,213,104));
            matrix->drawPixel(x+7, y+5, matrix->Color(248,213,104));
            matrix->drawPixel(x+3, y+6, matrix->Color(248,213,104));
            matrix->drawPixel(x+4, y+6, matrix->Color(248,213,104));
            matrix->drawPixel(x+5, y+6, matrix->Color(248,213,104));
            matrix->drawPixel(x+6, y+6, matrix->Color(248,213,104));
            //matrix->drawPixel(x+4, y+7, matrix->Color(248,213,104));
            matrix->drawPixel(x+3, y, matrix->Color(248,213,104));
            matrix->drawPixel(x+7, y+4, matrix->Color(248,213,104));
            break;
        case 153://夜间晴间多云
            matrix->drawPixel(x+2, y, matrix->Color(248,213,104));
            matrix->drawPixel(x+1, y+1, matrix->Color(248,213,104));
            matrix->drawPixel(x+2, y+1, matrix->Color(248,213,104));
            matrix->drawPixel(x, y+3, matrix->Color(248,213,104));
            matrix->drawPixel(x+1, y+2, matrix->Color(248,213,104));
            matrix->drawPixel(x+2, y+2, matrix->Color(248,213,104));
            matrix->drawPixel(x+1, y+3, matrix->Color(248,213,104));
            matrix->drawPixel(x+2, y+3, matrix->Color(248,213,104));
            matrix->drawPixel(x+3, y+3, matrix->Color(248,213,104));
            matrix->drawPixel(x+3, y+4, matrix->Color(248,213,104));
            matrix->drawPixel(x+3, y+5, matrix->Color(248,213,104));
            matrix->drawPixel(x+3, y, matrix->Color(248,213,104));

            matrix->drawPixel(x+5, y+3, matrix->Color(200,200,200));
            matrix->drawPixel(x+6, y+3, matrix->Color(200,200,200));
            for(int i = 0; i < 8; i++)
            {
                if(i != 3)
                {
                    matrix->drawPixel(x+i, y+4, matrix->Color(200,200,200));
                    matrix->drawPixel(x+i, y+5, matrix->Color(200,200,200));
                }
                matrix->drawPixel(x+i, y+6, matrix->Color(200,200,200));
                if(i != 0 && i != 7)
                {
                    matrix->drawPixel(x+i, y+7, matrix->Color(200,200,200));
                }
            }
            break;
        case 305://小雨
        case 309://毛毛雨/细雨
            matrix->drawPixel(x+5, y, matrix->Color(200,200,200));
            matrix->drawPixel(x+6, y, matrix->Color(200,200,200));
            for(int i = 0; i < 8; i++)
            {
                if(i != 3)
                {
                    matrix->drawPixel(x+i, y+1, matrix->Color(200,200,200));
                    matrix->drawPixel(x+i, y+2, matrix->Color(200,200,200));
                }
                matrix->drawPixel(x+i, y+3, matrix->Color(200,200,200));
                if(i != 0 && i != 7)
                {
                    matrix->drawPixel(x+i, y+4, matrix->Color(200,200,200));
                }
            }
            matrix->drawPixel(x+4, y+5, matrix->Color(137,207,240));
            matrix->drawPixel(x+3, y+7, matrix->Color(137,207,240));
            break;
        case 300://阵雨
        case 350://夜间阵雨
        case 306://中雨
        case 399://雨
        case 313://冻雨
        case 314://小到中雨
            matrix->drawPixel(x+5, y, matrix->Color(200,200,200));
            matrix->drawPixel(x+6, y, matrix->Color(200,200,200));
            for(int i = 0; i < 8; i++)
            {
                if(i != 3)
                {
                    matrix->drawPixel(x+i, y+1, matrix->Color(200,200,200));
                    matrix->drawPixel(x+i, y+2, matrix->Color(200,200,200));
                }
                matrix->drawPixel(x+i, y+3, matrix->Color(200,200,200));
                if(i != 0 && i != 7)
                {
                    matrix->drawPixel(x+i, y+4, matrix->Color(200,200,200));
                }
            }
            matrix->drawPixel(x+3, y+5, matrix->Color(137,207,240));
            matrix->drawPixel(x+2, y+7, matrix->Color(137,207,240));
            matrix->drawPixel(x+5, y+5, matrix->Color(137,207,240));
            matrix->drawPixel(x+4, y+7, matrix->Color(137,207,240));
            break;
        case 301://强阵雨
        case 351://夜间强阵雨
        case 307://大雨
        case 308://极端降雨
        case 310://暴雨
        case 311://大暴雨
        case 312://特大暴雨
        case 315://中到大雨
        case 316://大到暴雨
        case 317://暴雨到大暴雨
        case 318://大暴雨到特大暴雨
            matrix->drawPixel(x+5, y, matrix->Color(200,200,200));
            matrix->drawPixel(x+6, y, matrix->Color(200,200,200));
            for(int i = 0; i < 8; i++)
            {
                if(i != 3)
                {
                    matrix->drawPixel(x+i, y+1, matrix->Color(200,200,200));
                    matrix->drawPixel(x+i, y+2, matrix->Color(200,200,200));
                }
                matrix->drawPixel(x+i, y+3, matrix->Color(200,200,200));
                if(i != 0 && i != 7)
                {
                    matrix->drawPixel(x+i, y+4, matrix->Color(200,200,200));
                }
            }
            matrix->drawPixel(x+4, y+5, matrix->Color(137,207,240));
            matrix->drawPixel(x+3, y+7, matrix->Color(137,207,240));
            matrix->drawPixel(x+2, y+5, matrix->Color(137,207,240));
            matrix->drawPixel(x+1, y+7, matrix->Color(137,207,240));
            matrix->drawPixel(x+6, y+5, matrix->Color(137,207,240));
            matrix->drawPixel(x+5, y+7, matrix->Color(137,207,240));
            break;
        case 302://雷阵雨
            matrix->drawPixel(x+5, y, matrix->Color(200,200,200));
            matrix->drawPixel(x+6, y, matrix->Color(200,200,200));
            for(int i = 0; i < 8; i++)
            {
                if(i != 3)
                {
                    matrix->drawPixel(x+i, y+1, matrix->Color(200,200,200));
                    matrix->drawPixel(x+i, y+2, matrix->Color(200,200,200));
                }
                matrix->drawPixel(x+i, y+3, matrix->Color(200,200,200));
                if(i != 0 && i != 7)
                {
                    matrix->drawPixel(x+i, y+4, matrix->Color(200,200,200));
                }
            }

            matrix->drawPixel(x+4, y+5, matrix->Color(250,250,55));
            matrix->drawPixel(x+4, y+6, matrix->Color(250,250,55));
            matrix->drawPixel(x+3, y+6, matrix->Color(250,250,55));
            matrix->drawPixel(x+3, y+7, matrix->Color(250,250,55));
            break;
        case 303://强雷阵雨
            matrix->drawPixel(x+5, y, matrix->Color(200,200,200));
            matrix->drawPixel(x+6, y, matrix->Color(200,200,200));
            for(int i = 0; i < 8; i++)
            {
                if(i != 3)
                {
                    matrix->drawPixel(x+i, y+1, matrix->Color(200,200,200));
                    matrix->drawPixel(x+i, y+2, matrix->Color(200,200,200));
                }
                matrix->drawPixel(x+i, y+3, matrix->Color(200,200,200));
                if(i != 0 && i != 7)
                {
                    matrix->drawPixel(x+i, y+4, matrix->Color(200,200,200));
                }
            }
            matrix->drawPixel(x+2, y+5, matrix->Color(250,250,55));
            matrix->drawPixel(x+2, y+6, matrix->Color(250,250,55));
            matrix->drawPixel(x+1, y+6, matrix->Color(250,250,55));
            matrix->drawPixel(x+1, y+7, matrix->Color(250,250,55));
            
            matrix->drawPixel(x+6, y+5, matrix->Color(250,250,55));
            matrix->drawPixel(x+6, y+6, matrix->Color(250,250,55));
            matrix->drawPixel(x+5, y+6, matrix->Color(250,250,55));
            matrix->drawPixel(x+5, y+7, matrix->Color(250,250,55));
            break;

        //雪天
        case 400://小雪
        case 401://中雪
        case 402://大雪
        case 403://暴雪
        case 404://雨夹雪
        case 405://雨雪天气
        case 406://阵雨夹雪
        case 407://阵雨夹雪
        case 408://小到中雪
        case 409://中到大雪
        case 410://雪
        case 456://阵雨夹雪
        case 457://阵雪
        case 499://雪
            matrix->drawPixel(x+2, y, matrix->Color(200,200,200));
            matrix->drawPixel(x+5, y, matrix->Color(200,200,200));
            matrix->drawPixel(x+2, y+7, matrix->Color(200,200,200));
            matrix->drawPixel(x+5, y+7, matrix->Color(200,200,200));
            matrix->drawPixel(x, y+2, matrix->Color(200,200,200));
            matrix->drawPixel(x, y+5, matrix->Color(200,200,200));
            matrix->drawPixel(x+7, y+2, matrix->Color(200,200,200));
            matrix->drawPixel(x+7, y+5, matrix->Color(200,200,200));
            matrix->drawPixel(x+2, y+2, matrix->Color(200,200,200));
            matrix->drawPixel(x+2, y+5, matrix->Color(200,200,200));
            matrix->drawPixel(x+5, y+2, matrix->Color(200,200,200));
            matrix->drawPixel(x+5, y+5, matrix->Color(200,200,200));
            for(int i=3; i<5; i++)
            {
                matrix->drawPixel(x+i, y+1, matrix->Color(200,200,200));
                matrix->drawPixel(x+i, y+6, matrix->Color(200,200,200));
            }
            for(int i=1; i<7; i++)
            {
                if(i != 2 && i != 5)
                {
                    matrix->drawPixel(x+i, y+3, matrix->Color(200,200,200));
                    matrix->drawPixel(x+i, y+4, matrix->Color(200,200,200));
                }
            }
            break;

        //雾霾
        case 500://薄雾
        case 501://雾
        case 509://浓雾
        case 510://强浓雾
        case 514://大雾
        case 515://特强浓雾
            matrix->drawPixel(x+2, y, matrix->Color(173,216,230));
            matrix->drawPixel(x+5, y, matrix->Color(173,216,230));
            matrix->drawPixel(x+6, y, matrix->Color(173,216,230));
            matrix->drawPixel(x+7, y, matrix->Color(173,216,230));
            matrix->drawPixel(x, y+1, matrix->Color(173,216,230));
            matrix->drawPixel(x+1, y+1, matrix->Color(173,216,230));
            matrix->drawPixel(x+2, y+1, matrix->Color(173,216,230));
            matrix->drawPixel(x+5, y+1, matrix->Color(173,216,230));
            matrix->drawPixel(x+7, y+1, matrix->Color(173,216,230));
            matrix->drawPixel(x+7, y+2, matrix->Color(173,216,230));
            for(int i=0; i<8; i++)
                matrix->drawPixel(x+i, y+3, matrix->Color(173,216,230));
            for(int i=0; i<6; i++)
                matrix->drawPixel(x+i, y+5, matrix->Color(173,216,230));
            matrix->drawPixel(x+5, y+6, matrix->Color(173,216,230));
            matrix->drawPixel(x+3, y+7, matrix->Color(173,216,230));
            matrix->drawPixel(x+4, y+7, matrix->Color(173,216,230));
            matrix->drawPixel(x+5, y+7, matrix->Color(173,216,230));
            break;
        case 502://霾
        case 511://中度霾
        case 512://重度霾
        case 513://严重霾
            for(int i=1; i<7; i++)
            {
                if(i!=2 && i!=5)
                {
                    matrix->drawPixel(x+i,y,matrix->Color(212,0,0));
                    matrix->drawPixel(x+i,y+6,matrix->Color(212,0,0));
                }
            }
            for(int i=2; i<6; i++)
            {
                if(i!=3 && i!=4)
                {
                    matrix->drawPixel(x+i,y+2,matrix->Color(212,0,0));
                    matrix->drawPixel(x+i,y+4,matrix->Color(212,0,0));
                }
            }
            for(int i=1; i<7; i++)
            {
                if(i!=2 && i!=5)
                matrix->drawPixel(x+i,y+3,matrix->Color(212,0,0));
            }
            break;
    }
}

uint16_t hsv2rgb(Adafruit_NeoMatrix *matrix, uint16_t hue, uint8_t saturation, uint8_t value)
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

//数字雨
void movingPixel(Adafruit_NeoMatrix *matrix, int x, int y, int colorh)
{
    int pixel = y;
    matrix->drawPixel(x, pixel, hsv2rgb(matrix, colorh, 100, 100));
    if(pixel-1 >= 0){ matrix->drawPixel(x, pixel-1, hsv2rgb(matrix, colorh, 80, 80 ));}
    if(pixel-2 >= 0){ matrix->drawPixel(x, pixel-2, hsv2rgb(matrix, colorh, 70, 70 ));}
    if(pixel-3 >= 0){ matrix->drawPixel(x, pixel-3, hsv2rgb(matrix, colorh, 45, 45 ));}
    if(pixel-4 >= 0){ matrix->drawPixel(x, pixel-4, hsv2rgb(matrix, colorh, 25, 25 ));}
    if(pixel-5 >= 0){ matrix->drawPixel(x, pixel-5, hsv2rgb(matrix, colorh, 10, 10 ));}
    if(pixel-6 >= 0){ matrix->drawPixel(x, pixel-6, matrix->Color(0, 0, 0));}
}

void showRainingCode(Adafruit_NeoMatrix *matrix) 
{
    for(int i=0; i<8; i++)
    {
        if(stepIndex[i] > stepsTillNext[i])
        {
            Running[i] = true;
            stepsTillNext[i] = 13;  // 进行数组初始化
            columnIndex[i] = random((i*4), ((i+1)*4));
            glow[i] = random(0, 2);     //随机背景置灰值
            stepIndex[i] = 0;
        }
        if(Running[i] == true)
        {
            movingPixel(matrix, columnIndex[i], stepIndex[i], 240);
            if(stepIndex[i] == 13)
            {
                Running[i] = false;
            }
        }
        stepIndex[i] += 1;
    }
}

//温度计图标
void TempIcon(Adafruit_NeoMatrix *matrix, int x, int y)
{
    for(int i=5; i<7; i++)
    {
        matrix->drawPixel(x, y+i, matrix->Color(127, 127, 127));
        matrix->drawPixel(x+4, y+i, matrix->Color(127, 127, 127));
    }
    for(int i=0; i<8; i++)
    {
        if(i!=5&&i!=6)
        {
            matrix->drawPixel(x+1, y+i, matrix->Color(127, 127, 127));
            matrix->drawPixel(x+3, y+i, matrix->Color(127, 127, 127));
        }
    }
    matrix->drawPixel(x+2, y+7, matrix->Color(127, 127, 127));
    matrix->drawPixel(x+2, y, matrix->Color(127, 127, 127));
    matrix->drawPixel(x+5, y, matrix->Color(127, 127, 127));
    matrix->drawPixel(x+6, y, matrix->Color(127, 127, 127));
    matrix->drawPixel(x+5, y+1, matrix->Color(127, 127, 127));
    matrix->drawPixel(x+5, y+2, matrix->Color(127, 127, 127));
    matrix->drawPixel(x+6, y+2, matrix->Color(127, 127, 127));
    for(int i=1;i<4;i++)
    {
        for(int j=5;j<7;j++)
            matrix->drawPixel(x+i, y+j, matrix->Color(255, 0, 0));
    }
    switch(tempindex)
    {
        case 1:
        case 3:
            matrix->drawPixel(x+2,y+4, matrix->Color(255, 0, 0));
            break;
        case 2:
            matrix->drawPixel(x+2,y+3, matrix->Color(255, 0, 0));
            matrix->drawPixel(x+2,y+4, matrix->Color(255, 0, 0));
            break;
        case 0:
            break;
    }
    tempindex = (tempindex+1) % 4;
}

//湿度图标
void HumidIcon(Adafruit_NeoMatrix *matrix, int x, int y)
{
    matrix->drawPixel(x+2,y+2,matrix->Color(231,255,255));
    matrix->drawPixel(x+1,y+2,matrix->Color(231,255,255));
    matrix->drawPixel(x+1,y+3,matrix->Color(231,255,255));
    matrix->drawPixel(x,y+4,matrix->Color(231,255,255));
    matrix->drawPixel(x+2,y,matrix->Color(0,230,255));
    matrix->drawPixel(x+2,y+1,matrix->Color(0,230,255));
    matrix->drawPixel(x+3,y+2,matrix->Color(0,230,255));
    matrix->drawPixel(x+2,y+3,matrix->Color(0,230,255));
    matrix->drawPixel(x+3,y+3,matrix->Color(0,230,255));
    matrix->drawPixel(x+1,y+4,matrix->Color(0,230,255));
    matrix->drawPixel(x+2,y+4,matrix->Color(0,230,255));
    matrix->drawPixel(x+3,y+4,matrix->Color(0,230,255));
    matrix->drawPixel(x,y+5,matrix->Color(0,230,255));
    matrix->drawPixel(x+1,y+5,matrix->Color(0,230,255));
    matrix->drawPixel(x+2,y+5,matrix->Color(0,230,255));
    matrix->drawPixel(x+3,y+5,matrix->Color(0,230,255));
    matrix->drawPixel(x,y+6,matrix->Color(0,230,255));
    matrix->drawPixel(x+1,y+6,matrix->Color(0,230,255));
    matrix->drawPixel(x+2,y+6,matrix->Color(0,230,255));
    matrix->drawPixel(x+4,y+4,matrix->Color(30,170,255));
    matrix->drawPixel(x+4,y+5,matrix->Color(30,170,255));
    matrix->drawPixel(x+4,y+6,matrix->Color(30,170,255));
    matrix->drawPixel(x+3,y+6,matrix->Color(30,170,255));
    matrix->drawPixel(x+1,y+7,matrix->Color(30,170,255));
    matrix->drawPixel(x+2,y+7,matrix->Color(30,170,255));
    matrix->drawPixel(x+3,y+7,matrix->Color(30,170,255));
}

