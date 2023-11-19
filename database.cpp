#include "database.h"

DataBase *DataBase::getInstance()
{
    if (!p_instance)
        p_instance = new DataBase();
    return p_instance;
}

bool DataBase::dbCreated()
{
    return this->db.tables() == QStringList{"Users", "Tasks", "UserGroups"};
}

void DataBase::createDb()
{
    QSqlQuery query(this->db);
    query.exec("CREATE TABLE Users("
               "username VARCHAR(64) PRIMARY KEY, "
               "password VARCHAR(64) NOT NULL, "
               "type INTEGER, "
               "key INTEGER DEFAULT NULL "
               ")");
    query.exec("CREATE TABLE Tasks("
               "task_id INTEGER NOT NULL, "
               "username VARCHAR(64) NOT NULL, "
               "task_number INTEGER NOT NULL, "
               "answer TEXT, "
               "is_correct BOOLEAN NOT NULL"
               ")");
    query.exec("CREATE TABLE UserGroups("
               "teacher_username VARCHAR(64) NOT NULL, "
               "student_username VARCHAR(64) NOT NULL"
               ")");
}

DataBase::DataBase()
{
    this->db = QSqlDatabase::addDatabase("QSQLITE");
    this->db.setDatabaseName("database.sqlite");
    if (!(this->db.open()))
        qDebug()<<db.lastError().text();
    if (!this->dbCreated())
        this->createDb();
}

DataBase::~DataBase()
{
    db.close();
}

QVector<QMap<QString, QString>> DataBase::dbRequest(QString request)
{
    QSqlQuery query(this->db);
    query.exec(request);
    QVector<QMap<QString, QString>> result;
    if (query.lastError().isValid()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
            return result;
    }
    QSqlRecord record = query.record();
    QVector<QString> columns;
    for (int i = 0; i < record.count(); i++) {
        QString columnName = record.fieldName(i);
        columns.append(columnName);
    }
    while (query.next()) {
        QMap<QString, QString> row;
        for (int i = 0; i < columns.count(); i++) {
            QString columnName = query.record().fieldName(i);
            QString value = query.value(i).toString();
            row.insert(columnName, value);
        }
        result.append(row);
    }
    return result;
}

void DataBase::dbClear()
{
    // очищаются таблицы Users и Tasks, но не удаляются
    QSqlQuery query(this->db);
    query.exec("DELETE FROM TABLE Users");
    query.exec("DELETE FROM TABLE Tasks");
    query.exec("DELETE FROM TABLE UserGroups");
}
