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

#include <regex>
#include <vector>
#include <string>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTableWidget *tableLexems = ui->tableWidget_Lexems_Data;
    tableLexems->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch); // настройка таблицы

    ui->textEdit_sourceCode->setPlainText(QString("c := Ara + 233;week 622 ; 32")); // установка начального значения поля

    ui->lineEdit_inputSource->setPlaceholderText(QString("Insert path to file")); // установка пути к загружаемому файлу
    ui->lineEdit_inputSource->setText(QString("/home/pasha/Projects/examples/example.txt"));

    on_pushButton_updateFile_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::vector<std::string> MainWindow::tokenize(const std::string str, const std::regex re)
{
    std::sregex_token_iterator it{str.begin(),
                                  str.end(), re, -1};
    std::vector<std::string> tokenized{it, {}};

    // Additional check to remove empty strings
    tokenized.erase(
        std::remove_if(tokenized.begin(),
                       tokenized.end(),
                       [](std::string const &s)
                       {
                           return s.size() == 0;
                       }),
        tokenized.end());

    return tokenized;
}

void MainWindow::on_pushButton_analyze_clicked()
{
    QString sourceCode = ui->textEdit_sourceCode->toPlainText();
    std::string stdSourceCode = sourceCode.toLocal8Bit().constData();

    ui->tableWidget_Lexems_Data->setItem(0, 1, new QTableWidgetItem(sourceCode));

    QTableWidget *tableLexems = ui->tableWidget_Lexems_Data;
    QString temp;
    int c = 0;

    for (auto i = 0; i < sourceCode.length(); i++)
    { // добавить токены в таблицу значений
        if (sourceCode[i] == QChar(59))
        {
            if (temp.length() > 0)
            {
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
        if ((sourceCode[i] == QChar(32) || sourceCode[i] == QChar(10)) && temp.length() > 0)
        {
            AddToken(c, &temp, i - temp.length(), TypeOfToken(temp));
            c++;
            temp.clear();
            continue;
        }
        temp.append(sourceCode[i]);
        temp.remove(QChar(32));
        temp.remove(QChar(10));
        if (i == sourceCode.length() - 1 && temp.length() > 0 && temp != " ")
        {
            AddToken(c, &temp, sourceCode.length() - 1, TypeOfToken(temp));
        }
    }
}

void MainWindow::on_pushButton_analyze_clicked_Refacored() //// рефактор
{
    QString sourceCode = ui->textEdit_sourceCode->toPlainText();
    std::string stdSourceCode = sourceCode.toLocal8Bit().constData();
    ui->tableWidget_Lexems_Data->setItem(0, 1, new QTableWidgetItem(sourceCode));
    QTableWidget *tableLexems = ui->tableWidget_Lexems_Data;
    QString temp;
    int c = 0;
    for (auto i = 0; i < sourceCode.length(); i++)
    {
        if (sourceCode[i] == QChar(59))
        {
            if (!temp.isEmpty())
            {
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
        if ((sourceCode[i] == QChar(32) || sourceCode[i] == QChar(10)) && !temp.isEmpty())
        {
            AddToken(c, &temp, i - temp.length(), TypeOfToken(temp));
            c++;
            temp.clear();
            continue;
        }
        temp.append(sourceCode[i]);
        temp.remove(QChar(32));
        temp.remove(QChar(10));
        if (i == sourceCode.length() - 1 && !temp.isEmpty() && temp != " ")
        {
            AddToken(c, &temp, sourceCode.length() - 1, TypeOfToken(temp));
        }
    }
}

void MainWindow::AddToken(int c, QString *temp, int i, QString tokenType)
{
    QTableWidget *tableLexems = ui->tableWidget_Lexems_Data;
    tableLexems->setRowCount(c + 1);
    tableLexems->setItem(c, 0, new QTableWidgetItem(QString::number(i)));
    tableLexems->setItem(c, 1, new QTableWidgetItem(*temp));
    tableLexems->setItem(c, 2, new QTableWidgetItem(QString(tokenType)));
}

QString MainWindow::TypeOfToken(QString temp)
{
    QString tokenType("error");

    switch (temp)
    {
    case ";":
        tokenType = "точка-запятая";
        break;
    case ":=":
        tokenType = "присвоить";
        break;
    case "+":
        tokenType = "плюс";
        break;
    case "-":
        tokenType = "минус";
        break;
    case "*":
        tokenType = "умножить";
        break;
    case "/":
        tokenType = "делить";
        break;
    case "(":
        tokenType = "скобка левая";
        break;
    case ")":
        tokenType = "скобка правая";
        break;
    default:
        if (temp.contains(QRegularExpression("^[a-zA-Z_]+[a-zA-Z\\d_]*$")))
            tokenType = "идентификатор";
        else if (temp.contains(QRegularExpression("^'\\\\?.'$")))
            tokenType = "символьная константа";

        else if (temp.contains(QRegularExpression("^{.*}$")))
            tokenType = "комментарий";
        break;
    }

    return tokenType;
}

void MainWindow::on_pushButton_updateFile_clicked()
{
    QFile file(ui->lineEdit_inputSource->text()); // устанавливаем путь к файлу

    if (!file.open(QIODevice::ReadOnly))
    {                                                            // проверяем файл на доступ
        QMessageBox::information(0, "info", file.errorString()); // сообщение если файл не доступен
    }
    QTextStream in(&file); // загружаем текст из файла в поток

    ui->textEdit_sourceCode->setText(in.readAll()); // загружаем поток в текстовое поле

    file.close(); // закрываем файл
}
