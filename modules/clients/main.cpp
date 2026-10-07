#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("HighFashionWorkshop");
    app.setOrganizationName("FashioNova");

    MainWindow w;
    w.show();
    return app.exec();
}
