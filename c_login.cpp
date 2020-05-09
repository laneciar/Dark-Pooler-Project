#include "h_login.h"
#include "h_mainwindow.h"


Login::Login(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    login(new Ui::Login)
{
    login->setupUi(this);

    //Hides texts for incorrect login and EULA message
    login->incorrectLoginMessage->hide();
    login->acceptEULAMessage->hide();

    //Initializes the logo for the login window
    QPixmap pixLogo(":/content/Images/logo.png");
    int w = login->logoPic->width();
    int h = login->logoPic->height();
    login->logoPic->setPixmap(pixLogo.scaled(w,h,Qt::KeepAspectRatio));
}

Login::~Login()
{
    delete login;
}



//Checks if EULA box is checked on.
void Login::on_loginCheckBox_toggled(bool checked)
{
    if(checked == true)
    {
       isChecked = true;
    }
    else
        isChecked = false;
}

//For mouse movement position on click
void Login::mousePressEvent(QMouseEvent *event)
{
    mouseClick_X = event->x();
    mouseClick_Y = event->y();
}

//Moves window when user drags
void Login::mouseMoveEvent(QMouseEvent *event)
{
    move(event->globalX()-mouseClick_X, event->globalY()-mouseClick_Y);
}

//User preses login button
void Login::on_loginButton_clicked()
{
    //Reads what the user inputed for username and password fields.
    QString username = login->usernameEdit->text();
    QString password = login->passwordEdit->text();

    //EULA box is checked and username and password is correct, still need to find way to store real accounts to server.
    if(username == "username" && password == "password" && isChecked == true)
    {
        //Closes login screen
        this->close();

        //Opens main menu ui
        MainWindow *mainWindow = new MainWindow();
        mainWindow->show();
    }

    //EULA box not checked but username and password is correct.
    else if (username == "username" && password == "password" && isChecked == false)
        login->acceptEULAMessage->show();

    //Username or password is incorrect.
    else
        login->incorrectLoginMessage->show();
}

//When user clicks X button window closes
void Login::on_exitButton_clicked()
{
    this->close();
}

//When user clicks - button window minimizes
void Login::on_minimizeButton_clicked()
{
    this->showMinimized();
}


