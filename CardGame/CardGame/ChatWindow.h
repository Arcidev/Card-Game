#pragma once
#include <QTabWidget.h>
#include "GeneratedFiles\ui_ChatWindow.h"
#include "..\Shared\SharedDefines.h"

class CardGame;
class Packet;

class ChatWindow : public QTabWidget
{
    Q_OBJECT
    private:
        CardGame* m_cardGame;
        Ui::ChatWindow ui;

        void setupUi() const;
        void closeEvent(QCloseEvent* /*event*/) final;
        void onClickChatButton(ChatId chatId, QLineEdit* lineEdit) const;

    private slots:
        void onClickGlobalChatButton() { onClickChatButton(CHAT_GLOBAL, ui.globalChatLineEdit); }
        void onClickLocalChatButton() { onClickChatButton(CHAT_LOCAL, ui.localChatLineEdit); }
        void onClickTab(int index);
        void onCloseTab(int index);

    public:
        ChatWindow(CardGame* cardGame);

        void ShowTabs(bool globalChat, bool localChat);
        void HandleChatPacket(Packet* packet) const;
};

