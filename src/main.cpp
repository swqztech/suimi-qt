#include "mainwindow.h"

#include <QApplication>
#include <QFont>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QFont font(QStringLiteral("Microsoft YaHei UI"), 11);
  app.setFont(font);
  app.setStyleSheet(
      "QMainWindow, QWidget { background: #1a262a; color: #d2dade; }"
      "QTabWidget::pane { border: 1px solid #37484e; border-radius: 4px; top: -1px; }"
      "QTabBar::tab { background: #202c30; color: #78878e; padding: 8px 18px; margin-right: 4px; "
      "border-top-left-radius: 6px; border-top-right-radius: 6px; }"
      "QTabBar::tab:selected { background: #2d3e44; color: #d4af5f; }"
      "QSlider::groove:horizontal { height: 6px; background: #233033; border-radius: 3px; }"
      "QSlider::handle:horizontal { width: 14px; margin: -5px 0; background: #d4af5f; border-radius: 7px; }"
      "QPushButton#primaryBtn { background: #3ab26c; color: white; font-weight: bold; border: none; "
      "border-radius: 6px; }"
      "QPushButton#primaryBtn:hover { background: #47c478; }"
      "QPushButton#secondaryBtn { background: #202c30; color: #d2dade; border: 1px solid #37484e; "
      "border-radius: 6px; }"
      "QPushButton#secondaryBtn:hover { background: #2d3e44; }");

  MainWindow w;
  w.show();
  return app.exec();
}
