#include "ChatWindow.h"
#include "cardgame.h"
#include "..\Shared\Packet.h"

ChatWindow::ChatWindow(CardGame* cardGame) : m_cardGame(cardGame)
{
    ui.setupUi(this);
    setupUi();
}

void ChatWindow::setupUi() const
{
    connect(ui.globalChatLineEdit, SIGNAL(returnPressed()), this, SLOT(onClickGlobalChatButton()));
    connect(ui.localChatLineEdit, SIGNAL(returnPressed()), this, SLOT(onClickLocalChatButton()));
    connect(ui.globalChatPushButton, SIGNAL(clicked()), this, SLOT(onClickGlobalChatButton()));
    connect(ui.localChatPushButton, SIGNAL(clicked()), this, SLOT(onClickLocalChatButton()));
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(onClickTab(int)));
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(onCloseTab(int)));
}

void ChatWindow::onClickChatButton(ChatId chatId, QLineEdit* lineEdit) const
{
    QString const& str = lineEdit->text();
    if (str.isEmpty())
        return;

    Packet packet(CMSG_CHAT_MESSAGE);
    packet << (uint8_t)chatId;
    packet << str.toStdString();
    m_cardGame->SendPacket(&packet);

    lineEdit->clear();
}

void ChatWindow::onClickTab(int index)
{
    tabBar()->setTabTextColor(index, Qt::black);
}

void ChatWindow::onCloseTab(int index)
{
    removeTab(index);
    m_cardGame->SetCheckState(indexOf(ui.globalChat) != -1, indexOf(ui.localChat) != -1);
    if (count() == 0)
        close();
}

void ChatWindow::closeEvent(QCloseEvent* /*event*/)
{
    m_cardGame->SetCheckState(false, false);
}

void ChatWindow::ShowTabs(bool globalChat, bool localChat)
{
    globalChat ? addTab(ui.globalChat, "Global Chat") : removeTab(indexOf(ui.globalChat));
    localChat ? addTab(ui.localChat, "Local Chat") : removeTab(indexOf(ui.localChat));
}

void ChatWindow::HandleChatPacket(Packet* packet) const
{
    uint8_t chatId;
    std::string name, message;
    *packet >> chatId;
    *packet >> name;
    *packet >> message;

    QTextEdit* textEdit = nullptr;
    int index = -1;

    switch (chatId)
    {
        case CHAT_GLOBAL:
            textEdit = ui.globalChatTextEdit;
            index = indexOf(ui.globalChat);
            break;
        case CHAT_LOCAL:
            textEdit = ui.localChatTextEdit;
            index = indexOf(ui.localChat);
            break;
        default:
            return;
    }

    std::string output = name + "> " + message;
    textEdit->append(QString::fromStdString(output));
    if ((index != -1) && (index != currentIndex()))
        tabBar()->setTabTextColor(index, Qt::red);
}
