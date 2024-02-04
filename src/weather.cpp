#include "weather.h"

Weather::Weather(String apiKey, String location)
{
    this->apiKey = apiKey;
    this->location = location;
}

bool Weather::update()
{
    HTTPClient http;   //用于访问网络
    WiFiClient *stream;
    int size;
    
    http.begin("https://devapi.qweather.com/v7/weather/3d?location="+ this->location + "&key=" + this->apiKey); //获取近三天天气信息
    int httpcode = http.GET();   //发送GET请求
    if(httpcode > 0)
    {
        if(httpcode == HTTP_CODE_OK)
        {
            stream = http.getStreamPtr();   //读取服务器返回的数据
            size = http.getSize();
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpcode).c_str());
    }
    http.end();   //结束当前连接
    
    uint8_t inbuff[size];
    stream->readBytes(inbuff, size);
    uint8_t *outbuf = NULL;
    uint32_t out_size = 0;
    int result = ArduinoUZlib::decompress(inbuff,size, outbuf, out_size);
    //Serial.write(outbuf, out_size);

    deserializeJson(doc, outbuf);
    today = doc["daily"][0].as<JsonObject>();
    tomorrow = doc["daily"][1].as<JsonObject>();
    return true;
}

int Weather::getTempMax(bool is_today)
{
    if(is_today)
    {
        return today["tempMax"].as<int>();
    }
    else
    {
        return tomorrow["tempMax"].as<int>();
    }
}

int Weather::getTempMin(bool is_today)
{
    if(is_today)
    {
        return today["tempMin"].as<int>();
    }
    else
    {
        return tomorrow["tempMin"].as<int>();
    }
}

int Weather::getWeather(bool is_today, bool is_day)
{
    if(is_today)
    {
        if(is_day)
            return today["iconDay"].as<int>();
        else
            return today["iconNight"].as<int>();
    }
    else
    {
        if(is_day)
            return tomorrow["iconDay"].as<int>();
        else
            return tomorrow["iconNight"].as<int>();
    }
}