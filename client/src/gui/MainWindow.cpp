#include "../../include/gui/MainWindow.hpp"
#include "../../include/ClientNetworkHandler.hpp"
#include <../../common/include/TetrominoFactory.hpp>
#include <../../common/include/GameState.hpp>
#include <QScreen>
#include <QThread>

namespace Tetris::gui
{
    MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
    {
        initWindow();
        initWidgets();
        connectWidgets();

        networkHandler = std::make_unique<ClientNetworkHandler>("127.0.0.1", 8080);
        networkHandler->start(); // Start the network thread
    }

    MainWindow::~MainWindow()
    {
        if (networkHandler)
        {
            networkHandler->stop();
        }
    }

    void MainWindow::initWindow()
    {
        setFixedSize(m_windowWidth, m_windowHeight);
        setWindowTitle(QString("Tetris"));
        move(screen()->geometry().center() - frameGeometry().center());
    }

    void MainWindow::initWidgets()
    {
        /* Initialize private members related to game. */
        m_pieceRandomizer = Tetris::core::TetrominoFactory::UniformPieceRandomizer;
        m_timer = std::make_unique<QTimer>(this);

        /* Initialize game labels. */
        QFont labelFont("Courier", 12, QFont::Bold);
        m_labelNext = new QLabel("Next", this);
        m_labelNext->setFont(labelFont);
        m_labelLines = new QLabel("Lines\n0", this);
        m_labelLines->setFont(labelFont);
        m_labelLevel = new QLabel("Level\n0", this);
        m_labelLevel->setFont(labelFont);
        m_labelScore = new QLabel("Score\n0", this);
        m_labelScore->setText(QString("Score\n0"));
        m_labelScore->setFont((labelFont));

        /* Initialize option labels. */
        m_labelRandomizer = new QLabel("Randomizer", this);

        /* Initialize option combo boxes. */
        m_comboRandomizer = new QComboBox(this);
        m_comboRandomizer->addItem("uniform randomizer");
        m_comboRandomizer->addItem("7-bag randomizer");

        /* Initialize buttons */
        m_buttonStart = new QPushButton("start", this);
        m_buttonPause = new QPushButton("pause", this);
        m_buttonAbout = new QPushButton("about", this);

        /* Initialize rendering widgets. */
        m_renderGame = new Tetris::gui::RendererGame();
        m_renderPreview = new Tetris::gui::RendererPreview();

        /* Align game labels on the center. */
        m_labelNext->setAlignment(Qt::AlignCenter | Qt::AlignBottom);
        m_labelLines->setAlignment(Qt::AlignCenter);
        m_labelLevel->setAlignment(Qt::AlignCenter);
        m_labelScore->setAlignment(Qt::AlignCenter);

        /* Align option labels on the left. */
        m_labelRandomizer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        /* Set option combo boxes width. */
        m_comboRandomizer->setFixedWidth(m_comboBoxWidth);

        /* Set option combo boxes focus policies. */
        m_comboRandomizer->setFocusPolicy(Qt::FocusPolicy::NoFocus);

        /* Horizontal layout for randomizer widgets. */
        m_layoutRandomizer = new QHBoxLayout();
        m_layoutRandomizer->addWidget(m_labelRandomizer);
        m_layoutRandomizer->addWidget(m_comboRandomizer);

        /* Horizontal layout for buttons. */
        m_layoutButtons = new QHBoxLayout();
        m_layoutButtons->addWidget(m_buttonStart);
        m_layoutButtons->addWidget(m_buttonPause);
        m_layoutButtons->addWidget(m_buttonAbout);

        /* Vertical layout gathering language, randomizer, game labels, and buttons layouts. */
        m_layoutInformations = new QVBoxLayout();
        m_layoutInformations->addLayout(m_layoutRandomizer);
        m_layoutInformations->addWidget(m_labelNext);
        m_layoutInformations->addWidget(m_renderPreview);
        m_layoutInformations->addWidget(m_labelLines);
        m_layoutInformations->addWidget(m_labelLevel);
        m_layoutInformations->addWidget(m_labelScore);
        m_layoutInformations->addLayout(m_layoutButtons);

        /* Set size of game rendering widget 2/3 of window's width. */
        QSizePolicy spLeft(QSizePolicy::Preferred, QSizePolicy::Preferred);
        spLeft.setHorizontalStretch(2);
        m_renderGame->setSizePolicy(spLeft);

        /* Vertical layout for main layout. */
        m_layoutMain = new QHBoxLayout();
        m_layoutMain->addWidget(m_renderGame);
        m_layoutMain->addLayout(m_layoutInformations);

        // add Vertical layout to the main widget.
        QWidget *mainWidget = new QWidget(); // MainWindow will delete it
        mainWidget->setLayout(m_layoutMain);
        setFocusPolicy(Qt::TabFocus);
        this->setCentralWidget(mainWidget);
    }

    void MainWindow::connectWidgets()
    {
        QObject::connect(m_comboRandomizer, SIGNAL(currentTextChanged(QString)), this, SLOT(changePieceRandomizer()));
        // QObject::connect(m_buttonPause, SIGNAL(clicked()), this, SLOT(pauseGame()));
        // QObject::connect(m_buttonAbout, SIGNAL(clicked()), m_messageBox, SLOT(exec()));
        QObject::connect(m_buttonStart, SIGNAL(clicked()), this, SLOT(sendStartCommandToServer()));
        QObject::connect(m_buttonPause, SIGNAL(clicked()), this, SLOT(sendPauseCommandToServer()));
        // QObject::connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(receiveGameStateFromServer()));
        QObject::connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(updateGameArea()));
    }

    // void Tetris::gui::MainWindow::initGameArea(){
    //    if(m_buttonStart->text() == "resume"){
    //        m_timer->start();
    //        m_buttonStart->setText("restart");
    //    }else{
    //        m_board.clear();
    //        m_board.setCurrentPiece(m_pieceRandomizer());
    //        m_board.setNextPiece(m_pieceRandomizer());
    //        m_renderGame->setBoard(&m_board);
    //        m_renderGame->setGameOver(false);

    //        m_renderPreview->setTetromino(m_board.getNextPiece());
    //        m_renderPreview->update();
    //        m_buttonStart->setText(QString("restart"));

    //        m_lines = 0;
    //        m_level = 0;
    //        m_score = 0;

    //        m_labelLines->setText(QString("Lines\n0"));
    //        m_labelLevel->setText(QString("Level\n0"));
    //        m_labelScore->setText(QString("Score\n0"));

    //        m_timer->stop();
    //        m_timer->start(m_timeUpdate);
    //    }
    // }

    // void MainWindow::updateGameArea(const GameState &gameState)
    // {
    //     // Update game rendering based on the received game state
    //     m_renderGame->setBoard(gameState.board);
    //     m_renderPreview->setTetromino(gameState.nextPiece);

    //     // Update UI elements
    //     m_labelLines->setText(QString("Lines\n") + QString::number(gameState.linesCleared));
    //     m_labelScore->setText(QString("Score\n") + QString::number(gameState.score));
    //     m_labelLevel->setText(QString("Level\n") + QString::number(gameState.level));

    //     if (gameState.completedLinesRange.first && gameState.completedLinesRange.second)
    //     {
    //         blinkLines(gameState.completedLinesRange.first, gameState.completedLinesRange.second);
    //     }

    //     if (gameState.isGameOver)
    //     {
    //         m_renderGame->setGameOver(true);
    //     }

    //     m_renderGame->update();
    //     m_renderPreview->update();
    // }

    void Tetris::gui::MainWindow::updateGameArea()
    {
        if (!m_board.canMoveCurrentPieceDown())
        {
            m_board.dropCurrentPiece();
            m_board.swapPieces(m_pieceRandomizer());
            m_renderPreview->setTetromino(m_board.getNextPiece());

            if (auto completedRange = m_board.hasCompletedLines(); completedRange.first && completedRange.second)
            {
                m_lines += completedRange.second - completedRange.first;
                addScore(completedRange.second - completedRange.first);
                m_labelLines->setText(QString("Lines\n") + QString::number(m_lines));
                m_labelScore->setText(QString("Score\n") + QString::number(m_score));
                if (m_lines / 10 > (m_lines - (completedRange.second - completedRange.first)) / 10)
                {
                    m_level++;
                    m_labelLevel->setText(QString("Level\n") + QString::number(m_level));
                    m_timer->stop();
                    m_timer->start(m_timeUpdate * std::pow(1 - m_timeDecreaseRate, m_level));
                }

                m_timer->stop();
                blinkLines(completedRange.first, completedRange.second);
                m_timer->start();
                m_board.eraseLines(completedRange);
            }
            else if (m_board.isGameOver())
            {
                m_renderGame->setGameOver(true);
                m_renderGame->update();
                m_timer->stop();
            }
            m_renderPreview->update();
        }
        else
        {
            m_board.getCurrentPiece()->setY(m_board.getCurrentPiece()->getY() + 1);
        }
        m_renderGame->update();
    }

    // void Tetris::gui::MainWindow::receiveGameStateFromServer()
    // {
    //     std::string gameState = networkHandler->receiveGameState();
    //     if (!gameState.empty())
    //     {
    //         processGameState(gameState);
    //     }
    // }

    // void MainWindow::processGameState(const std::string &gameState)
    // {
    //     GameState state = deserializeGameState(gameState);

    //     // Update the UI based on the new game state
    //     m_labelLines->setText(QString("Lines\n") + QString::number(state.linesCleared));
    //     m_labelScore->setText(QString("Score\n") + QString::number(state.score));
    //     m_labelLevel->setText(QString("Level\n") + QString::number(state.level));

    //     // Update the game board rendering
    //     m_renderGame->setBoard(state.board);
    //     m_renderGame->update();

    //     if (receivedGameOver)
    //     {
    //         handleGameOver();
    //     }

    //     if (completedLinesExist)
    //     {
    //         blinkLines;
    //     }
    // }

    void MainWindow::startGameStateUpdateLoop()
    {
        m_timer->start(100);
    }

    void Tetris::gui::MainWindow::blinkLines(const int lineStart, const int lineStop)
    {
        QPainterPath blinkArea;
        blinkArea.lineTo(m_renderGame->getMarginLeft(), m_renderGame->getMarginTop() + m_renderGame->getCellSize() * lineStart);
        blinkArea.lineTo(m_renderGame->getMarginLeft() + m_renderGame->getCellSize() * Tetris::core::Board::m_width,
                         m_renderGame->getMarginTop() + m_renderGame->getCellSize() * lineStart);
        blinkArea.lineTo(m_renderGame->getMarginLeft() + m_renderGame->getCellSize() * Tetris::core::Board::m_width,
                         m_renderGame->getMarginTop() + m_renderGame->getCellSize() * lineStop);
        blinkArea.lineTo(m_renderGame->getMarginLeft(), m_renderGame->getMarginTop() + m_renderGame->getCellSize() * lineStop);
        blinkArea.lineTo(m_renderGame->getMarginLeft(), m_renderGame->getMarginTop() + m_renderGame->getCellSize() * lineStart);

        m_renderGame->setExtraShapes({blinkArea});
        m_renderGame->setExtraColor(Qt::black); // first blink
        m_renderGame->repaint();
        QThread::msleep(50);

        m_renderGame->setExtraColor(QColor(0, 0, 0, 0));
        m_renderGame->repaint();
        QThread::msleep(50);

        m_renderGame->setExtraColor(Qt::black); // second blink
        m_renderGame->repaint();
        QThread::msleep(50);

        m_renderGame->setExtraColor(QColor(0, 0, 0, 0));
        m_renderGame->repaint();

        QThread::msleep(50);
        m_renderGame->setExtraColor(Qt::black); // first blink
        m_renderGame->repaint();
        m_renderGame->setExtraShapes({});
        m_renderGame->setExtraColor(QColor(0, 0, 0, 0));
    }

    void Tetris::gui::MainWindow::addScore(const int completedLines)
    {
        switch (completedLines)
        {
        case 1:
            m_score += 40 * (m_level + 1);
            break;
        case 2:
            m_score += 100 * (m_level + 1);
            break;
        case 3:
            m_score += 300 * (m_level + 1);
            break;
        case 4:
            m_score += 1200 * (m_level + 1);
            break;
        default:
            throw std::runtime_error("Can't complete more than 4 lines at once.");
        }
    }

    // void MainWindow::keyReleaseEvent(QKeyEvent *e)
    // {
    //     // Change piece coordinates after checking if it can moves in
    //     // the pressed direction and rendering the move.
    //     std::string command;
    //     switch (e->key())
    //     {
    //     case Qt::Key_Left:
    //         command = "move_left";
    //         break;
    //     case Qt::Key_Right:
    //         command = "move_right";
    //         break;
    //     case Qt::Key_Up:
    //         command = "rotate";
    //         break;
    //     case Qt::Key_Down:
    //         command = "move_down";
    //         break;
    //     }
    //     networkHandler->sendUserInput(command);
    // }

    void MainWindow::keyReleaseEvent(QKeyEvent *e)
    {
        // Change piece coordinates after checking if it can moves in
        // the pressed direction and rendering the move.
        if (e->key() == Qt::Key_Left)
        {
            if (m_board.canMoveCurrentPieceLeft())
            {
                m_renderGame->update();
                m_board.getCurrentPiece()->setX(m_board.getCurrentPiece()->getX() - 1);
            }
        }
        else if (e->key() == Qt::Key_Right)
        {
            if (m_board.canMoveCurrentPieceRight())
            {
                m_renderGame->update();
                m_board.getCurrentPiece()->setX(m_board.getCurrentPiece()->getX() + 1);
            }
        }
        else if (e->key() == Qt::Key_Up)
        {
            if (m_board.canRotateCurrentPiece())
            {
                m_renderGame->update();
                m_board.getCurrentPiece()->setOrientation((m_board.getCurrentPiece()->getOrientation() + 1) % 4);
            }
        }
        else if (e->key() == Qt::Key_Down)
        {
            if (m_board.canMoveCurrentPieceDown())
            {
                m_renderGame->update();
                m_board.getCurrentPiece()->setY(m_board.getCurrentPiece()->getY() + 1);
            }
        }
    }

    // void MainWindow::sendStartCommandToServer()
    // {
    //     m_buttonStart->setText("restart");
    //     std::string startCommand = "START";
    //     networkHandler->sendUserInput(startCommand);

    //     // startGameStateUpdateLoop();
    // }

    void MainWindow::sendStartCommandToServer()
    {
        if (m_buttonStart->text() == "resume")
        {
            m_timer->start();
            m_buttonStart->setText("restart");
        }
        else
        {
            m_board.clear();
            m_board.setCurrentPiece(m_pieceRandomizer());
            m_board.setNextPiece(m_pieceRandomizer());
            m_renderGame->setBoard(&m_board);
            m_renderGame->setGameOver(false);

            m_renderPreview->setTetromino(m_board.getNextPiece());
            m_renderPreview->update();
            m_buttonStart->setText(QString("restart"));

            m_lines = 0;
            m_level = 0;
            m_score = 0;

            m_labelLines->setText(QString("Lines\n0"));
            m_labelLevel->setText(QString("Level\n0"));
            m_labelScore->setText(QString("Score\n0"));

            m_timer->stop();
            m_timer->start(m_timeUpdate);
        }
    }

    // void MainWindow::sendPauseCommandToServer()
    // {
    //     // Send pause command to server
    //     m_buttonStart->setText("resume");
    //     std::string startCommand = "PAUSE";
    //     networkHandler->sendUserInput(startCommand);
    // }

    void MainWindow::sendPauseCommandToServer()
    {
        m_timer->stop();
        m_buttonStart->setText("resume");
    }

    // void MainWindow::changePiecePandomizer()
    // {
    //     if (m_comboRandomizer->currentText().contains("uniform"))
    //     {
    //         m_pieceRandomizer = Tetris::core::TetrominoFactory::UniformPieceRandomizer;
    //     }
    //     else if (m_comboRandomizer->currentText().contains("7-bag"))
    //     {
    //         m_pieceRandomizer = Tetris::core::TetrominoFactory::BagPieceRandomizer;
    //     }
    //     else
    //     {
    //         throw std::runtime_error("Unknown piece randomizer");
    //     }
    // }
    void MainWindow::changePieceRandomizer()
    {
        // Handle change in piece randomizer selection
        std::string command = m_comboRandomizer->currentText().contains("uniform") ? "uniform_randomizer" : "7_bag_randomizer";
        networkHandler->sendUserInput(command);
    }

    void MainWindow::connectToServer()
    {
        try
        {
            networkHandler->start(); // Start the network thread and attempt to connect

            MessageHeader header;
            std::string serverResponse = networkHandler->receiveMessage(header);

            if (header.messageType == MessageType::ID_RESPONSE)
            {
                std::string idPrefix = "ID=";
                size_t idPos = serverResponse.find(idPrefix);

                if (idPos != std::string::npos)
                {
                    std::string idStr = serverResponse.substr(idPos + idPrefix.length());
                    int receivedClientID = std::stoi(idStr);

                    networkHandler->setClientID(receivedClientID);
                    std::cout << "Client ID received and set: " << receivedClientID << std::endl;
                }
                else
                {
                    std::cerr << "Invalid ID response from server." << std::endl;
                }
            }
            else
            {
                std::cerr << "Unexpected message type received: " << static_cast<int>(header.messageType) << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception in connectToServer: " << e.what() << std::endl;
        }
    }
}