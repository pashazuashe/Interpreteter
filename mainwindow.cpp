#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QPixmap>
#include <QTableView>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>
#include <QtDebug>
#include <QMap>
#include <QRegularExpression>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTableWidget *tableLexems = ui->tableWidget_Lexems_Data;
    tableLexems->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);// настройка таблицы

    ui->textEdit_sourceCode->setPlainText(QString("c := Ara + 233;week 622 ; 32"));// установка начального значения поля

    ui->lineEdit_inputSource->setPlaceholderText(QString("Insert path to file"));// установка пути к загружаемому файлу
    ui->lineEdit_inputSource->setText(QString("/home/pasha/Projects/examples/example.txt"));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_analyze_clicked()
{
    QString sourceCode = ui->textEdit_sourceCode->toPlainText();

    ui->tableWidget_Lexems_Data->setItem(0,1, new QTableWidgetItem(sourceCode));

//    QString temp = ui->textEdit_sourceCode->toPlainText();
//    QTextStream in(&temp);
//    ui->label_test->setText(in.readAll());

//    auto length = temp.length();
//    ui->statusbar->showMessage(length, 2000);

//    QMap<int, QString> map;

    QTableWidget *tableLexems = ui->tableWidget_Lexems_Data;
    QString temp;
    int c = 0;
    for (auto i = 0; i < sourceCode.length(); i++){ // добавить токены в таблицу значений
        if (sourceCode[i] == QChar(59)){
            if (temp.length() > 0){
                AddToken(c, &temp, i - temp.length(), TypeOfToken(temp));
                c++;
                temp.clear();
            }
            QString tempChar(";");
            AddToken(c, &tempChar, i, TypeOfToken(tempChar));
            c++;
            temp.clear();
            continue;
        }
        if ((sourceCode[i] == QChar(32) || sourceCode[i] == QChar(10)) && temp.length() > 0){
            AddToken(c, &temp, i - temp.length(), TypeOfToken(temp));
            c++;
            temp.clear();
            continue;
        }
        temp.append(sourceCode[i]);
        temp.remove(QChar(32));
        temp.remove(QChar(10));
        if (i == sourceCode.length()-1 && temp.length() > 0 && temp != " "){
            AddToken(c, &temp, sourceCode.length() - 1, TypeOfToken(temp));
        }
    }



}

void MainWindow::AddToken( int c, QString *temp, int i, QString tokenType){
    QTableWidget *tableLexems = ui->tableWidget_Lexems_Data;
    tableLexems->setRowCount(c + 1);
    tableLexems->setItem(c, 1, new QTableWidgetItem(*temp));
    tableLexems->setItem(c, 0, new QTableWidgetItem(QString::number(i)));
    tableLexems->setItem(c, 2, new QTableWidgetItem(QString(tokenType)));
}

QString MainWindow::TypeOfToken(QString temp){
    QString str("null");

    QRegularExpression re1("^;$");
    QRegularExpression re2("^:=$");
    QRegularExpression re3("^\\+$");
    QRegularExpression re4("^\\-$");
    QRegularExpression re5("^\\*$");
    QRegularExpression re6("^\\/$");
    QRegularExpression re7("^[a-zA-Z]+[a-zA-Z\\d]*");
    QRegularExpression re8("\\d+");
    QRegularExpression re81("\\D+");

    QRegularExpressionMatch match = re1.match(temp);
    if (match.hasMatch()){
        str = "точка-запятая";
        return str;
    }
    match = re2.match(temp);
    if (match.hasMatch()){
        str = "присвоить";
        return str;
    }
    match = re3.match(temp);
    if (match.hasMatch()){
        str = "плюс";
        return str;
    }
    match = re4.match(temp);
    if (match.hasMatch()){
        str = "минус";
        return str;
    }
    match = re5.match(temp);
    if (match.hasMatch()){
        str = "умножить";
        return str;
    }
    match = re6.match(temp);
    if (match.hasMatch()){
        str = "делить";
        return str;
    }
    match = re7.match(temp);
    if (match.hasMatch()){
        str = "переменная";
        return str;
    }
    match = re8.match(temp);
    if (match.hasMatch()){
        match = re81.match(temp);
        if (match.hasMatch()){
            return str = "null";
        }
        str = "число";
        return str;
    }



    return str;
}

void MainWindow::on_pushButton_updateFile_clicked()
{
    QFile file(ui->lineEdit_inputSource->text());// устанавливаем путь к файлу

    if (!file.open(QIODevice::ReadOnly)){// проверяем файл на доступ
        QMessageBox::information(0,"info", file.errorString());// сообщение если файл не доступен
    }
    QTextStream in(&file);// загружаем текст из файла в поток

    ui->textEdit_sourceCode->setText(in.readAll());// загружаем поток в текстовое поле

    file.close();// закрываем файл

}

