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
#include "Timeline.hpp"
#include "src/ui/windows/FlamegraphPopup.hpp"
#include "src/ui/ScrollSynchronizer.hpp"

#include <QGridLayout>


Timeline::Timeline(TraceDataProxy *data, QWidget *parent) : QWidget(parent), data(data) {
    //qInfo() << "Timeline ... " << this;
    this->data->triggerUITimerStartIfPossible();
    auto layout = new QGridLayout(this);

    this->header = new TimelineHeader(this->data, this);
    layout->addWidget(this->header, 0, 1);

    this->labelList = new TimelineLabelList(this->data, this);
    layout->addWidget(this->labelList, 1, 0);
    
    connect(this->data, SIGNAL(flamegraphRequest()), this, SLOT(showFlamegraphPopup()));

    // We don't want to have the context menu for layout elements in the label region
    this->labelList->setContextMenuPolicy(Qt::PreventContextMenu);

    this->view = new TimelineView(this->data, this);
    QBrush backgroundPattern = QBrush(QColorConstants::Svg::silver, Qt::Dense7Pattern);
    this->view->setBackgroundBrush(backgroundPattern);
    layout->addWidget(this->view, 1, 1);

    // This prevents the labelList from expanding to 50% of the width.
    // Not really a satisfactory solution.
    //layout->setColumnStretch(0, 1);
    //layout->setColumnStretch(1, 9);

    // We don't need more space than the max width of the contents (32+16 pixel for the +/- icons)
    this->labelList->setMaximumWidth(this->labelList->getMaxLabelLength()+48);
    //qInfo() << "width: " << this->labelList->getMaxLabelLength();
    // "MPI rank 5" -> "... rank 5" makes more sense than "MPI rank ..."
    this->labelList->setTextElideMode (Qt::ElideLeft);

    auto scrollSyncer = new ScrollSynchronizer(this);
    scrollSyncer->addWidget(this->labelList);
    scrollSyncer->addWidget(this->view);

    //qInfo() << "-------------Timeline Info--------------";
    //qInfo() << "obj: " << this;
    //this->dumpObjectInfo();
    //qInfo() << "----------------------------------------";
    //this->dumpObjectTree();
    //qInfo() << "----------------------------------------";
}

void Timeline::showFlamegraphPopup(){
    //qInfo() << "EXECUTING Timeline::showFlamegraphPopup ... for " << this;
    FlamegraphPopup* flamegraph = new FlamegraphPopup(this->data, this);
    flamegraph->openFlamegraphWindow();
}