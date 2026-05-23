#include <platform/common/platform_battery.h>

int mock_percent = 100;

int counter = 0;

int batt_percent()
{
    if (counter++ % 100 == 0) // update every 100 calls to simulate battery drain
        mock_percent -= 1;
    if (mock_percent < 0)
        mock_percent = 100;
    return mock_percent;
}