#include <QApplication>

#include "../headers/mainwindow.h"
#include "../headers/themes/darkTheme.h"
#include "../headers/net/client.h"
#include "../headers/controller/dashboardController.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    DarkTheme::applyDarkTheme(a);

    Client* client = new Client();
    DashboardController* controller = new DashboardController(client);
    MainWindow* w = new MainWindow(controller);

    client->setParent(w);
    controller->setParent(client);

    client->startConnection();

    return a.exec();
}
