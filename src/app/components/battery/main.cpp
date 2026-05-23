#include "../components.h"
#include <platform/common/platform_battery.h>
#include <lvgl.h>
#include <cstdio>
#include <app/utils/config.h>

static lv_obj_t *battery_bar = nullptr;
static lv_obj_t *battery_fill = nullptr;
static lv_obj_t *battery_label = nullptr;
static int last_percent = -1;

bool should_show_ = false;

static void update_fill(int percent)
{
    int fill_w = (percent * 72) / 100;
    if (fill_w < 0)
        fill_w = 0;
    lv_obj_set_size(battery_fill, fill_w, 25);
}

void init_battery()
{
    if (!load_config().features.top_bar || !load_config().features.battery)
        return;
    should_show_ = true;
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
    lv_obj_set_size(battery_bar, 80, 35);
    lv_obj_align_to(battery_bar, battery_label, LV_ALIGN_OUT_LEFT_MID, -8, 0);
    lv_obj_set_style_bg_opa(battery_bar, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(battery_bar, 2, 0);
    lv_obj_set_style_border_color(battery_bar, lv_color_black(), 0);
    lv_obj_set_style_radius(battery_bar, 3, 0);
    lv_obj_set_style_pad_all(battery_bar, 0, 0);
    lv_obj_clear_flag(battery_bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(battery_bar, LV_OBJ_FLAG_OVERFLOW_VISIBLE); // don't clip children

    battery_fill = lv_obj_create(battery_bar);
    lv_obj_set_pos(battery_fill, 3, 3); // inset from border
    lv_obj_set_style_bg_color(battery_fill, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(battery_fill, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(battery_fill, 0, 0);
    lv_obj_set_style_radius(battery_fill, 2, 0);
    lv_obj_clear_flag(battery_fill, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_move_foreground(battery_bar);
    lv_obj_move_foreground(battery_label);

    update_fill(last_percent);
}

void loop_battery()
{
    if (!should_show_)
        return;
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

    update_fill(percent);

    lv_obj_invalidate(lv_scr_act());
}