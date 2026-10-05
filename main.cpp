#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Fashionova Orders");
    app.setStyle("Fusion");

    MainWindow w;
    w.show();
    return app.exec();
}
