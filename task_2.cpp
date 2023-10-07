#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

class Domain {
public:
    Domain(std::string domain);

    bool operator==(const std::string& other) const noexcept;
    bool operator==(const Domain& other) const noexcept;
    bool operator<(const Domain& other) const noexcept;
    bool IsSubdomain(const Domain& other) const noexcept;

private:
    std::string domain_;
};

class DomainChecker {
public:
    template<typename InputIt>
    DomainChecker(InputIt begin, InputIt end);

    bool IsForbidden(const Domain& domain) const noexcept;

private:
    std::vector<Domain> forbidden_domains_;
};

//------------------------------------------------------
//------------------------Domain------------------------
//------------------------------------------------------

Domain::Domain(std::string domain) : domain_(std::move(domain)) {
    // Разворачиваем имя домена
    std::reverse(domain_.begin(), domain_.end());
    // Добавляем точку ('.') в конец перевернутого доменного имени.
    // Эта точка для более легкого поиска при проверке поддоменов
    domain_.push_back('.');
}

bool Domain::operator==(const std::string& other) const noexcept {
    return domain_ == other;
}

bool Domain::operator==(const Domain& other) const noexcept {
    return domain_ == other.domain_;
}

bool Domain::operator<(const Domain& other) const noexcept {
    return std::lexicographical_compare(domain_.begin(), domain_.end(), other.domain_.begin(), other.domain_.end());
}

bool Domain::IsSubdomain(const Domain& other) const noexcept {
    // Проверяем, является ли текущий домен поддоменом другого домена.
    return domain_.find(other.domain_) == 0;
}

//------------------------------------------------------
//--------------------DomainChecker---------------------
//------------------------------------------------------

template<typename InputIt>
DomainChecker::DomainChecker(InputIt begin, InputIt end) : forbidden_domains_(begin, end) {
    // Сортируем список запрещенных доменов по возрастанию.
    std::sort(forbidden_domains_.begin(), forbidden_domains_.end());

    // Удаляем дубликаты и поддомены из списка запрещенных доменов.
    // Для этого используем алгоритм std::unique с пользовательским компаратором,
    // который проверяет, что один домен из пары - поддомен другого.
    forbidden_domains_.erase(
        std::unique(
            forbidden_domains_.begin(), forbidden_domains_.end(),
            [](const Domain& a, const Domain& b) {
                return b.IsSubdomain(a);
            }),
        forbidden_domains_.end()
    );
}

bool DomainChecker::IsForbidden(const Domain& domain) const noexcept {
    // Используем бинарный поиск для проверки, является ли домен запрещенным.
    std::vector<Domain>::const_iterator it = std::upper_bound(forbidden_domains_.begin(), forbidden_domains_.end(), domain);
    if (it != forbidden_domains_.begin()) {
        std::advance(it, -1);  // Перемещаем итератор на одну позицию назад для проверки предыдущего элемента.
        if (domain.IsSubdomain(*it)) {
            return true;  // Если это поддомен, возвращаем true.
        }
    }
    return false;  // Если поддомен не найден, возвращаем false.
}

//------------------------------------------------------
//--------------------Misc Functions--------------------
//------------------------------------------------------

template <typename Number>
std::vector<Domain> ReadDomains(std::istream& in, Number number) {
    std::vector<Domain> result;

    if (number == 0) {
        return result;  // Если количество доменов равно нулю, вернуть пустой вектор.
    }

    for (size_t i = 0; i < number; ++i) {
        std::string domain;
        in >> domain;  // Считываем строку-домен из входного потока.
        result.emplace_back(domain);  // Добавляем считанный домен в вектор.
    }

    in.get();  // Считываем(удаляем) символ новой строки ('\n') после считывания числа.

    return result;
}

template <typename Number>
Number ReadNumberOnLine(std::istream& input) {
    std::string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

//------------------------------------------------------
//------------------------Tests-------------------------
//------------------------------------------------------

#include <cassert>

namespace tests {

    void TestDomain() {
        // Проверяем операторы сравнения
        assert(Domain("example.com") == "moc.elpmaxe.");
        assert(!(Domain("example.com") == Domain("anotherdomain.com")));

        // Проверяем IsSubdomain
        assert(Domain("google.com").IsSubdomain(Domain("com")));
        assert(!Domain("example.com").IsSubdomain(Domain("google.com")));

        // Проверяем с поддоменами
        assert(Domain("mail.google.com").IsSubdomain(Domain("google.com")));
        assert(Domain("sub.example.com").IsSubdomain(Domain("example.com")));
        assert(!Domain("example.com").IsSubdomain(Domain("sub.example.com")));
    }

    // Определяем пользовательскую функцию для тестирования класса DomainChecker
    void TestDomainChecker() {
        {
            // Создаем список запрещенных доменов для тестирования
            std::vector<Domain> forbidden_domains = {
                Domain("example.com"),
                Domain("google.com"),
                Domain("test.com"),
                Domain("sub.example.com"),
                Domain("mail.google.com")
            };

            DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

            // Проверяем IsForbidden
            assert(checker.IsForbidden(Domain("example.com")));
            assert(!checker.IsForbidden(Domain("subexample.com")));
            assert(checker.IsForbidden(Domain("google.com")));
            assert(checker.IsForbidden(Domain("sub.example.com")));
            assert(checker.IsForbidden(Domain("mail.google.com")));
            assert(!checker.IsForbidden(Domain("example.org")));
            assert(!checker.IsForbidden(Domain("m.example.org")));
            assert(!checker.IsForbidden(Domain("google.org")));
            assert(!checker.IsForbidden(Domain("test.org")));
        }
        {
            // Тестируем пустой список запрещенных доменов
            std::vector<Domain> forbidden_domains;
            DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());
            assert(!checker.IsForbidden(Domain("example.com")));
            assert(!checker.IsForbidden(Domain("")));
            assert(!checker.IsForbidden(Domain("subexample.com")));
            assert(!checker.IsForbidden(Domain("google.com")));
            assert(!checker.IsForbidden(Domain("sub.example.com")));
            assert(!checker.IsForbidden(Domain("mail.google.com")));
            assert(!checker.IsForbidden(Domain("example.org")));
            assert(!checker.IsForbidden(Domain("m.example.org")));
            assert(!checker.IsForbidden(Domain("google.org")));
            assert(!checker.IsForbidden(Domain("test.org")));
        }
    }
}//!namespace tests

int main() {
    using namespace std::literals;
    tests::TestDomain();
    tests::TestDomainChecker();
    const std::vector<Domain> forbidden_domains = ReadDomains(std::cin, ReadNumberOnLine<size_t>(std::cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(std::cin, ReadNumberOnLine<size_t>(std::cin));
    for (const Domain& domain : test_domains) {
        std::cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << std::endl;
    }
}
