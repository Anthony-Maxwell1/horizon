#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <vector>
#include <string>

struct StorageStats
{
    uint64_t total_bytes;
    uint64_t used_bytes;
    uint64_t free_bytes;
};

namespace platform::storage
{

    bool get_path(const std::string &path, std::vector<uint8_t> &out);
    bool set_path(const std::string &path, const std::vector<uint8_t> &data);
    bool patch_path(const std::string &path, const std::vector<uint8_t> &patch);
    bool create_path(const std::string &path);
    bool delete_path(const std::string &path);

    StorageStats storage_data();

}