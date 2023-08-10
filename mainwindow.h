#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtConcurrent>
#include "database.h"
#include "dbdata.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    void ReceiveStatusConnectionToDB(bool status);


private slots:
    void on_act_addData_triggered();
    void on_act_connect_triggered();
    void on_pb_request_clicked();
    void sl_error_handling_to_db(QSqlError err);
    void show_the_entire_table( QSqlTableModel *model);
    void show_the_query_table( QSqlQueryModel *model);


    void on_pb_clear_clicked();

signals:
    void sig_RequestToDb(QString request);

private:

    QVector<QString> dataForConnect; //Данные для подключения к БД.

    Ui::MainWindow *ui;
    DbData *dataDb;
    DataBase* dataBase;
    QMessageBox* msg;
    QVector<QString> request_genres;


};
#endif // MAINWINDOW_H
