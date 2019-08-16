#include "login.h"
#include "ui_login.h"
#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QRegExp>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include "common/des.h"
#include "common/logininfoinstance.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    //初始化
    m_manager = Common::getNetManager();

    // 此处无需指定父窗口
    m_mainWin = new MainWindow;

    //窗口图片
    this->setWindowIcon(QIcon(":/images/logo.ico"));

    // 去掉边框
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);

    //设置字体
    this->setFont(QFont("新宋体", 12, QFont::Bold, false));

    //密码
    ui->log_pwd->setEchoMode(QLineEdit::Password);
    ui->reg_pwd->setEchoMode(QLineEdit::Password);
    ui->reg_surepwd->setEchoMode(QLineEdit::Password);

    //当前显示窗口
    ui->stackedWidget->setCurrentIndex(0);
    ui->log_usr->setFocus();

    // 数据的格式提示
    ui->log_usr->setToolTip("合法字符:[a-z|A-Z|#|@|0-9|-|_|*],字符个数: 3~16");
    ui->reg_usr->setToolTip("合法字符:[a-z|A-Z|#|@|0-9|-|_|*],字符个数: 3~16");
    ui->reg_nickname->setToolTip("合法字符:[a-z|A-Z|#|@|0-9|-|_|*],字符个数: 3~16");
    ui->log_pwd->setToolTip("合法字符:[a-z|A-Z|#|@|0-9|-|_|*],字符个数: 6~18");
    ui->reg_pwd->setToolTip("合法字符:[a-z|A-Z|#|@|0-9|-|_|*],字符个数: 6~18");
    ui->reg_surepwd->setToolTip("合法字符:[a-z|A-Z|#|@|0-9|-|_|*],字符个数: 6~18");

    //读取配置文件完成初始化
    readCfg();

    // 注册
       connect(ui->log_register_btn, &QToolButton::clicked, [=]()
       {
           // 切换到注册界面
           ui->stackedWidget->setCurrentWidget(ui->register_page);
           ui->reg_usr->setFocus();
       });

       // 设置按钮
       connect(ui->title_widget, &TitleWidget::showSetWidget, [=]()
       {
           // 切换到设置界面
           ui->stackedWidget->setCurrentWidget(ui->set_page);
           ui->address_server->setFocus();
       });

       // 关闭按钮
       connect(ui->title_widget, &TitleWidget::closeWindow, [=]()
       {
           // 如果是注册窗口
           if(ui->stackedWidget->currentWidget() == ui->register_page)
           {
               // 清空数据
               ui->reg_mail->clear();
               ui->reg_usr->clear();
               ui->reg_nickname->clear();
               ui->reg_pwd->clear();
               ui->reg_surepwd->clear();
               ui->reg_phone->clear();
               // 窗口切换
               ui->stackedWidget->setCurrentWidget(ui->login_page);
               ui->log_usr->setFocus();
           }
           // 如果是设置窗口
           else if(ui->stackedWidget->currentWidget() == ui->set_page)
           {
               // 清空数据
               ui->address_server->clear();
               ui->port_server->clear();
               // 窗口切换
               ui->stackedWidget->setCurrentWidget(ui->login_page);
               ui->log_usr->setFocus();
           }
           // 如果是登录窗口
           else if(ui->stackedWidget->currentWidget() == ui->login_page)
           {
               close();
           }
       });

#if 1
    // 测试数据
    ui->reg_usr->setText("kevin_666");
    ui->reg_nickname->setText("kevin@666");
    ui->reg_pwd->setText("123456");
    ui->reg_surepwd->setText("123456");
    ui->reg_phone->setText("11111111111");
    ui->reg_mail->setText("abc@qq.com");
#endif


}

Login::~Login()
{
    delete ui;
}

QByteArray Login::setLoginJson(QString user, QString pwd)
{
    QMap<QString, QVariant> login;
       login.insert("user", user);
       login.insert("pwd", pwd);

       /*json数据如下
           {
               user:xxxx,
               pwd:xxx
           }
       */

       QJsonDocument jsonDocument = QJsonDocument::fromVariant(login);
       if ( jsonDocument.isNull() )
       {
           cout << " jsonDocument.isNull() ";
           return "";
       }

       return jsonDocument.toJson();
}

QByteArray Login::setRegisterJson(QString userName, QString nickName, QString firstPwd, QString phone, QString email)
{
    QMap<QString, QVariant> reg;
    reg.insert("userName", userName);
    reg.insert("nickName", nickName);
    reg.insert("firstPwd", firstPwd);
    reg.insert("phone", phone);
    reg.insert("email", email);

    /*json数据如下
        {
            userName:xxxx,
            nickName:xxx,
            firstPwd:xxx,
            phone:xxx,
            email:xxx
        }
    */
    QJsonDocument jsonDocument = QJsonDocument::fromVariant(reg);
    if ( jsonDocument.isNull() )
    {
        cout << " jsonDocument.isNull() ";
        return "";
    }

    //cout << jsonDocument.toJson().data();

    return jsonDocument.toJson();
}

QStringList Login::getLoginStatus(QByteArray json)
{

}

void Login::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap pixmap(":/images/login_bk.jpg");
    painter.drawPixmap(0, 0, width(), height(), pixmap);
}

void Login::readCfg()
{
    //获取配置文件的值
    QString user = m_cm.getCfgValue("login", "user");
    QString pwd = m_cm.getCfgValue("login", "pwd");
    QString remember = m_cm.getCfgValue("login", "remember");
    int ret = 0;

    //记住密码
    if (remember == "yes")
    {
       unsigned char pwdOutData[4096];
       int pwdLen = 0;

       QByteArray tmp =  QByteArray::fromBase64(pwd.toLocal8Bit());
      ret = DesDec((unsigned char *)tmp.data(), tmp.size(), pwdOutData, &pwdLen);
       if (ret != 0)
       {
           cout << "解密失败";
           return;
       }
    #ifdef _WIN32
       ui->log_pwd->setText(QString::fromLocal8Bit((const char *)pwdOutData, pwdLen));
    #else
       ui->log_pwd->setText((const char *)pwdOutData);
    #endif

    }
    else
    {
        ui->log_pwd->setText("");
        ui->rember_pwd->setCheckable(false);
    }

    //处理用户
    QByteArray userarry = QByteArray::fromBase64(user.toLocal8Bit());
    unsigned char userDes[4096] = {0};
    int userLen = 0;
    ret = DesDec((unsigned char *)userarry.data(), userarry.size(), userDes, &userLen);
    if (ret != 0)
    {
        cout << "解密用户失败";
        return;
    }
#ifdef _WIN32
    ui->log_usr->setText(QString::fromLocal8Bit((const char *)userDes, userLen));
#else
    ui->log_usr->setText((const char *)userDes);
#endif

    QString ip = m_cm.getCfgValue("web_server", ip);
    QString port = m_cm.getCfgValue("web_server", port);
    ui->address_server->setText(ip);
    ui->port_server->setText(port);
}

void Login::on_set_ok_btn_clicked()
{
    QString ip = ui->address_server->text();
    QString port = ui->port_server->text();

    // 数据判断
    // 服务器IP
    // \\d 和 \\. 中第一个\是转义字符, 这里使用的是标准正则
    QRegExp regexp(IP_REG);
    if(!regexp.exactMatch(ip))
    {
        QMessageBox::warning(this, "警告", "您输入的IP格式不正确, 请重新输入!");
        return;
    }
    // 端口号
    regexp.setPattern(PORT_REG);
    if(!regexp.exactMatch(port))
    {
        QMessageBox::warning(this, "警告", "您输入的端口格式不正确, 请重新输入!");
        return;
    }
    // 跳转到登陆界面
    ui->stackedWidget->setCurrentWidget(ui->login_page);
    // 将配置信息写入配置文件中
    m_cm.writeWebInfo(ip, port);
}

// 用户登录操作
void Login::on_login_btn_clicked()
{
    // 获取用户登录信息
    QString user = ui->log_usr->text();
    QString pwd = ui->log_pwd->text();
    QString address = ui->address_server->text();
    QString port = ui->port_server->text();

#if 0
    // 数据校验
    QRegExp regexp(USER_REG);
    if(!regexp.exactMatch(user))
    {
        QMessageBox::warning(this, "警告", "用户名格式不正确");
        ui->log_usr->clear();
        ui->log_usr->setFocus();
        return;
    }
    regexp.setPattern(PASSWD_REG);
    if(!regexp.exactMatch(pwd))
    {
        QMessageBox::warning(this, "警告", "密码格式不正确");
        ui->log_pwd->clear();
        ui->log_pwd->setFocus();
        return;
    }
#endif


    // 登录信息写入配置文件cfg.json
    // 登陆信息加密
    m_cm.writeLoginInfo(user, pwd, ui->rember_pwd->isChecked());
    // 设置登陆信息json包, 密码经过md5加密， getStrMd5()
    QByteArray array = setLoginJson(user, m_cm.getStrMd5(pwd));
    // 设置登录的url
    QNetworkRequest request;
    QString url = QString("http://%1:%2/login").arg(address).arg(port);
    request.setUrl(QUrl(url));
    // 请求头信息
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(array.size()));
    // 向服务器发送post请求
    QNetworkReply* reply = m_manager->post(request, array);
    cout << "post url:" << url << "post data: " << array;

    // 接收服务器发回的http响应消息
    connect(reply, &QNetworkReply::finished, [=]()
    {
        // 出错了
        if (reply->error() != QNetworkReply::NoError)
        {
            cout << reply->errorString();
            //释放资源
            reply->deleteLater();
            return;
        }

        // 将server回写的数据读出
        QByteArray json = reply->readAll();
        /*
            登陆 - 服务器回写的json数据包格式：
                成功：{"code":"000"}
                失败：{"code":"001"}
        */
        cout << "server return value: " << json;
        QStringList tmpList = getLoginStatus(json); //common.h
        if( tmpList.at(0) == "000" )
        {
            cout << "登陆成功";

            // 设置登陆信息，显示文件列表界面需要使用这些信息
            LoginInfoInstance *p = LoginInfoInstance::getInstance(); //获取单例
            p->setLoginInfo(user, address, port, tmpList.at(1));
            cout << p->getUser().toUtf8().data() << ", " << p->getIp() << ", " << p->getPort() << tmpList.at(1);

            // 当前窗口隐藏
            this->hide();
            // 主界面窗口显示
            m_mainWin->showNormal();
        }
        else
        {
            QMessageBox::warning(this, "登录失败", "用户名或密码不正确！！！");
        }

        reply->deleteLater(); //释放资源
    });
}

// 用户注册操作
void Login::on_register_btn_clicked()
{
    // 从控件中取出用户输入的数据
    QString userName = ui->reg_usr->text();
    QString nickName = ui->reg_nickname->text();
    QString firstPwd = ui->reg_pwd->text();
    QString surePwd = ui->reg_surepwd->text();
    QString phone = ui->reg_phone->text();
    QString email = ui->reg_mail->text();

#if 0
    // 数据校验
    QRegExp regexp(USER_REG);
    if(!regexp.exactMatch(userName))
    {
        QMessageBox::warning(this, "警告", "用户名格式不正确");
        ui->reg_usr->clear();
        ui->reg_usr->setFocus();
        return;
    }
    if(!regexp.exactMatch(nickName))
    {
        QMessageBox::warning(this, "警告", "昵称格式不正确");
        ui->reg_nickname->clear();
        ui->reg_nickname->setFocus();
        return;
    }
    regexp.setPattern(PASSWD_REG);
    if(!regexp.exactMatch(firstPwd))
    {
        QMessageBox::warning(this, "警告", "密码格式不正确");
        ui->reg_pwd->clear();
        ui->reg_pwd->setFocus();
        return;
    }
    if(surePwd != firstPwd)
    {
        QMessageBox::warning(this, "警告", "两次输入的密码不匹配, 请重新输入");
        ui->reg_surepwd->clear();
        ui->reg_surepwd->setFocus();
        return;
    }
    regexp.setPattern(PHONE_REG);
    if(!regexp.exactMatch(phone))
    {
        QMessageBox::warning(this, "警告", "手机号码格式不正确");
        ui->reg_phone->clear();
        ui->reg_phone->setFocus();
        return;
    }
    regexp.setPattern(EMAIL_REG);
    if(!regexp.exactMatch(email))
    {
        QMessageBox::warning(this, "警告", "邮箱码格式不正确");
        ui->reg_mail->clear();
        ui->reg_mail->setFocus();
        return;
    }
#endif

    // 将注册信息打包为json格式
    QByteArray array = setRegisterJson(userName, nickName, m_cm.getStrMd5(firstPwd), phone, email);
    qDebug() << "register json data" << array;
    // 设置连接服务器要发送的url
    QNetworkRequest request;
    QString url = QString("http://%1:%2/reg").arg(ui->address_server->text()).arg(ui->port_server->text());
    request.setUrl(QUrl(url));
    // 设置请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(array.size()));
    // 发送数据
    QNetworkReply* reply = m_manager->post(request, array);

    // 判断请求是否被成功处理
    connect(reply, &QNetworkReply::readyRead, [=]()
    {
        // 读sever回写的数据
        QByteArray jsonData = reply->readAll();
        /*
        注册 - server端返回的json格式数据：
            成功:         {"code":"002"}
            该用户已存在：  {"code":"003"}
            失败:         {"code":"004"}
        */
        // 判断状态码
        if("002" == m_cm.getCode(jsonData))
        {
            //注册成功
            QMessageBox::information(this, "注册成功", "注册成功，请登录");

            //清空行编辑内容
            ui->reg_usr->clear();
            ui->reg_nickname->clear();
            ui->reg_pwd->clear();
            ui->reg_surepwd->clear();
            ui->reg_phone->clear();
            ui->reg_mail->clear();

            //设置登陆窗口的登陆信息
            ui->log_usr->setText(userName);
            ui->log_pwd->setText(firstPwd);
            ui->rember_pwd->setChecked(true);

            //切换到登录界面
            ui->stackedWidget->setCurrentWidget(ui->login_page);
        }
        else if("003" == m_cm.getCode(jsonData))
        {
            QMessageBox::warning(this, "注册失败", QString("[%1]该用户已经存在!!!").arg(userName));
        }
        else if("004" == m_cm.getCode(jsonData))
        {
            QMessageBox::warning(this, "注册失败", "注册失败！！！");
        }
        // 释放资源
        delete reply;
    });
}
