/*
 * Marvelous OTF2 Traces Interactive Visualizer (MOTIV)
 * Copyright (C) 2023 Florian Gallrein, Björn Gehrke
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
#include "MainWindow.hpp"

#include <QApplication>
#include <QCoreApplication>
#include <QErrorMessage>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QToolBar>
#include <QStatusBar>
#include <utility>

#include "src/models/AppSettings.hpp"
#include "src/models/ColorMap.hpp"
#include "src/ui/ColorGenerator.hpp"
#include "src/ui/ColorSynchronizer.hpp"
#include "src/ui/Constants.hpp"
#include "src/ui/widgets/License.hpp"
#include "src/ui/widgets/Help.hpp"
#include "src/ui/widgets/TimeInputField.hpp"
#include "src/ui/widgets/Timeline.hpp"
#include "src/ui/TimeUnit.hpp"
#include "src/ui/windows/FilterPopup.hpp"
#include "src/ui/widgets/About.hpp"
#include "src/ui/widgets/TraceOverviewDock.hpp"
#include "src/ui/widgets/InformationDock.hpp"
#include "src/ui/widgets/infostrategies/InformationDockSlotStrategy.hpp"
#include "src/ui/widgets/infostrategies/InformationDockTraceStrategy.hpp"
#include "src/ui/widgets/infostrategies/InformationDockCommunicationStrategy.hpp"
#include "src/ui/widgets/infostrategies/InformationDockCollectiveCommunicationStrategy.hpp"

extern bool testRun;


ColorSynchronizer* colorsynchronizer = ColorSynchronizer::getInstance();


MainWindow::MainWindow(QString filepath) : QMainWindow(nullptr), filepath(std::move(filepath)) {
    qInfo() << "MainWindow ... " << this;
    if (this->filepath.isEmpty()) {
        this->promptFile();
    }
    this->loadSettings();   
    AppSettings::getInstance().setColorConfigName(this->filepath);
    AppSettings::getInstance().loadColorConfigs();
 
    this->loadTrace();

    this->createToolBars();
    this->createDockWidgets();
    this->createCentralWidget();
    this->createMenus();

    colorsynchronizer->setData(this->data);
}

MainWindow::~MainWindow() {
    delete this->data;
    delete this->callbacks;
    delete this->reader;
    delete this->settings;

    delete this->traceOverview;
    delete this->information;

    delete this->licenseWindow;
    delete this->helpWindow;
    delete this->aboutWindow;
}

void MainWindow::createMenus() {
    qInfo() << "EXECUTING MainWindow::createMenus ... for " << this;
    auto menuBar = this->menuBar();

    /// File menu
    auto openTraceAction = new QAction(tr("&Open..."), this);
    openTraceAction->setShortcut(tr("Ctrl+O"));
    connect(openTraceAction, &QAction::triggered, this, &MainWindow::openNewTrace);
    auto openRecentMenu = new QMenu(tr("&Open recent"));
    if (AppSettings::getInstance().recentlyOpenedFiles().isEmpty()) {
        auto emptyAction = openRecentMenu->addAction(tr("&(Empty)"));
        emptyAction->setEnabled(false);
    } else {
        // TODO this is not updated on call to clear
        for (const auto &recent: AppSettings::getInstance().recentlyOpenedFiles()) {
            auto recentAction = new QAction(recent, openRecentMenu);
            openRecentMenu->addAction(recentAction);
            connect(recentAction, &QAction::triggered, [&, this] {
                this->openNewWindow(recent);
            });
        }
        openRecentMenu->addSeparator();

        auto clearRecentMenuAction = new QAction(tr("&Clear history"));
        openRecentMenu->addAction(clearRecentMenuAction);
        connect(clearRecentMenuAction, &QAction::triggered, [&, openRecentMenu] {
        AppSettings::getInstance().recentlyOpenedFilesClear(this->filepath);
            
        // Make a copy of the list of actions
        QList<QAction*> actions = openRecentMenu->actions();

        for (auto action : actions) {                
            if (action != clearRecentMenuAction && action->text()!= this->filepath) {
                openRecentMenu->removeAction(action);                    
                action->deleteLater();
            }
        }
        });
    }

    auto quitAction = new QAction(tr("&Quit"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    auto fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction(openTraceAction);
    fileMenu->addMenu(openRecentMenu);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    /// View Menu
    auto filterAction = new QAction(tr("&Filter"));
    filterAction->setShortcut(tr("Ctrl+S"));
    connect(filterAction, SIGNAL(triggered()), this, SLOT(openFilterPopup()));

    auto searchAction = new QAction(tr("&Find"));
    searchAction->setShortcut(tr("Ctrl+F"));
    connect(searchAction, SIGNAL(triggered()), this, SLOT(openSearchPopup()));

    auto resetZoomAction = new QAction(tr("&Reset zoom"));
    connect(resetZoomAction, SIGNAL(triggered()), this, SLOT(resetZoom()));
    resetZoomAction->setShortcut(tr("Ctrl+R"));

    auto widgetMenuCustomColors = new QMenu(tr("Custom Colors"));

    auto loadGlobalColorsAction = new QAction(tr("&Load gobal colors"));
    connect(loadGlobalColorsAction, SIGNAL(triggered()),this,SLOT(loadGlobalColors()));

    auto saveAsGlobalColorsAction = new QAction(tr("&Save as gobal colors"));
    connect(saveAsGlobalColorsAction, SIGNAL(triggered()),this,SLOT(saveAsGlobalColors()));

    auto grayFilterAction = new QAction (tr("Grayfilter"));
    connect(grayFilterAction, SIGNAL(triggered()),this,SLOT(grayFilter()));
 
    auto deleteCustomColorsAction = new QAction (tr("&Delete custom colors"));
    connect(deleteCustomColorsAction, SIGNAL(triggered()),this,SLOT(deleteCustomColors()));

    widgetMenuCustomColors->addAction(loadGlobalColorsAction);
    widgetMenuCustomColors->addAction(saveAsGlobalColorsAction);
    widgetMenuCustomColors->addAction(grayFilterAction);
    widgetMenuCustomColors->addAction(deleteCustomColorsAction);

    auto widgetMenuToolWindows = new QMenu(tr("Tool Windows"));

    auto showOverviewAction = new QAction(tr("Show &trace overview"));
    showOverviewAction->setCheckable(true);
    connect(showOverviewAction, SIGNAL(toggled(bool)), this->traceOverview, SLOT(setVisible(bool)));
    connect(this->traceOverview, SIGNAL(visibilityChanged(bool)), showOverviewAction, SLOT(setChecked(bool)));

    auto showDetailsAction = new QAction(tr("Show &detail view"));
    showDetailsAction->setCheckable(true);
    connect(showDetailsAction, SIGNAL(toggled(bool)), this->information, SLOT(setVisible(bool)));
    connect(this->information, SIGNAL(visibilityChanged(bool)), showDetailsAction, SLOT(setChecked(bool)));

    widgetMenuToolWindows->addAction(showOverviewAction);
    widgetMenuToolWindows->addAction(showDetailsAction);

    auto viewMenu = menuBar->addMenu(tr("&View"));
    viewMenu->addAction(filterAction);
    viewMenu->addAction(searchAction);
    viewMenu->addAction(resetZoomAction);   
    viewMenu->addMenu(widgetMenuCustomColors);
    viewMenu->addMenu(widgetMenuToolWindows);

    /// Help menu
    auto showLicenseAction = new QAction(tr("&View license"));
    connect(showLicenseAction, &QAction::triggered, this, [this] {
        if(!this->licenseWindow) this->licenseWindow = new License;
        this->licenseWindow->show();
    });
    auto showHelpAction = new QAction(tr("&Show help"));
    showHelpAction->setShortcut(tr("F1"));
    connect(showHelpAction, &QAction::triggered, this, [this] {
        if(!this->helpWindow) this->helpWindow = new Help;
        this->helpWindow->show();
    });
    auto showAboutQtAction = new QAction(tr("&About Qt"));
    connect(showAboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
    auto showAboutAction = new QAction(tr("&About"));
    showAboutAction->setShortcut(tr("Shift+F1"));
    connect(showAboutAction, &QAction::triggered, this, [this] {
        if(!this->aboutWindow) this->aboutWindow= new About;
        this->aboutWindow->show();
    });

    auto helpMenu = menuBar->addMenu(tr("&Help"));
    helpMenu->addAction(showLicenseAction);
    helpMenu->addAction(showHelpAction);
    helpMenu->addAction(showAboutQtAction);
    helpMenu->addAction(showAboutAction);
}

void MainWindow::createToolBars() {

    qInfo() << "EXECUTING MainWindow::createToolBars ... for " << this;

    // Top toolbar contains preview/control of whole trace
//    this->topToolbar = new QToolBar(this);
//    this->topToolbar->setMovable(false);
//    addToolBar(Qt::TopToolBarArea, this->topToolbar);

    // Bottom toolbar contains control fields
    this->bottomToolbar = new QToolBar(this);
    this->bottomToolbar->setMovable(false);
    //this->bottomToolbar
    this->addToolBar(Qt::BottomToolBarArea, this->bottomToolbar);

    auto bottomContainerWidget = new QWidget(this->bottomToolbar);
    auto containerLayout = new QHBoxLayout(bottomContainerWidget);
    bottomContainerWidget->setLayout(containerLayout);
    this->bottomToolbar->addWidget(bottomContainerWidget);

    // TODO populate with initial time stamps
    this->startTimeInputField = new TimeInputField("Start", TimeUnit::Second, data->getFullTrace()->getStartTime(),
                                                   bottomContainerWidget);
    this->startTimeInputField->setUpdateFunction(
            [this](auto newStartTime) { this->data->setSelectionBegin(newStartTime); });
    containerLayout->addWidget(this->startTimeInputField);
    this->endTimeInputField = new TimeInputField("End", TimeUnit::Second, data->getFullTrace()->getEndTime(),
                                                 bottomContainerWidget);
    this->endTimeInputField->setUpdateFunction([this](auto newEndTime) { this->data->setSelectionEnd(newEndTime); });
    containerLayout->addWidget(this->endTimeInputField);

    connect(data, SIGNAL(beginChanged(types::TraceTime)), this->startTimeInputField, SLOT(setTime(types::TraceTime)));
    connect(data, SIGNAL(endChanged(types::TraceTime)), this->endTimeInputField, SLOT(setTime(types::TraceTime)));

    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setContentsMargins(0,-3,0,-3);
    containerLayout->addWidget(line);
    containerLayout->addSpacing(5);

    // Zoom Buttons
    auto zoomInButton = new QPushButton(tr(""));
    zoomInButton->setIcon(*(this->settings->getInstance()->getIcon("zoom_in")));
    zoomInButton->setIconSize(QSize(32, 32));
    
    auto zoomOutButton = new QPushButton(tr(""));
    zoomOutButton->setIcon(*(this->settings->getInstance()->getIcon("zoom_out")));
    zoomOutButton->setIconSize(QSize(32, 32));

    auto resetZoomButton = new QPushButton(tr(""));
    resetZoomButton->setIcon(*(this->settings->getInstance()->getIcon("zoom_fit")));
    resetZoomButton->setIconSize(QSize(32, 32));
    resetZoomButton->setToolTip("Reset Zoom\nCtrl+R");

    auto searchButton = new QPushButton(tr(""));
    searchButton->setIcon(*(this->settings->getInstance()->getIcon("book")));
    searchButton->setIconSize(QSize(32, 32));
    searchButton->setToolTip("Search\nCtrl+F");
    
    containerLayout->addWidget(zoomInButton);
    containerLayout->addWidget(zoomOutButton);
    containerLayout->addWidget(resetZoomButton);
    containerLayout->addWidget(searchButton);

    connect(zoomInButton, &QPushButton::clicked, this, &MainWindow::verticalZoomIn);
    connect(zoomOutButton, &QPushButton::clicked, this, &MainWindow::verticalZoomOut);
    connect(resetZoomButton, &QPushButton::clicked, this, &MainWindow::resetZoom);
    connect(searchButton, &QPushButton::clicked,this, &MainWindow::openSearchPopup);

    // Refresh Button
    auto refreshButton = new QPushButton(tr(""));
    refreshButton->setIcon(*(this->settings->getInstance()->getIcon("refresh")));
    containerLayout->addWidget(refreshButton);
    refreshButton->setIconSize(QSize(32, 32));

    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshView);
    //connect(data, SIGNAL(labelInteractionTrigger()), this, SLOT(labelInteractionEvent()));
    connect(data, &TraceDataProxy::labelInteractionTrigger, this, &MainWindow::labelInteractionEvent);

    // infoBar
    this->infoBar = new QStatusBar(this);
    QFont font;
    font.setPointSize(10);
    //font.setItalic(true);
    this->infoBar->setFont(font);
    QPalette palette;
    palette.setColor(QPalette::WindowText, QColorConstants::Svg::slategray);
    this->infoBar->setPalette (palette);
    this->infoBar->setFixedHeight(26);
    this->infoBar->showMessage("  "+this->filepath.section("/", -3), 0);
    //this->infoBar->showMessage(this->filepath);
    this->setStatusBar(this->infoBar);
}

void MainWindow::createDockWidgets() {

    qInfo() << "EXECUTING MainWindow::createDockWidgets ... for " << this;

    this->information = new InformationDock();
    information->addElementStrategy(new InformationDockSlotStrategy());
    information->addElementStrategy(new InformationDockTraceStrategy());
    information->addElementStrategy(new InformationDockCommunicationStrategy());
    information->addElementStrategy(new InformationDockCollectiveCommunicationStrategy());

    this->information->setElement(this->data->getFullTrace());
    // @formatter:off
    connect(information, SIGNAL(zoomToWindow(types::TraceTime, types::TraceTime)), data,
            SLOT(setSelection(types::TraceTime, types::TraceTime)));

    connect(data, SIGNAL(infoElementSelected(TimedElement * )), information, SLOT(setElement(TimedElement * )));
    // @formatter:on
    this->addDockWidget(Qt::RightDockWidgetArea, this->information);

    this->traceOverview = new TraceOverviewDock(this->data);
    QPalette palette;
    palette.setColor(QPalette::WindowText, QColorConstants::Svg::slategray);
    this->traceOverview->setPalette (palette);
    QFont font;
    font.setPointSize(10);
    this->traceOverview->setFont(font);
    this->traceOverview->setWindowTitle("  Timeline - Overview");
    this->addDockWidget(Qt::TopDockWidgetArea, this->traceOverview);
}

void MainWindow::createCentralWidget() {
    qInfo() << "EXECUTING MainWindow::createCentralWidget ... for " << this;
    auto timeline = new Timeline(data, this);
    this->setCentralWidget(timeline);
}

QString MainWindow::promptFile() {
    qInfo() << "EXECUTING MainWindow::promptFile ... for " << this;
    auto newFilePath = QFileDialog::getOpenFileName(this, QFileDialog::tr("Open trace"), QString(),
                                                    QFileDialog::tr("OTF Traces (*.otf *.otf2)"));

    // TODO this is still not really a great way to deal with that, especially for the initial open
    if (newFilePath.isEmpty()) {
        auto errorMsg = new QErrorMessage(nullptr);
        errorMsg->showMessage("The chosen file is invalid!");
    }

    return newFilePath;
}

void MainWindow::loadTrace() {

    qInfo() << "EXECUTING MainWindow::loadTrace ... for " << this;

    QElapsedTimer loadTraceTimer;
    if(testRun==true){
        loadTraceTimer.start();
    }

    this->reader = new otf2::reader::reader(this->filepath.toStdString());
    this->callbacks = new ReaderCallbacks(*reader);

    this->reader->set_callback(*callbacks);
    this->reader->read_definitions();
    this->reader->read_events();

    auto slots = this->callbacks->getSlots();
    auto communications = this->callbacks->getCommunications();
    auto collectives = this->callbacks->getCollectiveCommunications();

    std::map<std::string, std::map<otf2::chrono::clock::rep, std::pair<otf2::chrono::clock::rep, std::string>>> fullTimeTableSlots;
    for (auto entry : slots){
        auto rankName = entry->location->location_group().name().get()->str().c_str();
        auto slotName = entry->region->name().get()->str().c_str();
        auto startTime = entry->getStartTime().count();
        auto endTime = entry->getEndTime().count();
        //qInfo() << "mw: " << rankName << slotName << startTime;
        if(fullTimeTableSlots.count(rankName)==0){
            std::map<otf2::chrono::clock::rep, std::pair<otf2::chrono::clock::rep, std::string>> dummyMap{};
            fullTimeTableSlots.insert(std::make_pair(rankName, dummyMap));
        }
        fullTimeTableSlots.at(rankName).insert(std::make_pair(startTime, std::make_pair(endTime, slotName)));
    }

    //qInfo() << "mw ..." << "table size " << fullTimeTableSlots.size();
    this->settings->setFullTimeTableSlots(fullTimeTableSlots);

    auto trace = new FileTrace(slots, communications, collectives, this->callbacks->duration());

    this->data = new TraceDataProxy(trace, this->settings, this);

    qInfo() << "                                        ";
    qInfo() << "--------------TraceDataProxy------------";
    qInfo() << "obj: " << this->data;
    this->data->dumpObjectInfo();
    qInfo() << "----------------------------------------";
    this->data->dumpObjectTree();
    qInfo() << "----------------------------------------";

    if(testRun==true){
        std::cout << "%MainWindow::loadTrace()%" << loadTraceTimer.elapsed() << "%ms%";
    }
}

void MainWindow::loadSettings() {
    qInfo() << "EXECUTING MainWindow::loadSettings ... for " << this;
    this->settings = ViewSettings::getInstance();
}

void MainWindow::resetZoom() {
    qInfo() << "EXECUTING MainWindow::resetZoom ... for " << this;
    data->setSelection(types::TraceTime(0), data->getTotalRuntime());
}

void MainWindow::grayFilter(){

    qInfo() << "EXECUTING MainWindow::grayFilter ... for " << this;

    // Shows a warning message if save as global color is checked
    if(AppSettings::getInstance().getuseGlobalColorConfig()){
        QMessageBox warningBox;
        warningBox.setIcon(QMessageBox::Warning);
        warningBox.setWindowTitle("Saving gray filter globally");
        warningBox.setText("You are about to save the color changes globally. This will affect all traces in the application. \n\nAre you sure you want to continue?");
        warningBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        warningBox.setDefaultButton(QMessageBox::No);
        int choice = warningBox.exec();
        if(choice == QMessageBox::No) return;
    }
    colorsynchronizer->synchronizeColors(colors::COLOR_SLOT_PLAIN);
}

void MainWindow::deleteCustomColors(){
    qInfo() << "EXECUTING MainWindow::deleteCustomColors ... for " << this;
    AppSettings::getInstance().clearColorConfig();
    ColorMap::getInstance()->clearColorMap();
    ColorGenerator::getInstance()->setDefault();
    ColorSynchronizer::getInstance()->reCalculateColors();
}

void MainWindow::loadGlobalColors(){
    qInfo() << "EXECUTING MainWindow::loadGlobalColors ... for " << this;
    AppSettings::getInstance().loadGlobalColors();
    colorsynchronizer->synchronizeColors();
}

void MainWindow::saveAsGlobalColors(){
    qInfo() << "EXECUTING MainWindow::saveAsGlobalColors ... for " << this;
    AppSettings::getInstance().saveAsGlobalColors();
    QMessageBox infoBox;
    infoBox.setIcon(QMessageBox::Information);
    infoBox.setWindowTitle("Global Color Option");
    infoBox.setText("Current colors saved as global");
    infoBox.exec();
}

void MainWindow::openFilterPopup() {
    qInfo() << "EXECUTING MainWindow::openFilterPopup ... for " << this;
    FilterPopup filterPopup(data->getSettings()->getFilter());

    auto connection = connect(&filterPopup, SIGNAL(filterChanged(Filter)), this->data, SLOT(setFilter(Filter)));

    filterPopup.exec();

    disconnect(connection);
}

void MainWindow::openNewTrace() {
    qInfo() << "EXECUTING MainWindow::openNewTrace ... for " << this;
    auto path = this->promptFile();
    this->openNewWindow(path);
}

void MainWindow::openNewWindow(QString path) {
    qInfo() << "EXECUTING MainWindow::openNewWindow ... for " << this;
    QProcess::startDetached(
            QFileInfo(QCoreApplication::applicationFilePath()).absoluteFilePath(),
            QStringList(path));
}

void MainWindow::verticalZoomIn(){
    qInfo() << "EXECUTING MainWindow::verticalZoomIn ... for " << this;
    auto currentRowHeight=this->settings->getRowHeight();
    this->settings->setRowHeight(currentRowHeight+5);
    auto timeline = new Timeline(data, this);
    this->setCentralWidget(timeline);
    Q_EMIT this->data->verticalZoomChanged();    
}

void MainWindow::verticalZoomOut(){
    qInfo() << "EXECUTING MainWindow::verticalZoomOut ... for " << this;
    auto currentRowHeight=this->settings->getRowHeight();
    this->settings->setRowHeight(currentRowHeight-5);
    auto timeline = new Timeline(data, this);
    this->setCentralWidget(timeline);
    Q_EMIT this->data->verticalZoomChanged();    
}

void MainWindow::labelInteractionEvent(){
    qInfo() << "EXECUTING MainWindow::labelInteractionEvent ... for " << this;
    auto currentRowHeight=this->settings->getRowHeight();
    this->settings->setRowHeight(currentRowHeight);
    auto timeline = new Timeline(data, this);
    this->setCentralWidget(timeline);
}

void MainWindow::refreshView(){
    qInfo() << "EXECUTING MainWindow::refreshView ... for " << this;
    Q_EMIT this->data->refreshButtonPressed();
    qInfo() << "                                        ";
    qInfo() << "-------------MainWindow Info------------";
    qInfo() << "obj: " << this;
    this->dumpObjectInfo();
    qInfo() << "----------------------------------------";
    this->dumpObjectTree();
    qInfo() << "----------------------------------------";
}

void MainWindow::openSearchPopup(){
    qInfo() << "EXECUTING MainWindow::openSearchPopup ... for " << this;
    if(this->searchWindow == nullptr) this->searchWindow = new SearchPopup(this->data,this->information, this);
    Q_EMIT this->searchWindow->searchButtonPressed();
}