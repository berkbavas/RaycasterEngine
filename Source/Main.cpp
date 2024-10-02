#include "Controller.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    RaycasterEngine::Controller c;
    c.Run();

    return app.exec();
}
