#include "gui/MainWindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Tetris::gui::MainWindow window;
    window.show();
    return app.exec();
}
