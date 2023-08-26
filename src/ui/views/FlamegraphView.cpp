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
#include "FlamegraphView.hpp"
#include "src/models/ViewSettings.hpp"
#include "src/ui/views/CommunicationIndicator.hpp"
#include "src/ui/views/SlotIndicator.hpp"
#include "src/ui/Constants.hpp"
#include "CollectiveCommunicationIndicator.hpp"
#include "src/ui/ColorGenerator.hpp"

#include <map>
#include <set>
#include <QGraphicsRectItem>
#include <QApplication>
#include <QWheelEvent>
#include <QElapsedTimer>

//test
#include <QGraphicsTextItem>
#include <QLabel>
#include <QGraphicsProxyWidget>

FlamegraphView::FlamegraphView(TraceDataProxy *data, QWidget *parent) : QGraphicsView(parent), data(data) {
    auto scene = new QGraphicsScene();
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setAutoFillBackground(false);
    this->setStyleSheet("background: transparent");
    this->setScene(scene);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->requestedRank = ViewSettings::getInstance()->getFlamegraphRankRef();
    this->globalMaxHeight = 0;
    
    // @formatter:off
    connect(this->data, SIGNAL(selectionChanged(types::TraceTime,types::TraceTime)), this, SLOT(updateView()));
    connect(this->data, SIGNAL(filterChanged(Filter)), this, SLOT(updateView()));
    connect(this->data, SIGNAL(colorChanged()),this, SLOT(updateView()));
    connect(this->data, SIGNAL(verticalZoomChanged()),this,SLOT(updateView()));
    connect(this->data, SIGNAL(refreshButtonPressed()),this,SLOT(updateView()));
    // @formatter:on

    qInfo() << "FlamegraphView was created ...";
}


void FlamegraphView::populateScene(QGraphicsScene *scene) {
    QElapsedTimer populateSceneTimer;
    populateSceneTimer.start();
    auto width = scene->width();
    auto selection = this->data->getSelection();
    auto runtime = selection->getRuntime().count();
    //auto runtimeR = static_cast<qreal>(runtime);
    auto begin = this->data->getBegin().count();
    //auto beginR = static_cast<qreal>(begin);
    auto end = begin + runtime;
    //auto endR = static_cast<qreal>(end);
    int allSlotsCount=0;
    std::multiset<std::string> allSlotsMultiset{};
    int drawnSlotsCount=0;
    std::multiset<std::string> drawnSlotsMultiset{};

    QPen arrowPen(Qt::black, 1);
    QPen collectiveCommunicationPen(colors::COLOR_COLLECTIVE_COMMUNICATION, 2);

    qInfo() << "FlamegraphView::populateScene is executed ...";
    auto onTimedElementSelected = [this](TimedElement *element) { this->data->setTimeElementSelection(element); };
    auto onTimedElementDoubleClicked = [this](TimedElement *element) {
        this->data->setSelection(element->getStartTime(), element->getEndTime());
    };
    
    int top = 0;
    int baseRowLevel = 0;
    int localMaxHeight = 0;
    int threadDrawOffset = 30;
    this->globalMaxHeight=0;

    auto * settings = ViewSettings::getInstance();
    auto ROW_HEIGHT = settings->getRowHeight();
    auto * rankThreadMap = settings->getRankThreadMap();
    auto * fullTimeTableSlots = settings->getFullTimeTableSlots();

    std::string rankNameStd;
    QString rankName;

    // To skip any other rank in order to work on the right one is quite sloppy
    for (const auto &item: selection->getSlots()) {
        if(this->requestedRank!=item.first->ref().get()) continue;

        rankNameStd = item.first->name().str();
        rankName = QString::fromStdString(rankNameStd);

        // What's the 'real' number of slots for this particular rank?
        auto stop = this->data->getEnd().count();
        for (auto thing : fullTimeTableSlots->at(rankNameStd)){
            if(thing.second.first <= begin)continue;
            if(thing.first > stop)break;
            allSlotsCount++;
            allSlotsMultiset.insert(thing.second.second);
        }

        // Preparation
        int threadCount = rankThreadMap->at(item.first->ref().get()).second.size();
        std::vector<std::string> threadRefVector(threadCount, std::to_string(0));
        for (const auto& [threadRef, threadInfo]: rankThreadMap->at(item.first->ref().get()).second) {
            // First threadRef has to go to index 0 etc.
            threadRefVector[threadInfo.first-1]=threadRef;
        }

        for (int i = 0; i < threadCount; i++) {

            std::string threadName = "";
            int drawnSlotsCountLocal = 0;
            bool firstSlot = true;

            // We want to shift the baselevel-hight if we start to draw another thread
            baseRowLevel=this->globalMaxHeight;

            // We need to have the slots sortet by their starting time for the drawing logic
            std::map<std::chrono::nanoseconds::rep, Slot*> sortedSlotMap{};
            for (const auto &slot: item.second) {
                auto threadRef = std::to_string(slot->location->ref().get());
                if (!(threadRef == threadRefVector[i])) continue;
                sortedSlotMap.insert({slot->startTime.count(), slot});
            }

            // Is there anything to do? (no visible slots => skip the drawing logic)
            if(sortedSlotMap.size()==0) continue;

            // That's where we store the information regarding our row-level
            std::vector<std::chrono::nanoseconds::rep> endtimeVector{sortedSlotMap.begin()->second->getEndTime().count()};
            qInfo() << "START endTimes# " << endtimeVector.size();
            for (const auto& entry : sortedSlotMap) {
                auto threadRef = std::to_string(entry.second->location->ref().get());
                if (!(threadRef == threadRefVector[i])) continue;
                if (threadName=="")threadName=entry.second->location->name().str();
                auto region = entry.second->region;
                auto regionName = region->name();
                auto regionNameStr = regionName.str();
                auto startTime = entry.second->startTime.count();
                auto endTime = entry.second->endTime.count();

                // Ensures slots starting before `begin` (like main) are considered to start at begin
                auto effectiveStartTime = qMax(begin, startTime);
                // Ensures slots ending after `end` (like main) are considered to end at end
                auto effectiveEndTime = qMin(end, endTime);

                auto slotBeginPos = (static_cast<qreal>(effectiveStartTime - begin) / static_cast<qreal>(runtime)) * width;
                auto slotRuntime = static_cast<qreal>(effectiveEndTime - effectiveStartTime);
                auto rectWidth = (slotRuntime / static_cast<qreal>(runtime)) * width;

                // Are we within the limits of our last frame?
                if(endTime <= endtimeVector.back()){
                    //qInfo() << "within limit : " << endTime << "<=" << endtimeVector.back();
                    endtimeVector.push_back(endTime);
                }
                // Otherwise we have to scale down until we encounter a frame beneath us, that has a bigger endTime
                else{
                    while(endTime > endtimeVector.back()){
                        //qInfo() << "scaling down : " << endTime << ">" << endtimeVector.back();
                        endtimeVector.pop_back();
                    }
                    if(endTime <= endtimeVector.back()){
                        //qInfo() << "within limit : " << endTime << "<=" << endtimeVector.back();
                        endtimeVector.push_back(endTime);
                    }
                }
                top=threadDrawOffset+baseRowLevel+ROW_HEIGHT*(endtimeVector.size()-1);
                if(top>localMaxHeight)localMaxHeight=top;

                //QRectF rect(slotBeginPos, top, qMax(rectWidth, 5.0), ROW_HEIGHT);
                QRectF rect(slotBeginPos, top, rectWidth, ROW_HEIGHT);
                auto rectItem = new SlotIndicator(rect, entry.second);
                //qInfo() << "name: " << regionNameStr.c_str() << "base:" << baseRowLevel << "top: " << top << "endTimes# " << endtimeVector.size() << " --- maxV: " << localMaxHeight << "/" << this->globalMaxHeight;

                rectItem->setOnDoubleClick(onTimedElementDoubleClicked);
                rectItem->setOnSelected(onTimedElementSelected);
                rectItem->setToolTip(entry.second->location->name().str().c_str());

                QGraphicsTextItem *text = new QGraphicsTextItem();
                QFontMetrics fm(text->font());
                QString elidedText = fm.elidedText(regionNameStr.c_str(), Qt::ElideRight, rectItem->rect().width());
                text->setPlainText(elidedText);
                qreal x = rectItem->rect().center().x() - text->boundingRect().width() / 2;
                qreal y = rectItem->rect().center().y() - text->boundingRect().height() / 2;
                text->setPos(x,y);
                text->setTextInteractionFlags(Qt::NoTextInteraction);
                text->setTextWidth(rectItem->rect().width());
                text->setParentItem(rectItem);

                if(firstSlot){
                    firstSlot=false;
                    QGraphicsTextItem *threadDscr = new QGraphicsTextItem();
                    QFont font;
                    font.setPointSize(10);
                    font.setItalic(true);
                    threadDscr->setFont(font);
                    threadDscr->setDefaultTextColor(Qt::darkGray);
                    threadDscr->setPlainText(threadName.c_str());
                    // Slightly above the Slot
                    y = rectItem->rect().center().y() - (threadDrawOffset+6);
                    threadDscr->setPos(slotBeginPos,y);
                    threadDscr->setTextInteractionFlags(Qt::NoTextInteraction);
                    threadDscr->setParentItem(rectItem);
                }

                // Determine color based on name
                rectItem->setBrush(entry.second->getColor());
                rectItem->setZValue(entry.second->priority);
                drawnSlotsCountLocal++;
                drawnSlotsMultiset.insert(regionNameStr);
                scene->addItem(rectItem);
            }
            drawnSlotsCount+=drawnSlotsCountLocal;

            // That's relevant for the scene height in updates
            if(localMaxHeight>this->globalMaxHeight)this->globalMaxHeight=localMaxHeight;
        }

    }

    // That's the infoBar (QStatusBar) logic
    QString infoStructure =rankName+(" --- drawn items "+std::to_string(drawnSlotsCount)+"/"+std::to_string(allSlotsCount)).c_str();
    infoStructure+=(" in "+std::to_string(populateSceneTimer.elapsed())+"[ms]").c_str();
    std::map<std::string, int> difference{};
    for (std::string name : allSlotsMultiset) {
        int countDrawn = drawnSlotsMultiset.count(name);
        int countExisting = allSlotsMultiset.count(name);
        int diff = countExisting - countDrawn;
        if(diff!=0)difference.insert_or_assign(name, diff);
    }
    QFontMetrics fm(this->font());
    auto capacity = this->sceneRect().width() - fm.boundingRect(infoStructure).width();
    if(difference.size()>0){
        infoStructure+="  ---  not drawn: ";
        for (auto diffPair : difference) {
            QString extraInfo = (diffPair.first+" *"+std::to_string(diffPair.second)+"times  ").c_str();
            if(capacity > 300){
                infoStructure+=extraInfo;
                capacity-=fm.boundingRect(extraInfo).width();
            }
            else{
                infoStructure+=", and more ...";
                break;
            }
        }
    }
    this->statusInfo=infoStructure;
    statusChanged();
}

void FlamegraphView::resizeEvent(QResizeEvent *event) {
    auto rectVal = this->rect();
    // We don't want to make the scene height depandand on the window height
    rectVal.setHeight(this->scene()->height());
    this->scene()->setSceneRect(rectVal);
    this->updateView();
    QGraphicsView::resizeEvent(event);
}

void FlamegraphView::updateView() {
    this->scene()->clear();
    this->populateScene(this->scene());
}

void FlamegraphView::wheelEvent(QWheelEvent *event) {
    // Calculation according to https://doc.qt.io/qt-6/qwheelevent.html#angleDelta:
    // @c angleDelta is in eights of a degree and most mouse wheels work in steps of 15 degrees.
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numDegrees.isNull() && QApplication::keyboardModifiers() & (Qt::CTRL | Qt::SHIFT)) {
        // See documentation and comment above
        QPoint numSteps = numDegrees / 15;
        //qInfo() << "wheel event...";
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