#include "functions.h"
#include "database.h"
#include <QString>
#include <QDebug>
#include <cmath>
#include <ctime>

Functions::Functions() {

}


QByteArray Functions::registerUser(QString username, QString password, int userType, QString teacherUsername) {
    bool registrationSuccess;
    QVector<QMap<QString, QString>> existingUser = DataBase::getInstance()->dbRequest(
        QString("select * from Users "
                "where username = '%1'").arg(username));

    if (existingUser.size() != 0) {
        registrationSuccess = false;
    } else {
        DataBase::getInstance()->dbRequest(QString("insert into Users(type, username, password) "
                                                    "values (%1, '%2', '%3')").arg(QString::number(userType), username, password));

        QVector<QMap<QString, QString>> currentUser =
            DataBase::getInstance()->dbRequest(QString("select * from Users "
                                                        "where username = '%1' and password = '%2'").arg(username, password));

        if (userType == 0) {
            DataBase::getInstance()->dbRequest(QString("insert into UserGroups "
                                                        "values('%1', '%2')").arg(teacherUsername, username));
        }
        registrationSuccess = true;
    }
    return QByteArray((QString("reg$")+QString::number(registrationSuccess)).toUtf8());
}

QByteArray Functions::generateErrorMessage() {
    return QByteArray(QString("invalid_arguments").toUtf8());
}

QByteArray Functions::authenticateUser(QString username, QString password) {
    QVector<QMap<QString, QString>> result = DataBase::getInstance()->dbRequest(
        QString("select * from Users "
                "where username = '%1' and password = '%2'").arg(username, password));

    if (result.size() == 1) {
        srand(time(0));
        int key = 1 + rand() % 1000;
        DataBase::getInstance()->dbRequest(
            QString("update Users "
                    "set key = %1 "
                    "where username = '%2' and password = '%3'").arg(QString::number(key), username, password));
        return QByteArray(QString(QString("auth$") + QString::number(key)).toUtf8());
    } else {
        return QByteArray(QString(QString("auth$") + QString::number(0)).toUtf8());
    }
}

bool Functions::checkAuthentication(int userKey) {
    bool checkAuthSuccess;
    QVector<QMap<QString, QString>> result = DataBase::getInstance()->dbRequest(
        QString("select * from Users "
                "where key = %1").arg(QString::number(userKey)));

    if (result.size() == 0) {
        checkAuthSuccess = false;
    } else if (result.size() > 1) {
        qDebug() << "Error. Multiple users with the same key." << '\n';
        checkAuthSuccess = false;
    } else {
        checkAuthSuccess = true;
    }
    return checkAuthSuccess;
}

QByteArray Functions::completeTask(int userKey, int taskNumber, int taskKey, QString userAnswer) {
    bool isAnswerCorrect = checkTask(taskNumber, taskKey, userAnswer);

    if (userKey == -1) {
        return QByteArray((QString("task_is_done$")+QString::number(isAnswerCorrect)).toUtf8());
    }

    QVector<QMap<QString, QString>> userTemp = DataBase::getInstance()->dbRequest(
        QString("select username from Users where key = %1").arg(QString::number(userKey)));

    if (userTemp.size() == 0) {
        qDebug() << "Error. User with the given key not found.";
        return QByteArray();
    }

    QString userUsername = userTemp[0]["username"];

    DataBase::getInstance()->dbRequest(QString("insert into Tasks(username, task_id, task_number, is_correct, answer) "
                                                "values('%1', %2, %3, %4, '%5')").arg
                                        (userUsername, QString::number(taskKey),
                                         QString::number(taskNumber), QString::number(isAnswerCorrect), userAnswer));
    return QByteArray((QString("task_is_done$")+QString::number(isAnswerCorrect)).toUtf8());
}

QByteArray Functions::resetUserConnections() {
    DataBase::getInstance()->dbRequest(QString("update Users "
                                                "set key = null"));
    return QByteArray(QString("reset_connections").toUtf8());
}

QByteArray Functions::userLogout(QString username, QString password) {
    bool checkLogoutSuccess;
    QVector<QMap<QString, QString>> result = DataBase::getInstance()->dbRequest(
        QString("select * "
                "from Users "
                "where username = '%1' and password = '%2'").arg(username, password));

    if (result.size() == 0) {
        qDebug() << "User not found." << '\n';
        checkLogoutSuccess = false;
    } else {
        DataBase::getInstance()->dbRequest(
            QString("update Users "
                    "set key = null "
                    "where username = '%1' and password = '%2'").arg(username, password));
        checkLogoutSuccess = true;
    }
    return QByteArray(QString(QString("user_logout$") + QString::number(checkLogoutSuccess)).toUtf8());
}

QByteArray Functions::deleteGroup(int userKey) {
    if (isTeacher(userKey)) {
        QString teacherUsername = DataBase::getInstance()->dbRequest(QString("select username "
                                                                              "from Users where key = %1").arg(userKey))[0]["username"];
        DataBase::getInstance()->dbRequest(QString("delete from UserGroups "
                                                    "where teacher_username = '%1'").arg(teacherUsername));
        return QByteArray(QString(QString("del_group$") + QString(teacherUsername)).toUtf8());
    } else {
        return QByteArray(QString(QString("del_group$") + QString("")).toUtf8());
    }
}

bool Functions::isTeacher(int userKey) {
    bool checkTeacher;
    QVector<QMap<QString, QString>> currentUser =
        DataBase::getInstance()->dbRequest(
            QString("select type from Users "
                    "where key = %1").arg(QString::number(userKey)));

    if (currentUser.size() == 0) {
        qDebug() << "User not found." << '\n';
        checkTeacher = false;
    } else if (currentUser[0]["type"] == "0") {
        checkTeacher = false;
    } else {
        checkTeacher = true;
    }
    return checkTeacher;
}

QByteArray Functions::getStudentsList(int userKey) {
    bool checkTeacher = isTeacher(userKey);
    QString response;
    if (!checkTeacher) {
        qDebug() << "The user is not a teacher." << '\n';
        return QByteArray(response.toUtf8());
    } else {
        QStringList studentList;
        QVector<QMap<QString, QString>> studentListTemp =
            DataBase::getInstance()->dbRequest(
                QString("select username from "
                        "Users as s1 "
                        "left join "
                        "UserGroups as s2 "
                        "on s1.username = s2.student_username "
                        "where s2.teacher_username = "
                        "(select username from Users where key = %1)").arg(QString::number(userKey)));
        for (int i = 0; i < studentListTemp.size(); i++) {
            studentList.push_back(studentListTemp[i]["username"]);
        }
        for (int i = 0; i < studentList.size(); i++) {
            response += studentList[i];
            if (i != studentList.size()-1) {
                response += "$";
            }
        }
        return QByteArray((QString("get_student_list$") + response).toUtf8());
    }
}

QByteArray Functions::getTask(int taskNumber) {
    return QByteArray(QString("get_task$").toUtf8()+generateTask(taskNumber).toUtf8());
}

bool Functions::checkTask(int taskNumber, int taskKey, QString userAnswer) {

    switch (taskNumber) {
    case 1:
        return checkTask1(taskKey, userAnswer);
        break;
    default:
        return false;
        break;
    }
}

QString Functions::generateTask(int taskNumber) {
    std::srand(std::time(nullptr));
    int taskKey = 1 + rand() % 10000;
    switch (taskNumber) {
    case 1:
        return generateTask1(taskKey);
        break;
    default:
        return QString("error");
        break;
    }
}


bool Functions::checkTask1(int taskKey, QString userAnswer) {
    srand(taskKey);
    double a, b, c;
    a = 2 + rand() % 100;
    b = 2 + rand() % 100;
    c = 2 + rand() % 100;
    double dis = b * b - 4 * a * c;
    qDebug() << dis;
    qDebug() << " " << a << " " << b << " " << c;

    const double epsilon = 1e-6;

    if (dis < 0 && userAnswer == "-") {
        return true;
    } else if (std::abs(dis) < epsilon) {
        if (std::abs(-b / (2 * a) - userAnswer.toDouble()) < epsilon) {
            qDebug() << QString::number(-b / (2 * a), 'f', 2);
            return true;
        }
    } else {
        double root1 = (-b - std::sqrt(dis)) / (2 * a);
        double root2 = (-b + std::sqrt(dis)) / (2 * a);
        QString expectedAnswer = QString::number(root1, 'f', 2) + "&" + QString::number(root2, 'f', 2);
        qDebug() << expectedAnswer;
        if (expectedAnswer == userAnswer) {
            return true;
        }
    }

    return false;
}


QString Functions::generateTask1(int taskKey) {
    srand(taskKey);
    int a,b,c;
    a = 2 + rand() % 100;
    b = 2 + rand() % 100;
    c = 2 + rand() % 100;
    QString d = QString("Дано квадратное уравнение %1x^2 + %2x + %3 = 0. "
                        "Найти корни уравнения, если корень один, то ввести его один раз, если корней нет то ввести \"-\". "
                        "Корни ввести с точностью до двух знаков после запятой. "
                        "Сначала вводится меньший корень, затем больший, разделяются \"&\".").arg(QString::number(a),QString::number(b),QString::number(c));
    return d+"$"+QString::number(taskKey);
}

QByteArray Functions::getStatistics(int userKey, QString studentUsername, int taskNumber) {
    bool checkAccess = isTeacher(userKey);
    QString response;
    if (!checkAccess) {
        qDebug() << "Access denied." << '\n';
        return QByteArray("");
    } else {
        QVector<QMap<QString, QString>> studentTasks =
            DataBase::getInstance()->dbRequest(
                QString("select s1.task_id, s1.is_correct from "
                        "Tasks as s1 "
                        "left join "
                        "Users as s2 "
                        "on s1.username = s2.username "
                        "where s2.username = '%1' and "
                        "s1.task_number = %2").arg(studentUsername, QString::number(taskNumber)));

        int correctAnswers = 0;
        int wrongAnswers = 0;
        QStringList wrongAnswerTasks;
        for (int i = 0; i < studentTasks.size(); i++) {
            if (studentTasks[i]["is_correct"] == "1") {
                correctAnswers++;
            } else {
                wrongAnswers++;
            }
        }
        for (int i = 0; i < studentTasks.size(); i++) {
            if (studentTasks[i]["is_correct"] == "0") {
                wrongAnswerTasks.push_back(studentTasks[i]["task_id"]);
            }
        }
        response = QString::number(correctAnswers) + "$" + QString::number(wrongAnswers);
        for (int i = 0; i < wrongAnswerTasks.size(); i++) {
            response += "$";
            response += wrongAnswerTasks[i];
        }
    }
    return QByteArray(response.toUtf8());
}

QByteArray Functions::parseUserMessage(QString message) {
    message.chop(2);
    QStringList data = message.split(' ');
    if (data[0] == "reg") {
        return registerUser(data[1], data[2], data[3].toInt(), data[4]);
    } else if (data[0] == "auth") {
        return authenticateUser(data[1], data[2]);
    } else if (data[0] ==  "check_auth") {
        return QByteArray(QString("check auth$" + QString::number(checkAuthentication(data[1].toInt()))).toUtf8());
    } else if (data[0] == "reset_connections") {
        return resetUserConnections();
    } else if (data[0] ==  "task") {
        return completeTask(data[1].toInt(), data[2].toInt(), data[3].toInt(), data[4]);
    } else if (data[0] ==  "user_logout") {
        return userLogout(data[1], data[2]);
    } else if (data[0] == "get_task") {
        return getTask(data[1].toInt());
    } else if (data[0] == "del_group") {
        return deleteGroup(data[1].toInt());
    } else if (data[0] == "is_it_a_teacher") {
        return QByteArray(QString("is it a teacher$" + QString::number(isTeacher(data[1].toInt()))).toUtf8());
    } else if (data[0] == "get_students_list") {
        return getStudentsList(data[1].toInt());
    } else if (data[0] == "get_statistics") {
        return getStatistics(data[1].toInt(), data[2], data[3].toInt());
    } else {
        return generateErrorMessage();
    }
}

Functions *Functions::getInstance() {
    if (!p_instance)
        p_instance = new Functions();
    return p_instance;
}
