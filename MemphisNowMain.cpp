#include "MPSMainWindow.hpp"
#include <QApplication>
//#include <QDesktopWidget>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MPSMainWindow window;
    window.adjustSize();
    //window.move(QApplication::desktop()->screen()->rect().center() - window.rect().center());
    window.show();

    return app.exec();
}
