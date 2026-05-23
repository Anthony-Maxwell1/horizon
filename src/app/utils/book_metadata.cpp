#include <vector>
#include <string>
#include <ArduinoJson.h>
#include "book_metadata.h"
#include <platform/common/platform_storage.h>
#include "config.h"
#include "../pages/read/read.h"

#define METADATA_PATH "books.json"

std::vector<BookEntry> get_booklist()
{
    std::vector<BookEntry> books;
    std::vector<uint8_t> bytes;

    if (!platform::storage::get_path(METADATA_PATH, bytes))
        return books;

    JsonDocument doc;

    DeserializationError err =
        deserializeJson(doc, bytes.data(), bytes.size());

    if (err)
    {
        printf("JSON parse failed: %s\n", err.c_str());
        return books;
    }

    for (JsonPair kv : doc.as<JsonObject>())
    {
        BookEntry entry;

        entry.filename = kv.key().c_str();

        JsonObject obj = kv.value().as<JsonObject>();

        entry.title =
            obj["title"] | entry.filename;

        entry.curr_pos =
            obj["curr_pos"] | 0;

        entry.hash =
            obj["hash"] | "";

        books.push_back(entry);
    }

    return books;
}

void set_booklist(const std::vector<BookEntry> &books)
{
    JsonDocument doc;

    for (const auto &book : books)
    {
        JsonObject obj =
            doc[book.filename.c_str()].to<JsonObject>();

        obj["title"] = book.title;
        obj["curr_pos"] = book.curr_pos;
        obj["hash"] = book.hash;
    }

    std::string output;
    serializeJsonPretty(doc, output);

    std::vector<uint8_t> bytes(output.begin(), output.end());

    platform::storage::set_path("books.json", bytes);
}

void set_book(BookEntry &updated_book, bool recalculate_hash)
{
    auto books = get_booklist();

    bool found = false;

    if (recalculate_hash)
    {
        std::vector<uint8_t> bytes;
        if (platform::storage::get_path(updated_book.filename.c_str(), bytes))
        {
            updated_book.hash = compute_book_hash(bytes);
        }
    }

    for (auto &book : books)
    {
        if (book.filename == updated_book.filename)
        {
            book = updated_book;
            found = true;
            break;
        }
    }

    if (!found)
    {
        books.push_back(updated_book);
    }

    set_booklist(books);
}

std::string compute_book_hash(const std::vector<uint8_t> &bytes)
{
    return std::to_string(bytes.size());
}

void selectBook(BookEntry &book)
{
    std::vector<uint8_t> bytes;

    if (platform::storage::get_path(book.filename.c_str(), bytes))
    {
        std::string current_hash = compute_book_hash(bytes);

        Config config = load_config();

        config.reader_config.loaded_book_name = book.filename;

        if (current_hash == book.hash)
        {
            config.reader_config.current_offset = book.curr_pos;
        }
        else
        {
            printf("Book hash changed, resetting offset\n");
            config.reader_config.current_offset = 0;
            book.hash = current_hash;
            set_book(book);
        }

        set_config(config);

        reset_bookstate();
    }
}