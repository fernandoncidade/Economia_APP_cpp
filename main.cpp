/**
 * @file main.cpp
 * @brief Ponto de entrada principal da aplicação Economia_APP (Qt6 / C++).
 */

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <exception>
#include <iostream>

#include "source/fca_01_FinancialCalculatorAPP.hpp"
#include "source/utils/LogManager.hpp"
#include "source/utils/TrialManager.hpp"

int main(int argc, char *argv[]) {
    Logger logger = LogManager::get_logger();

    try {
        QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
        QCoreApplication::addLibraryPath(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("platforms")));
        QCoreApplication::addLibraryPath(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("plugins")));

        QApplication app(argc, argv);

        FinancialCalculatorApp window;
        // TrialManager::enforce_trial();  // Descomente esta linha para forçar o uso da versão de avaliação
        // TrialManager::delete_first_run_timestamp();  // Use esta linha para testes, removendo o timestamp de primeiro uso
        window.show();

        int exit_code = app.exec();
        logger.debug(QString("Aplicação encerrada com código de saída: %1").arg(exit_code));
        return exit_code;

    } catch (const std::exception& e) {
        logger.critical(QString("Erro fatal ao iniciar aplicação: %1").arg(e.what()));
        return 1;
    } catch (...) {
        logger.critical("Erro fatal desconhecido ao iniciar aplicação.");
        return 1;
    }
}
