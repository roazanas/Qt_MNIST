#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QPalette p;
    p = qApp->palette();
    p.setColor(QPalette::Window, QColor(53,53,53));
    p.setColor(QPalette::Button, QColor(63,63,63));
    p.setColor(QPalette::Disabled, QPalette::Button, QColor(43,43,43));
    p.setColor(QPalette::Highlight, QColor(142,45,197));
    p.setColor(QPalette::Disabled, QPalette::Highlight, QColor(43,43,43));
    p.setColor(QPalette::AlternateBase, QColor(43,43,43));
    qApp->setPalette(p);
    MainWindow w;
    w.show();
    return a.exec();
}
