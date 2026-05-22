#include "drawer.h"
#include <app/pages/read/read.h>
#include <lvgl.h>
#include <app/dimensions.h>
#include <platform/common/platform_storage.h>
#include "../components.h"
#include <cstdio>
#include <platform/common/platform.h>
#include <cstring>
#include <app/utils/config.h>

lv_obj_t *drawer_obj = nullptr;

std::vector<std::string> get_booklist()
{
    std::vector<std::string> booklist;
    std::vector<uint8_t> bytes;
    if (platform::storage::get_path("books.txt", bytes))
    {
        std::string content(bytes.begin(), bytes.end());
        while (!content.empty())
        {
            auto newline = content.find('\n');
            std::string line = content.substr(0, newline);
            content = newline == std::string::npos ? "" : content.substr(newline + 1);
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            if (!line.empty())
                booklist.push_back(line);
        }
    }
    return booklist;
}

void toggle_drawer()
{
    if (drawer_obj == nullptr)
        return;
    if (lv_obj_has_flag(drawer_obj, LV_OBJ_FLAG_HIDDEN))
    {
        lv_obj_clear_flag(drawer_obj, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(drawer_obj);
        lv_obj_invalidate(lv_scr_act());
        if (next_btn)
            lv_obj_add_flag(next_btn, LV_OBJ_FLAG_HIDDEN);
        if (prev_btn)
            lv_obj_add_flag(prev_btn, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_flag(drawer_obj, LV_OBJ_FLAG_HIDDEN);
        lv_obj_invalidate(lv_scr_act());
        if (next_btn)
            lv_obj_clear_flag(next_btn, LV_OBJ_FLAG_HIDDEN);
        if (prev_btn)
            lv_obj_clear_flag(prev_btn, LV_OBJ_FLAG_HIDDEN);
    }
}

void selectBook(const char *bookPath)
{
    std::vector<uint8_t> bytes;
    if (platform::storage::get_path(bookPath, bytes))
    {
        Config config = load_config();
        config.reader_config.loaded_book_name = bookPath;
        config.reader_config.current_offset = 0;
        set_config(config);

        toggle_drawer();
        reset_bookstate();
    }
}

void init_book_drawer()
{
    lv_obj_t *scr = lv_scr_act();

    lv_obj_add_event_cb(scr, [](lv_event_t *e)
                        { 
                            if (!lv_obj_has_flag(drawer_obj, LV_OBJ_FLAG_HIDDEN))
                                toggle_drawer(); }, LV_EVENT_CLICKED, NULL);

    drawer_obj = lv_obj_create(scr);
    lv_obj_set_size(drawer_obj, 800, SCREEN_H);
    lv_obj_align(drawer_obj, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_style_bg_color(drawer_obj, lv_color_hex(0xeeeeee), 0);
    lv_obj_set_style_bg_opa(drawer_obj, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(drawer_obj, 0, 0);
    lv_obj_set_style_border_width(drawer_obj, 0, 0);
    lv_obj_set_flex_flow(drawer_obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_flag(drawer_obj, LV_OBJ_FLAG_HIDDEN);

    auto booklist = get_booklist();
    printf("Booklist:\n");
    for (const auto &book : booklist)
    {
        printf("- %s\n", book.c_str());
    }
    for (size_t i = 0; i < booklist.size(); i++)
    {
        const std::string &bookname = booklist[i];
        lv_obj_t *btn = lv_btn_create(drawer_obj);
        lv_obj_set_width(btn, LV_PCT(100));
        lv_obj_set_height(btn, 120);
        lv_obj_set_style_bg_color(btn, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(btn, 2, 0);
        lv_obj_set_style_border_color(btn, lv_color_black(), 0);

        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, bookname.c_str());
        lv_obj_center(label);
        lv_obj_set_style_text_color(label, lv_color_black(), 0);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);

        lv_obj_add_event_cb(btn, [](lv_event_t *e)
                            {
                                size_t idx = (size_t)lv_event_get_user_data(e);
                                auto booklist = get_booklist();
                                printf("Selected book index: %zu\n", idx);
                                if (idx < booklist.size())
                                {
                                    printf("Selected book: %s\n", booklist[idx].c_str());
                                    selectBook(booklist[idx].c_str());
                                }
                                lv_obj_add_flag(drawer_obj, LV_OBJ_FLAG_HIDDEN); }, LV_EVENT_CLICKED, (void *)i);
        lv_obj_add_event_cb(btn, [](lv_event_t *e)
                            {
    lv_point_t p;
    lv_indev_get_point(lv_indev_get_act(), &p);
    printf("Click at screen pos: %d %d, scroll: %d\n", 
        p.x, p.y, lv_obj_get_scroll_y(drawer_obj)); }, LV_EVENT_CLICKED, (void *)i);
    }

    lv_obj_add_event_cb(drawer_obj, [](lv_event_t *e)
                        { lv_obj_invalidate(lv_scr_act()); }, LV_EVENT_SCROLL, NULL);

    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(scr, LV_OBJ_FLAG_GESTURE_BUBBLE);
    register_swipe_callback([](const char *dir)
                            {
        if (strcmp(dir, "left") == 0 && lv_obj_has_flag(drawer_obj, LV_OBJ_FLAG_HIDDEN))
            toggle_drawer(); 
        else if (strcmp(dir, "right") == 0 && !lv_obj_has_flag(drawer_obj, LV_OBJ_FLAG_HIDDEN))
            toggle_drawer(); });
}

void loop_book_drawer() {}