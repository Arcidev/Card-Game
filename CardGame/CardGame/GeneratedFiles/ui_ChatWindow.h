/********************************************************************************
** Form generated from reading UI file 'ChatWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATWINDOW_H
#define UI_CHATWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatWindow
{
public:
    QWidget *globalChat;
    QLineEdit *globalChatLineEdit;
    QPushButton *globalChatPushButton;
    QTextEdit *globalChatTextEdit;
    QWidget *localChat;
    QLineEdit *localChatLineEdit;
    QPushButton *localChatPushButton;
    QTextEdit *localChatTextEdit;

    void setupUi(QTabWidget *ChatWindow)
    {
        if (ChatWindow->objectName().isEmpty())
            ChatWindow->setObjectName(QStringLiteral("ChatWindow"));
        ChatWindow->resize(600, 403);
        ChatWindow->setAutoFillBackground(false);
        ChatWindow->setTabShape(QTabWidget::Triangular);
        ChatWindow->setDocumentMode(true);
        ChatWindow->setTabsClosable(true);
        ChatWindow->setMovable(false);
        globalChat = new QWidget();
        globalChat->setObjectName(QStringLiteral("globalChat"));
        globalChatLineEdit = new QLineEdit(globalChat);
        globalChatLineEdit->setObjectName(QStringLiteral("globalChatLineEdit"));
        globalChatLineEdit->setGeometry(QRect(10, 330, 491, 31));
        globalChatLineEdit->setMaxLength(255);
        globalChatPushButton = new QPushButton(globalChat);
        globalChatPushButton->setObjectName(QStringLiteral("globalChatPushButton"));
        globalChatPushButton->setGeometry(QRect(510, 330, 75, 31));
        globalChatTextEdit = new QTextEdit(globalChat);
        globalChatTextEdit->setObjectName(QStringLiteral("globalChatTextEdit"));
        globalChatTextEdit->setGeometry(QRect(10, 10, 571, 311));
        globalChatTextEdit->setReadOnly(true);
        ChatWindow->addTab(globalChat, QString());
        localChat = new QWidget();
        localChat->setObjectName(QStringLiteral("localChat"));
        localChatLineEdit = new QLineEdit(localChat);
        localChatLineEdit->setObjectName(QStringLiteral("localChatLineEdit"));
        localChatLineEdit->setGeometry(QRect(10, 330, 491, 31));
        localChatLineEdit->setMaxLength(255);
        localChatPushButton = new QPushButton(localChat);
        localChatPushButton->setObjectName(QStringLiteral("localChatPushButton"));
        localChatPushButton->setGeometry(QRect(510, 330, 75, 31));
        localChatTextEdit = new QTextEdit(localChat);
        localChatTextEdit->setObjectName(QStringLiteral("localChatTextEdit"));
        localChatTextEdit->setGeometry(QRect(10, 10, 571, 311));
        localChatTextEdit->setReadOnly(true);
        ChatWindow->addTab(localChat, QString());

        retranslateUi(ChatWindow);

        ChatWindow->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ChatWindow);
    } // setupUi

    void retranslateUi(QTabWidget *ChatWindow)
    {
        ChatWindow->setWindowTitle(QApplication::translate("ChatWindow", "TabWidget", 0));
        globalChatPushButton->setText(QApplication::translate("ChatWindow", "Send", 0));
        ChatWindow->setTabText(ChatWindow->indexOf(globalChat), QApplication::translate("ChatWindow", "Global Chat", 0));
        localChatPushButton->setText(QApplication::translate("ChatWindow", "Send", 0));
        ChatWindow->setTabText(ChatWindow->indexOf(localChat), QApplication::translate("ChatWindow", "Local Chat", 0));
    } // retranslateUi

};

namespace Ui {
    class ChatWindow: public Ui_ChatWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATWINDOW_H
