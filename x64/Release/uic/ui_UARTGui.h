/********************************************************************************
** Form generated from reading UI file 'UARTGui.ui'
**
** Created by: Qt User Interface Compiler version 6.1.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UARTGUI_H
#define UI_UARTGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UARTGuiClass
{
public:
    QWidget *centralWidget;
    QTextBrowser *textBrowser;
    QComboBox *comboBox;
    QPushButton *pushButton;
    QComboBox *comboBox_2;
    QLabel *label;
    QLabel *label_2;

    void setupUi(QMainWindow *UARTGuiClass)
    {
        if (UARTGuiClass->objectName().isEmpty())
            UARTGuiClass->setObjectName(QString::fromUtf8("UARTGuiClass"));
        UARTGuiClass->resize(600, 400);
        centralWidget = new QWidget(UARTGuiClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        textBrowser = new QTextBrowser(centralWidget);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(220, 10, 371, 381));
        comboBox = new QComboBox(centralWidget);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(70, 100, 141, 22));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(10, 310, 201, 71));
        comboBox_2 = new QComboBox(centralWidget);
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));
        comboBox_2->setGeometry(QRect(70, 40, 141, 22));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(40, 40, 41, 21));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 100, 51, 21));
        UARTGuiClass->setCentralWidget(centralWidget);

        retranslateUi(UARTGuiClass);

        QMetaObject::connectSlotsByName(UARTGuiClass);
    } // setupUi

    void retranslateUi(QMainWindow *UARTGuiClass)
    {
        UARTGuiClass->setWindowTitle(QCoreApplication::translate("UARTGuiClass", "UARTGui", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("UARTGuiClass", "1. BIT Komutu", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("UARTGuiClass", "2. I Komutu", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("UARTGuiClass", "3. M Komutu", nullptr));
        comboBox->setItemText(3, QCoreApplication::translate("UARTGuiClass", "4. L Komutu", nullptr));
        comboBox->setItemText(4, QCoreApplication::translate("UARTGuiClass", "5. GC Komutu", nullptr));
        comboBox->setItemText(5, QCoreApplication::translate("UARTGuiClass", "6. F Komutu", nullptr));
        comboBox->setItemText(6, QCoreApplication::translate("UARTGuiClass", "7. A Komutu", nullptr));
        comboBox->setItemText(7, QCoreApplication::translate("UARTGuiClass", "8. O Komutu", nullptr));
        comboBox->setItemText(8, QCoreApplication::translate("UARTGuiClass", "9. OperasyonSuresi", nullptr));
        comboBox->setItemText(9, QCoreApplication::translate("UARTGuiClass", "10. Guncelle", nullptr));
        comboBox->setItemText(10, QCoreApplication::translate("UARTGuiClass", "11. Versiyon", nullptr));
        comboBox->setItemText(11, QCoreApplication::translate("UARTGuiClass", "12. VoltajSorgu", nullptr));
        comboBox->setItemText(12, QCoreApplication::translate("UARTGuiClass", "13. Reset", nullptr));
        comboBox->setItemText(13, QCoreApplication::translate("UARTGuiClass", "14. P Komutu", nullptr));
        comboBox->setItemText(14, QCoreApplication::translate("UARTGuiClass", "15. S Komutu", nullptr));
        comboBox->setItemText(15, QCoreApplication::translate("UARTGuiClass", "16. FrenKomutu", nullptr));
        comboBox->setItemText(16, QCoreApplication::translate("UARTGuiClass", "17. VTReset", nullptr));
        comboBox->setItemText(17, QCoreApplication::translate("UARTGuiClass", "18. TVTYazma", nullptr));
        comboBox->setItemText(18, QCoreApplication::translate("UARTGuiClass", "19. LG Komutu", nullptr));
        comboBox->setItemText(19, QCoreApplication::translate("UARTGuiClass", "20. Piroteknik BIT", nullptr));

        pushButton->setText(QCoreApplication::translate("UARTGuiClass", "Send", nullptr));
        comboBox_2->setItemText(0, QCoreApplication::translate("UARTGuiClass", "COM1", nullptr));
        comboBox_2->setItemText(1, QCoreApplication::translate("UARTGuiClass", "COM2", nullptr));

        label->setText(QCoreApplication::translate("UARTGuiClass", "Port:", nullptr));
        label_2->setText(QCoreApplication::translate("UARTGuiClass", " Message:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UARTGuiClass: public Ui_UARTGuiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UARTGUI_H
