/*
 * Marvelous OTF2 Traces Interactive Visualizer (MOTIV)
 * Copyright (C) 2023 Florian Gallrein, Björn Gehrke, Jessica Lafontaine
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
#include "TimeUnitLabel.hpp"
#include "src/models/AppSettings.hpp"
#include "src/ui/ColorSynchronizer.hpp"
#include "src/utils.hpp"
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include "InformationDock.hpp"



InformationDock::InformationDock(QWidget *parent) : QDockWidget(parent) {
    setWindowTitle(tr("Details"));
}

InformationDock::~InformationDock() {
    delete this->element_;
    for(auto &item : this->strategies_) {
        delete item.first;
        //delete item.second;
    }
}

void InformationDock::updateView() {
    if(!element_) return;

    for (auto &item: strategies_) {
        auto widget = item.first;
        auto strategy = item.second;
        if(strategy->update(static_cast<QFormLayout *>(widget->layout()), element_)) {
            setWidget(widget);
            setWindowTitle("Details - " + QString::fromStdString(strategy->title()));
            break;
        }
    }
}

void InformationDock::zoomIntoViewPressed() {
    if(!element_) return;

    auto padding = element_->getDuration() / 10;

    Q_EMIT zoomToWindow(element_->getStartTime() - padding, element_->getEndTime() + padding);
}

void InformationDock::setCustomColorPressed() {
    if(!element_) return;
    auto colorPicker = new ColorPicker();    
    auto color = colorPicker->selectColor();
    if (!color.isValid()) return;
    else
    {
        auto slot = dynamic_cast<Slot*>(element_);
        AppSettings::getInstance().colorConfigPush(QString::fromStdString(slot->region->name().str()),color);
        ColorSynchronizer::getInstance()->synchronizeColors(slot->region->name().str(), color);
    }
}

void InformationDock::setElement(TimedElement* element) {  
    element_ = element;
    updateView();
    Q_EMIT slotSelected(dynamic_cast<Slot*>(element) != nullptr);
}

void InformationDock::addElementStrategy(InformationDockElementStrategy* s) {
    auto widget = new QWidget();
    auto layout = new QFormLayout(widget);
    layout->setAlignment(Qt::AlignLeading | Qt::AlignTop | Qt::AlignLeft );

    s->setup(layout);

    auto gridWidget = new QWidget(); 
    auto gridLayout = new QGridLayout(gridWidget);
    gridLayout->setAlignment(Qt::AlignLeft);

    auto zoomIntoViewButton = new QPushButton(tr("Zoom into &view"));
    connect(zoomIntoViewButton, SIGNAL(clicked()), this, SLOT(zoomIntoViewPressed()));
    gridLayout->addWidget(zoomIntoViewButton,0,0);

    auto customColorButton = new QPushButton(tr("Set custom &color"));   
    connect(customColorButton, SIGNAL(clicked()), this, SLOT(setCustomColorPressed()));
    connect(this, SIGNAL(slotSelected(bool)), customColorButton, SLOT(setEnabled(bool)));
    connect(this, &InformationDock::slotSelected, this, [this, customColorButton](bool selected) {
        if (selected) customColorButton->setStyleSheet("");        
        else customColorButton->setStyleSheet("background-color: #d3d3d3;");
    });
    gridLayout->addWidget(customColorButton,1,0);

    auto globalCheckBox = new QCheckBox("global");
    connect(globalCheckBox, &QCheckBox::stateChanged, this, [this](int state) {
    Q_EMIT globalColorChanged(state == Qt::Checked);
    });
    connect(this, &InformationDock::globalColorChanged, &AppSettings::getInstance(), &AppSettings::toggleGlobalColorConfig);
    gridLayout->addWidget(globalCheckBox,1,1);
    
    auto infoLabel = new QLabel(tr("ⓘ"));
    infoLabel->setToolTip(tr("Check the box to store the colors in GlobalColors.conf"));
    connect(infoLabel, SIGNAL(hovered()), infoLabel, SLOT(showToolTip()));
    connect(infoLabel, SIGNAL(unhovered()), infoLabel, SLOT(hideToolTip()));
    gridLayout->addWidget(infoLabel,1,2);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    gridLayout->setAlignment(Qt::AlignLeft);

    layout->addRow(gridWidget);
    
    widget->setLayout(layout);

    strategies_.emplace_back(widget, s);
}
