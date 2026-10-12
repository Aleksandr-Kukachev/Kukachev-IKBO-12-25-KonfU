#include <QApplication>
#include <QMessageBox>
#include <iostream>
#include <stdexcept>
#include "MainWindow.h"
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    try {
        std::vector<std::string> args;
        const auto qtArgs = app.arguments();
        for (qsizetype i = 1; i < qtArgs.size(); ++i) args.push_back(qtArgs[i].toStdString());
        auto config = Config::parse(args);
        if (config.help) {
            QMessageBox::information(nullptr, "Help", QString::fromStdString(Config::usage()));
            return 0;
        }
        MainWindow window(config);
        window.show();
        return app.exec();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        QMessageBox::critical(nullptr, "Startup error", QString::fromUtf8(e.what()));
        return 1;
    }
}
