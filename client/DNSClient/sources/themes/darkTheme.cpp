#include "../../headers/themes/darkTheme.h"

/*static*/
void DarkTheme::applyDarkTheme(QApplication& app) {
    // ----------------------------- Palette ------------------------------
    QPalette dark;

    dark.setColor(QPalette::Window, BG_MAIN);
    dark.setColor(QPalette::WindowText, ACCENT_WHITE);

    dark.setColor(QPalette::Base, BG_PANEL);
    dark.setColor(QPalette::AlternateBase, BG_ELEVATED);

    dark.setColor(QPalette::Text, ACCENT_WHITE);
    dark.setColor(QPalette::Button, BG_PANEL);
    dark.setColor(QPalette::ButtonText, ACCENT_WHITE);

    dark.setColor(QPalette::Highlight, ACCENT_BLUE);
    dark.setColor(QPalette::HighlightedText, Qt::black);

    // ----------------------------- Style -------------------------------
    app.setStyleSheet(R"(
        QWidget {
            background-color: #121214;
            color: #e6e6e6;
            font-family: 'Inter', 'Segoe UI', sans-serif;
            font-size: 12px;
            border: none;
        }

        /* ====== КАРТОЧКИ — без обводки ====== */
        QFrame {
            background-color: #1c1c20;
            border: none;
            border-radius: 6px;
        }
        QGroupBox {
            background-color: #1c1c20;
            border: none;
            border-radius: 6px;
            margin-top: 20px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            top: 4px;
            color: #6b6b75;
            font-size: 11px;
        }

        /* ====== РАЗДЕЛИТЕЛИ между ячейками топбара ====== */
        /* Делай через QFrame с фиксированной шириной 1px вместо border */

        /* ====== КНОПКИ ====== */
        QPushButton {
            background-color: #1c1c20;
            border: none;
            padding: 6px 12px;
            border-radius: 6px;
            color: #e6e6e6;
        }
        QPushButton:hover { background-color: #232328; }
        QPushButton:pressed { background-color: #2a2a30; }

        /* ====== ТАБЛИЦА ====== */
        QTableWidget {
            background-color: #1c1c20;
            border: none;
            gridline-color: transparent;
            outline: none;
        }
        QTableWidget::item {
            padding: 6px 10px;
            border: none;
            color: #e6e6e6;
        }
        QTableWidget::item:selected {
            background-color: #232328;
            color: #e6e6e6;
        }
        QHeaderView {
            background-color: #1c1c20;
            border: none;
        }
        QHeaderView::section {
            background-color: #1c1c20;
            color: #6b6b75;
            font-size: 11px;
            padding: 6px 10px;
            border: none;
        }

        /* ====== СПИСОК ====== */
        QListWidget {
            background-color: #1c1c20;
            border: none;
            border-radius: 6px;
            outline: none;
        }
        QListWidget::item {
            padding: 8px 10px;
            border: none;
            border-radius: 0px;
            color: #e6e6e6;
        }
        QListWidget::item:selected {
            background-color: #232328;
        }

        /* ====== ИНПУТЫ ====== */
        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: #1c1c20;
            border: none;
            border-radius: 6px;
            padding: 5px 8px;
            color: #e6e6e6;
        }
        QLineEdit:focus {
            background-color: #232328;
        }

        /* ====== СКРОЛЛБАР ====== */
        QScrollBar:vertical {
            background: transparent;
            width: 6px;
            margin: 0;
            border: none;
        }
        QScrollBar::handle:vertical {
            background: #2d2d32;
            border-radius: 3px;
            min-height: 20px;
            border: none;
        }
        QScrollBar::handle:vertical:hover { background: #3a3a40; }
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical { height: 0px; }
        QScrollBar:horizontal {
            background: transparent;
            height: 6px;
            border: none;
        }
        QScrollBar::handle:horizontal {
            background: #2d2d32;
            border-radius: 3px;
            border: none;
        }

        /* ====== СПЛИТТЕР ====== */
        QSplitter::handle {
            background: #2d2d32;
            width: 1px;
            border: none;
        }

        /* ====== ЛЕЙБЛЫ ====== */
        QLabel {
            background: transparent;
            border: none;
        }
    )");

    app.setPalette(dark);
}
