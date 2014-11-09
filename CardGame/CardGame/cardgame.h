#pragma once
#include <QtWidgets\QMainWindow>
#include <QPushButton>
#include "ChatWindow.h"
#include "ui_cardgame.h"

class ClientGame;
class Packet;

class CardGame : public QMainWindow
{
    Q_OBJECT
    private:
        QTimer* m_timer;
        ClientGame* m_game;
        ChatWindow m_chatWindow;
        Ui::CardGameClass ui;
        
        void setupUI() const;
        void closeEvent(QCloseEvent* /*event*/) final;

    private slots:
        void onClickStartGame();
        void onClickChat();
        void onTimedOut();

    public:
        CardGame(QWidget *parent = 0);
        ~CardGame();

        void ShowWaitForPlayer(bool show);
        void SetCheckState(bool globalChat, bool localChat);
        void SendPacket(Packet const* packet) const;
        void HandleChatPacket(Packet* packet) const { m_chatWindow.HandleChatPacket(packet); }
};
