#pragma once
#include <QWidget>
#include <QString>
#include "Shell.h"
class QPlainTextEdit;
class QLineEdit;
class QLabel;

class MainWindow : public QWidget {
public:
    explicit MainWindow(Config config, QWidget* parent = nullptr);
private:
    Config config_;
    Shell shell_;
    QPlainTextEdit* output_;
    QLineEdit* input_;
    QLabel* prompt_;
    QString identity_;
    QString promptText() const;
    void append(const QString& text);
    void runStartupScript();
    Result submit(const QString& line);
};
