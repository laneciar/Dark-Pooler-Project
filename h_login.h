#ifndef login_H
#define login_H

#include <QApplication>
#include <QDialog>
#include <QMouseEvent>
#include <QPoint>
#include <QPixmap>
#include <QStackedWidget>
#include <QMessageBox>
#include "ui_u_login.h"


namespace Ui {
class Login;
}
class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

private slots:
    void on_loginButton_clicked();

    void on_loginCheckBox_toggled(bool checked);

    void on_exitButton_clicked();

    void on_minimizeButton_clicked();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::Login *login;
    QSizePolicy QSizePolicy;
    bool isChecked = false;
    int mouseClick_X;
    int mouseClick_Y;

};

#endif // LOGIN_H
