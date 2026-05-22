#include "app_entry.h"
#include <lvgl.h>
#include <stdio.h>
#include <string>
#include "pages/pages.h"
#include "components/components.h"
// static lv_obj_t *demo_label = nullptr;
// static lv_obj_t *action_button = nullptr;
// static lv_obj_t *action_button_label = nullptr;
// static int click_count = 0;

enum CURR_PAGE
{
    READ,
};

CURR_PAGE curr_page = READ;

// static void btn_event_cb(lv_event_t *e)
// {
//     if (lv_event_get_code(e) == LV_EVENT_PRESSED)
//     {
//         click_count++;
//         char buf[64];
//         snprintf(buf, sizeof(buf), "Clicked %d", click_count);
//         lv_label_set_text(demo_label, buf);
//         lv_label_set_text(action_button_label, "Pressed");
//         lv_obj_set_style_bg_color(action_button, lv_color_white(), 0);
//         lv_obj_set_style_bg_opa(action_button, LV_OPA_COVER, 0);
//         lv_obj_set_style_border_color(action_button, lv_color_black(), 0);
//         lv_obj_set_style_text_color(action_button_label, lv_color_black(), 0);
//     }
// }

void app_entrypoint()
{
    switch (curr_page)
    {
    case READ:
        init_read_page();
        init_book_drawer();
        break;
    default:
        break;
    }

    // lv_obj_t *scr = lv_scr_act();
    // lv_obj_set_style_bg_color(scr, lv_color_white(), 0);
    // lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    // lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    // lv_obj_add_flag(scr, LV_OBJ_FLAG_CLICKABLE);

    // lv_obj_add_event_cb(scr, [](lv_event_t *e)
    //                     {
    //     lv_point_t p;
    //     lv_indev_get_point(lv_indev_get_act(), &p);
    //     printf("Click at %d %d\n", p.x, p.y);

    //     lv_obj_t *circle = lv_obj_create(lv_scr_act());
    //     lv_obj_remove_style_all(circle);
    //     lv_obj_set_size(circle, 40, 40);
    //     lv_obj_set_style_bg_color(circle, lv_color_black(), 0);
    //     lv_obj_set_style_bg_opa(circle, LV_OPA_COVER, 0);
    //     lv_obj_set_pos(circle, p.x - 20, p.y - 20); }, LV_EVENT_CLICKED, NULL);

    // lv_obj_set_style_bg_color(scr, lv_color_white(), 0);
    // lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    // demo_label = lv_label_create(scr);
    // lv_label_set_text(demo_label, "Hello, Horizon!");
    // lv_obj_align(demo_label, LV_ALIGN_TOP_MID, 0, 24);
    // lv_obj_set_style_text_font(demo_label, &lv_font_montserrat_28, 0);
    // lv_obj_set_style_text_color(demo_label, lv_color_black(), 0);

    // lv_obj_t *btn = lv_btn_create(scr);
    // action_button = btn;
    // lv_obj_set_size(btn, 480, 170);
    // lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_PRESSED, NULL);
    // lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    // lv_obj_set_style_border_width(btn, 6, 0);
    // lv_obj_set_style_border_color(btn, lv_color_black(), 0);
    // lv_obj_set_style_bg_color(btn, lv_color_white(), 0);
    // lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);

    // action_button_label = lv_label_create(btn);
    // lv_label_set_text(action_button_label, "Click me");
    // lv_obj_center(action_button_label);
    // lv_obj_set_style_text_font(action_button_label, &lv_font_montserrat_28, 0);
    // lv_obj_set_style_text_color(action_button_label, lv_color_black(), 0);
}

void app_loop()
{
    switch (curr_page)
    {
    case READ:
        loop_read_page();
        loop_book_drawer();
        break;
    default:
        break;
    }
}
