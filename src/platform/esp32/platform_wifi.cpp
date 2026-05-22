#include <WiFi.h>
#include <HTTPClient.h>
#include <platform/common/platform_wifi.h>

void wifi_init()
{
    WiFi.mode(WIFI_STA);
}

int wifi_scan(WifiNetwork *out, int max)
{
    WiFi.disconnect();
    delay(100);

    int n = WiFi.scanNetworks();

    if (n > max)
        n = max;

    for (int i = 0; i < n; i++)
    {
        out[i].ssid = WiFi.SSID(i);
        out[i].rssi = WiFi.RSSI(i);
        out[i].open = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN);
    }

    return n;
}

bool wifi_connect(const char *ssid, const char *pass)
{
    WiFi.begin(ssid, pass);

    uint32_t start = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(200);

        if (millis() - start > 15000)
            return false;
    }

    return true;
}

bool wifi_connected()
{
    return WiFi.status() == WL_CONNECTED;
}

HttpResponse http_get(const char *url)
{
    HttpResponse res;
    res.status = -1;
    res.ok = false;

    if (!wifi_connected())
        return res;

    HTTPClient http;

    if (!http.begin(url))
        return res;

    int code = http.GET();

    res.status = code;
    res.body = http.getString();
    res.ok = (code > 0);

    http.end();

    return res;
}
