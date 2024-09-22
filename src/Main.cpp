#include "Constants.h"
#include "Window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    RaycasterEngine::Window w;
    w.resize(SCREEN_WIDTH, SCREEN_HEIGHT);
    w.show();
    return app.exec();
}
