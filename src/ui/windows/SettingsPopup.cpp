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

    // Show performance settings for the main window scenery (rendered via TimelineView)
    auto mainWindowSettings = new QGroupBox(tr("Main window"));
    countIndicatorsREG = new QCheckBox(tr("Count all traced regions/functions: <#drawn> / <#all> [REG]"));
    countIndicatorsREG->setChecked(settings->getCountIndicatorsREG());
    countIndicatorsP2P = new QCheckBox(tr("Count all traced point-to-point comm.: <#drawn> / <#all> [P2P]"));
    countIndicatorsP2P->setChecked(settings->getCountIndicatorsP2P());
    countIndicatorsCCM = new QCheckBox(tr("Count all traced collective comm. <#drawn> / <#all> [CCM]"));
    countIndicatorsCCM->setChecked(settings->getCountIndicatorsCCM());
    useThresholdTimelineView = new QCheckBox(tr("Use 1px-threshold for drawing"));
    useThresholdTimelineView->setChecked(settings->getPxThresholdTimelineView());

    auto mainWindowSettingsLayout = new QVBoxLayout();
    mainWindowSettingsLayout->addWidget(countIndicatorsREG);
    mainWindowSettingsLayout->addWidget(countIndicatorsP2P);
    mainWindowSettingsLayout->addWidget(countIndicatorsCCM);
    mainWindowSettingsLayout->addWidget(useThresholdTimelineView);


    // Show performance settings for flamegraphs (rendered via FlamegraphView)
    auto flamegraphWindowSettings = new QGroupBox(tr("Flamegraphs"));
    countIndicatorDetailsFlamegraph = new QCheckBox(tr("Show missing regions/functions-traces by name"));
    countIndicatorDetailsFlamegraph->setChecked(settings->getCountIndicatorDetailsFlamegraph());
    useThresholdFlamegraph = new QCheckBox(tr("Use 1px-threshold for drawing"));
    useThresholdFlamegraph->setChecked(settings->getPxThresholdFlamegraph());

    auto flamegraphWindowSettingsLayout = new QVBoxLayout();
    flamegraphWindowSettingsLayout->addWidget(countIndicatorDetailsFlamegraph);
    flamegraphWindowSettingsLayout->addWidget(useThresholdFlamegraph);


    //mainWindowSettingsLayout->addStretch(1);
    mainWindowSettings->setLayout(mainWindowSettingsLayout);
    flamegraphWindowSettings->setLayout(flamegraphWindowSettingsLayout);

    grid->addWidget(mainWindowSettings, 0, 0);
    grid->addWidget(flamegraphWindowSettings, 0, 1);

    auto okButton = new QPushButton(tr("&Ok"));
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

    auto cancelButton = new QPushButton(tr("&Cancel"));
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    okButton->setDefault(true);

    grid->addWidget(cancelButton, 1, 0, Qt::AlignLeft);
    grid->addWidget(okButton, 1, 1, Qt::AlignRight);

    connect(this, &QDialog::accepted, this, &SettingsPopup::checkSettings);

    this->setLayout(grid);
}

void SettingsPopup::checkSettings() {
    auto settings = ViewSettings::getInstance();

    // Updates for main window settings
    settings->setCountIndicatorsREG(this->countIndicatorsREG->isChecked());
    settings->setCountIndicatorsP2P(this->countIndicatorsP2P->isChecked());
    settings->setCountIndicatorsCCM(this->countIndicatorsCCM->isChecked());
    settings->setPxThresholdTimelineView(this->useThresholdTimelineView->isChecked());

    // Updates for flamegraphs
    settings->setCountIndicatorDetailsFlamegraph(this->countIndicatorDetailsFlamegraph->isChecked());
    settings->setPxThresholdFlamegraph(this->useThresholdFlamegraph->isChecked());
    
    qInfo() << "settings set! via dedicated window :) " << this;
    Q_EMIT this->data->refreshButtonPressed();
}