#include "mainwindow.h"

#include <QApplication>
#include <QClipboard>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSlider>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>

namespace {

QLabel* makeMutedLabel(const QString& text) {
  auto* l = new QLabel(text);
  l->setStyleSheet("color: #78878e; font-size: 10pt;");
  l->setWordWrap(true);
  return l;
}

QPushButton* makePill(const QString& text) {
  auto* b = new QPushButton(text);
  b->setCheckable(true);
  b->setCursor(Qt::PointingHandCursor);
  b->setMinimumHeight(34);
  return b;
}

}  // namespace

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  setWindowTitle(QStringLiteral("随密"));
  resize(500, 680);

  auto* central = new QWidget(this);
  auto* root = new QVBoxLayout(central);
  root->setContentsMargins(20, 18, 20, 18);
  root->setSpacing(12);

  auto* title = new QLabel(QStringLiteral("随密"));
  title->setAlignment(Qt::AlignCenter);
  title->setStyleSheet("color: #d4af5f; font-size: 26pt; font-weight: bold;");
  root->addWidget(title);

  auto* sub = new QLabel(QStringLiteral("掷一次，拿一把新钥匙"));
  sub->setAlignment(Qt::AlignCenter);
  sub->setStyleSheet("font-size: 11pt;");
  root->addWidget(sub);

  auto* hint = makeMutedLabel(QStringLiteral("本地生成 · 不上传 · 系统安全随机源"));
  hint->setAlignment(Qt::AlignCenter);
  root->addWidget(hint);

  tabs_ = new QTabWidget;
  tabs_->addTab(new QWidget, QStringLiteral("密码"));
  tabs_->addTab(new QWidget, QStringLiteral("用户名"));
  tabs_->addTab(new QWidget, QStringLiteral("网名"));
  connect(tabs_, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
  root->addWidget(tabs_);

  output_ = new QTextEdit;
  output_->setReadOnly(true);
  output_->setMinimumHeight(72);
  output_->setMaximumHeight(120);
  QFont mono = output_->font();
  mono.setStyleHint(QFont::Monospace);
  mono.setPointSize(11);
  output_->setFont(mono);
  output_->setStyleSheet(
      "QTextEdit { background: #121c20; border: 1px solid #37484e; border-radius: 8px; padding: 8px; }");
  root->addWidget(output_);

  strength_row_ = new QWidget;
  auto* strength_layout = new QVBoxLayout(strength_row_);
  strength_layout->setContentsMargins(0, 0, 0, 0);
  strength_bar_ = new QProgressBar;
  strength_bar_->setTextVisible(false);
  strength_bar_->setFixedHeight(8);
  strength_bar_->setStyleSheet(
      "QProgressBar { background: #233033; border: none; border-radius: 4px; }"
      "QProgressBar::chunk { background: #48c47a; border-radius: 4px; }");
  strength_label_ = makeMutedLabel("");
  strength_layout->addWidget(strength_bar_);
  strength_layout->addWidget(strength_label_);
  root->addWidget(strength_row_);

  length_row_ = new QWidget;
  auto* len_layout = new QHBoxLayout(length_row_);
  len_layout->setContentsMargins(0, 0, 0, 0);
  auto* len_title = new QLabel(QStringLiteral("长度"));
  len_title->setStyleSheet("color: #78878e;");
  length_slider_ = new QSlider(Qt::Horizontal);
  length_slider_->setRange(6, 64);
  length_slider_->setValue(password_opts_.length);
  length_value_ = new QLabel(QString::number(password_opts_.length));
  length_value_->setStyleSheet("color: #d4af5f; font-weight: bold; min-width: 28px;");
  connect(length_slider_, &QSlider::valueChanged, this, &MainWindow::onLengthChanged);
  len_layout->addWidget(len_title);
  len_layout->addWidget(length_slider_, 1);
  len_layout->addWidget(length_value_);
  root->addWidget(length_row_);

  password_opts_panel_ = new QWidget;
  auto* opt_flow = new QHBoxLayout(password_opts_panel_);
  opt_flow->setContentsMargins(0, 0, 0, 0);
  opt_flow->setSpacing(8);
  opt_lower_ = makePill(QStringLiteral("小写 a-z"));
  opt_upper_ = makePill(QStringLiteral("大写 A-Z"));
  opt_digit_ = makePill(QStringLiteral("数字 0-9"));
  opt_symbol_ = makePill(QStringLiteral("符号 !@#"));
  opt_ambig_ = makePill(QStringLiteral("排除易混 0OIl1"));
  opt_lower_->setChecked(true);
  opt_upper_->setChecked(true);
  opt_digit_->setChecked(true);
  opt_symbol_->setChecked(true);
  opt_ambig_->setChecked(true);
  for (auto* b : {opt_lower_, opt_upper_, opt_digit_, opt_symbol_, opt_ambig_}) {
    connect(b, &QPushButton::toggled, this, &MainWindow::onOptionToggled);
    stylePill(b, b->isChecked());
  }
  opt_flow->addWidget(opt_lower_);
  opt_flow->addWidget(opt_upper_);
  opt_flow->addWidget(opt_digit_);
  opt_flow->addWidget(opt_symbol_);
  opt_flow->addWidget(opt_ambig_);
  opt_flow->addStretch();
  root->addWidget(password_opts_panel_);

  mode_hint_ = makeMutedLabel("");
  root->addWidget(mode_hint_);

  auto* actions = new QHBoxLayout;
  auto* roll_btn = new QPushButton(QStringLiteral("再掷一次"));
  roll_btn->setObjectName("primaryBtn");
  roll_btn->setMinimumSize(112, 40);
  auto* copy_btn = new QPushButton(QStringLiteral("复制"));
  copy_btn->setObjectName("secondaryBtn");
  copy_btn->setMinimumSize(80, 40);
  auto* six_btn = new QPushButton(QStringLiteral("一次六个"));
  six_btn->setObjectName("secondaryBtn");
  six_btn->setMinimumSize(100, 40);
  connect(roll_btn, &QPushButton::clicked, this, &MainWindow::rollOnce);
  connect(copy_btn, &QPushButton::clicked, this, &MainWindow::copyOutput);
  connect(six_btn, &QPushButton::clicked, this, &MainWindow::rollSix);
  actions->addWidget(roll_btn);
  actions->addWidget(copy_btn);
  actions->addWidget(six_btn);
  actions->addStretch();
  root->addLayout(actions);

  auto* foot = makeMutedLabel(QStringLiteral("Passroll · 字符从勾选集合中均匀抽取"));
  foot->setAlignment(Qt::AlignCenter);
  root->addWidget(foot);

  root->addStretch();
  setCentralWidget(central);

  rebuildUiForMode();
  rollOnce();
}

void MainWindow::stylePill(QPushButton* btn, bool on) {
  if (on) {
    btn->setStyleSheet(
        "QPushButton { background: #2d3e44; color: #d4af5f; border: 1px solid #8c7648; border-radius: 16px; "
        "padding: 6px 12px; }");
  } else {
    btn->setStyleSheet(
        "QPushButton { background: #202c30; color: #78878e; border: 1px solid #37484e; border-radius: 16px; "
        "padding: 6px 12px; }");
  }
}

void MainWindow::onTabChanged(int index) {
  if (index == 0) mode_ = Mode::Password;
  else if (index == 1) mode_ = Mode::Username;
  else mode_ = Mode::Nickname;
  rebuildUiForMode();
  rollOnce();
}

void MainWindow::rebuildUiForMode() {
  const bool pwd = mode_ == Mode::Password;
  const bool user = mode_ == Mode::Username;
  strength_row_->setVisible(pwd);
  password_opts_panel_->setVisible(pwd);
  length_row_->setVisible(pwd || user);

  if (pwd) {
    length_slider_->setRange(6, 64);
    length_slider_->setValue(password_opts_.length);
    length_value_->setText(QString::number(password_opts_.length));
    mode_hint_->clear();
  } else if (user) {
    length_slider_->setRange(4, 32);
    length_slider_->setValue(username_length_);
    length_value_->setText(QString::number(username_length_));
    mode_hint_->setText(QStringLiteral("以小写字母开头，可含数字与下划线"));
  } else {
    mode_hint_->setText(QStringLiteral("从内置词库随机组合中文网名，可带数字后缀"));
  }
}

void MainWindow::onLengthChanged(int value) {
  length_value_->setText(QString::number(value));
  if (mode_ == Mode::Password) {
    password_opts_.length = value;
    updateStrength();
  } else if (mode_ == Mode::Username) {
    username_length_ = value;
  }
}

void MainWindow::onOptionToggled() {
  password_opts_.lowercase = opt_lower_->isChecked();
  password_opts_.uppercase = opt_upper_->isChecked();
  password_opts_.digits = opt_digit_->isChecked();
  password_opts_.symbols = opt_symbol_->isChecked();
  password_opts_.exclude_ambiguous = opt_ambig_->isChecked();
  stylePill(opt_lower_, password_opts_.lowercase);
  stylePill(opt_upper_, password_opts_.uppercase);
  stylePill(opt_digit_, password_opts_.digits);
  stylePill(opt_symbol_, password_opts_.symbols);
  stylePill(opt_ambig_, password_opts_.exclude_ambiguous);
  updateStrength();
}

void MainWindow::updateStrength() {
  if (mode_ != Mode::Password) return;
  const Strength s = password_strength(password_opts_);
  strength_bar_->setValue(static_cast<int>(s.ratio * 100));
  strength_label_->setText(
      QStringLiteral("%1 · 约 %2 bit").arg(QString::fromUtf8(s.label)).arg(static_cast<int>(s.bits)));
}

void MainWindow::setOutput(const std::string& text) {
  output_->setPlainText(QString::fromUtf8(text.c_str()));
}

void MainWindow::rollOnce() {
  switch (mode_) {
    case Mode::Password:
      setOutput(generate_password(password_opts_));
      updateStrength();
      break;
    case Mode::Username:
      setOutput(generate_username(length_slider_->value()));
      break;
    case Mode::Nickname:
      setOutput(generate_nickname());
      break;
  }
}

void MainWindow::rollSix() {
  switch (mode_) {
    case Mode::Password:
      setOutput(generate_passwords(password_opts_, 6));
      updateStrength();
      break;
    case Mode::Username: {
      std::string lines;
      const int len = length_slider_->value();
      for (int i = 0; i < 6; ++i) {
        if (i) lines += '\n';
        lines += generate_username(len);
      }
      setOutput(lines);
      break;
    }
    case Mode::Nickname: {
      std::string lines;
      for (int i = 0; i < 6; ++i) {
        if (i) lines += '\n';
        lines += generate_nickname();
      }
      setOutput(lines);
      break;
    }
  }
}

void MainWindow::copyOutput() {
  const QString t = output_->toPlainText();
  if (t.isEmpty()) return;
  QApplication::clipboard()->setText(t);
}
