#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

/**
 * @brief Класс DataBase реализует взаимодействие с базой данных.
 *
 * Этот класс обеспечивает выполнение запросов к базе данных, очистку базы и вывод таблицы в консоль.
 */
class DataBase
{
private:
    DataBase();
    DataBase(const DataBase& ) = delete;
    DataBase& operator = (DataBase &) = delete;
    static inline DataBase *p_instance = nullptr;
    ~DataBase();

    QSqlDatabase db;

    /**
     * @brief db_created проверяет существование базы данных.
     * @return true, если база данных существует, false в противном случае.
     */
    bool dbCreated();

    /**
     * @brief create_db создает базу данных.
     */
    void createDb();

public:
    /**
     * @brief db_request отправляет запрос в базу данных.
     * @param request SQL-запрос.
     * @return QVector<QMap<QString, QString>> с результатом запроса.
     */
    QVector<QMap<QString, QString>> dbRequest(QString request);

    /**
     * @brief db_clear очищает базу данных.
     */
    void dbClear();

public:
    /**
     * @brief getInstance возвращает экземпляр синглтона DataBase.
     * @return Указатель на экземпляр DataBase.
     */
    static DataBase* getInstance();
};

#endif // DATABASE_H
