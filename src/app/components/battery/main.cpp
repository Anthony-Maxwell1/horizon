#include "../components.h"
#include <platform/common/platform_battery.h>
#include <lvgl.h>
#include <cstdio>

static lv_obj_t *battery_bar = nullptr;
static lv_obj_t *battery_fill = nullptr;
static lv_obj_t *battery_label = nullptr;
static int last_percent = -1;

void init_battery()
{
    battery_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(battery_label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(battery_label, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(battery_label, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(battery_label, 0, 0);
    lv_obj_set_style_pad_all(battery_label, 0, 0);
    lv_obj_clear_flag(battery_label, LV_OBJ_FLAG_SCROLLABLE);

    last_percent = batt_percent();
    char buf[8];
    snprintf(buf, sizeof(buf), "%d%%", last_percent);
    lv_label_set_text(battery_label, buf);
    lv_obj_align(battery_label, LV_ALIGN_TOP_RIGHT, -8, 10);

    battery_bar = lv_obj_create(lv_scr_act());
    lv_obj_set_size(battery_bar, 80, 10);
    lv_obj_align_to(battery_bar, battery_label, LV_ALIGN_TOP_RIGHT, -92, 0);
    lv_obj_set_style_bg_opa(battery_bar, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(battery_bar, 2, 0);
    lv_obj_set_style_border_color(battery_bar, lv_color_black(), 0);
    lv_obj_set_style_radius(battery_bar, 3, 0);
    lv_obj_set_style_pad_all(battery_bar, 0, 0);
    lv_obj_clear_flag(battery_bar, LV_OBJ_FLAG_SCROLLABLE);

    battery_fill = lv_obj_create(battery_bar);
    lv_obj_set_pos(battery_fill, 2, 2);
    lv_obj_set_style_bg_color(battery_fill, lv_color_black(), 0);
    lv_obj_set_style_border_width(battery_fill, 0, 0);
    lv_obj_set_style_radius(battery_fill, 2, 0);
    lv_obj_clear_flag(battery_fill, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_move_foreground(battery_bar);
    lv_obj_move_foreground(battery_label);

    int fill_w = (last_percent * 76) / 100;
    lv_obj_set_size(battery_fill, fill_w, 6);
}

void loop_battery()
{
    int percent = batt_percent();
    if (percent == last_percent)
        return;
    last_percent = percent;
    if (percent < 0)
        percent = 0;
    if (percent > 100)
        percent = 100;

    char buf[8];
    snprintf(buf, sizeof(buf), "%d%%", percent);
    lv_label_set_text(battery_label, buf);

    int fill_w = (percent * 76) / 100;
    lv_obj_set_size(battery_fill, fill_w, 6);

    if (percent > 30)
    {
        lv_obj_set_style_opa(battery_bar, LV_OPA_70, 0);
        lv_obj_set_style_opa(battery_label, LV_OPA_70, 0);
    }
    else
    {
        lv_obj_set_style_opa(battery_bar, LV_OPA_100, 0);
        lv_obj_set_style_opa(battery_label, LV_OPA_100, 0);
    }

    lv_obj_invalidate(lv_scr_act());
}