/********************************************************************************
** Form generated from reading UI file 'cardgame.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CARDGAME_H
#define UI_CARDGAME_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CardGameClass
{
public:
    QAction *actionGlobalChat;
    QAction *actionLocalChat;
    QWidget *centralWidget;
    QPushButton *startGameButton;
    QLabel *waitForPlayerLabel;
    QLineEdit *userNameLineEdit;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menuChat;

    void setupUi(QMainWindow *CardGameClass)
    {
        if (CardGameClass->objectName().isEmpty())
            CardGameClass->setObjectName(QStringLiteral("CardGameClass"));
        CardGameClass->resize(600, 400);
        CardGameClass->setAcceptDrops(true);
        CardGameClass->setDocumentMode(false);
        actionGlobalChat = new QAction(CardGameClass);
        actionGlobalChat->setObjectName(QStringLiteral("actionGlobalChat"));
        actionGlobalChat->setCheckable(true);
        actionGlobalChat->setEnabled(true);
        actionLocalChat = new QAction(CardGameClass);
        actionLocalChat->setObjectName(QStringLiteral("actionLocalChat"));
        actionLocalChat->setCheckable(true);
        centralWidget = new QWidget(CardGameClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        startGameButton = new QPushButton(centralWidget);
        startGameButton->setObjectName(QStringLiteral("startGameButton"));
        startGameButton->setGeometry(QRect(240, 220, 91, 31));
        waitForPlayerLabel = new QLabel(centralWidget);
        waitForPlayerLabel->setObjectName(QStringLiteral("waitForPlayerLabel"));
        waitForPlayerLabel->setGeometry(QRect(220, 120, 131, 51));
        userNameLineEdit = new QLineEdit(centralWidget);
        userNameLineEdit->setObjectName(QStringLiteral("userNameLineEdit"));
        userNameLineEdit->setGeometry(QRect(220, 180, 131, 31));
        userNameLineEdit->setInputMethodHints(Qt::ImhNone);
        userNameLineEdit->setMaxLength(15);
        userNameLineEdit->setClearButtonEnabled(false);
        CardGameClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(CardGameClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        CardGameClass->setStatusBar(statusBar);
        menuBar = new QMenuBar(CardGameClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 21));
        menuChat = new QMenu(menuBar);
        menuChat->setObjectName(QStringLiteral("menuChat"));
        menuChat->setEnabled(false);
        menuChat->setAcceptDrops(true);
        CardGameClass->setMenuBar(menuBar);

        menuBar->addAction(menuChat->menuAction());
        menuChat->addAction(actionGlobalChat);
        menuChat->addAction(actionLocalChat);

        retranslateUi(CardGameClass);

        QMetaObject::connectSlotsByName(CardGameClass);
    } // setupUi

    void retranslateUi(QMainWindow *CardGameClass)
    {
        CardGameClass->setWindowTitle(QApplication::translate("CardGameClass", "CardGame", 0));
        actionGlobalChat->setText(QApplication::translate("CardGameClass", "GlobalChat", 0));
        actionLocalChat->setText(QApplication::translate("CardGameClass", "LocalChat", 0));
        startGameButton->setText(QApplication::translate("CardGameClass", "Start Game", 0));
        waitForPlayerLabel->setText(QApplication::translate("CardGameClass", "Waiting for another player", 0));
        userNameLineEdit->setInputMask(QString());
        userNameLineEdit->setText(QString());
        userNameLineEdit->setPlaceholderText(QApplication::translate("CardGameClass", "Set Your Name", 0));
        menuChat->setTitle(QApplication::translate("CardGameClass", "Chat", 0));
    } // retranslateUi

};

namespace Ui {
    class CardGameClass: public Ui_CardGameClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CARDGAME_H
