#include "MainWindow.h"
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSysInfo>
#include <QFontDatabase>
#include <QByteArray>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <sstream>
#include "ScriptRunner.h"

MainWindow::MainWindow(Config config, QWidget* parent) : QWidget(parent), config_(config), shell_(config) {
    QString user = qEnvironmentVariable("USER");
    if (user.isEmpty()) user = qEnvironmentVariable("USERNAME");
    if (user.isEmpty()) user = qEnvironmentVariable("LOGNAME", "unknown");
    identity_ = user + "@" + QSysInfo::machineHostName();
    setWindowTitle(QStringLiteral("Эмулятор - [") + identity_ + "]");
    resize(850, 520);
    output_ = new QPlainTextEdit(this);
    output_->setReadOnly(true);
    input_ = new QLineEdit(this);
    prompt_ = new QLabel(promptText(), this);
    auto font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    output_->setFont(font); input_->setFont(font); prompt_->setFont(font);
    setStyleSheet("QWidget { background: #14181e; color: #dde7ed; } QLineEdit { border: 1px solid #52616b; padding: 6px; } QPlainTextEdit { border: 0; }");
    auto* layout = new QVBoxLayout(this);
    auto* row = new QHBoxLayout;
    row->addWidget(prompt_); row->addWidget(input_, 1);
    layout->addWidget(output_, 1); layout->addLayout(row);
    connect(input_, &QLineEdit::returnPressed, this, [this] {
        auto line = input_->text(); input_->clear(); submit(line);
    });
    append("UNIX shell emulator");
    append(QString::fromStdString(config_.dump()));
    qInfo().noquote() << QString::fromStdString(config_.dump());
    input_->setFocus();
    if (!config_.scriptPath.empty()) QTimer::singleShot(0, this, [this] { runStartupScript(); });
}

QString MainWindow::promptText() const {
    return identity_ + ":" + QString::fromStdString(shell_.cwd()) + "$ ";
}
void MainWindow::append(const QString& text) { output_->appendPlainText(text); }
Result MainWindow::submit(const QString& line) {
    append(promptText() + line);
    auto result = shell_.execute(line.toStdString());
    if (!result.output.empty()) append(QString::fromStdString(result.output));
    prompt_->setText(promptText());
    if (result.exit) close();
    return result;
}

void MainWindow::runStartupScript() {
    QFile file(QString::fromStdString(config_.scriptPath));
    if (!file.open(QIODevice::ReadOnly)) {
        append("script: cannot open " + file.fileName() + ": " + file.errorString());
        return;
    }
    std::istringstream input(file.readAll().toStdString());
    auto status = ScriptRunner::run(input, [this](const std::string& line) {
        return submit(QString::fromStdString(line));
    });
    if (!status.ok) append(QString::fromStdString(status.error));
}
