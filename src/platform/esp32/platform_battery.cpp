#include <platform/common/platform_battery.h>

int mock_percent = 100;

int batt_percent()
{
    mock_percent -= 0.5;
    if (mock_percent < 0)
        mock_percent = 100;
    return mock_percent;
}

// HAS NOT BEEN IMPLEMENTED IN SCHEM YET
// TODO: ADD TO SCHEM AND IMP HERE.