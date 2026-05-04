#ifndef DARKTHEME_H
#define DARKTHEME_H

#include <QWidget>
#include <QPalette>
#include <QApplication>

// ----------------------------- Colors -------------------------------
static const QColor BG_MAIN            = QColor(26, 26, 30);
static const QColor BG_ELEVATED        = QColor(35, 35, 40);

static const QColor BG_PANEL           = QColor(34, 34, 38);
static const QColor BG_CARD            = QColor(30, 30, 30);    //#1E1E1E

static const QColor BORDER             = QColor(42, 42, 42);    //#2A2A2A

static const QColor ACCENT_GRAY        = QColor(128, 128, 128);
static const QColor ACCENT_WHITE       = QColor(232, 232, 232);
static const QColor ACCENT_MUTED       = QColor(77, 168, 255);
static const QColor ACCENT_BLUE        = QColor(77, 168, 255);
static const QColor ACCENT_GREEN       = QColor(61, 203, 138);
static const QColor ACCENT_AMBER       = QColor(196, 145, 26);
static const QColor ACCENT_RED         = QColor(224, 85, 85);

static const QColor ACCENT_BLUE_BRUSH  = QColor(77, 168, 255, 50);
static const QColor ACCENT_GREEN_BRUSH = QColor(61, 203, 138, 50);
static const QColor ACCENT_AMBER_BRUSH = QColor(196, 145, 26, 50);
static const QColor ACCENT_RED_BRUSH   = QColor(224, 85, 85, 50);

class DarkTheme final {
public:
    static void applyDarkTheme(QApplication& app);
};

#endif // DARKTHEME_H
