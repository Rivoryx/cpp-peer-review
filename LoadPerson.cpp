// Конфигурация базы данных
struct DBConfig {
    string_view name;               // Название базы данных
    int connection_timeout;         // Время ожидания подключения
    bool allow_exceptions;          // Разрешение на исключения
    DBLogLevel log_level;           // Уровень подробности ведения журнала
};

// Конфигурация запроса для выборки людей
struct QueryConfig {
    int min_age;                    // Минимальный возраст
    int max_age;                    // Максимальный возраст
    string_view name_filter;        // Фильтр по имени
};

// Загрузка списка людей из базы данных на основе конфигурации
vector<Person> LoadPersons(const DBConfig& db_config, const QueryConfig& query_config) {
    // Создание подключения к базе данных
    DBConnector connector(db_config.allow_exceptions, db_config.log_level);
    DBHandler db;

    // Установка соединения в зависимости от типа базы данных
    if (db_config.name.starts_with("tmp."s)) {
        db = connector.ConnectTmp(db_config.name, db_config.connection_timeout);
    }
    else {
        db = connector.Connect(db_config.name, db_config.connection_timeout);
    }

    // Если запрещено выкидывать исключения и база данных не готова к работе
    // Возвращаем пустой результат и завершаем работу функции
    if (!db_config.allow_exceptions && !db.IsOK()) {
        return {};
    }

    // Построение запроса на основе конфигурации
    DBQuery query = BuildPersonQuery(query_config);

    // Загрузка людей из базы данных
    vector<Person> persons = LoadPersonsFromDB(db, query);

    return persons;
}

// Построение запроса на выборку пюдей на основе конфигурации
DBQuery BuildPersonQuery(const QueryConfig& query_config) {
    ostringstream query_str;
    query_str << "from Persons "s
        << "select Name, Age "s
        << "where Age between "s << query_config.min_age << " and "s << query_config.max_age << " "s
        << "and Name like '%"s << db.Quote(query_config.name_filter) << "%'"s;
    return DBQuery(query_str.str());
}

// Загрузка списка людей из базы данных на основе запроса
vector<Person> LoadPersonsFromDB(DBHandler& db, const DBQuery& query) {
    vector<Person> persons;

    // Загрузка данных о людях на основе запроса
    for (auto [name, age] : db.LoadRows<string, int>(query)) {
        persons.push_back({ move(name), age });
    }

    return persons;
}
