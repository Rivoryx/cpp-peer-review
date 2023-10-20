#include <string>
#include <vector>
#include <list>

using namespace std;

struct Country {
public:
    // Добавляем языки из списка к стране
    void AddLanguages(const std::list<Language>& lang_list) {
        for (const auto& lang_obj : lang_list) {
            languages.push_back(FromString<Language>(lang_obj.AsString()));
        }
    }

    std::string name;
    std::string iso_code;
    std::string phone_code;
    std::string time_zone;
    std::vector<Language> languages;
};

class CityBuilder {
public:
    CityBuilder& SetName(const std::string& name) {
        name_ = name;
        return *this;
    }
    CityBuilder& SetIsoCode(const std::string& iso_code) {
        iso_code_ = iso_code;
        return *this;
    }
    // Устанавливаем общий телефонный код, объединяя код страны и код города
    CityBuilder& SetPhoneCode(const std::string& country_phone_code, const std::string& city_phone_code) {
        phone_code_ = country_phone_code + city_phone_code;
        return *this;
    }
    CityBuilder& SetCountryName(const std::string& country_name) {
        country_name_ = country_name;
        return *this;
    }
    CityBuilder& SetCountryIsoCode(const std::string& country_iso_code) {
        country_iso_code_ = country_iso_code;
        return *this;
    }
    CityBuilder& SetTimeZone(const std::string& time_zone) {
        time_zone_ = time_zone;
        return *this;
    }
    CityBuilder& SetLanguage(const std::vector<Language>& language) {
        language_ = language;
        return *this;
    }

    // Преобразуем билдер в объект города
    operator City() const {
        return {
            name_,
            iso_code_,
            phone_code_,
            country_name_,
            country_iso_code_,
            time_zone_,
            languages_
        };
    }

private:
    std::string name_;
    std::string iso_code_;
    std::string phone_code_;
    std::string country_name_;
    std::string country_iso_code_;
    std::string time_zone_;
    std::vector<Language> languages_;
};

// Функция ParseCitySubjson обрабатывает JSON-объект со списком городов конкретной страны:
void ParseCitySubjson(vector<City>& cities, const Json& json, const Country& country) {
    for (const auto& city_json : json.AsList()) {
        const auto& city_obj = city_json.AsObject();

        // Создаем объект города с помощью билдера
        City city = CityBuilder()
            .SetName(city_obj["name"s].AsString())
            .SetIsoCode(city_obj["iso_code"s].AsString())
            .SetPhoneCode(country.phone_code, city_obj["phone_code"s].AsString())
            .SetCountryName(country.name)
            .SetCountryIsoCode(country.iso_code)
            .SetTimeZone(country.time_zone)
            .SetLanguage(country.languages);

        // Добавляем город в вектор городов
        cities.push_back(std::move(city));
    }
}

// Функция ParseCitySubjson вызывается только из функции ParseCountryJson следующим образом:
void ParseCountryJson(vector<Country>& countries, vector<City>& cities, const Json& json) {
    for (const auto& country_json : json.AsList()) {
        const auto& country_obj = country_json.AsObject();

        // Создаем объект страны
        Country country{
            .name = country_obj["name"s].AsString(),
            .iso_code = country_obj["iso_code"s].AsString(),
            .phone_code = country_obj["phone_code"s].AsString(),
            .time_zone = country_obj["time_zone"s].AsString()
        };

        // Добавляем языки к стране
        country.AddLanguages(country_obj["languages"s].AsList());

        // Добавляем страну в вектор стран
        countries.push_back(std::move(country));

        // Обрабатываем города для данной страны
        ParseCitySubjson(cities, country_obj["cities"s], countries.back());
    }
}
