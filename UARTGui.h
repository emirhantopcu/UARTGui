#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_UARTGui.h"

class UARTGui : public QMainWindow
{
    Q_OBJECT

public:
    UARTGui(QWidget *parent = Q_NULLPTR);
    

private:
    Ui::UARTGuiClass ui;
    LPCWSTR port_input;

private slots:
    void on_pushButton_clicked();
    void UARTGui::on_pushButton_2_clicked()
};
