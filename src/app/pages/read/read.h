#include <lvgl.h>

struct BookState
{
    std::string currBook;
    std::string currBookPath;
    size_t offset = 0;
};

static BookState book;

void reset_bookstate();
void save_boookstate();

extern lv_obj_t *next_btn;
extern lv_obj_t *prev_btn;