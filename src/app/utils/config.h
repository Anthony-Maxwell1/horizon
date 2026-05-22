#include <string>

struct ReaderConfig
{
    int font_size = 14;
    std::string loaded_book_name = "";
    size_t current_offset = 0;
};

struct Features
{
    bool home_page = false;
    bool book_drawer = true;
    bool clock = true;
    bool status_bar = true;
    bool book_progress = true;            // deps on status_bar
    bool book_readtime_estimation = true; // deps on status_bar
};

struct Config
{
    ReaderConfig reader_config;
    Features features;
};

Config load_config();
bool set_config(const Config &config);