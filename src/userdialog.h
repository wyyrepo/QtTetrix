//
// Created by newcoderlife on 17-9-19.
//

#ifndef QTTETRIX_USERDIALOG_H
#define QTTETRIX_USERDIALOG_H

#include <QtWidgets/QDialog>

class QLineEdit;

class UserDialog : public QDialog {
Q_OBJECT

public:
    explicit UserDialog(QWidget *parent = nullptr);

    static void save(QWidget *parent = nullptr);

    static void load(QWidget *parent = nullptr);

private:
    QLineEdit *usernameLineEdit, *passwordLineEdit;
    QPushButton *pushButton;
};


#endif //QTTETRIX_USERDIALOG_H
