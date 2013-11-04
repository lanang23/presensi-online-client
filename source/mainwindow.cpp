#include "mainwindow.h"
#include "ui_mainwindow.h"

situs::situs()
{
    if (QLibraryInfo::isDebugBuild()) {
        login = "http://localhost/lab/presensi/";
        loginCheck = "http://localhost/lab/presensi/index.php/welcome/logincheck/";
    }
    else
    {
        login = "http://bumijonet.com/presensi/";
        loginCheck = "http://bumijonet.com/presensi/index.php/welcome/logincheck/";
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // disable all widget on startup
    changeAllWidgetSatus(false);

    // hide login label on startup
    ui->sudahLogin1->hide();
    ui->sudahLogin2->hide();

    ui->pushButton1->setProperty("id", 1);
    ui->pushButton1->setProperty("status", "logout");
    ui->pushButton2->setProperty("id", 2);
    ui->pushButton2->setProperty("status", "logout");

    ui->sudahLogin1->setAlignment(Qt::AlignCenter);
    ui->sudahLogin2->setAlignment(Qt::AlignCenter);

    // check already user login
    QTimer::singleShot(100, this, SLOT(alreadyLogin()));

    // action
    connect(ui->pushButton1, SIGNAL(clicked()), this, SLOT(checkLogin()));
    connect(ui->pushButton2, SIGNAL(clicked()), this, SLOT(checkLogin()));
    connect(ui->password1, SIGNAL(returnPressed()), ui->pushButton1, SLOT(click()));
    connect(ui->password2, SIGNAL(returnPressed()), ui->pushButton2, SLOT(click()));
    connect(ui->keterangan1, SIGNAL(returnPressed()), ui->pushButton1, SLOT(click()));
    connect(ui->keterangan2, SIGNAL(returnPressed()), ui->pushButton2, SLOT(click()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::checkLogin()
{
    QPushButton *button = qobject_cast<QPushButton *> (QObject::sender());

    int id = button->property("id").toInt();
    QString status(button->property("status").toString());
    QString username;
    QString password;
    QString posisi;
    QString keterangan;
    QString Time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    if (id==1 && status=="logout") {
        username = ui->username1->text();
        password = ui->password1->text();
        keterangan = ui->keterangan1->text();
        posisi = "1";
    }
    else if (id==2 && status=="logout") {
        username = ui->username2->text();
        password = ui->password2->text();
        keterangan = ui->keterangan2->text();
        posisi = "2";
    }
    else if (id==1 && status=="login") {
        username = ui->username1->text();
        password = ui->password1->text();
        posisi = "1";
    }
    else if (id==2 && status=="login") {
        username = ui->username2->text();
        password = ui->password2->text();
        posisi = "2";
    }

    ui->loadingText->setText("Loading...");

    QUrl url = QUrl::fromEncoded(site.login);
    QNetworkRequest request(url);

    QHttpPart data1;
    data1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"username\""));
    data1.setBody(username.toUtf8());

    QHttpPart data2;
    data2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"password\""));
    data2.setBody(password.toUtf8());

    QHttpPart macAddress;
    macAddress.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"mac\""));
    macAddress.setBody(getMacAddress());

    QHttpPart theTime;
    theTime.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"time\""));
    theTime.setBody(Time.toUtf8());

    QHttpPart dataPosisi;
    dataPosisi.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"posisi\""));
    dataPosisi.setBody(posisi.toUtf8());

    QHttpPart dataStatus;
    dataStatus.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"currentStatus\""));
    dataStatus.setBody(status.toUtf8());

    QHttpPart dataKeterangan;
    dataKeterangan.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"keterangan\""));
    dataKeterangan.setBody(keterangan.toUtf8());

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->append(data1);
    multiPart->append(data2);
    multiPart->append(macAddress);
    multiPart->append(theTime);
    multiPart->append(dataPosisi);
    multiPart->append(dataStatus);
    multiPart->append(dataKeterangan);

    reply = nwam.post(request,multiPart);
    connect(reply, SIGNAL(finished()),this,SLOT(getResult()));
    changeAllWidgetSatus(false);
}

void MainWindow::changeAllWidgetSatus(bool opt)
{
    ui->username1->setEnabled(opt);
    ui->username2->setEnabled(opt);
    ui->password1->setEnabled(opt);
    ui->password2->setEnabled(opt);
    ui->pushButton1->setEnabled(opt);
    ui->pushButton2->setEnabled(opt);
    ui->keterangan1->setEnabled(opt);
    ui->keterangan2->setEnabled(opt);
}

void MainWindow::getResult()
{
    QJsonDocument d = QJsonDocument::fromJson(reply->readAll());
    QJsonObject obj = d.object();
    bool error = obj.value("error").toBool();
    QString msg = obj.value("msg").toString();
    QString timeLogin = obj.value("timeLogin").toString();
    QString posisi = obj.value("posisi").toString();
    QString status = obj.value("status").toString();

    ui->loadingText->clear();

    if (error) {
        QMessageBox::warning(this, "Error", msg);
    } else {
        if (status=="logout") {
            if (posisi=="1") {
                ui->sudahLogin1->setText(timeLogin);
                ui->sudahLogin1->show();
                ui->username1->hide();
                ui->labelUsername1->hide();
                ui->pushButton1->setText("Logout");
                ui->pushButton1->setProperty("status", "login");
                ui->password1->clear();
                ui->keterangan1->hide();
                ui->labelKeterangan1->hide();
            } else {
                ui->sudahLogin2->setText(timeLogin);
                ui->sudahLogin2->show();
                ui->username2->hide();
                ui->labelUsername2->hide();
                ui->pushButton2->setText("Logout");
                ui->pushButton2->setProperty("status", "login");
                ui->password2->clear();
                ui->keterangan2->hide();
                ui->labelKeterangan2->hide();
            }
        } else if (status=="login") {
            if (posisi=="1") {
                ui->sudahLogin1->hide();
                ui->username1->show();
                ui->labelUsername1->show();
                ui->pushButton1->setText("Login");
                ui->pushButton1->setProperty("status", "logout");
                ui->username1->clear();
                ui->password1->clear();
                ui->labelKeterangan1->show();
                ui->keterangan1->show();
                ui->keterangan1->clear();
            } else {
                ui->sudahLogin2->hide();
                ui->username2->show();
                ui->labelUsername2->show();
                ui->pushButton2->setText("Login");
                ui->pushButton2->setProperty("status", "logout");
                ui->username2->clear();
                ui->password2->clear();
                ui->labelKeterangan2->show();
                ui->keterangan2->show();
                ui->keterangan2->clear();
            }
        }
    }
    changeAllWidgetSatus(true);
    reply->deleteLater();
}

void MainWindow::allowed()
{
    QJsonDocument d = QJsonDocument::fromJson(reply->readAll());
    QJsonObject obj = d.object();
    bool success = obj.value("success").toBool();
    QString msg = obj.value("msg").toString();
    QString username1 = obj.value("username1").toString();
    QString time1 = obj.value("time1").toString();
    QString username2 =obj.value("username2").toString();
    QString time2 = obj.value("time2").toString();

    ui->loadingText->clear();

    if (success)  {
        if (username1 != "") {
            ui->pushButton1->setProperty("status", "login");
            ui->pushButton1->setText("Logout");
            ui->username1->hide();
            ui->username1->setText(username1);
            ui->labelUsername1->hide();
            ui->sudahLogin1->setText(time1);
            ui->sudahLogin1->show();
            ui->keterangan1->hide();
            ui->labelKeterangan1->hide();
        }
        if (username2 != "") {
            ui->pushButton2->setProperty("status", "login");
            ui->pushButton2->setText("Logout");
            ui->username2->hide();
            ui->username2->setText(username2);
            ui->labelUsername2->hide();
            ui->sudahLogin2->setText(time2);
            ui->sudahLogin2->show();
            ui->keterangan2->hide();
            ui->labelKeterangan2->hide();
        }
        changeAllWidgetSatus(true);
    } else {
        QMessageBox::warning(this, "Error", msg);
    }
}

void MainWindow::alreadyLogin()
{
    ui->loadingText->setText("Connecting server...");
    QUrl url = QUrl::fromEncoded(site.loginCheck);
    QNetworkRequest request(url);

    QHttpPart macAddress;
    macAddress.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"mac\""));
    macAddress.setBody(getMacAddress());

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->append(macAddress);

    reply = nwam.post(request,multiPart);
    connect(reply, SIGNAL(finished()),this,SLOT(allowed()));
}

QByteArray MainWindow::getMacAddress()
{
    QString text;
    foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces())
    {
        if (!interface.hardwareAddress().isEmpty()) {
            text += interface.hardwareAddress()+" ";
        }
    }
    return text.toUtf8();
}
