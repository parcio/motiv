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
#include "TimelineLabelList.hpp"
#include "src/models/Trace.hpp"
#include "src/models/ViewSettings.hpp"
#include "src/ui/TraceDataProxy.hpp"

#include <QLabel>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <set>
#include <vector>

#include <QMenu>

TimelineLabelList::TimelineLabelList(TraceDataProxy *data, QWidget *parent) : QListWidget(parent), data(data) {
    //qInfo() << "TimelineLabelList... " << this;
    this->setFrameShape(QFrame::NoFrame);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //this->setSelectionMode(QAbstractItemView::NoSelection);

    this->menu = new QMenu(this);
    this->menu->setStyleSheet("QMenu { background-color: rgba(250, 250, 250, 180); border: 2px solid rgba(170, 170, 170, 120); } QMenu::item:selected { background-color: rgba(250, 250, 250, 230); color: black; } ");
    this->labelAction1 = this->menu->addAction("highlight process");
    this->labelAction1->setDisabled(true);
    this->labelAction2 = this->menu->addAction("toggle arrows (P2P)");
    this->labelAction3 = this->menu->addAction("show flamegraph");
    connect(this->labelAction1, &QAction::triggered, this, &TimelineLabelList::highlightPreparation);
    connect(this->labelAction2, &QAction::triggered, this, &TimelineLabelList::togglePointToPointPreparation);
    connect(this->labelAction3, &QAction::triggered, this, &TimelineLabelList::flamegraphPreparation);
    connect(this->data, &TraceDataProxy::verticalZoomChanged, this, &TimelineLabelList::resizeLabels);

    // This prevents asynchronous scrolling relative to TimelineView
    this->setVerticalScrollMode(ScrollPerPixel);
    this->setStyleSheet("background: transparent");
    // Why top=20? <= top in TimelineView.cpp
    //setViewportMargins(0, 20, 0, 0);
    // Alternative with a bufferzone we can scroll into
    auto extraSpaceTop = new QListWidgetItem(this);
    // We dont want the extra space to be selectable etc.
    extraSpaceTop->setFlags(Qt::NoItemFlags);
    extraSpaceTop->setSizeHint(QSize(0, 20));
    this->addItem(extraSpaceTop);
 

    for (const auto &rank: this->data->getSelection()->getSlots()) {
        const QString &rankName = QString::fromStdString(rank.first->name().str());
        auto rankThreadMap = ViewSettings::getInstance()->getRankThreadMap();
        auto rankAdrMap = ViewSettings::getInstance()->getRankAdrMap();
        //auto plusIcon = ViewSettings::getInstance()->getIcon("plus");
        //auto plusIconGrey = ViewSettings::getInstance()->getIcon("plus_grey");
        //auto minusIcon = ViewSettings::getInstance()->getIcon("minus");
        auto item = new QListWidgetItem(this);
        

        //QFont font = item->font();
        //font.setPointSize(12);
        //item->setFont(font);
        if(maxLabelLength < fontMetrics().boundingRect(rankName).width()) maxLabelLength=fontMetrics().boundingRect(rankName).width();

        // Multithreading check
        // If we haven't seen this rank before...
        if(rankThreadMap->count(rank.first->ref().get())==0){
            //qInfo() << "    ... Multithreadingcheck " << this;       
            std::map<std::string, std::pair<int, std::vector<bool>>> threadMap{};
            // We use that to figure out the thread position <=> threadNumber
            std::map<std::string, std::string> threadMap_{};
            //qInfo() << "... working on rank ..." << rank.first->ref().get();
            for (const auto &slot_: rank.second) {
                auto threadRef = std::to_string(slot_->location->ref().get());
                auto threadName = slot_->location->name().str();
                //qInfo() << "keys -> [" << threadName.c_str() << "]";
                // If we haven't seen this thread before...
                if(!threadMap_.count(threadRef)){
                    threadMap_.insert({threadName, threadRef});
                }
            }
            //std::make_pair(threadRef, bool[3]{false, false, false})
            // Whats their position? (threadNumber for main thread => 1)
            auto threadNumber = threadMap.size();
            for (auto &threadItem: threadMap_) {
                threadNumber++;
                // Settings vector for individual threads = "highlighted?" - "toggled P2P?"
                std::vector<bool> boolVector {false, false};
                std::pair<int, std::vector<bool>> entry {threadNumber, boolVector};
                //qInfo() << threadItem.first.c_str() << "... is nr. ..." << threadNumber;
                threadMap.insert({threadItem.second, entry});
            }
            rankThreadMap->insert({rank.first->ref().get(), std::make_pair(false, threadMap)});
            rankAdrMap->insert({rank.first->ref().get(), rank.first});
        }

        item->setText(rankName);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::UserRole, rank.first->ref().get());
        item->setToolTip(QString::fromStdString("node: "+rank.first->parent().name().str()));
        item->setTextAlignment(Qt::AlignCenter);
        this->widgetPointerList.push_front(item);

        // This handles the +/- icon, background-brush and size
        this->adjustIcon(item, rank.first->ref().get());

        this->addItem(item);
    }
    auto extraSpaceBottom = new QListWidgetItem(this);
    // We dont want the extra space to be selectable etc.
    extraSpaceBottom->setFlags(Qt::NoItemFlags);
    extraSpaceBottom->setSizeHint(QSize(0, 20));
    this->addItem(extraSpaceBottom);
}

void TimelineLabelList::mousePressEvent(QMouseEvent *event) {
    //qInfo() << "EXECUTING TimelineLabelList::mousePressEvent ... for " << this;
    //qInfo() << "mousePressEvent";
    // First: determine where the click happend
    auto *item = this->itemAt(event->pos());

    if (item != nullptr) {
        QString rankName = item->text();
        // Filter: we don't want any click behavior for padding labels (extraSpaceTop, extraSpaceBottom), this is how we spot them 
        if(rankName.isEmpty()) return;
    }
    else {
        return;
    }

    /*
    // Assuming we have clicked on a rank...
    if (event->button()==Qt::LeftButton) {
        auto settings = ViewSettings::getInstance();
        QString rankName = item->text();

        // Filter: e can't expand thre multithreading view for ranks without multithreading, this is how we spot them 
        if(item->background().color()==QColorConstants::Svg::silver) return;

        int rankRef = item->data(Qt::UserRole).toInt();
        //qInfo() << "expansion ---> " << rankRef;
        // We don't really care if the threads were expanded or not...
        bool formerStatus = settings->getRankThreadMap()->at(rankRef).first;
        // ... we just want to flip the bool value for every mousePressEvent 
        settings->getRankThreadMap()->insert_or_assign(rankRef, std::pair(!formerStatus, settings->getRankThreadMap()->at(rankRef).second));
        
        // This handles the +/- icon, background-brush and size
        this->adjustIcon(item, rankRef);
        Q_EMIT this->data->refreshButtonPressed();
        //Q_EMIT this->data->labelInteractionTrigger();
    }
    else if (event->button()==Qt::RightButton) {
        item->setSelected(true);
        int rankRef = item->data(Qt::UserRole).toInt();
        // This is how we know what label we interacted with, that's relevant for ignorePreparation, flamegraphPreparation etc.
        this->labelAction2->setData(rankRef);
        this->labelAction3->setData(rankRef);
        //this->menu->exec(event->globalPos());
        this->menu->exec(event->globalPosition().toPoint());
    }
    */

    // We need to know the associated rank
    int rankRef = item->data(Qt::UserRole).toInt();
    auto settings = ViewSettings::getInstance();

    switch (event->button()) {
        case Qt::LeftButton: {
            // Filter: e can't expand thre multithreading view for ranks without multithreading, this is how we spot them 
            if(item->background().color()==QColorConstants::Svg::silver) return;
            // We flip the multithreading-view bool via XOR
            settings->getRankThreadMap()->at(rankRef).first ^= true;
            // This handles the +/- icon, background-brush and size
            this->adjustIcon(item, rankRef);
            Q_EMIT this->data->refreshButtonPressed();
            //Q_EMIT this->data->labelInteractionTrigger();
            
        }break;
        case Qt::RightButton: {
            item->setSelected(true);
            this->labelAction2->setData(rankRef);
            this->labelAction3->setData(rankRef);
            //this->menu->exec(event->globalPos());
            this->menu->exec(event->globalPosition().toPoint());
        }break;
    }

    return;
}

void TimelineLabelList::mouseReleaseEvent(QMouseEvent *) {
    return;
}

void TimelineLabelList::mouseMoveEvent(QMouseEvent *) {
    return;
}

int TimelineLabelList::getMaxLabelLength() {
    return maxLabelLength;
}

void TimelineLabelList::highlightPreparation(){
    //qInfo() << "highlight ...";
    //Q_EMIT this->data->labelInteractionTrigger();
}

void TimelineLabelList::togglePointToPointPreparation(){
    //qInfo() << "ignore ...";

    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) return;

    int rankRef = action->data().toInt();
    auto settings = ViewSettings::getInstance();

    // We flip the toggle status for all threads
    for (auto &entry : settings->getRankThreadMap()->at(rankRef).second){
        entry.second.second.at(1).flip();
        //qInfo() << "did that for ... " << entry.first.c_str();
    }
    
    Q_EMIT this->data->refreshButtonPressed();
    //Q_EMIT this->data->labelInteractionTrigger();
}

void TimelineLabelList::flamegraphPreparation(){   

    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) return;

    int rankRef = action->data().toInt();
    auto settings = ViewSettings::getInstance();
    settings->setFlamegraphRankRef(rankRef);

    Q_EMIT this->data->flamegraphRequest();
    //qInfo() << "flamegraph ...";
}

void TimelineLabelList::resizeLabels() {
    //const QSize newSize = ViewSettings::getInstance()->getRowHeight();
    const QSize newSize(0,ViewSettings::getInstance()->getRowHeight());
    for(auto labelEntry : this->widgetPointerList){
        labelEntry->setSizeHint(newSize);
    }
}

void TimelineLabelList::adjustIcon(QListWidgetItem * rankLabel, int rankKey) {
    auto rankThreadMap = ViewSettings::getInstance()->getRankThreadMap();
    auto plusIcon = ViewSettings::getInstance()->getIcon("plus");
    auto plusIconGrey = ViewSettings::getInstance()->getIcon("plus_grey");
    auto minusIcon = ViewSettings::getInstance()->getIcon("minus");

    qInfo() << plusIconGrey->name();

    auto iconToggleStatus = rankThreadMap->at(rankKey).first;
    auto rankSizeOffset = rankThreadMap->at(rankKey).second.size()-1;
    auto currentRowHeight = ViewSettings::getInstance()->getRowHeight();

    QBrush backgroundPatternOpen = QBrush(QColorConstants::Svg::lightsteelblue, Qt::Dense6Pattern);
    QBrush backgroundPatternClosed = QBrush(QColorConstants::Svg::lightsteelblue, Qt::Dense4Pattern);
    QBrush backgroundPatternNoMultithreading = QBrush(QColorConstants::Svg::silver, Qt::Dense4Pattern);

    // Standard procedure
    rankLabel->setSizeHint(QSize(0, currentRowHeight+(rankSizeOffset*currentRowHeight*iconToggleStatus)));
    rankLabel->setTextAlignment(Qt::AlignCenter);

    // If the amount of threads (<=> rankSizeOffset+1) < 2 => non-expandable <=> grey icon
    if(rankSizeOffset<1){
        rankLabel->setIcon(*plusIconGrey);
        rankLabel->setBackground(backgroundPatternNoMultithreading);
    }
    else{
        iconToggleStatus ? (rankLabel->setIcon(*minusIcon), rankLabel->setBackground(backgroundPatternOpen)) : (rankLabel->setIcon(*plusIcon), rankLabel->setBackground(backgroundPatternClosed));
    }
}