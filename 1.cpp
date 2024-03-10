#include <iostream>
#include <mysql/mysql.h>
#include <string>
#include <iomanip>

class Database {
protected:
    MYSQL* conn;

public:
    Database(const char* server, const char* user, const char* password, const char* database) {
        conn = mysql_init(NULL);
        if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
            std::cerr << "Ошибка подключения: " << mysql_error(conn) << std::endl;
            exit(1);
        }
    }

    virtual ~Database() {
        mysql_close(conn);
    }

    virtual void executeQuery(const std::string& query) {
        if (mysql_query(conn, query.c_str())) {
            std::cerr << "Ошибка запроса: " << mysql_error(conn) << std::endl;
            exit(1);
        }
    }

    virtual MYSQL_RES* getResult() {
        return mysql_store_result(conn);
    }

    virtual void freeResult(MYSQL_RES* res) {
        mysql_free_result(res);
    }
};

class CustomDatabase : public Database {
public:
    CustomDatabase(const char* server, const char* user, const char* password, const char* database)
        : Database(server, user, password, database) {}

    void executeCustomQuery(const std::string& query) {
        executeQuery(query);
    }

    MYSQL_RES* getCustomResult() {
        return getResult();
    }

    void freeCustomResult(MYSQL_RES* res) {
        freeResult(res);
    }
};

int main() {
    const char* server = "localhost";
    const char* user = "root"; // имя пользователя базы данных
    const char* password = "root"; // пароль пользователя базы данных
    const char* database = "cardofaccounts"; // имя базы данных, к которой подключаемся

    CustomDatabase db(server, user, password, database);

    // Выполнение запроса
    db.executeCustomQuery("SELECT * FROM cardofaccounts.`01`"); // замените table1 на имя таблицы, из которой берем данные

    // Вывод результатов запроса
    MYSQL_RES* res = db.getCustomResult();
    MYSQL_ROW row;

    // Вывод названий столбцов
    MYSQL_FIELD* field;
    while ((field = mysql_fetch_field(res))) {
        std::cout << "| " << std::setw(17) << field->name << " ";
    }
    std::cout << "|" << std::endl;

    // Вывод данных
    while ((row = mysql_fetch_row(res)) != NULL) {
        for (int i = 0; i < mysql_num_fields(res); i++) {
            if (row[i] != NULL) {
                std::cout << "| " << std::setw(17) << row[i] << " ";
            }
            else {
                std::cout << "| " << std::setw(17) << "NULL ";
            }
        }
        std::cout << "|" << std::endl;
    }

    db.freeCustomResult(res);

    return 0;
}
