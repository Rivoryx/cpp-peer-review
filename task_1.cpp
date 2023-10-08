#include <iomanip> //std::setprecision
#include <iostream>
#include <numeric> //std::accumulate
#include <string>
#include <unordered_map>
#include <vector>

class Book {
public:
    Book();
    void Read(int reader_id, int page_num);
    double Cheer(int reader_id);
private:
    size_t MAX_PAGE_SIZE = 1000;
    int readers_amount_ = 0; //Общее кол-во читателей книги
    std::vector<int> readers_; //Читатели книги. Тут хранится текущая страница каждого читателя.
    std::vector<int> page_stats_; //Индекс страниц. Индексы вектора - номер страницы, значение - кол-во читателей на этой странице в данный момент
    std::unordered_map<int, int> index_; //Индекс читателей. Ключ - айди читателя, значение - его индекс в векторе читателей
};

Book::Book() {
    page_stats_.resize(MAX_PAGE_SIZE + 1);
}

void Book::Read(int reader_id, int page_num) {
    if (!index_.count(reader_id)) {
        //Если это новый читатель, увеличиваем общее число читателей, добавляем его в индексы
        index_[reader_id] = readers_amount_++;
        readers_.push_back(page_num);
    }
    else {
        //Если не новый, то меняем текущую страницу у читателя.
        //Если она была не нулевой(читатель первый раз взял книгу), 
        //в индексе страниц уменьшаем старое значение на единицу
        int& prev_page = page_stats_[readers_[index_.at(reader_id)]];
        if (prev_page > 0) {
            prev_page -= 1;
        }
    }
    //Прибавляем к новой текущей странице единицу
    page_stats_[page_num] += 1;

    //Обновляем индекс читателей
    readers_[index_.at(reader_id)] = page_num;
}

double Book::Cheer(int reader_id) {
    if (index_.count(reader_id) == 0) {
        return 0;
    }
    else if (readers_amount_ == 1) {
        return 1;
    }
    else {
        double accum = 0;
        //Считаем кол-во людей, прочитавших менее текущего читателя
        //Смотрим от 0 страницы до текущей страницы (readers_[reader_id])
        accum = std::accumulate(page_stats_.begin(), page_stats_.begin() + readers_[index_.at(reader_id)], 0.0);
        if (accum == 0) {
            //Если никто не прочитал меньше него
            return 0;
        }
        else {
            return accum / (readers_amount_ - 1);
        }
    }
}

void ParseInput(Book& book, std::istream& in = std::cin, std::ostream& out = std::cout) {
    using namespace std::literals;
    out << std::setprecision(6);
    size_t requests_amount = 0;
    in >> requests_amount;
    for (size_t i = 0; i < requests_amount; ++i) {
        std::string command;
        in >> command;
        if (command == "CHEER"s) {
            int reader_id = 0;
            in >> reader_id;
            out << book.Cheer(reader_id) << std::endl;;
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