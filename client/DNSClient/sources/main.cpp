#include <QApplication>

#include "../headers/mainwindow.h"
#include "../headers/themes/darkTheme.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    DarkTheme::applyDarkTheme(a);
    MainWindow w;
    w.show();
    return a.exec();
}
