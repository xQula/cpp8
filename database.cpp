#include "database.h"

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{

    dataBase = new QSqlDatabase();
    simplieQuery = new QSqlQuery();
    tableWidget = new QTableWidget();
}

DataBase::~DataBase()
{
    delete dataBase;
    delete simplieQuery;
    delete tableWidget;
}

/*!
 * \brief Метод добавляет БД к экземпляру класса QSqlDataBase
 * \param driver драйвер БД
 * \param nameDB имя БД (Если отсутствует Qt задает имя по умолчанию)
 */
void DataBase::AddDataBase(QString driver, QString nameDB)
{

    *dataBase = QSqlDatabase::addDatabase(driver, nameDB);

}

/*!
 * \brief Метод подключается к БД
 * \param для удобства передаем контейнер с данными необходимыми для подключения
 * \return возвращает тип ошибки
 */
void DataBase::ConnectToDataBase(QVector<QString> data)
{

    dataBase->setHostName(data[hostName]);
    dataBase->setDatabaseName(data[dbName]);
    dataBase->setUserName(data[login]);
    dataBase->setPassword(data[pass]);
    dataBase->setPort(data[port].toInt());


    ///Тут должен быть код ДЗ


    bool status;
    status = dataBase->open( );
    emit sig_SendStatusConnection(status);

}
/*!
 * \brief Метод производит отключение от БД
 * \param Имя БД
 */
void DataBase::DisconnectFromDataBase(QString nameDb)
{

    *dataBase = QSqlDatabase::database(nameDb);
    dataBase->close();

}
/*!
 * \brief Метод формирует запрос к БД.
 * \param request - SQL запрос
 * \return
 */
void DataBase::RequestToDB(QString request)
{
    ///Тут должен быть код ДЗ
    *simplieQuery = QSqlQuery(*dataBase);
    QSqlError err;
    if(!simplieQuery->exec(request)){
        emit sig_SendStatusRequest(simplieQuery->lastError());
    }

    emit sig_SendStatusRequest(err);
}

/*!
 * @brief Метод возвращает последнюю ошибку БД
 */
QSqlError DataBase::GetLastError()
{
    return dataBase->lastError();
}

void DataBase::ReadAnswerFromDB(int requestType)
{
    /*
     * Используем оператор switch для разделения запросов
    */
    switch (requestType) {
    //Для наших запросов вид таблицы не поменяетя. Поэтому бужет единый обработчик.

    case requestComedy:{

    }
    case requestHorrors:{
        tableWidget->setColumnCount(2);
        tableWidget->setRowCount(0);
        QStringList hdrs;
        hdrs << "Название фильма" << "Описание фильма";
        tableWidget->setHorizontalHeaderLabels(hdrs);
        uint32_t conterRows = 0;
        while(simplieQuery->next()){
            QString str;
            tableWidget->insertRow(conterRows);

            for(int i = 0; i<tableWidget->columnCount(); ++i){

                str = simplieQuery->value(i).toString();
                QTableWidgetItem *item = new QTableWidgetItem(str);
                tableWidget->setItem(tableWidget->rowCount() - 1, i, item);

            }
            ++conterRows;
        }

        emit sig_SendDataFromDB(tableWidget, requestAllFilms);

        break;
    }
    case requestAllFilms:{
        /*
         * Объект tableWidget представляет из себя таблицу, которую можно отобразить
         * в ПИ. Зfдадим ее параметры и заголовки. Мы получаем из БД название фильма, год выпуска и жанр
        */

        /* QSqlQuery
         * После запроса к БД ответ помещается в объект QSqlQuery.
         * Его необходимо считвать построчно. Для чтения ответа используется метод
         * next, он работает следующим образом: при вызове метода указатель перемещается
         * на следующую запись, в ответе. После получения ответа он указывает, если можно так
         * выразится на -1 запись. Т.е. при первом вызове метода next указатель переместится
         * на первую запись, а сам метод вернет true. В случае если указатель переместится
         * за диапазон записей метод вернет false. Также он вернет false в случае отсутствия
         * записей.
         * Каждая запись является стройкой прочитанной из БД. Для перемещения по столбцам
         * используется метод QSqlQuery value(индекс столбца).
        */
        /* QTableWidget
         * Необходимо сформировать таблицу, которую потом передадим в MainWindow для отображения
         * Для каждой записи прочитанной при помощи метода next будем добавлять строку в tableWidget
         * и заполнять значениями из ответа. Каждая ячейка таблицы является объектом класса
         * QTableWidgetItem. Т.е. для заполнения каждой ячеки мы должны создать экземпляр этого метода.
         * Значение ячейки в таблицк устанавливается при помощи метода setItem.
        */
        /*
         * После заполнения всей таблицы мы ее передаем в MainWindow при помощи сигнала.
        */

        tableWidget->setColumnCount(3);
        tableWidget->setRowCount(0);
        QStringList hdrs;
        hdrs << "Название" << "Год выпуска" << "Жанр";
        tableWidget->setHorizontalHeaderLabels(hdrs);

        uint32_t conterRows = 0;

        while(simplieQuery->next()){
            QString str;
            tableWidget->insertRow(conterRows);

            for(int i = 0; i<tableWidget->columnCount(); ++i){

                str = simplieQuery->value(i).toString();
                QTableWidgetItem *item = new QTableWidgetItem(str);
                tableWidget->setItem(tableWidget->rowCount() - 1, i, item);

            }
            ++conterRows;
        }

        emit sig_SendDataFromDB(tableWidget, requestAllFilms);

        break;
    }

    default:
        break;
    }

}
