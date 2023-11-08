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
    useRealWidthMainWindow->setChecked(settings->getUseRealWidthMainWindow());

    auto mainWindowGeneralSettingsLayout = new QVBoxLayout();
    mainWindowGeneralSettingsLayout->addWidget(useRealWidthMainWindow);

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
    usePriorityOverview = new QCheckBox(tr("Use priority levels to elevate functions/regions"));
    usePriorityOverview->setChecked(settings->getUsePriorityOverview());

    auto overviewSettingsLayout = new QVBoxLayout();
    overviewSettingsLayout->addWidget(useBorderOverview);
    overviewSettingsLayout->addWidget(usePriorityOverview);
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
    settings->setCountIndicatorsREG(this->countIndicatorsREG->isChecked());
    settings->setCountIndicatorsP2P(this->countIndicatorsP2P->isChecked());
    settings->setCountIndicatorsCCM(this->countIndicatorsCCM->isChecked());

    // Updates for flamegraphs
    settings->setUseRealWidthFlamegraph(this->useRealWidthFlamegraph->isChecked());
    settings->setCountIndicatorDetailsFlamegraph(this->countIndicatorDetailsFlamegraph->isChecked());
    settings->setPxThresholdFlamegraph(this->useThresholdFlamegraph->isChecked());

    // Updates for overview
    settings->setUseBorderOverview(this->useBorderOverview->isChecked());
    settings->setUsePriorityOverview(this->usePriorityOverview->isChecked());

    // Misc updates
    settings->setColorCodingTimeRecords(this->colorCodingTimeRecords->isChecked());
    
    Q_EMIT this->data->refreshButtonPressed();
}