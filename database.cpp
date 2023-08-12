#include "database.h"

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{

    dataBase = new QSqlDatabase();
    simplieQuery = new QSqlQuery();
    tableWidget = new QTableWidget();
    table_model_ = new QSqlTableModel();
    query_model_ = new QSqlQueryModel();
}

DataBase::~DataBase()
{
    delete dataBase;
    delete simplieQuery;
    delete tableWidget;
    delete table_model_;
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
    ///Тут должен быть код ДЗ/*
    /// !!Вот тут вопрос, почему-то выводится просто пустое поле, без данных
    query_model_->setQuery(request, *dataBase);
    QSqlError err;
    if(!query_model_->query().isActive()){
        emit sig_SendStatusRequest(query_model_->lastError());
    }

    emit sig_SendStatusRequest(err);
//    simplieQuery = new QSqlQuery(*dataBase);
//    QSqlError err;
//    if(!(simplieQuery->exec(request)))
//        emit sig_SendStatusRequest(query_model_->lastError());
//    query_model_->setQuery(std::move(*simplieQuery));
//        emit sig_SendStatusRequest(err);
}

void DataBase::RequestToDB()
{
    if(table_model_) delete table_model_;
    table_model_ = new QSqlTableModel(this, *dataBase);
    table_model_->setTable("film");
    if(!(table_model_->select())){
        emit sig_SendStatusRequest(table_model_->lastError());
    }
    table_model_->setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlError err;
    emit sig_SendStatusRequest(err);
}

void DataBase::ClearModelTable()
{
    table_model_->clear();
    query_model_->clear();
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

        emit sig_SendDataFromDB_query_table(query_model_);

        break;
    }
    case requestAllFilms:{

        emit sig_SendDataFromDB_all_table(table_model_);

        break;
    }

    default:
        break;
    }

}


