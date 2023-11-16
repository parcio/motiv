/*
 * Marvelous OTF2 Traces Interactive Visualizer (MOTIV)
 * Copyright (C) 2023 Tomas Cirkov
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


#include "src/ui/windows/SettingsPopup.hpp"

#include <QGridLayout>
#include <QRadioButton>
#include <QPushButton>
#include <qdialog.h>

SettingsPopup::SettingsPopup(TraceDataProxy *data, QWidget *parent):QDialog(parent), data(data){

    this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    this->setWindowTitle(tr("Settings"));
    this->setModal(true);

    auto settings = ViewSettings::getInstance();

    auto *grid = new QGridLayout();

    // Show settings for the main window scenery (rendered via TimelineView)
    
    // General - start
    auto mainWindowGeneralSettings = new QGroupBox(tr("Main window - general"));
    useRealWidthMainWindow = new QCheckBox(tr("Use real width for regions/functions"));
    useRealWidthMainWindow->setToolTip("If this option is unchecked you will have:\n\t* generally easier navigation, since functions are then limited in their minimum size and thus can't get extremly tiny\n\t* visible function collisions, especially if the option below (use of priority levels) is also unchecked");
    useRealWidthMainWindow->setChecked(settings->getUseRealWidthMainWindow());
    usePriorityOverview = new QCheckBox(tr("Use priority levels to elevate functions/regions"));
    usePriorityOverview->setToolTip("If this option is unchecked functions will have their priority based on their length only: priority <=> 1/duration, which means smaller duration <=> higher priority!");
    usePriorityOverview->setChecked(settings->getUsePriorityOverview());
    absoluteDurationsForSliders = new QCheckBox(tr("Use absolute durations for active threshold sliders"));
    absoluteDurationsForSliders->setToolTip("For example: assuming that a function runs for 5 min but our current selection only shows the first half,\nis this function visible with an active threshold of 3 min?\n\n\t* IF this checkbox is checked we will compare 5min (absolute duration) < 3min, which is false => the function will be invisible!\n\n\t* Otherwise we will compare 2.5min (drawn duration) < 3min, which is true => it will be visible!");
    absoluteDurationsForSliders->setChecked(settings->getAbsoluteDurationsForSliders());
    useREGSliderForOV = new QCheckBox(tr("Use the active threshold slider for regions/functions for the overview"));
    useREGSliderForOV->setToolTip("This allows a more synchronized relationship between overview and selection, altough there are possible downsides:\nThe filter process via the OV-slider doesn't depend on the selection width, with this option checked the overview might change drastically with zoom or selection events!\n\nHint: it's possible to move through the trace with a constant selection width via Shift+Scrolling");
    useREGSliderForOV->setChecked(settings->getUseREGSliderForOV());

    auto mainWindowGeneralSettingsLayout = new QVBoxLayout();
    mainWindowGeneralSettingsLayout->addWidget(useRealWidthMainWindow);
    mainWindowGeneralSettingsLayout->addWidget(usePriorityOverview);
    mainWindowGeneralSettingsLayout->addWidget(absoluteDurationsForSliders);
    mainWindowGeneralSettingsLayout->addWidget(useREGSliderForOV);
    

    mainWindowGeneralSettings->setLayout(mainWindowGeneralSettingsLayout);
    // General - end
    
    // Performance - start
    auto mainWindowPerformanceSettings = new QGroupBox(tr("Main window - performance"));
    countIndicatorsREG = new QCheckBox(tr("Count all traced regions/functions: <#drawn> / <#all> [REG]"));
    countIndicatorsREG->setChecked(settings->getCountIndicatorsREG());
    countIndicatorsP2P = new QCheckBox(tr("Count all traced point-to-point comm.: <#drawn> / <#all> [P2P]"));
    countIndicatorsP2P->setChecked(settings->getCountIndicatorsP2P());
    countIndicatorsCCM = new QCheckBox(tr("Count all traced collective comm. <#drawn> / <#all> [CCM]"));
    countIndicatorsCCM->setChecked(settings->getCountIndicatorsCCM());

    auto mainWindowPerformanceSettingsLayout = new QVBoxLayout();
    mainWindowPerformanceSettingsLayout->addWidget(countIndicatorsREG);
    mainWindowPerformanceSettingsLayout->addWidget(countIndicatorsP2P);
    mainWindowPerformanceSettingsLayout->addWidget(countIndicatorsCCM);

    mainWindowPerformanceSettings->setLayout(mainWindowPerformanceSettingsLayout);
    // Performance - end


    // Show settings for flamegraphs (rendered via FlamegraphView)
    
    // General - start
    auto flamegraphWindowGeneralSettings = new QGroupBox(tr("Flamegraphs - general"));
    useRealWidthFlamegraph = new QCheckBox(tr("Use real width for regions/functions"));
    useRealWidthFlamegraph->setToolTip("Unchecking this option is not recommended: while it appears to be a similar trade-off like the one in the main window (easier navigation for possible collisions), \nwe actually might have even worse navigation, because the collisions can cause false call hierarchies!");
    useRealWidthFlamegraph->setChecked(settings->getUseRealWidthFlamegraph());

    auto flamegraphWindowGeneralSettingsLayout = new QVBoxLayout();
    flamegraphWindowGeneralSettingsLayout->addWidget(useRealWidthFlamegraph);

    flamegraphWindowGeneralSettings->setLayout(flamegraphWindowGeneralSettingsLayout);
    // General - end
    
    // Performance - start
    auto flamegraphWindowPerformanceSettings = new QGroupBox(tr("Flamegraphs - performance"));
    countIndicatorDetailsFlamegraph = new QCheckBox(tr("Show missing regions/functions-traces by name"));
    countIndicatorDetailsFlamegraph->setChecked(settings->getCountIndicatorDetailsFlamegraph());
    useThresholdFlamegraph = new QCheckBox(tr("Use 1px-threshold for drawing"));
    useThresholdFlamegraph->setToolTip("Unchecking this option is not recommended: to stack functions that are very close to each other and smaller than this can cause false hierarchies");
    useThresholdFlamegraph->setChecked(settings->getPxThresholdFlamegraph());

    auto flamegraphWindowPerformanceSettingsLayout = new QVBoxLayout();
    flamegraphWindowPerformanceSettingsLayout->addWidget(countIndicatorDetailsFlamegraph);
    flamegraphWindowPerformanceSettingsLayout->addWidget(useThresholdFlamegraph);
    
    flamegraphWindowPerformanceSettings->setLayout(flamegraphWindowPerformanceSettingsLayout);
    // Performance - end


    // Overview options
    auto overviewSettings = new QGroupBox(tr("Overview"));
    useBorderOverview = new QCheckBox(tr("Draw borders for functions/regions"));
    useBorderOverview->setChecked(settings->getUseBorderOverview());

    auto overviewSettingsLayout = new QVBoxLayout();
    overviewSettingsLayout->addWidget(useBorderOverview);
    overviewSettingsLayout->addStretch(1);
    
    overviewSettings->setLayout(overviewSettingsLayout);


    // Misc options
    auto miscSettings = new QGroupBox(tr("Misc"));
    colorCodingTimeRecords = new QCheckBox(tr("Colorize time records for <=25/50/100/250[ms]"));
    colorCodingTimeRecords->setChecked(settings->getColorCodingTimeRecords());

    auto miscSettingsLayout = new QVBoxLayout();
    miscSettingsLayout->addWidget(colorCodingTimeRecords);
    miscSettingsLayout->addStretch(1);
    
    miscSettings->setLayout(miscSettingsLayout);


    // These share their row
    grid->addWidget(mainWindowGeneralSettings, 0, 0);
    grid->addWidget(flamegraphWindowGeneralSettings, 0, 1);
    grid->addWidget(mainWindowPerformanceSettings, 1, 0);
    grid->addWidget(flamegraphWindowPerformanceSettings, 1, 1);
    // These don't share their row <=> addStretch(1)
    grid->addWidget(overviewSettings, 2, 0);
    grid->addWidget(miscSettings, 3, 0);

    // Buttons
    auto okButton = new QPushButton(tr("&Ok"));
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    auto cancelButton = new QPushButton(tr("&Cancel"));
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    okButton->setDefault(true);

    grid->addWidget(cancelButton, 4, 0, Qt::AlignLeft);
    grid->addWidget(okButton, 4, 1, Qt::AlignRight);

    connect(this, &QDialog::accepted, this, &SettingsPopup::checkSettings);

    // Finish
    this->setLayout(grid);
}

void SettingsPopup::checkSettings() {
    auto settings = ViewSettings::getInstance();

    // Updates for main window settings
    settings->setUseRealWidthMainWindow(this->useRealWidthMainWindow->isChecked());
    settings->setUsePriorityOverview(this->usePriorityOverview->isChecked());
    settings->setAbsoluteDurationsForSliders(this->absoluteDurationsForSliders->isChecked());
    settings->setUseREGSliderForOV(this->useREGSliderForOV->isChecked());
    settings->setCountIndicatorsREG(this->countIndicatorsREG->isChecked());
    settings->setCountIndicatorsP2P(this->countIndicatorsP2P->isChecked());
    settings->setCountIndicatorsCCM(this->countIndicatorsCCM->isChecked());

    // Updates for flamegraphs
    settings->setUseRealWidthFlamegraph(this->useRealWidthFlamegraph->isChecked());
    settings->setCountIndicatorDetailsFlamegraph(this->countIndicatorDetailsFlamegraph->isChecked());
    settings->setPxThresholdFlamegraph(this->useThresholdFlamegraph->isChecked());

    // Updates for overview
    settings->setUseBorderOverview(this->useBorderOverview->isChecked());

    // Misc updates
    settings->setColorCodingTimeRecords(this->colorCodingTimeRecords->isChecked());
    
    Q_EMIT this->data->refreshButtonPressed();
}