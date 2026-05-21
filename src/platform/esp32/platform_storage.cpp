#include <platform/common/platform_storage.h>

#include <driver/sdmmc_host.h>
#include <driver/sdmmc_types.h>

#include <SD_MMC.h>

#define SD_PIN_CLK    36
#define SD_PIN_CMD    35
#define SD_PIN_DAT0   37
#define SD_PIN_DAT1   38
#define SD_PIN_DAT2   33
#define SD_PIN_DAT3   34
#define SD_PIN_CD     -1
#define SD_PIN_WP     -1

namespace platform::storage
{
    bool get_path(const std::string &path, std::vector<uint8_t> &out)
    {
        File f = SD_MMC.open(path.c_str(), FILE_READ);
        if (!f) return false;
        out.resize(f.size());
        f.read(out.data(), out.size());
        f.close();
        return true;
    }

    bool set_path(const std::string &path, const std::vector<uint8_t> &data)
    {
        File f = SD_MMC.open(path.c_str(), FILE_WRITE);
        if (!f) return false;
        f.write(data.data(), data.size());
        f.close();
        return true;
    }

    bool patch_path(const std::string &path, const std::vector<uint8_t> &patch)
    {
        File f = SD_MMC.open(path.c_str(), FILE_APPEND);
        if (!f) return false;
        f.write(patch.data(), patch.size());
        f.close();
        return true;
    }

    bool create_path(const std::string &path)
    {
        File f = SD_MMC.open(path.c_str(), FILE_WRITE);
        if (!f) return false;
        f.close();
        return true;
    }

    bool delete_path(const std::string &path)
    {
        return SD_MMC.remove(path.c_str());
    }

    StorageStats storage_data()
    {
        StorageStats s{};
        s.total_bytes = SD_MMC.totalBytes();
        s.used_bytes = SD_MMC.usedBytes();
        s.free_bytes = SD_MMC.totalBytes() - SD_MMC.usedBytes();
        return s;
    }
} // namespace platform::storage

void platform_init_storage()
{
    SD_MMC.setPins(SD_PIN_CLK, SD_PIN_CMD, SD_PIN_DAT0, SD_PIN_DAT1, SD_PIN_DAT2, SD_PIN_DAT3);

    if (!SD_MMC.begin("/sdcard", false)) { // false = 4-bit mode
        Serial.println("Failed to mount SD card");
        return;
    }

    std::vector<uint8_t> bytes;
    if (!platform::storage::get_path("/config.txt", bytes))
    {
        platform::storage::set_path("/config.txt", std::vector<uint8_t>{});
    }
}