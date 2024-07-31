/*
 * Marvelous OTF2 Traces Interactive Visualizer (MOTIV)
 * Copyright (C) 2023   Florian Gallrein,
 *                      Björn Gehrke, 
 *                      Jessica Lafontaine,
 *                      Tomas Cirkov
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QIODeviceBase>

#include "src/ui/windows/MainWindow.hpp"
#include "src/ui/windows/MpiAnalysisWindow.hpp"
#include "src/ui/windows/RecentFilesDialog.hpp"

// Decides wether performance information will be printed
bool testRun = false;

int main(int argc, char *argv[])
{
    QElapsedTimer appTimer;
    appTimer.start();
    QApplication app(argc, argv);
    QApplication::setApplicationName("Motiv");
    QApplication::setApplicationVersion(MOTIV_VERSION_STRING);
    QApplication::setWindowIcon(QIcon(":/res/motiv.png"));

    // Load an application style
    QFile styleFile( ":/res/style.qss" );
    styleFile.open( QFile::ReadOnly );

    // Apply the loaded stylesheet
    QString style( styleFile.readAll() );
    app.setStyleSheet( style );

    QCommandLineParser parser;
    parser.setApplicationDescription("Visualizer for OTF2 trace files");

    QCommandLineOption helpOption = parser.addHelpOption();
    QCommandLineOption versionOption = parser.addVersionOption();
	QCommandLineOption testrunOption("t", QCoreApplication::translate("main", "Runs motiv in test mode, i.e. pure trace-loading without GUI representation for benchmark purposes."), "file");
	parser.addOption(testrunOption);
    parser.addPositionalArgument("file", QCoreApplication::translate("main", "filepath of the .otf2 trace file to open"), "[file]");

    QCommandLineOption modeOption("m", QCoreApplication::translate("main", "Selects the visualisation mode of Motiv.\n0: Default, 1: MPI Communication"), "mode");
    parser.addOption(modeOption);

    parser.process(app);

    // Early return if help or version is shown
    if (parser.isSet(helpOption) || parser.isSet(versionOption)) {
        return EXIT_SUCCESS;
    }

    QStringList positionalArguments = parser.positionalArguments();
    QString filepath;
    if (!positionalArguments.isEmpty()) {
        filepath = positionalArguments.first();
    }

    bool modeValidity = true;
    int mode = parser.value(modeOption).toInt(&modeValidity); 

    // Test run without window display
	if (parser.isSet(testrunOption)){     
		testRun = true;
        [[maybe_unused]] auto dummyMainWindow = new MpiAnalysisWindow(parser.value(testrunOption));
        app.quit();
        std::cout << "%application in general%" << appTimer.elapsed() << "%ms%";
        return EXIT_SUCCESS;
	}

    RecentFilesDialog recentFilesDialog(&filepath);

    // Swap mode control to run Motiv in alternative mode by default
    // Also, update mode ID in AppSettings.hpp 
    if((modeValidity || (!parser.isSet(modeOption))) && (!filepath.isEmpty() || recentFilesDialog.exec() == QDialog::Accepted)) {
        if((!parser.isSet(modeOption)) || mode == 0){
        //  if (parser.isSet(modeOption) && mode == 1){           
            auto mainWindow = new MainWindow(filepath);            
            QString fullTitle;
            QTextStream text(&fullTitle);
            text << "Motiv " MOTIV_VERSION_STRING;
            mainWindow->setWindowTitle(fullTitle);
            qInfo() << "motiv ready";
            mainWindow->show();
            }
        //if((!parser.isSet(modeOption)) || mode == 0){ 
        if (parser.isSet(modeOption) && mode == 1){         
            auto mainWindow = new MpiAnalysisWindow(filepath);            
            QString fullTitle;
            QTextStream text(&fullTitle);
            text << "Motiv " MOTIV_VERSION_STRING;
            mainWindow->setWindowTitle(fullTitle);
            qInfo() << "motiv ready";
            mainWindow->show();

        }
    } else {
        app.quit();
        return EXIT_SUCCESS;
    }

    return app.exec();
}
