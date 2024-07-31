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

#ifndef MOTIV_MPIANALYSISWINDOW_HPP
#define MOTIV_MPIANALYSISWINDOW_HPP

#include <QMainWindow>
#include <map>


#include "src/ReaderCallbacks.hpp"
#include "src/models/communication/CommunicationEvent.hpp"
#include "src/models/Slot.hpp"
#include "src/models/mpianalysis/Node.hpp"
#include "src/ui/TraceDataProxy.hpp"
#include "src/ui/widgets/Help.hpp"
#include "src/ui/widgets/InformationDock.hpp"
#include "src/ui/widgets/License.hpp"



//#include "src/ui/widgets/About.hpp"

class MpiAnalysisWindow : public QMainWindow {
    Q_OBJECT

public: // constructors & timer for the UI
    /**
     * @brief Creates a new instance of the MpiAnalysisWindow class.
     *
     * @param filepath Path to trace file. If omitted the user is promted for it.
     */
    explicit MpiAnalysisWindow(QString filepath = QString());
    ~MpiAnalysisWindow() override;

public Q_SLOTS:
    /**
     * @brief Asks for a new trace file and opens the trace
     */
    void openNewTrace();

private: // methods
    void createMenus();
    void createCentralWidget();
    void createInformationWidget();
    void openNewWindow(QString path);

    void loadTrace();
    void buildNodes();

    bool customP2PCommunicationCompare(Communication* a, Communication* b, uint64_t locationKey);
    bool timeMatching(const CommunicationEvent* event, Slot slot);
    size_t getTypeIndex(otf2::common::collective_type collectiveType);
    size_t getTypeIndex(otf2::common::role_type roleType);

    QString promptFile();

   //void showInfo();

private: // widgets
    // QStatusBar *infoBar = nullptr;
    // QLabel *infoLabel = nullptr;
    InformationDock *information = nullptr;
   
    License *licenseWindow = nullptr;
    Help *helpWindow = nullptr;
    //About *aboutWindow = nullptr;

private: // properties
    QString filepath;
    TraceDataProxy *data = nullptr;    
    std::map<uint64_t, std::vector<Node*>> nodes;

    otf2::reader::reader *reader = nullptr;  
    ReaderCallbacks *callbacks = nullptr;

    ViewSettings *settings = nullptr;
};

#endif //MOTIV_MPIANALYSISWINDOW_HPP