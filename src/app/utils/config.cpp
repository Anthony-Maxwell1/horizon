#include "config.h"
#include <platform/common/platform_storage.h>

#define CONFIG_PATH "config.txt"

Config load_config()
{
    Config config;
    std::vector<uint8_t> bytes;
    if (platform::storage::get_path("config.txt", bytes))
    {
        std::string content(bytes.begin(), bytes.end());
        while (!content.empty())
        {
            auto newline = content.find('\n');
            std::string line = content.substr(0, newline);
            content = newline == std::string::npos ? "" : content.substr(newline + 1);
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            auto eq = line.find('=');
            if (eq == std::string::npos)
                continue;

            std::string key = line.substr(0, eq);
            std::string val = line.substr(eq + 1);

            if (key == "reader_config.font_size")
                config.reader_config.font_size = std::stoi(val);
            else if (key == "top_bar")
                config.features.top_bar = val == "1";
            else if (key == "clock")
                config.features.clock = val == "1";
            else if (key == "battery")
                config.features.battery = val == "1";
            else if (key == "status_bar")
                config.features.status_bar = val == "1";
            else if (key == "book_progress")
                config.features.book_progress = val == "1";
            else if (key == "book_readtime_estimation")
                config.features.book_readtime_estimation = val == "1";
            else if (key == "reader_config.loaded_book_name")
                config.reader_config.loaded_book_name = val;
            else if (key == "reader_config.current_offset")
                config.reader_config.current_offset = std::stoull(val);
            // unknown keys silently ignored — forward compatible
        }
    }
    return config;
}

bool set_config(const Config &config)
{
    std::string content = "reader_config.font_size=" + std::to_string(config.reader_config.font_size) + "\n" +
                          "reader_config.loaded_book_name=" + config.reader_config.loaded_book_name + "\n" +
                          "reader_config.current_offset=" + std::to_string(config.reader_config.current_offset) + "\n" +
                          "top_bar=" + std::to_string(config.features.top_bar) + "\n" +
                          "clock=" + std::to_string(config.features.clock) + "\n" +
                          "battery=" + std::to_string(config.features.battery) + "\n" +
                          "status_bar=" + std::to_string(config.features.status_bar) + "\n" +
                          "book_progress=" + std::to_string(config.features.book_progress) + "\n" +
                          "book_readtime_estimation=" + std::to_string(config.features.book_readtime_estimation) + "\n";
    std::vector<uint8_t> bytes(content.begin(), content.end());
    return platform::storage::set_path("config.txt", bytes);
};