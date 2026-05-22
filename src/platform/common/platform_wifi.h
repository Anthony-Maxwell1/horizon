#pragma once
#include <string>

struct WifiNetwork
{
    std::string ssid;
    int rssi;
    bool secured;
};

struct HttpResponse
{
    int status;
    std::string body;
    bool ok;
};

int wifi_scan(WifiNetwork *out, int max);
bool wifi_connect(const char *ssid, const char *pass);
bool wifi_connected();
HttpResponse http_get(const std::string &url);