//
// Created by newcoderlife on 17-9-17.
//

#include "tetrixwindow.h"
#include "tetrixboard.h"
#include "usermanager.h"
#include "userdialog.h"

#include <QtCore/QDebug>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QTableWidget>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QtCore/QCoreApplication>

TetrixWindow::TetrixWindow() {
    auto *palette = new QPalette;
    palette->setBrush(QPalette::Background,
                      QBrush(QPixmap(":/images/background/1").scaled(size(), Qt::IgnoreAspectRatio,
                                                                     Qt::SmoothTransformation)));
    setAutoFillBackground(true);
    setPalette(*palette);

    auto *mediaPlayer = new QMediaPlayer(this);
    auto *playList = new QMediaPlaylist;
    for (int i = 1; i < 11; i++)
        playList->addMedia(QUrl(tr("qrc:/musics/background/%1").arg(i)));
    playList->setPlaybackMode(QMediaPlaylist::Loop);
    mediaPlayer->setPlaylist(playList);
    mediaPlayer->setVolume(50);

    board = new TetrixBoard(this);
    userManager = new UserManager(this);
    userDialog = new UserDialog(this);

    nextPieceLabel = new QLabel(this);
    nextPieceLabel->setFrameStyle(QFrame::Box | QFrame::Raised);
    nextPieceLabel->setAlignment(Qt::AlignCenter);
    board->setNextPieceLabel(nextPieceLabel);

    scoreLcd = new QLCDNumber(5, this);
    scoreLcd->setSegmentStyle(QLCDNumber::Filled);
    levelLcd = new QLCDNumber(2, this);
    levelLcd->setSegmentStyle(QLCDNumber::Filled);
    linesLcd = new QLCDNumber(5, this);
    linesLcd->setSegmentStyle(QLCDNumber::Filled);

    startButton = new QPushButton(tr("开始新的"), this);
    startButton->setFocusPolicy(Qt::NoFocus);
    startButton->setDisabled(true);
    quitButton = new QPushButton(tr("退出"), this);
    quitButton->setFocusPolicy(Qt::NoFocus);
    pauseButton = new QPushButton(tr("暂停"), this);
    pauseButton->setFocusPolicy(Qt::NoFocus);
    pauseButton->setDisabled(true);
    goOnButton = new QPushButton(tr("继续存档"), this);
    goOnButton->setFocusPolicy(Qt::NoFocus);
    goOnButton->setDisabled(true);

    menuBar = new QMenuBar(this);
    signInAction = new QAction(tr("登录"), menuBar);
    signUpAction = new QAction(tr("注册"), menuBar);
    menuBar->addAction(signInAction);
    menuBar->addAction(signUpAction);

    tableWidget = new QTableWidget(10, 2, this);
    QStringList headers;
    headers << tr("用户名") << tr("分数");
    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->setFixedWidth(static_cast<int>(width() / 2.5));

    connect(startButton, &QPushButton::clicked, board, &TetrixBoard::start);
    connect(goOnButton, &QPushButton::clicked, board, &TetrixBoard::goOn);
    connect(pauseButton, &QPushButton::clicked, board, &TetrixBoard::pause);
    connect(quitButton, &QPushButton::clicked, qApp, &QCoreApplication::quit);
    connect(startButton, &QPushButton::clicked, mediaPlayer, &QMediaPlayer::play);
    connect(goOnButton, &QPushButton::clicked, mediaPlayer, &QMediaPlayer::play);
    connect(pauseButton, &QPushButton::clicked, mediaPlayer, &QMediaPlayer::pause);
    connect(goOnButton, &QPushButton::clicked, [this](bool) { goOnButton->setDisabled(true); });

    connect(board, &TetrixBoard::scoreChanged, [this](int displayNum) { scoreLcd->display(displayNum); });
    connect(board, &TetrixBoard::scoreChanged, userManager, &UserManager::setCurrentScore);
    connect(board, &TetrixBoard::scoreChanged, userManager, &UserManager::setMaxScore);
    connect(board, &TetrixBoard::scoreChanged, userManager, &UserManager::handleRankTable);
    connect(board, &TetrixBoard::levelChanged, [this](int displayNum) { levelLcd->display(displayNum); });
    connect(board, &TetrixBoard::levelChanged, userManager, &UserManager::setCurrentLevel);
    connect(board, &TetrixBoard::levelChanged, playList, &QMediaPlaylist::next);
    connect(board, &TetrixBoard::linesRemovedChanged, [this](int displayNum) { linesLcd->display(displayNum); });
    connect(board, &TetrixBoard::levelChanged, [this, palette](int level) {
        palette->setBrush(QPalette::Background,
                          QBrush(QPixmap(tr(":/images/background/%1").arg(level)).scaled(size(), Qt::IgnoreAspectRatio,
                                                                                         Qt::SmoothTransformation)));
        setPalette(*palette);
    });

    connect(userManager, &UserManager::scoreChanged, board, &TetrixBoard::setScore);
    connect(userManager, &UserManager::levelChanged, board, &TetrixBoard::setLevel);
    connect(userManager, &UserManager::logInSuccess, startButton, &QPushButton::setDisabled);
    connect(userManager, &UserManager::logInSuccess, pauseButton, &QPushButton::setDisabled);
    connect(userManager, &UserManager::logInSuccess, goOnButton, &QPushButton::setDisabled);
    connect(userManager, &UserManager::rankListAddUser, tableWidget, &QTableWidget::setItem);

    connect(signInAction, &QAction::triggered, userDialog, &UserDialog::signIn);
    connect(signUpAction, &QAction::triggered, userDialog, &UserDialog::signUp);
    connect(userDialog, &UserDialog::sendSignInInfo, userManager, &UserManager::auth);
    connect(userDialog, &UserDialog::sendSignUpInfo, userManager, &UserManager::newUser);

    auto layout = new QGridLayout(this);
    layout->addWidget(createLabel(tr("下一块")), 0, 0, 1, 1);
    layout->addWidget(nextPieceLabel, 1, 0, 2, 1);
    layout->addWidget(createLabel(tr("等级")), 3, 0, 1, 1);
    layout->addWidget(levelLcd, 4, 0, 1, 1);
    layout->addWidget(createLabel(tr("分数")), 5, 0, 1, 1);
    layout->addWidget(scoreLcd, 6, 0, 1, 1);
    layout->addWidget(startButton, 7, 0, 1, 1);
    layout->addWidget(goOnButton, 8, 0, 1, 1);

    layout->addWidget(board, 0, 1, 9, 2);

    layout->addWidget(createLabel(tr("已移除的行数")), 0, 3, 1, 1);
    layout->addWidget(linesLcd, 1, 3, 1, 1);
    layout->addWidget(quitButton, 2, 3, 1, 1);
    layout->addWidget(pauseButton, 3, 3, 1, 1);
    layout->addWidget(createLabel(tr("排行")), 4, 3, 1, 1);
    layout->addWidget(tableWidget, 5, 3, 4, 1);
    layout->setMenuBar(menuBar);
    setLayout(layout);

    setWindowTitle(tr("俄罗斯方块"));
    resize(1024, 768);
}

QLabel *TetrixWindow::createLabel(const QString &text) {
    auto *lbl = new QLabel(text);
    lbl->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    return lbl;
}
