#include <app/utils/config.h>
#include "read.h"
#include <lvgl.h>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <platform/common/platform_storage.h>
#include "../pages.h"

static lv_obj_t *page_label = nullptr;

bool change = true;
BookState book;

std::unordered_map<size_t, size_t> page_cache;

size_t calc_end_offset(size_t start_offset, const lv_font_t *font, int max_width, int max_height, const char *text)
{
    if (page_cache.count(start_offset))
        return page_cache[start_offset];
    size_t lo = start_offset;
    size_t hi = start_offset;
    size_t len = strlen(text + start_offset);
    size_t step = 64; // BYTES
    while (hi - start_offset < len)
    {
        hi = start_offset + std::min(hi - start_offset + step, len);

        lv_point_t size;

        char chunk[hi - start_offset + 1];
        memcpy(chunk, text + start_offset, hi - start_offset);
        chunk[hi - start_offset] = '\0';
        lv_text_get_size(&size, chunk, font, 0, 0, max_width, LV_TEXT_FLAG_NONE);
        if (size.y > max_height)
            break;
        step *= 2;
    }
    while (hi - lo > 1)
    {
        size_t mid = lo + (hi - lo) / 2;

        while (mid > lo && text[mid] != ' ')
            mid--;

        lv_point_t size;

        char chunk[mid - start_offset + 1];
        memcpy(chunk, text + start_offset, mid - start_offset);
        chunk[mid - start_offset] = '\0';
        lv_text_get_size(&size, chunk, font, 0, 0, max_width, LV_TEXT_FLAG_NONE);

        if (size.y <= max_height)
            lo = mid;
        else
            hi = mid;
    }

    if (page_cache.size() > 100)
    {
        page_cache.clear();
    }
    page_cache[start_offset] = lo;
    return lo;
}

size_t calc_start_offset(size_t end_offset, const lv_font_t *font, int max_width, int max_height, const char *text)
{
    // skip cache cause its too much effort lol
    size_t lo = 0;
    size_t hi = end_offset;
    while (hi - lo > 1)
    {
        size_t mid = lo + (hi - lo) / 2;

        while (mid < hi && text[mid] != ' ')
            mid++;

        lv_point_t size;

        char chunk[end_offset - mid + 1];
        memcpy(chunk, text + mid, end_offset - mid);
        chunk[end_offset - mid] = '\0';
        lv_text_get_size(&size, chunk, font, 0, 0, max_width, LV_TEXT_FLAG_NONE);

        if (size.y <= max_height)
            hi = mid;
        else
            lo = mid;
    }

    if (page_cache.size() > 100)
    {
        page_cache.clear();
    }
    page_cache[end_offset] = hi;
    return hi;
}

void next_page(const lv_font_t *font, int max_width, int max_height, const char *text)
{
    size_t next_offset = calc_end_offset(book.offset, font, max_width, max_height, text);
    if (next_offset == book.offset)
        return; // can't fit even a single char
    book.offset = next_offset;
}

void prev_page(const lv_font_t *font, int max_width, int max_height, const char *text)
{
    if (book.offset == 0)
        return;
    size_t prev_offset = calc_start_offset(book.offset, font, max_width, max_height, text);
    book.offset = prev_offset;
}

void render_page()
{
    if (book.currBook.empty())
        return;
    size_t end_offset = calc_end_offset(book.offset, &lv_font_montserrat_14, 480, 800, book.currBook.c_str());
    if (end_offset == book.offset)
        return; // can't fit even a single char
    std::string page_text = book.currBook.substr(book.offset, end_offset - book.offset);
    lv_label_set_text_fmt(page_label, "%.*s",
                          (int)(end_offset - book.offset),
                          book.currBook.c_str() + book.offset);
}

void init_read_page()
{
    Config config = load_config();
    book.currBookPath = config.reader_config.loaded_book_name;
    if (book.currBookPath.empty())
    {
        lv_obj_t *scr = lv_scr_act();

        lv_obj_set_style_bg_color(scr, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

        page_label = lv_label_create(scr);
        lv_label_set_text(page_label, "Select a book!");
        lv_obj_align(page_label, LV_ALIGN_TOP_LEFT, 0, 24);
        lv_obj_set_style_text_font(page_label, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(page_label, lv_color_black(), 0);
        return;
    }
    std::vector<uint8_t> bytes;
    if (platform::storage::get_path(book.currBookPath, bytes))
    {
        book.currBook = std::string(bytes.begin(), bytes.end());
    }

    book.offset = config.reader_config.current_offset;
    lv_obj_t *scr = lv_scr_act();

    lv_obj_set_style_bg_color(scr, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    page_label = lv_label_create(scr);
    lv_label_set_text(page_label, "Loading...");
    lv_obj_align(page_label, LV_ALIGN_TOP_LEFT, 0, 24);
    lv_obj_set_style_text_font(page_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(page_label, lv_color_black(), 0);
    render_page();
};

void loop_read_page()
{
    if (change)
    {
        render_page();
        change = false;
    }
};