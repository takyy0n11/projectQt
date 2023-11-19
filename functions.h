#ifndef SERVER_H
#define SERVER_H

#include <QByteArray>
#include <QString>
#include <QDebug>

/**
 * @brief Класс Server представляет собой логику сервера.
 *
 * Этот класс обеспечивает авторизацию, регистрацию, отправку заданий, проверку заданий,
 * управление подключениями и другие функции, связанные с обработкой запросов от клиентов.
 */
class Functions
{
private:
    Functions();
    Functions(const Functions& );
    Functions& operator = (Functions &);
    static inline Functions *p_instance = nullptr;

public:
    /**
     * @brief generateErrorMessage генерирует сообщение об ошибке.
     * @return QByteArray с ответом сервера.
     */
    QByteArray generateErrorMessage();

    /**
     * @brief authenticateUser аутентифицирует пользователя.
     * @param username Логин пользователя.
     * @param password Пароль пользователя.
     * @return QByteArray с ответом сервера.
     */
    QByteArray authenticateUser(QString username, QString password);

    /**
     * @brief registerUser регистрирует нового пользователя.
     * @param username Логин пользователя.
     * @param password Пароль пользователя.
     * @param userType Тип пользователя.
     * @param teacherUsername Логин преподавателя (если применимо).
     * @return QByteArray с ответом сервера.
     */
    QByteArray registerUser(QString username, QString password, int userType, QString teacherUsername);

    /**
     * @brief checkAuthentication проверяет, авторизировано ли устройство.
     * @param userKey Уникальный ключ пользователя.
     * @return true, если устройство авторизовано, false в противном случае.
     */
    bool checkAuthentication(int userKey);

    /**
     * @brief completeTask отдаёт задание на проверку и возвращает ответ.
     * @param userKey Уникальный ключ пользователя.
     * @param taskNumber Номер задания.
     * @param taskKey Уникальный ключ задания.
     * @param userAnswer Ответ пользователя.
     * @return QByteArray с ответом сервера.
     */
    QByteArray completeTask(int userKey, int taskNumber, int taskKey, QString userAnswer);

    /**
     * @brief resetUserConnections сбрасывает подключения для зарегистрированных пользователей.
     * @return QByteArray с ответом сервера.
     */
    QByteArray resetUserConnections();

    /**
     * @brief userLogout удаляет подключения для пользователя с данными логином и паролем.
     * @param username Логин пользователя.
     * @param password Пароль пользователя.
     * @return QByteArray с ответом сервера.
     */
    QByteArray userLogout(QString username, QString password);

    /**
     * @brief deleteGroup удаляет группу заданного преподавателя.
     * @param userKey Уникальный ключ пользователя (преподавателя).
     * @return QByteArray с ответом сервера.
     */
    QByteArray deleteGroup(int userKey);

    /**
     * @brief isTeacher проверяет, является ли пользователь преподавателем.
     * @param userKey Уникальный ключ пользователя.
     * @return true, если пользователь - преподаватель, false в противном случае.
     */
    bool isTeacher(int userKey);

    /**
     * @brief getStudentsList возвращает список студентов преподавателя.
     * @param userKey Уникальный ключ пользователя (преподавателя).
     * @return QByteArray с ответом сервера.
     */
    QByteArray getStudentsList(int userKey);

    /**
     * @brief getTask генерирует задание и передает клиенту условие задания и его уникальный номер.
     * @param taskNumber Номер задания.
     * @return QByteArray с ответом сервера.
     */
    QByteArray getTask(int taskNumber);

    /**
     * @brief checkTask проверяет задание.
     * @param taskNumber Номер задания.
     * @param taskKey Уникальный ключ задания.
     * @param userAnswer Ответ пользователя.
     * @return true, если задание выполнено верно, false в противном случае.
     */
    bool checkTask(int taskNumber, int taskKey, QString userAnswer);

    /**
     * @brief getStatistics возвращает статистику по заданию ученика.
     * @param userKey Уникальный ключ пользователя (преподавателя).
     * @param studentUsername Логин ученика.
     * @param taskNumber Номер задания.
     * @return QByteArray с ответом сервера.
     */
    QByteArray getStatistics(int userKey, QString studentUsername, int taskNumber);

    /**
     * @brief parseUserMessage принимает сообщение пользователя и возвращает ответ подходящей функции.
     * @param message Сообщение пользователя.
     * @return QByteArray с ответом сервера.
     */
    QByteArray parseUserMessage(QString message);

private:
    /**
     * @brief generateTask генерирует условие задания.
     * @param taskNumber Номер задания.
     * @return Сгенерированное условие задания.
     */
    QString generateTask(int taskNumber);

    /**
     * @brief generateTask1 генерирует условие задания 1.
     * @param taskKey Уникальный ключ задания.
     * @return Сгенерированное условие задания 1.
     */
    QString generateTask1(int taskKey);

public:
    /**
     * @brief getInstance возвращает экземпляр синглтона Server.
     * @return Указатель на экземпляр Server.
     */
    static Functions* getInstance();
    bool checkTask1(int taskKey, QString userAnswer);
};

#endif // SERVER_H
