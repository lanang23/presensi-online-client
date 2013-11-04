#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QtNetwork/QNetworkAccessManager"
#include "QtNetwork/QNetworkRequest"
#include "QtNetwork/QNetworkReply"
#include "QtNetwork/QHttpMultiPart"
#include "QNetworkInterface"
#include "QUrlQuery"
#include "QJsonDocument"
#include "QJsonObject"
#include "QDateTime"
#include "QMessageBox"
#include "QTimer"
#include "QLibraryInfo"

class situs
{
public:
    situs();
    QByteArray login;
    QByteArray loginCheck;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QNetworkAccessManager nwam;
    QNetworkReply *reply;
    QByteArray getMacAddress();
    situs site;

    void changeAllWidgetSatus(bool opt);
private:
    Ui::MainWindow *ui;

public slots:
    void checkLogin();
    void getResult();
    void allowed();
    void alreadyLogin();
};

#endif // MAINWINDOW_H
