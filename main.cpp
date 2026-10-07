#include "mainwindow.h"

#include <QApplication>
#include <QColor>
#include <QPalette>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("HighFashionWorkshop");
    app.setOrganizationName("FashioNova");

    // Thème clair forcé (ignore le mode sombre de Windows) - repris du module Articles.
    // Pour revenir au style natif, supprimer ce bloc.
    app.setStyle("Fusion");
    QPalette p;
    p.setColor(QPalette::Window, QColor("#f7f1e8"));
    p.setColor(QPalette::WindowText, QColor("#1d2a3a"));
    p.setColor(QPalette::Base, Qt::white);
    p.setColor(QPalette::AlternateBase, QColor("#fbf6ee"));
    p.setColor(QPalette::Text, QColor("#1d2a3a"));
    p.setColor(QPalette::PlaceholderText, QColor("#9aa3b0"));
    p.setColor(QPalette::Button, Qt::white);
    p.setColor(QPalette::ButtonText, QColor("#1d2a3a"));
    p.setColor(QPalette::ToolTipBase, Qt::white);
    p.setColor(QPalette::ToolTipText, QColor("#1d2a3a"));
    p.setColor(QPalette::Highlight, QColor("#d9a566"));
    p.setColor(QPalette::HighlightedText, QColor("#1d2a3a"));
    app.setPalette(p);

    MainWindow w;
    w.show();
    return app.exec();
}
