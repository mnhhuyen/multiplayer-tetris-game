#include "gui/MainWindow.hpp"  // This assumes your CMakeLists.txt has included the directories properly

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Tetris::gui::MainWindow window;  // Update this according to the actual namespace where MainWindow is defined
    window.show();
    return app.exec();
}
