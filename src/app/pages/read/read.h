struct BookState
{
    std::string currBook;
    std::string currBookPath;
    size_t offset = 0;
};

static BookState book;

void reset_bookstate();