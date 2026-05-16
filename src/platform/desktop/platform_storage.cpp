#include <platform/common/platform.h>
#include <platform/common/platform_storage.h>
#include <filesystem>
#include <fstream>
#include <cstdint>
#include <vector>
#include <string>

namespace fs = std::filesystem;

namespace platform::storage
{

    static std::string root = "./data";

    bool ensure_root()
    {
        if (!fs::exists(root))
            fs::create_directories(root);
        return true;
    }

    static std::string full_path(const std::string &p)
    {
        return root + "/" + p;
    }

    bool get_path(const std::string &path, std::vector<uint8_t> &out)
    {
        ensure_root();
        std::ifstream f(full_path(path), std::ios::binary);
        if (!f)
            return false;
        out.assign(std::istreambuf_iterator<char>(f), {});
        return true;
    }

    bool set_path(const std::string &path, const std::vector<uint8_t> &data)
    {
        ensure_root();
        std::ofstream f(full_path(path), std::ios::binary | std::ios::trunc);
        if (!f)
            return false;
        f.write((char *)data.data(), data.size());
        return true;
    }

    bool patch_path(const std::string &path, const std::vector<uint8_t> &patch)
    {
        std::vector<uint8_t> existing;
        if (!get_path(path, existing))
            return false;
        existing.insert(existing.end(), patch.begin(), patch.end());
        return set_path(path, existing);
    }

    bool create_path(const std::string &path)
    {
        ensure_root();
        std::ofstream f(full_path(path));
        return f.good();
    }

    bool delete_path(const std::string &path)
    {
        return fs::remove(full_path(path));
    }

    StorageStats storage_data()
    {
        StorageStats s{};
        s.total_bytes = 10ULL * 1024 * 1024 * 1024; // fake 10GB
        s.used_bytes = 0;
        for (auto &p : fs::recursive_directory_iterator(root))
        {
            if (fs::is_regular_file(p))
            {
                s.used_bytes += fs::file_size(p);
            }
        }
        s.free_bytes = s.total_bytes - s.used_bytes;
        return s;
    }
}

void platform_init_storage()
{
    platform::storage::ensure_root();
    // not much to do here
}