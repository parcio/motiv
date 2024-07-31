/*
 * Marvelous OTF2 Traces Interactive Visualizer (MOTIV)
 * Copyright (C) 2024 Jessica Lafontaine
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

#include <algorithm>
#include <unordered_set>
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
#include <string>
#include <utility>

#include <QGraphicsScene>
#include <QGraphicsView>

#include "src/models/AppSettings.hpp"
#include "src/ui/widgets/infostrategies/InformationDockSlotStrategy.hpp"
#include "src/ui/widgets/infostrategies/InformationDockTraceStrategy.hpp"
#include "src/ui/widgets/infostrategies/InformationDockCommunicationStrategy.hpp"
#include "src/ui/widgets/infostrategies/InformationDockCollectiveCommunicationStrategy.hpp"
#include "src/ui/windows/MpiAnalysisWindow.hpp"
#include "src/ui/widgets/mpianalysis/LogicalClock.hpp"

#include "src/models/Slot.hpp"
#include <tuple>
#include <bitset>

extern bool testRun;

MpiAnalysisWindow::MpiAnalysisWindow(QString filepath) : QMainWindow(nullptr), filepath(std::move(filepath)){
    if (this->filepath.isEmpty()) {
            this->promptFile();
        }
    
    AppSettings::getInstance().setColorConfigName(this->filepath);
    AppSettings::getInstance().setMode(MPI_Analysis);

    this->createMenus();
    this->loadTrace();
    this->createCentralWidget();
}

MpiAnalysisWindow::~MpiAnalysisWindow() {}


void MpiAnalysisWindow::createMenus(){
    auto menuBar = this->menuBar();

    /// File menu
    auto openTraceAction = new QAction(tr("&Open..."), this);
    openTraceAction->setShortcut(tr("Ctrl+O"));
    connect(openTraceAction, &QAction::triggered, this, &MpiAnalysisWindow::openNewTrace);
    auto openRecentMenu = new QMenu(tr("&Open recent"));
    if (AppSettings::getInstance().recentlyOpenedFiles().isEmpty()) {
        auto emptyAction = openRecentMenu->addAction(tr("&(Empty)"));
        emptyAction->setEnabled(false);
    } else {        
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

    /// Help menu
    auto showLicenseAction = new QAction(tr("&View license"));
    connect(showLicenseAction, &QAction::triggered, this, [this] {
        if(!this->licenseWindow) this->licenseWindow = new License;
        this->licenseWindow->show();
    });
    auto showHelpAction = new QAction(tr("&Show help"));
    showHelpAction->setShortcut(tr("Ctrl+H"));
    connect(showHelpAction, &QAction::triggered, this, [this] {
        if(!this->helpWindow) this->helpWindow = new Help;
        this->helpWindow->show();
    });
    // auto showAboutQtAction = new QAction(tr("&About Qt"));
    // connect(showAboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
    // auto showAboutAction = new QAction(tr("&About"));
    // connect(showAboutAction, &QAction::triggered, this, [this] {
    //     if(!this->aboutWindow) this->aboutWindow= new About;
    //     this->aboutWindow->show();
    // });

    auto helpMenu = menuBar->addMenu(tr("&Help"));
    helpMenu->addAction(showLicenseAction);
    helpMenu->addAction(showHelpAction);
    // helpMenu->addAction(showAboutQtAction);
    // helpMenu->addAction(showAboutAction);
}

void MpiAnalysisWindow::openNewTrace() {
    auto path = this->promptFile();
    this->openNewWindow(path);
}

//Open new Window in MPI Communication mode
void MpiAnalysisWindow::openNewWindow(QString path) {
    QStringList arguments;
    arguments << "-m" << "1" << path;
    QProcess::startDetached(QFileInfo(QCoreApplication::applicationFilePath()).absoluteFilePath(),
                        arguments);
}

void MpiAnalysisWindow::createCentralWidget(){
    QWidget *centralWidget = new QWidget(this);   
    
    auto layout = new QGridLayout(centralWidget);
    

    auto logicalClock = new LogicalClock(this->data, this->nodes, this);
    
    QBrush backgroundPattern = QBrush(QColorConstants::Svg::silver, Qt::Dense7Pattern);
    logicalClock->setBackgroundBrush(backgroundPattern);

    layout->addWidget(logicalClock,0,1);
    
    createInformationWidget();
    layout->addWidget(this->information, 0,2);
    layout->setAlignment(this->information, Qt::AlignRight);
    
    centralWidget->setLayout(layout);
    this->setCentralWidget(centralWidget);

}

void MpiAnalysisWindow::createInformationWidget(){
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
 
}

QString MpiAnalysisWindow::promptFile() {
    auto newFilePath = QFileDialog::getOpenFileName(this, QFileDialog::tr("Open trace"), QString(),
                                                    QFileDialog::tr("OTF Traces (*.otf *.otf2)"));

    // TODO this is still not really a great way to deal with that, especially for the initial open
    if (newFilePath.isEmpty()) {
        auto errorMsg = new QErrorMessage(nullptr);
        errorMsg->showMessage("The chosen file is invalid!");
    }

    return newFilePath;
}

void MpiAnalysisWindow::loadTrace() {

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

    auto trace = new FileTrace(slots, communications, collectives, this->callbacks->duration());

    this->data = new TraceDataProxy(trace, this->settings, this);
    this->buildNodes();

    if(testRun==true){
        std::cout << "%MpiAnalysisWindow::loadTrace()%" << loadTraceTimer.elapsed() << "%ms%";
    }
}

// constexpr size_t IDX_P2P_EVENTS = 0;
// constexpr size_t IDX_COLLECTIVES_EVENTS = 1;

constexpr size_t IDX_NODES = 0;
constexpr size_t IDX_COMMUNICATIONS = 1;

constexpr size_t IDX_BARRIER = 0;
constexpr size_t IDX_COLL_ONE2ALL = 1;
constexpr size_t IDX_ALL2ONE = 2;
constexpr size_t IDX_COLL_ALL2ALL = 3;
constexpr size_t IDX_OTHER = 4;


void MpiAnalysisWindow::buildNodes(){

    // Pending Events: Connects Slots with corresponding CommunicationEvents (P2P or collective communication).
    using pendingP2PEvents =  std::tuple<std::vector<Node*>, std::vector<Communication*>>;    
    using pendingCollectivesEvents =  std::tuple<std::vector<Node*>, std::vector<CollectiveCommunicationEvent*>>;

    std::map<uint16_t, pendingP2PEvents> pendingP2PMap;
    std::map<uint16_t, pendingCollectivesEvents> pendingCollectiveMap;

    // Pending non-blocking Nodes awaiting connection with potential subsequent mpi_wait
    std::map<uint16_t, std::vector<Node*>> pendingNodesForWait;
    
    // Pending Node to connect with matching Node (e.g sender, receiver), key is their CommunicationEvent
    std::map<Communication*, Node*> pendingNodesP2P;
    
    // std::map<CollectiveCommunicationEvent*, Node*> pendingNodesCollectives;
    std::map<CollectiveCommunicationEvent*, std::vector<Node*>> pendingNodesCollectives;

        
    std::unordered_set<otf2::common::role_type> collectiveRoles ={
        otf2::common::role_type::barrier,
        otf2::common::role_type::coll_one2all,
        otf2::common::role_type::coll_all2one,
        otf2::common::role_type::coll_all2all,
        otf2::common::role_type::coll_other,
    };

    for(auto &item: data->getFullTrace()->getSlots()){
        for(auto &slot: item.second){
            Node* node_ = new Node(slot);           
            QString regionName = QString::fromStdString(slot->region->name().str());
            auto DEBUG_REGION_NAME = slot->region->name().str();
            auto roleType = slot->region->role();
            auto location = slot->location->ref(); 


            // Pending non-blocking nodes awaiting upcoming wait operations
            if(regionName.contains("MPI_I") && roleType != otf2::common::role_type::function){
                if(pendingNodesForWait.find(location)==pendingNodesForWait.end()) pendingNodesForWait[location] = std::vector<Node*>();
                pendingNodesForWait[location].push_back(node_);
            }

            if(roleType == otf2::common::role_type::point2point){
                    if (pendingP2PMap.find(location) == pendingP2PMap.end()) pendingP2PMap[location] = pendingP2PEvents();
                    std::get<IDX_NODES>(pendingP2PMap[location]).push_back(node_);

            }else // Pending nodes with collective communications
            if(collectiveRoles.find(roleType) != collectiveRoles.end()){
                if (pendingCollectiveMap.find(location) == pendingCollectiveMap.end()) pendingCollectiveMap[location] = pendingCollectivesEvents();
                std::get<IDX_NODES>(pendingCollectiveMap[location]).push_back(node_);
            }else 
            
            // Connecting wait-nodes with operation nodes
            if(regionName.contains("wait", Qt::CaseInsensitive)){
                if(pendingNodesForWait.find(location)==pendingNodesForWait.end() || pendingNodesForWait[location].empty()) {
                    throw std::runtime_error("Attempted to wait for a non-blocking operation at location " + std::to_string(location) + ", but no preceding non-blocking operation was initiated.");
                }
                if(regionName=="MPI_Wait"){
                    node_->addConnectedNode(pendingNodesForWait[location].front());
                    pendingNodesForWait[location].erase(pendingNodesForWait[location].begin());
                }else{
                    node_->addConnectedNode(location, pendingNodesForWait[location]);
                    pendingNodesForWait.erase(location);
                }
            }

            this->nodes[location].push_back(node_);
        }
            
    }
    
    //Pending communications
    for(auto &item: data->getFullTrace()->getCommunications()){
        const auto startEvent = item->getStartEvent();
        const auto endEvent = item->getEndEvent();
        auto startLoc = startEvent->getLocation()->ref();
        auto endLoc = endEvent->getLocation()->ref();

        if (pendingP2PMap.find(startLoc) == pendingP2PMap.end()) pendingP2PMap[startLoc] = pendingP2PEvents();
        if (pendingP2PMap.find(endLoc) == pendingP2PMap.end()) pendingP2PMap[endLoc] = pendingP2PEvents();

        // if(startLoc != endLoc){           
        std::get<IDX_COMMUNICATIONS>(pendingP2PMap[startLoc]).push_back(item);
        std::get<IDX_COMMUNICATIONS>(pendingP2PMap[endLoc]).push_back(item);
        // } else std::get<1>(pendingMap[startLoc]).push_back(item);
    }

    // Resort P2P communication by rank
    for (auto &item : pendingP2PMap) {
        auto locationKey = item.first;
        auto &communications = std::get<IDX_COMMUNICATIONS>(item.second);
        std::sort(communications.begin(), communications.end(), 
                  [this, locationKey](Communication* a, Communication* b) {
                      return this->customP2PCommunicationCompare(a, b, locationKey);
                  });
    }

    //Pending collective communications
    for(auto &item: data->getFullTrace()->getCollectiveCommunications()){
        if(getTypeIndex(item->getOperation()) == IDX_OTHER) continue;          
        for(auto &member: item->getMembers()){
            auto memberLocation = member->getLocation()->ref();
            if(pendingCollectiveMap.find(memberLocation) == pendingCollectiveMap.end()) pendingCollectiveMap[memberLocation] = pendingCollectivesEvents();
            std::get<IDX_COMMUNICATIONS>(pendingCollectiveMap[memberLocation]).push_back(item);
        }
    }
    
    //Adds communication to matching Node
    for (auto &locationEntry : pendingP2PMap) {     
        auto &communications = std::get<IDX_COMMUNICATIONS>(locationEntry.second);
        auto &nodes_ = std::get<IDX_NODES>(locationEntry.second);

        bool sizeMatchP2P = (nodes_.size()==communications.size());

        for (auto node : nodes_){
            auto slot = node->getSlot();
            // QString regionName = QString::fromStdString(slot->region->name().str());     

            if(!communications.empty()){
                // Less communication events than nodes, some communications seem unfinished.
                if(!sizeMatchP2P){                   
                    if(communications.front()->getStartEvent()->getLocation()->ref() == node->getLocation()){
                         if(!timeMatching(communications.front()->getStartEvent(), *slot)) continue;
                    } else if(!timeMatching(communications.front()->getEndEvent(), *slot)) continue;
                }                
                node->setCommunication(communications.front());

                // Add connected Nodes
                if(pendingNodesP2P.find(communications.front()) == pendingNodesP2P.end()) pendingNodesP2P[communications.front()] = node;
                else {
                    node->addConnectedNode(pendingNodesP2P[communications.front()]);
                    pendingNodesP2P[communications.front()]->addConnectedNode(node);
                }
                communications.erase(communications.begin());
                if(QString::fromStdString(node->getSlot()->region->name().str()).contains("sendrecv",Qt::CaseInsensitive)) communications.erase(communications.begin());
            }
        }
    
    }

    //Adds collective communication to matching Node
    for (auto &locationEntry : pendingCollectiveMap){
        for (auto node : std::get<IDX_NODES>(locationEntry.second)) {        
            auto &collectives = std::get<IDX_COMMUNICATIONS>(locationEntry.second); 

            if(!collectives.empty()){
                node->setCollectiveCommunication(collectives.front());
                auto event = collectives.front();
                // Pending member for connectedNodes
                if(pendingNodesCollectives.find(event) == pendingNodesCollectives.end()) pendingNodesCollectives[event] = std::vector<Node*>();             
                pendingNodesCollectives[event].push_back(node);               
                collectives.erase(collectives.begin());
            }
        }
    }

    // Add connectedNodes
    for(auto &collectiveCommunication : pendingNodesCollectives) {
        std::vector<Node*>& nodes = collectiveCommunication.second;
        for(auto node : nodes){
            node->addConnectedNode(nodes);
        }
    }
}

bool MpiAnalysisWindow::customP2PCommunicationCompare(Communication* a, Communication* b, uint64_t locationKey) {
    auto startEventA = a->getStartEvent();
    auto startEventB = b->getStartEvent();
    
    auto EndEventA = a->getEndEvent();
    auto EndEventB = b->getEndEvent();
    
    bool isStartEventAMatching = startEventA->getLocation()->ref() == locationKey;
    bool isStartEventBMatching = startEventB->getLocation()->ref() == locationKey;

    auto timeA = isStartEventAMatching ? startEventA->getStartTime() : EndEventA->getStartTime();
    auto timeB = isStartEventBMatching ? startEventB->getStartTime() : EndEventB->getStartTime();

    return timeA < timeB;
}

bool MpiAnalysisWindow::timeMatching(const CommunicationEvent* event, Slot slot){
    auto startTime = event->getStartTime();
    auto endTime = event->getEndTime();

    auto differenceStart = abs(event->getStartTime() - slot.startTime);
    auto differenceEnd = abs(event->getEndTime() - slot.endTime);
    std::chrono::nanoseconds threshold(500000);
    if(differenceStart < threshold || differenceEnd < threshold) return true;
    else return false;
}

size_t MpiAnalysisWindow::getTypeIndex(otf2::common::collective_type collectiveType){
    switch(collectiveType){
        case otf2::common::collective_type::barrier:
            return IDX_BARRIER;
        case otf2::common::collective_type::broadcast:
        case otf2::common::collective_type::scatter:
        case otf2::common::collective_type::scatterv:
            return IDX_COLL_ONE2ALL;
        case otf2::common::collective_type::gather:
        case otf2::common::collective_type::gatherv:
        case otf2::common::collective_type::reduce:
            return IDX_ALL2ONE;
        case otf2::common::collective_type::all_gather:
        case otf2::common::collective_type::all_gatherv:
        case otf2::common::collective_type::all_to_all:
        case otf2::common::collective_type::all_to_allv:
        case otf2::common::collective_type::all_to_allw:
        case otf2::common::collective_type::all_reduce:
        case otf2::common::collective_type::reduce_scatter:
        case otf2::common::collective_type::reduce_scatter_block:
        case otf2::common::collective_type::scan:
        case otf2::common::collective_type::exscan:
            return IDX_COLL_ALL2ALL;
        case otf2::common::collective_type::create_handle:
        case otf2::common::collective_type::destroy_handle:
        case otf2::common::collective_type::allocate:
        case otf2::common::collective_type::deallocate:
        case otf2::common::collective_type::create_handle_and_allocate:
        case otf2::common::collective_type::destroy_handle_and_deallocate:
            return IDX_OTHER;
        default:
            return IDX_OTHER;
    } 
}

size_t MpiAnalysisWindow::getTypeIndex(otf2::common::role_type roleType){
      switch(roleType){
        case otf2::common::role_type::barrier: 
            return IDX_BARRIER;
        case otf2::common::role_type::coll_one2all:
            return IDX_COLL_ONE2ALL;
        case otf2::common::role_type::coll_all2one:
            return IDX_ALL2ONE;
        case otf2::common::role_type::coll_all2all:
            return IDX_COLL_ALL2ALL;
        default:
            return IDX_OTHER;
    }  
}