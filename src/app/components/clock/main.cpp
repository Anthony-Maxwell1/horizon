#include <lvgl.h>
#include <platform/common/platform.h>
#include <chrono>

static lv_obj_t *clock_label = nullptr;
static std::chrono::system_clock::time_point last_time;

void init_clock()
{
    clock_label = lv_label_create(lv_scr_act());

    lv_obj_align(clock_label, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_set_style_text_font(clock_label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(clock_label, lv_color_black(), 0);

    lv_obj_set_style_bg_opa(clock_label, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(clock_label, 0, 0);
    lv_obj_set_style_pad_all(clock_label, 0, 0);

    lv_obj_clear_flag(clock_label, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_move_foreground(clock_label);

    last_time = get_time();

    char buf[16];
    std::time_t t = std::time(nullptr);
    std::tm *tm_info = std::localtime(&t);

    std::strftime(buf, sizeof(buf), "%H:%M", tm_info);

    lv_label_set_text(clock_label, buf);

    lv_obj_invalidate(lv_scr_act());
}

void loop_clock()
{
    if (!clock_label)
        return;

    auto now = get_time();
    if (now - last_time < std::chrono::seconds(5))
        return;

    last_time = now;

    std::time_t t = std::time(nullptr);
    std::tm *tm_info = std::localtime(&t);

    char buf[16];
    std::strftime(buf, sizeof(buf), "%H:%M", tm_info);

    lv_label_set_text(clock_label, buf);

    lv_obj_invalidate(lv_scr_act());
}