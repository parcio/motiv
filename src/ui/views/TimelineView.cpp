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
#include <QGraphicsRectItem>
#include <QApplication>
#include <QWheelEvent>
#include <set>


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
    // @formatter:on
}


void TimelineView::populateScene(QGraphicsScene *scene) {
    auto width = scene->width();
    auto selection = this->data->getSelection();
    auto runtime = selection->getRuntime().count();
    auto runtimeR = static_cast<qreal>(runtime);
    auto begin = this->data->getBegin().count();
    auto beginR = static_cast<qreal>(begin);
    auto end = begin + runtime;
    auto endR = static_cast<qreal>(end);

    QPen arrowPen(Qt::black, 1);
    QPen collectiveCommunicationPen(colors::COLOR_COLLECTIVE_COMMUNICATION, 2);


    auto onTimedElementSelected = [this](TimedElement *element) { this->data->setTimeElementSelection(element); };
    auto onTimedElementDoubleClicked = [this](TimedElement *element) {
        this->data->setSelection(element->getStartTime(), element->getEndTime());
    };


    auto top = 20;
    auto ROW_HEIGHT = ViewSettings::getInstance()->getRowHeight();
    for (const auto &item: selection->getSlots()) {


        std::set<std::string> threadKeySet;
        // Rank offset has to be set to the amount of threads (- 1, we don't want to double count the master thread)
        // rankOffset.insert({item.first->name().str(), threadKeySet.size()-1});
        rankOffset.insert({item.first->ref().get(), -1});
        rankOffset.at(item.first->ref().get())=-1;
        // First variant, very weird behavior for resize
        /*
        std::multimap<std::string, Slot*> slotMap;

        // Prepare slots
        for (const auto &slot: item.second) {
            //qInfo() << "\nslot preparation ... " << slot->region->name().str().c_str() << slot->location->name().str().c_str();
            if (!(slot->getKind() & data->getSettings()->getFilter().getSlotKinds())) continue;
            auto threadKey = slot->location->name().str();
            slotMap.insert({threadKey, slot});
            threadKeySet.insert(threadKey);
            //qInfo() << "\nslotMap size: " << slotMap.size() << "\n";
        }

        // Display slots
        for (const std::string & key: threadKeySet) {
            while(slotMap.count(key) > 0) {
                // extract returns a node!
                auto & slot = slotMap.extract(key).mapped();
                auto region = slot->region;
                auto regionName = region->name();
                auto regionNameStr = regionName.str();
                auto startTime = slot->startTime.count();
                auto endTime = slot->endTime.count();
                //auto mpiRankStr = slot->location->location_group().name().str();

                // Ensures slots starting before `begin` (like main) are considered to start at begin
                auto effectiveStartTime = qMax(begin, startTime);
                // Ensures slots ending after `end` (like main) are considered to end at end
                auto effectiveEndTime = qMin(end, endTime);

                auto slotBeginPos = (static_cast<qreal>(effectiveStartTime - begin) / static_cast<qreal>(runtime)) * width;
                auto slotRuntime = static_cast<qreal>(effectiveEndTime - effectiveStartTime);
                auto rectWidth = (slotRuntime / static_cast<qreal>(runtime)) * width;

                QRectF rect(slotBeginPos, top, qMax(rectWidth, 5.0), ROW_HEIGHT);
                auto rectItem = new SlotIndicator(rect, slot);
                rectItem->setOnDoubleClick(onTimedElementDoubleClicked);
                rectItem->setOnSelected(onTimedElementSelected);
                //QString qstr = "blabla";
                //rectItem->setToolTip(qstr);
                rectItem->setToolTip(slot->location->name().str().c_str());
                //rectItem->setToolTip(regionNameStr.c_str());

                // Determine color based on name
                rectItem->setBrush(slot->getColor());
                rectItem->setZValue(slot->priority);
                scene->addItem(rectItem);
            }
            //top += ROW_HEIGHT;

        }
        // Rank offset has to be set to the amount of threads (- 1, we don't want to double count the master thread)
        rankOffset.insert({item.first->name().str(), threadKeySet.size()-1});
        top += ROW_HEIGHT;
        */

        // Prepare slots
        for (const auto &slot_: item.second) {
            //qInfo() << "\nslot preparation ... " << slot_->region->name().str().c_str() << slot_->location->name().str().c_str();
            threadKeySet.insert(slot_->location->name().str());
            //qInfo() << "\nslotMap size: " << slotMap.size() << "\n";
        }

        // Display slots
        for (const std::string & key: threadKeySet) {
            for (const auto &slot: item.second) {
                // Do we really want to draw this slot?
                if (!(slot->getKind() & data->getSettings()->getFilter().getSlotKinds())) continue;
                // Does it belong to the currently drawn thread?
                if (!(slot->location->name().str() == key)) continue;
                auto region = slot->region;
                auto regionName = region->name();
                auto regionNameStr = regionName.str();
                auto startTime = slot->startTime.count();
                auto endTime = slot->endTime.count();

                // Ensures slots starting before `begin` (like main) are considered to start at begin
                auto effectiveStartTime = qMax(begin, startTime);
                // Ensures slots ending after `end` (like main) are considered to end at end
                auto effectiveEndTime = qMin(end, endTime);

                auto slotBeginPos = (static_cast<qreal>(effectiveStartTime - begin) / static_cast<qreal>(runtime)) * width;
                auto slotRuntime = static_cast<qreal>(effectiveEndTime - effectiveStartTime);
                auto rectWidth = (slotRuntime / static_cast<qreal>(runtime)) * width;

                QRectF rect(slotBeginPos, top, qMax(rectWidth, 5.0), ROW_HEIGHT);
                auto rectItem = new SlotIndicator(rect, slot);
                rectItem->setOnDoubleClick(onTimedElementDoubleClicked);
                rectItem->setOnSelected(onTimedElementSelected);
                rectItem->setToolTip(slot->location->name().str().c_str());
                //rectItem->setToolTip(regionNameStr.c_str());

                // Determine color based on name
                rectItem->setBrush(slot->getColor());
                rectItem->setZValue(slot->priority);
                scene->addItem(rectItem);
            }
            top += ROW_HEIGHT;
            // We need to consider every extra row
            rankOffset.at(item.first->ref().get())++;
        }
        //top += ROW_HEIGHT;

        // Classic variant
        /*
        for (const auto &slot: item.second) {    
            if (!(slot->getKind() & data->getSettings()->getFilter().getSlotKinds())) continue;

            auto region = slot->region;
            auto regionName = region->name();
            auto regionNameStr = regionName.str();
            auto startTime = slot->startTime.count();
            auto endTime = slot->endTime.count();


            // Ensures slots starting before `begin` (like main) are considered to start at begin
            auto effectiveStartTime = qMax(begin, startTime);
            // Ensures slots ending after `end` (like main) are considered to end at end
            auto effectiveEndTime = qMin(end, endTime);

            auto slotBeginPos = (static_cast<qreal>(effectiveStartTime - begin) / static_cast<qreal>(runtime)) * width;
            auto slotRuntime = static_cast<qreal>(effectiveEndTime - effectiveStartTime);
            auto rectWidth = (slotRuntime / static_cast<qreal>(runtime)) * width;

            QRectF rect(slotBeginPos, top, qMax(rectWidth, 5.0), ROW_HEIGHT);
            auto rectItem = new SlotIndicator(rect, slot);
            rectItem->setOnDoubleClick(onTimedElementDoubleClicked);
            rectItem->setOnSelected(onTimedElementSelected);
            rectItem->setToolTip(regionNameStr.c_str());

            // Determine color based on name
            rectItem->setBrush(slot->getColor());
            rectItem->setZValue(slot->priority);

            scene->addItem(rectItem);
        }
        top += ROW_HEIGHT;
        */

    }

    // Thread-Handling
    //auto offsetThreads = rankOffset.at(endEvent->getLocation()->name().str());
    int offsetThreadsSum = 0;
    for (const auto& r : rankOffset) {
        offsetThreadsSum += r.second;
    }

    for (const auto &communication: selection->getCommunications()) {
        const CommunicationEvent *startEvent = communication->getStartEvent();
        auto startEventEnd = static_cast<qreal>(startEvent->getEndTime().count());
        auto startEventStart = static_cast<qreal>(startEvent->getStartTime().count());


        const CommunicationEvent *endEvent = communication->getEndEvent();
        auto endEventEnd = static_cast<qreal>(startEvent->getEndTime().count());
        auto endEventStart = static_cast<qreal>(startEvent->getStartTime().count());


        auto fromTime = startEventStart + (startEventEnd - startEventStart) / 2;
        auto effectiveFromTime = qMax(beginR, fromTime) - beginR;

        auto toTime = endEventStart + (endEventEnd - endEventStart) / 2;
        auto effectiveToTime = qMin(endR, toTime) - beginR;

        auto fromRank = startEvent->getLocation()->ref().get();
        auto toRank = endEvent->getLocation()->ref().get();

        int higherPos = 0;
        int lowerPos = 0;
        // the arrow points to the bottom
        toRank > fromRank ? (higherPos=fromRank, lowerPos=toRank) : (higherPos=toRank, lowerPos=fromRank);

        // How many threads were rendered above the arrow?
        int higherOffset = 0;
        for (int i = higherPos; i > 0; i--) {
            higherOffset += rankOffset.at(i)*ROW_HEIGHT;
        }

        // How many threads were rendered extra within the arrow?
        int innerOffset = 0;
        for (int i = lowerPos; i > higherPos; i--) {
            innerOffset += rankOffset.at(i)*ROW_HEIGHT;
        }

        int toOffset = 0;
        int fromOffset = 0;
        // if the arrow points to the bottom (higher ranks are rendered lower) we have to use the toOffset as the sum of higherOffset and innerOffset
        toRank > fromRank ? (toOffset=higherOffset+innerOffset, fromOffset=higherOffset) : (toOffset=higherOffset, fromOffset=higherOffset+innerOffset);

        QString dbgInfo = QString::fromStdString("higherPos["+std::to_string(higherPos)+"]"+"lowerPos["+std::to_string(lowerPos)+"]"+"toOffset["+std::to_string(toOffset)+"]"+"fromOffset["+std::to_string(fromOffset)+"]");

        auto fromX = effectiveFromTime / runtimeR * width;
        auto fromY = static_cast<qreal>(fromRank * ROW_HEIGHT) + .5 * ROW_HEIGHT + 20 + fromOffset;

        auto toX = effectiveToTime / runtimeR * width;
        auto toY = static_cast<qreal> (toRank * ROW_HEIGHT) + .5 * ROW_HEIGHT + 20 + toOffset;

        auto arrow = new CommunicationIndicator(communication, fromX, fromY, toX, toY);
        arrow->setOnSelected(onTimedElementSelected);
        arrow->setOnDoubleClick(onTimedElementDoubleClicked);
        arrow->setPen(arrowPen);
        arrow->setZValue(layers::Z_LAYER_P2P_COMMUNICATIONS);
        arrow->setToolTip(dbgInfo);
        scene->addItem(arrow);
    }

    for (const auto &communication: selection->getCollectiveCommunications()) {
        auto fromTime = static_cast<qreal>(communication->getStartTime().count());
        auto effectiveFromTime = qMax(beginR, fromTime) - beginR;

        auto toTime = static_cast<qreal>(communication->getEndTime().count());
        auto effectiveToTime = qMin(endR, toTime) - beginR;

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

        for (const auto &member: communication->getMembers()){
            auto memberFromTime = static_cast<qreal>(member->start.count());
            auto memberEffectiveFromTime = qMax(beginR, memberFromTime) - beginR;

            auto memberToTime =  static_cast<qreal>(member->end.count());
            auto memberEffectiveToTime = qMin(endR, memberToTime) - beginR;

            auto locationGroupNameStr = member->getLocation()->location_group().name().str();
            size_t pos = locationGroupNameStr.find_last_of(' ');
            int y = std::stoi(locationGroupNameStr.substr(pos + 1));      

            auto memberFromX = (memberEffectiveFromTime / runtimeR) * width;
            auto memberFromY = y*ROW_HEIGHT+20;

            auto memberToX = (memberEffectiveToTime / runtimeR) * width;
            auto memberToY = top - (top - (y+1)*ROW_HEIGHT)+20;

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

}


void TimelineView::resizeEvent(QResizeEvent *event) {
    this->updateView();
    QGraphicsView::resizeEvent(event);
}

void TimelineView::updateView() {
    // TODO it might be more performant to keep track of items and add/remove new/leaving items and resizing them
    this->scene()->clear();

    auto ROW_HEIGHT = ViewSettings::getInstance()->getRowHeight();
    auto sceneHeight = this->data->getSelection()->getSlots().size() * ROW_HEIGHT;
    auto sceneRect = this->rect();
    sceneRect.setHeight(sceneHeight);

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
