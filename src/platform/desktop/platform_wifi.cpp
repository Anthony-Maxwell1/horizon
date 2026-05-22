#include <string>
#include <cstdio>
#include <platform/common/platform_wifi.h>
#include <curl/curl.h>

int wifi_scan(WifiNetwork *out, int max)
{
    if (max < 3)
        return 0;
    out[0] = {"Home WiFi", -40, false};
    out[1] = {"Coffee Shop", -70, true};
    out[2] = {"Phone Hotspot", -55, true};
    return 3;
}

bool wifi_connect(const char *ssid, const char *pass)
{
    (void)ssid;
    (void)pass;
    return true;
}

bool wifi_connected() { return true; }

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)ptr, size * nmemb);
    return size * nmemb;
}

HttpResponse http_get(const std::string &url)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return {500, "", false};

    std::string body;
    long code = 0;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    curl_easy_cleanup(curl);

    return {(int)code, body, res == CURLE_OK};
}