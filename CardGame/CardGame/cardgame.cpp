#include <QHBoxLayout>
#include <qtimer.h>
#include "cardgame.h"
#include "clientGame.h"

CardGame::CardGame(QWidget *parent) : QMainWindow(parent), m_timer(new QTimer(this)), m_game(nullptr), m_chatWindow(ChatWindow(this))
{
    ui.setupUi(this);
    setupUI();
}

CardGame::~CardGame()
{
    m_timer->stop();
    delete m_timer;

    if (m_game)
        delete m_game;
}

void CardGame::setupUI() const
{
    ui.waitForPlayerLabel->hide();
    ui.userNameLineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z]+")));
    connect(ui.startGameButton, SIGNAL(clicked()), this, SLOT(onClickStartGame()));
    connect(ui.actionGlobalChat, SIGNAL(triggered()), this, SLOT(onClickChat()));
    connect(ui.actionLocalChat, SIGNAL(triggered()), this, SLOT(onClickChat()));
}

void CardGame::closeEvent(QCloseEvent* /*event*/)
{
    m_chatWindow.close();
}

void CardGame::onClickStartGame()
{
    QString const& name = ui.userNameLineEdit->text();
    if (name.isEmpty() || m_game)
        return;

    m_game = new ClientGame(this, name);
    ui.menuChat->setEnabled(true);
    ui.startGameButton->hide();
    ui.userNameLineEdit->hide();
    
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimedOut()));
    m_timer->start(1000);
}

void CardGame::onClickChat()
{
    bool globalChat = ui.actionGlobalChat->isChecked();
    bool localChat = ui.actionLocalChat->isChecked();
    bool show = globalChat || localChat;

    if (show != m_chatWindow.isVisible())
        show ? m_chatWindow.show() : m_chatWindow.hide();

    m_chatWindow.ShowTabs(globalChat, localChat);
}

void CardGame::onTimedOut()
{
    m_game->Update();
    m_timer->start(100);
}

void CardGame::ShowWaitForPlayer(bool show)
{
    if (ui.waitForPlayerLabel->isHidden() != show)
        return;

    if (show)
        ui.waitForPlayerLabel->show();
    else
        ui.waitForPlayerLabel->hide();
}

void CardGame::SetCheckState(bool globalChat, bool localChat)
{
    ui.actionGlobalChat->setChecked(globalChat);
    ui.actionLocalChat->setChecked(localChat);
}

void CardGame::SendPacket(Packet const* packet) const
{
    if (m_game)
        m_game->SendPacket(*packet);
}
