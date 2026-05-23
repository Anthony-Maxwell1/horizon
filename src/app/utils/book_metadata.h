
struct BookEntry
{
    std::string filename;
    std::string title;
    size_t curr_pos;
    std::string hash;
};

std::vector<BookEntry> get_booklist();
void set_booklist(const std::vector<BookEntry> &books);
void set_book(BookEntry &updated_book, bool recalculate_hash = true);
void selectBook(BookEntry &book);
std::string compute_book_hash(const std::vector<uint8_t> &bytes);