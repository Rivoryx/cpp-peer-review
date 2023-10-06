#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <iomanip>

class Book {
public:
    int GetReadersAmount() const noexcept { return readers_amount_; }
    void Read(int reader_id, int page_num);
    void Cheer(int reader_id, std::ostream& out = std::cout);
private:
    std::vector<int> readers_; //Индекс читателей. Индексы вектора - айди читателей, значение - кол-во прочитанных страниц
    std::vector<int> page_stats_; //Индекс страниц. Индексы вектора - номер страницы, значение - кол-во читателей на этой странице в данный момент
    int readers_amount_ = 0; //Общее кол-во читателей книги

    //Функции проверяют, не выходим ли мы за пределы индексов, если да, то увеличивает размер индексов
    void CheckReadersSize(size_t);
    void CheckPageStatsSize(size_t);
};

void Book::CheckReadersSize(size_t size) {
    if (readers_.size() <= size) {
        readers_.resize(size + 1);
    }
}

void Book::CheckPageStatsSize(size_t size) {
    if (page_stats_.size() <= size) {
        page_stats_.resize(size + 1);
    }
}

void Book::Read(int reader_id, int page_num) {
    //Проверяем, не выходят ли reader_id и page_num за пределы наших индексов
    CheckReadersSize(reader_id);
    CheckPageStatsSize(page_num);

    //Меняем текущую страницу у читателя.
    //Если она была не нулевой(читатель первый раз взял книгу), 
    //в индексе страниц уменьшаем старое значение на единицу
    int& prev_page = page_stats_[readers_[reader_id]];
    if (prev_page > 0) {
        prev_page -= 1;
    }
    //Прибавляем к новому значению единицу
    page_stats_[page_num] += 1;

    //Если до этого у читателя было прочитано 0 страниц, значит это новый читатель
    //увеличиваем значение читателей на 1
    if (readers_[reader_id] == 0) {
        readers_amount_ += 1;
    }

    //Обновляем индекс читателей
    readers_[reader_id] = page_num;
}

void Book::Cheer(int reader_id, std::ostream& out) {
    //Проверяем, не выходит ли reader_id за пределы индекса читателей
    CheckReadersSize(reader_id);
    //Новый читатель, для него не вызывался READ, выводим 0
    if (readers_[reader_id] == 0) {
        out << 0 << std::endl;
    }
    else {
        double accum = 0;
        //Считаем кол-во людей, прочитавших менее текущего читателя
        //Смотрим от 0 страницы до текущей страницы (readers_[reader_id])
        accum = std::accumulate(page_stats_.begin(), page_stats_.begin() + readers_[reader_id], 0.0);
        if (readers_amount_ == 1) {
            //Если это единственный читатель
            out << 1 << std::endl;
        }
        else if (accum == 0) {
            //Если никто не прочитал меньше него
            out << 0 << std::endl;
        }
        else {
            out << std::setprecision(6);
            out << accum / (GetReadersAmount() - 1) << std::endl;
        }
    }
}

void ParseInput(Book& book, std::istream& in = std::cin) {
    using namespace std::literals;
    size_t requests_amount = 0;
    in >> requests_amount;
    for (size_t i = 0; i < requests_amount; ++i) {
        std::string command;
        in >> command;
        if (command == "CHEER"s) {
            int reader_id = 0;
            in >> reader_id;
            book.Cheer(reader_id);
        }
        else if (command == "READ"s) {
            int reader_id = 0, page_num = 0;
            in >> reader_id >> page_num;
            book.Read(reader_id, page_num);
        }
        else {
            throw std::logic_error("Wrong command input.");
        }
    }
}

int main() {
    Book book;
    ParseInput(book);
    return 0;
}