#include <app/utils/config.h>
#include "read.h"
#include <lvgl.h>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <platform/common/platform_storage.h>
#include "../pages.h"

#define SCREEN_W 1404
#define SCREEN_H 1872

static lv_obj_t *page_label = nullptr;

const lv_font_t FONT = lv_font_montserrat_48;

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
        size_t next_hi = start_offset + std::min(hi - start_offset + step, len);
        hi = next_hi;

        std::string chunk(text + start_offset, hi - start_offset);
        lv_point_t size;
        lv_text_get_size(&size, chunk.c_str(), font, 0, 0, max_width, LV_TEXT_FLAG_NONE);
        if (size.y > max_height)
            break;

        if (hi - start_offset >= len)
            break;

        step *= 2;
    }
    while (hi - lo > 1)
    {
        size_t mid = lo + (hi - lo) / 2;

        size_t candidate = mid;
        while (candidate > lo && text[candidate] != ' ')
            candidate--;

        if (candidate == lo)
        {
            candidate = mid;
            while (candidate < hi && text[candidate] != ' ')
                candidate++;
            if (candidate >= hi)
                candidate = mid;
        }

        std::string chunk(text + start_offset, candidate - start_offset);
        lv_point_t size;
        lv_text_get_size(&size, chunk.c_str(), font, 0, 0, max_width, LV_TEXT_FLAG_NONE);

        if (size.y <= max_height)
            lo = candidate;
        else
            hi = candidate;
    }
    if (lo == start_offset && len > 0)
    {
        size_t fallback = std::min(start_offset + len, start_offset + size_t{1});
        lv_point_t size;
        char chunk[fallback - start_offset + 1];
        memcpy(chunk, text + start_offset, fallback - start_offset);
        chunk[fallback - start_offset] = '\0';
        lv_text_get_size(&size, chunk, font, 0, 0, max_width, LV_TEXT_FLAG_NONE);
        if (size.y <= max_height)
            lo = fallback;
    }

    if (page_cache.size() > 100)
    {
        page_cache.clear();
    }
    page_cache[start_offset] = lo;
    lv_point_t final_size;
    std::string final_chunk(text + start_offset, lo - start_offset);
    lv_text_get_size(&final_size, final_chunk.c_str(), font, 0, 0, max_width, LV_TEXT_FLAG_NONE);
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

        size_t candidate = mid;
        while (candidate < hi && text[candidate] != ' ')
            candidate++;
        if (candidate == hi && text[candidate] != ' ')
            candidate = mid;

        lv_point_t size;

        char chunk[end_offset - candidate + 1];
        memcpy(chunk, text + candidate, end_offset - candidate);
        chunk[end_offset - candidate] = '\0';
        lv_text_get_size(&size, chunk, font, 0, 0, max_width, LV_TEXT_FLAG_NONE);

        if (size.y <= max_height)
            hi = candidate;
        else
            lo = candidate;

        if (candidate >= hi && size.y <= max_height)
            break;
    }

    if (hi == 0 && end_offset > 0)
        hi = 1;

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
    lv_obj_t *scr = lv_scr_act();
    int max_width = lv_obj_get_width(scr) - 8;
    int max_height = lv_obj_get_height(scr) - 24 - 8; // account for top offset and margin
    printf("screen: %d x %d, max: %d x %d\n",
           lv_obj_get_width(scr), lv_obj_get_height(scr), max_width, max_height);
    size_t end_offset = calc_end_offset(book.offset, &FONT, max_width, max_height, book.currBook.c_str());
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
    lv_label_set_long_mode(page_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(page_label, lv_obj_get_width(scr) - 8);
    lv_label_set_text(page_label, "Loading...");
    lv_obj_align(page_label, LV_ALIGN_TOP_LEFT, 0, 24);
    lv_obj_set_style_text_font(page_label, &FONT, 0);
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