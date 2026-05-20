#include <string>

struct ReaderConfig
{
    int font_size = 14;
    std::string loaded_book_name = "";
    size_t current_offset = 0;
};

struct Features
{
    // Nothing for now.
};

struct Config
{
    ReaderConfig reader_config;
    Features features;
};

Config load_config();
bool set_config(const Config& config);