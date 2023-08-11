#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Исходное состояние виджетов
    ui->setupUi(this);
    ui->lb_statusConnect->setStyleSheet("color:red");
    ui->pb_request->setEnabled(false);

    /*
     * Выделим память под необходимые объекты. Все они наследники
     * QObject, поэтому воспользуемся иерархией.
    */

    dataDb = new DbData(this);
    dataBase = new DataBase(this);
    msg = new QMessageBox(this);

    //Установим размер вектора данных для подключения к БД
    dataForConnect.resize(NUM_DATA_FOR_CONNECT_TO_DB);

    /*
     * Добавим БД используя стандартный драйвер PSQL и зададим имя.
    */
    dataBase->AddDataBase(POSTGRE_DRIVER, DB_NAME);

    /*
     * Устанавливаем данные для подключениея к БД.
     * Поскольку метод небольшой используем лямбда-функцию.
     */
    connect(dataDb, &DbData::sig_sendData, this, [&](QVector<QString> receivData){
        dataForConnect = receivData;
    });

    /*
     *  Сигнал для подключения к БД
     */
    connect(dataBase, &DataBase::sig_SendStatusConnection, this, &MainWindow::ReceiveStatusConnectionToDB);
    connect(dataBase, &DataBase::sig_SendStatusRequest, this, &MainWindow::sl_error_handling_to_db);
    connect(dataBase, &DataBase::sig_SendDataFromDB_all_table, this, &MainWindow::show_the_entire_table);
    connect(dataBase, &DataBase::sig_SendDataFromDB_query_table, this, &MainWindow::show_the_query_table);

    request_genres.emplaceBack("SELECT title, release_year, c.name  FROM film f "
                               "JOIN film_category fc on f.film_id = fc.film_id "
                               "JOIN category c on c.category_id  = fc.category_id");
    request_genres.emplaceBack("SELECT title, description  FROM film f "
                               "JOIN film_category fc on f.film_id = fc.film_id "
                               "JOIN category c on c.category_id  = fc.category_id "
                               "WHERE c.name = 'Comedy'");
    request_genres.emplaceBack("SELECT title, description  FROM film f "
                               "JOIN film_category fc on f.film_id = fc.film_id "
                               "JOIN category c on c.category_id  = fc.category_id "
                               "WHERE c.name = 'Horror'");
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*!
 * @brief Слот отображает форму для ввода данных подключения к БД
 */
void MainWindow::on_act_addData_triggered()
{
    //Отобразим диалоговое окно. Какой метод нужно использовать?
    dataDb->show();
}

/*!
 * @brief Слот выполняет подключение к БД. И отображает ошибки.
 */

void MainWindow::on_act_connect_triggered()
{
    /*
     * Обработчик кнопки у нас должен подключаться и отключаться от БД.
     * Можно привязаться к надписи лейбла статуса. Если он равен
     * "Отключено" мы осуществляем подключение, если "Подключено" то
     * отключаемся
    */

    if(ui->lb_statusConnect->text() == "Отключено"){

       ui->lb_statusConnect->setText("Подключение");
       ui->lb_statusConnect->setStyleSheet("color : black");


       auto conn = [&]{dataBase->ConnectToDataBase(dataForConnect);};
       QtConcurrent::run(conn);

    }
    else{
        dataBase->DisconnectFromDataBase(DB_NAME);
        ui->lb_statusConnect->setText("Отключено");
        ui->act_connect->setText("Подключиться");
        ui->lb_statusConnect->setStyleSheet("color:red");
        ui->pb_request->setEnabled(false);
    }

}

/*!
 * \brief Обработчик кнопки "Получить"
 */
void MainWindow::on_pb_request_clicked()
{
    ///Тут должен быть код ДЗ
    dataBase->ClearModelTable();
    if(ui->cb_category->currentIndex() == 0){
        dataBase->RequestToDB();

    }else if(ui->cb_category->currentIndex() == 1){
        dataBase->RequestToDB(request_genres[ui->cb_category->currentIndex()]);
    }else if(ui->cb_category->currentIndex() == 2){
        dataBase->RequestToDB(request_genres[ui->cb_category->currentIndex()]);
    }

    //dataBase->RequestToDB(request_genres[ui->cb_category->currentIndex()]);
}

void MainWindow::sl_error_handling_to_db(QSqlError err)
{
    if(err.type() != QSqlError::NoError){
        msg->setText(err.text());
        msg->show();
    }else{
       dataBase->ReadAnswerFromDB(ui->cb_category->currentIndex() + 1);
    }
}

void MainWindow::show_the_entire_table( QSqlTableModel *model)
{
    ui->tb_view->setModel(model);
    ui->tb_view->show();
    ui->tb_view->update();
    ui->tb_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for(int i = 0; i < model->columnCount(); ++i){
        if(i != 1 && i != 2 ){
            ui->tb_view->hideColumn(i);
        }
    }
}

void MainWindow::show_the_query_table(QSqlQueryModel *model)
{
    ui->tb_view->setModel(model);
    ui->tb_view->show();
    ui->tb_view->update();
    ui->tb_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


/*!
 * \brief Метод изменяет стотояние формы в зависимости от статуса подключения к БД
 * \param status
 */
void MainWindow::ReceiveStatusConnectionToDB(bool status)
{
    if(status){
        ui->act_connect->setText("Отключиться");
        ui->lb_statusConnect->setText("Подключено к БД");
        ui->lb_statusConnect->setStyleSheet("color:green");
        ui->pb_request->setEnabled(true);
    }
    else{
        dataBase->DisconnectFromDataBase(DB_NAME);
        msg->setIcon(QMessageBox::Critical);
        msg->setText(dataBase->GetLastError().text());
        ui->lb_statusConnect->setText("Отключено");
        ui->lb_statusConnect->setStyleSheet("color:red");
        msg->exec();
    }

}

void MainWindow::on_pb_clear_clicked()
{
    dataBase->ClearModelTable();
}

