#pragma once

#include "generate.h"

#include <QMainWindow>

class QLabel;
class QProgressBar;
class QPushButton;
class QSlider;
class QTabWidget;
class QTextEdit;
class QWidget;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);

 private slots:
  void onTabChanged(int index);
  void onLengthChanged(int value);
  void rollOnce();
  void rollSix();
  void copyOutput();
  void onOptionToggled();

 private:
  void rebuildUiForMode();
  void updateStrength();
  void setOutput(const std::string& text);
  void stylePill(QPushButton* btn, bool on);

  Mode mode_ = Mode::Password;
  PasswordOptions password_opts_{};
  int username_length_ = 12;

  QTabWidget* tabs_ = nullptr;
  QTextEdit* output_ = nullptr;
  QProgressBar* strength_bar_ = nullptr;
  QLabel* strength_label_ = nullptr;
  QWidget* strength_row_ = nullptr;

  QWidget* length_row_ = nullptr;
  QSlider* length_slider_ = nullptr;
  QLabel* length_value_ = nullptr;

  QWidget* password_opts_panel_ = nullptr;
  QPushButton* opt_lower_ = nullptr;
  QPushButton* opt_upper_ = nullptr;
  QPushButton* opt_digit_ = nullptr;
  QPushButton* opt_symbol_ = nullptr;
  QPushButton* opt_ambig_ = nullptr;

  QLabel* mode_hint_ = nullptr;
};
