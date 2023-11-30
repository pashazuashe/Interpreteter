#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <regex>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:


    void on_pushButton_analyze_clicked();

    void on_pushButton_updateFile_clicked();

private:
    Ui::MainWindow *ui;
    void AddToken( int c, QString *temp, int i, QString tokenType);
    QString TypeOfToken(QString temp);
    std::vector<std::string> tokenize(const std::string str,const std::regex re);
};
#endif // MAINWINDOW_H
