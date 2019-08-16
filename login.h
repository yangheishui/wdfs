#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QNetworkAccessManager>
#include "common/common.h"
#include <mainwindow.h>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

    // 设置登陆用户信息的json包
    QByteArray setLoginJson(QString user, QString pwd);

    // 设置注册用户信息的json包
    QByteArray setRegisterJson(QString userName, QString nickName, QString firstPwd, QString phone, QString email);

    // 得到服务器回复的登陆状态， 状态码返回值为 "000", 或 "001"，还有登陆section
    QStringList getLoginStatus(QByteArray json);


private:

    //读取配置文件
    void readCfg();

protected:
    void paintEvent(QPaintEvent *event);

private slots:

    void on_set_ok_btn_clicked();

    void on_login_btn_clicked();

    void on_register_btn_clicked();

private:
    Ui::Login *ui;
    QNetworkAccessManager *m_manager;
    // 主窗口指针
    MainWindow* m_mainWin;
    Common m_cm;
};

#endif // LOGIN_H
