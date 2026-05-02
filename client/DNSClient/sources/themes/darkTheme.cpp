#include "../../headers/themes/darkTheme.h"

/*static*/
void DarkTheme::applyDarkTheme(QApplication& app) {
    // ----------------------------- Colors -------------------------------
    static const QColor BG_MAIN      = QColor(18, 18, 20);   // main background
    static const QColor TEXT_MAIN    = QColor(230, 230, 230);
    static const QColor TEXT_MUTED   = QColor(140, 140, 150);

    static const QColor BG_PANEL     = QColor(28, 28, 32);   // cards
    static const QColor BG_ELEVATED  = QColor(35, 35, 40);   // hover

    static const QColor ACCENT_BLUE  = QColor(0, 153, 255);
    static const QColor ACCENT_GREEN = QColor(0, 200, 120);
    static const QColor ACCENT_ORANGE= QColor(255, 170, 60);
    static const QColor ACCENT_RED   = QColor(255, 80, 80);

    static const QColor BORDER       = QColor(45, 45, 50);

    // ----------------------------- Palette ------------------------------
    QPalette dark;

    dark.setColor(QPalette::Window, BG_MAIN);
    dark.setColor(QPalette::WindowText, TEXT_MAIN);

    dark.setColor(QPalette::Base, BG_PANEL);
    dark.setColor(QPalette::AlternateBase, BG_ELEVATED);

    dark.setColor(QPalette::Text, TEXT_MAIN);
    dark.setColor(QPalette::Button, BG_PANEL);
    dark.setColor(QPalette::ButtonText, TEXT_MAIN);

    dark.setColor(QPalette::Highlight, ACCENT_BLUE);
    dark.setColor(QPalette::HighlightedText, Qt::black);

    // ----------------------------- Style -------------------------------
    app.setStyleSheet(R"(
        /* ====== MAIN WINDOW ====== */
        QWidget {
            background-color: #121214;
            color: #e6e6e6;
            font-size: 12px;
        }

        /* ====== CARDS / PANELS ====== */
        QFrame, QGroupBox {
            background-color: #1c1c20;
            border: 1px solid #2d2d32;
            border-radius: 10px;
        }

        /* ====== BUTTONS ====== */
        QPushButton {
            background-color: #1c1c20;
            border: 1px solid #2d2d32;
            padding: 6px 10px;
            border-radius: 8px;
        }

        QPushButton:hover {
            background-color: #232328;
            border: 1px solid #3a3a40;
        }

        QPushButton:pressed {
            background-color: #2a2a30;
        }

        /* ====== INPUTS ====== */
        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: #1c1c20;
            border: 1px solid #2d2d32;
            border-radius: 6px;
            padding: 5px;
        }

        /* ====== TABLE ====== */
        QTableWidget {
            background-color: #1c1c20;
            border: 1px solid #2d2d32;
            gridline-color: #2d2d32;
        }

        QHeaderView::section {
            background-color: #232328;
            padding: 5px;
            border: none;
            border-bottom: 1px solid #2d2d32;
        }

        /* ====== SIDEBAR STYLE (как на скрине) ====== */
        QListWidget {
            background-color: #121214;
            border: none;
        }

        QListWidget::item {
            padding: 10px;
            border-radius: 6px;
        }

        QListWidget::item:selected {
            background-color: #232328;
        }

        /* ====== SCROLLBAR ====== */
        QScrollBar:vertical {
            background: #121214;
            width: 8px;
        }

        QScrollBar::handle:vertical {
            background: #2d2d32;
            border-radius: 4px;
        }

        QScrollBar::handle:vertical:hover {
            background: #3a3a40;
        }
    )");

    app.setPalette(dark);
}
