#ifndef DARKTHEME_H
#define DARKTHEME_H

#include <QWidget>
#include <QPalette>
#include <QApplication>

class DarkTheme final {
public:
    static void applyDarkTheme(QApplication& app);
};

#endif // DARKTHEME_H
