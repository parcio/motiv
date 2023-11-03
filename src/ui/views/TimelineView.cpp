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
#include "TimelineView.hpp"
#include "src/models/ViewSettings.hpp"
#include "src/ui/views/CommunicationIndicator.hpp"
#include "src/ui/views/SlotIndicator.hpp"
#include "src/ui/Constants.hpp"
#include "CollectiveCommunicationIndicator.hpp"
#include "src/ui/ColorGenerator.hpp"

#include <map>
#include <qtmetamacros.h>
#include <set>
#include <QGraphicsRectItem>
#include <QApplication>
#include <QWheelEvent>
#include <string>
#include <vector>
#include <array>


TimelineView::TimelineView(TraceDataProxy *data, QWidget *parent) : QGraphicsView(parent), data(data) {
    auto scene = new QGraphicsScene();
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setAutoFillBackground(false);
    this->setStyleSheet("background: transparent");
    this->setScene(scene);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // @formatter:off
    connect(this->data, SIGNAL(selectionChanged(types::TraceTime,types::TraceTime)), this, SLOT(updateView()));
    connect(this->data, SIGNAL(filterChanged(Filter)), this, SLOT(updateView()));
    connect(this->data, SIGNAL(colorChanged()),this, SLOT(updateView()));
    connect(this->data, SIGNAL(verticalZoomChanged()),this,SLOT(updateView()));
    connect(this->data, SIGNAL(refreshButtonPressed()),this,SLOT(updateView()));
    // @formatter:on
}


void TimelineView::populateScene(QGraphicsScene *scene) {
    this->data->triggerUITimerStartIfPossible();
    auto width = scene->width();
    auto selection = this->data->getSelection();
    auto runtime = selection->getRuntime().count();
    auto runtimeR = static_cast<qreal>(runtime);
    auto begin = this->data->getBegin().count();
    auto beginR = static_cast<qreal>(begin);
    auto end = begin + runtime;
    auto endR = static_cast<qreal>(end);

    std::array<int, 3> globalDrawCount{};
    std::array<int, 3> globalFullCount{};

    QPen arrowPen(Qt::black, 1);
    QPen collectiveCommunicationPen(colors::COLOR_COLLECTIVE_COMMUNICATION, 2);

    auto onTimedElementSelected = [this](TimedElement *element) { this->data->setTimeElementSelection(element); };
    auto onTimedElementDoubleClicked = [this](TimedElement *element) {
        this->data->setSelection(element->getStartTime(), element->getEndTime());
    };
    
    auto top = 20;
    auto settings = ViewSettings::getInstance();

    auto ROW_HEIGHT = settings->getRowHeight();
    auto * rankThreadMap = settings->getRankThreadMap();
    std::string searchName_ = settings->getSearchName().toStdString();

    bool countIndicatorsREG = settings->getCountIndicatorsREG();
    bool countIndicatorsP2P = settings->getCountIndicatorsP2P();
    bool countIndicatorsCCM = settings->getCountIndicatorsCCM();
    bool useRealWidth = settings->getUseRealWidthMainWindow();

    // Experimental***
    auto activeThresholdREG = settings->getActiveThresholdREG();
    auto activeThresholdP2P = settings->getActiveThresholdP2P();
    auto activeThresholdCCM = settings->getActiveThresholdCCM();
    // Experimental***

    for (const auto &item: selection->getSlots()) {

        auto rankNameStd = item.first->name().str();
        auto rankName = QString::fromStdString(rankNameStd);

        int localDrawCount = 0;
        if(countIndicatorsREG) {
            auto rawSLTs = this->data->getFullTrace()->subtrace(this->data->getBegin(), this->data->getEnd())->getSlots().at(item.first);
            for(auto SLT : rawSLTs){
                globalFullCount[0]++;
            }
        }

        // Do we have the thread view expanded?
        auto toggleStatus = rankThreadMap->at(item.first->ref().get()).first;
        int threadCount = rankThreadMap->at(item.first->ref().get()).second.size();

        // Preparation
        std::vector<std::string> threadRefVector(threadCount, std::to_string(0));
        for (const auto& [threadRef, threadInfo]: rankThreadMap->at(item.first->ref().get()).second) {
            // First threadRef has to go to index 0 etc.
            threadRefVector[threadInfo.first-1]=threadRef;
        }

        // Draw slots
        for (int i = 0; i < threadCount; i++) {
            for (const auto &slot: item.second) {
                // Do we really want to draw this slot?
                if (!(slot->getKind() & data->getSettings()->getFilter().getSlotKinds())) continue;
                // Does it belong to the currently drawn thread?
                auto threadRef = std::to_string(slot->location->ref().get());
                if (!(threadRef == threadRefVector[i])) continue;
                auto region = slot->region;
                auto regionName = region->name();
                auto regionNameStr = regionName.str();
                auto startTime = slot->startTime.count();
                auto endTime = slot->endTime.count();

                // Ensures slots starting before `begin` (like main) are considered to start at begin
                auto effectiveStartTime = qMax(begin, startTime);
                // Ensures slots ending after `end` (like main) are considered to end at end
                auto effectiveEndTime = qMin(end, endTime);

                // Experimental***
                if(activeThresholdREG){
                    long regLength = effectiveEndTime - effectiveStartTime;
                    long timeFraction = (runtime/1000) * activeThresholdREG;
                    if(regLength<timeFraction)continue;
                }
                // Experimental***

                auto slotBeginPos = (static_cast<qreal>(effectiveStartTime - begin) / static_cast<qreal>(runtime)) * width;
                auto slotRuntime = static_cast<qreal>(effectiveEndTime - effectiveStartTime);
                auto rectWidth = (slotRuntime / static_cast<qreal>(runtime)) * width;

                if(!useRealWidth) rectWidth = qMax(rectWidth, 5.0);

                QRectF rect(slotBeginPos, top, rectWidth, ROW_HEIGHT);
                auto rectItem = new SlotIndicator(rect, slot);
                rectItem->setOnDoubleClick(onTimedElementDoubleClicked);
                rectItem->setOnSelected(onTimedElementSelected);
                //rectItem->setToolTip(slot->location->name().str().c_str());
                rectItem->setToolTip(regionNameStr.c_str());

                // Determine color based on name
                QColor rectColor = slot->getColor();
                rectItem->setBrush(rectColor);
                
                // Set search filter
                if(searchName_!= ""){
                    if(searchName_ != slot->region->name().str()){ 
                        rectItem->setBrush(colors::COLOR_SLOT_PLAIN);
                    } else rectItem->setZValue(20);

                } else rectItem->setZValue(slot->priority);
                    scene->addItem(rectItem);
                    localDrawCount++;
                }
                
            if(toggleStatus){
                top += ROW_HEIGHT;
            }
        }
        if(!toggleStatus){
            top += ROW_HEIGHT;
        }
        globalDrawCount[0]+=localDrawCount;
    }

    for (const auto &communication: selection->getCommunications()) {
        const CommunicationEvent *startEvent = communication->getStartEvent();
        auto startEventEnd = static_cast<qreal>(startEvent->getEndTime().count());
        auto startEventStart = static_cast<qreal>(startEvent->getStartTime().count());


        const CommunicationEvent *endEvent = communication->getEndEvent();
        auto endEventEnd = static_cast<qreal>(endEvent->getEndTime().count());
        auto endEventStart = static_cast<qreal>(endEvent->getStartTime().count());


        // With the new support for threads we have also to consider the thread-location (fromThread, to Thread)
        auto fromRank = startEvent->getLocation()->location_group().ref().get();
        auto fromThreadRef = startEvent->getLocation()->ref().get();
        auto toRank = endEvent->getLocation()->location_group().ref().get(); 
        auto toThreadRef = endEvent->getLocation()->ref().get(); 

        // Is the P2P communication toggled?
        if(rankThreadMap->at(fromRank).second.at(std::to_string(fromThreadRef)).second.at(1)) continue;
        if(rankThreadMap->at(toRank).second.at(std::to_string(toThreadRef)).second.at(1)) continue;

        auto fromTime = startEventStart + (startEventEnd - startEventStart) / 2;
        auto effectiveFromTime = qMax(beginR, fromTime) - beginR;

        auto toTime = endEventStart + (endEventEnd - endEventStart) / 2;
        auto effectiveToTime = qMin(endR, toTime) - beginR;

        // Experimental***
        if(activeThresholdP2P){
            long commLength = effectiveToTime - effectiveFromTime;
            long timeFraction = (runtime/1000) * activeThresholdP2P;
            if(commLength<timeFraction)continue;
        }
        // Experimental***

        /*
        This is the original way to get rank references:
        auto fromRank = startEvent->getLocation()->ref().get();
        auto toRank = endEvent->getLocation()->ref().get(); 

        We changed it based on the assumption that:
        location <=> Thread
        location group <=> Rank
        (Page 4, MOTIV Report)
        */
      
        // Correction if we point from and/or to somthing else than a mainthread (=> -1)
        bool fromToggleStatus = rankThreadMap->at(fromRank).first;
        int fromThreadNumber = rankThreadMap->at(fromRank).second.at(std::to_string(fromThreadRef)).first;
        bool toToggleStatus = rankThreadMap->at(toRank).first;
        int toThreadNumber = rankThreadMap->at(toRank).second.at(std::to_string(toThreadRef)).first;
        int fromThreadOffset = fromToggleStatus*(fromThreadNumber-1)*ROW_HEIGHT;
        int toThreadOffset = toToggleStatus*(toThreadNumber-1)*ROW_HEIGHT;

        int higherPos = 0;
        int lowerPos = 0;
        QString currentRank;
        // the arrow points to the bottom
        toRank > fromRank ? (higherPos=fromRank, lowerPos=toRank) : (higherPos=toRank, lowerPos=fromRank);

        // How many threads were rendered above the arrow?
        // Idea: We only accumulate the offset from positions above (<=> lower ranks) higherPos => higherPos-1
        int higherOffset = 0;
        for (int i = higherPos-1; i >= 0; i--) {
            // Do we have the thread view expanded?
            bool toggleStatus = rankThreadMap->at(i).first;
            // How many Threads are there? apart from the main thread => -1
            int threadCount = rankThreadMap->at(i).second.size()-1;
            higherOffset += toggleStatus*threadCount*ROW_HEIGHT;
        }

        // How many threads were rendered extra within the arrow?
        // Idea: The same as for higherOffset
        int innerOffset = 0;
        for (int i = lowerPos-1; i >= higherPos; i--) {
            // Do we have the thread view expanded?
            bool toggleStatus = rankThreadMap->at(i).first;
            // How many Threads are there? apart from the main thread => -1
            int threadCount = rankThreadMap->at(i).second.size()-1;
            innerOffset += toggleStatus*threadCount*ROW_HEIGHT;
        }

        int fromOffset = 0;
        int toOffset = 0;

        // if the arrow points to the bottom (higher ranks are rendered lower) we have to use the toOffset as the sum of higherOffset and innerOffset
        toRank > fromRank ? (toOffset=higherOffset+innerOffset+toThreadOffset, fromOffset=higherOffset+fromThreadOffset) : (toOffset=higherOffset+toThreadOffset, fromOffset=higherOffset+innerOffset+fromThreadOffset);

        auto rankNameFrom = startEvent->getLocation()->location_group().name().str();
        auto threadNameFrom = startEvent->getLocation()->name().str();
        auto rankNameTo = endEvent->getLocation()->location_group().name().str();
        auto threadNameTo = endEvent->getLocation()->name().str();

        QString Info = QString::fromStdString("From:\t"+rankNameFrom+"\n\t"+threadNameFrom+"\nTo:\t"+rankNameTo+"\n\t"+threadNameTo);

        auto fromX = effectiveFromTime / runtimeR * width;
        auto fromY = static_cast<qreal>(fromRank * ROW_HEIGHT) + .5 * ROW_HEIGHT + 20 + fromOffset;

        auto toX = effectiveToTime / runtimeR * width;
        auto toY = static_cast<qreal> (toRank * ROW_HEIGHT) + .5 * ROW_HEIGHT + 20 + toOffset;

        auto arrow = new CommunicationIndicator(communication, fromX, fromY, toX, toY);
        arrow->setOnSelected(onTimedElementSelected);
        arrow->setOnDoubleClick(onTimedElementDoubleClicked);
        arrow->setPen(arrowPen);
        arrow->setZValue(layers::Z_LAYER_P2P_COMMUNICATIONS);
        arrow->setToolTip(Info);
        // Line modification, currently not used, problem: CommunicationIndicator is a QGraphicsPolygonItem (closed polygon)
        //QPolygonF pointPath = arrow->polygon();
        //pointPath.insert(1, QPointF(fromX, toY));    
        //arrow->setPolygon(pointPath);

        scene->addItem(arrow);
        globalDrawCount[1]++;
    }

    for (const auto &communication: selection->getCollectiveCommunications()) {
        auto fromTime = static_cast<qreal>(communication->getStartTime().count());
        auto effectiveFromTime = qMax(beginR, fromTime) - beginR;

        auto toTime = static_cast<qreal>(communication->getEndTime().count());
        auto effectiveToTime = qMin(endR, toTime) - beginR;

        // Experimental***
        if(activeThresholdCCM){
            long commLength = effectiveToTime - effectiveFromTime;
            long timeFraction = (runtime/1000) * activeThresholdCCM;
            if(commLength<timeFraction)continue;
        }
        // Experimental***

        auto fromX = (effectiveFromTime / runtimeR) * width;
        auto fromY = 10;

        auto toX = (effectiveToTime / runtimeR) * width;
        auto toY = top + 10;

        auto rectItem = new CollectiveCommunicationIndicator(communication);
        rectItem->setOnSelected(onTimedElementSelected);
        rectItem->setRect(QRectF(QPointF(fromX, fromY), QPointF(toX, toY)));
        rectItem->setPen(collectiveCommunicationPen);
        rectItem->setZValue(layers::Z_LAYER_COLLECTIVE_COMMUNICATIONS);
        scene->addItem(rectItem);
        globalDrawCount[2]++;

        for (const auto &member: communication->getMembers()){
            auto memberFromTime = static_cast<qreal>(member->start.count());
            auto memberEffectiveFromTime = qMax(beginR, memberFromTime) - beginR;

            auto memberToTime =  static_cast<qreal>(member->end.count());
            auto memberEffectiveToTime = qMin(endR, memberToTime) - beginR;

            int IndicatorOffset = 0;
            auto rankRef = member->getLocation()->location_group().ref().get();
            auto threadRef = std::to_string(member->getLocation()->ref().get());
           
            // We accumulate the ROW_HEIGHTs for every expanded thread
            // Hint: lower rank <=> above our current position
            for (int i = rankRef-1; i >= 0; i--) {
                // Do we have the thread view expanded?
                bool toggleStatus = rankThreadMap->at(i).first;
                // How many Threads are there? apart from the main thread => -1
                int threadCount = rankThreadMap->at(i).second.size()-1;
                IndicatorOffset += toggleStatus*threadCount*ROW_HEIGHT;
            }

            // Correction if the Indicator is *itself* situated within a thread
            // No need to multiply with toggleStatus: main thread case => (1-1)*ROW_HEIGHT
            IndicatorOffset += ((rankThreadMap->at(rankRef).second.at(threadRef).first)-1)*ROW_HEIGHT;

            auto memberFromX = (memberEffectiveFromTime / runtimeR) * width;
            auto memberFromY = rankRef*ROW_HEIGHT+20+IndicatorOffset;

            auto memberToX = (memberEffectiveToTime / runtimeR) * width;
            auto memberToY = top - (top - (rankRef+1)*ROW_HEIGHT)+20+IndicatorOffset;

            auto memberRectItem = new CollectiveCommunicationIndicator(communication);
            memberRectItem->setOnSelected(onTimedElementSelected);
            memberRectItem->setRect(QRectF(QPointF(memberFromX, memberFromY), QPointF(memberToX, memberToY)));            
            memberRectItem->setZValue(layers::Z_LAYER_COLLECTIVE_COMMUNICATIONS);
            QBrush brush(Qt::black);
            brush.setStyle(Qt::BDiagPattern);
            memberRectItem->setBrush(brush);
            scene->addItem(memberRectItem);
        }
    }


    QString infoString;
    std::string infoExtra="";

    if(countIndicatorsREG) infoExtra="/"+std::to_string(globalFullCount[0]);
        // globalFullCount was already counted on the way
    infoString+=(std::to_string(globalDrawCount[0])+infoExtra+"[REG] ").c_str();
    infoExtra="";

    if(countIndicatorsP2P) {
        auto rawP2Ps = this->data->getFullTrace()->subtrace(this->data->getBegin(), this->data->getEnd())->getCommunications();
        for(auto P2P : rawP2Ps){
            globalFullCount[1]++;
        }
        infoExtra="/"+std::to_string(globalFullCount[1]);
    }
    infoString+=(std::to_string(globalDrawCount[1])+infoExtra+"[P2P] ").c_str();
    infoExtra="";

    if(countIndicatorsCCM) {
        auto rawCCMs = this->data->getFullTrace()->subtrace(this->data->getBegin(), this->data->getEnd())->getCollectiveCommunications();
        for(auto CCM : rawCCMs){
            globalFullCount[2]++;
        }
        infoExtra="/"+std::to_string(globalFullCount[2]);
    }
    infoString+=(std::to_string(globalDrawCount[2])+infoExtra+"[CCM] ").c_str();
    infoExtra="";

    settings->globalMessage+=infoString;

    this->data->triggerUITimerEndIfPossible();
}


void TimelineView::resizeEvent(QResizeEvent *event) {
    this->data->triggerUITimerStartIfPossible();
    this->updateView();
    QGraphicsView::resizeEvent(event);
}

void TimelineView::updateView() {
    this->data->triggerUITimerStartIfPossible();
    // TODO it might be more performant to keep track of items and add/remove new/leaving items and resizing them
    this->scene()->clear();
    auto ROW_HEIGHT = ViewSettings::getInstance()->getRowHeight();
    auto * rankThreadMap = ViewSettings::getInstance()->getRankThreadMap();
    auto sceneHeight = this->data->getSelection()->getSlots().size() * ROW_HEIGHT;
    // The base offset accounts for "top" (TimelineView) and "extraSpaceBottom" (TimelineLabelList)
    int sceneHeightOffset = 40;
    for (const auto& [rankRef, threadMap]: *rankThreadMap) {
        // calc: is the thread view expanded? * how many extra rows do we have? * ROW_HEIGHT
        sceneHeightOffset+=threadMap.first*(threadMap.second.size()-1)*ROW_HEIGHT;
    }
    auto sceneRect = this->rect();
    sceneRect.setHeight(sceneHeight+sceneHeightOffset);
    this->scene()->setSceneRect(sceneRect);
    this->populateScene(this->scene());
}

void TimelineView::wheelEvent(QWheelEvent *event) {
    // Calculation according to https://doc.qt.io/qt-6/qwheelevent.html#angleDelta:
    // @c angleDelta is in eights of a degree and most mouse wheels work in steps of 15 degrees.
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numDegrees.isNull() && QApplication::keyboardModifiers() & (Qt::CTRL | Qt::SHIFT)) {
        // See documentation and comment above
        QPoint numSteps = numDegrees / 15;
        auto stepSize = data->getSelection()->getRuntime() / data->getSettings()->getZoomQuotient();
        auto deltaDuration = stepSize * numSteps.y();
        auto delta = static_cast<double>(deltaDuration.count());

        types::TraceTime newBegin;
        types::TraceTime newEnd;
        if (QApplication::keyboardModifiers() == Qt::CTRL) {
            // Calculate the position of the mouse relative to the scene to zoom to where the mouse is pointed
            auto originFactor = event->scenePosition().x() / this->scene()->width();

            auto leftDelta = types::TraceTime(static_cast<long>(originFactor * 2 * delta));
            auto rightDelta = types::TraceTime(static_cast<long>((1 - originFactor) * 2 * delta));

            newBegin = data->getSelection()->getStartTime() + leftDelta;
            newEnd = data->getSelection()->getStartTime() + data->getSelection()->getRuntime() - rightDelta;
        } else {
            // Calculate new absolute times (might be negative or to large)
            auto newBeginAbs = data->getSelection()->getStartTime() - deltaDuration;
            auto newEndAbs = data->getSelection()->getStartTime() + data->getSelection()->getRuntime() - deltaDuration;

            // Limit the times to their boundaries (0 for start and end of entire trace for end)
            auto newBeginBounded = qMax(newBeginAbs, types::TraceTime(0));
            auto newEndBounded = qMin(newEndAbs, data->getTotalRuntime());

            // If one time exceeds the bounds reject the changes
            newBegin = qMin(newBeginBounded, newEndBounded - data->getSelection()->getRuntime());
            newEnd = qMax(newEndBounded, newBeginBounded + data->getSelection()->getRuntime());
        }

        data->setSelection(newBegin, newEnd);
        event->accept();
    }

    QGraphicsView::wheelEvent(event);
}