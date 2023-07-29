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
#include "src/models/ViewSettings.hpp"

#include <QLabel>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <set>

TimelineLabelList::TimelineLabelList(TraceDataProxy *data, QWidget *parent) : QListWidget(parent), data(data) {
    this->setFrameShape(QFrame::NoFrame);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setStyleSheet("background: transparent");
    setViewportMargins(0, 20, 0, 0);

    for (const auto &rank: this->data->getSelection()->getSlots()) {
        const QString &rankName = QString::fromStdString(rank.first->name().str());
        auto toggledRankMap = ViewSettings::getInstance()->getToggledRankMap();
        auto rankOffsetMap = ViewSettings::getInstance()->getRankOffsetMap();
        auto multithreadingRankMap = ViewSettings::getInstance()->getMultithreadingRankMap();
        auto plusIcon = ViewSettings::getInstance()->getIcon("plus");
        auto plusIconGrey = ViewSettings::getInstance()->getIcon("plus_grey");
        auto minusIcon = ViewSettings::getInstance()->getIcon("minus");
        auto item = new QListWidgetItem(this);
        

        // Multithreading check
        std::set<std::string> threadKeySet{};
        if(multithreadingRankMap->count(rankName)==0){
            for (const auto &slot_: rank.second) {         
                threadKeySet.insert(slot_->location->name().str());
            }
            threadKeySet.size() > 1 ? multithreadingRankMap->insert({rankName, true}) : multithreadingRankMap->insert({rankName, false});
            rankOffsetMap->insert({rankName, threadKeySet.size()-1});
            toggledRankMap->insert({rankName, false});
        }

        // Determine the fitting icon
        if(!multithreadingRankMap->at(rankName)){
            item->setIcon(*plusIconGrey);
        }
        else if(toggledRankMap->count(rankName)>0){
            toggledRankMap->at(rankName) ? item->setIcon(*minusIcon) : item->setIcon(*plusIcon);
        }
        else{
            // Well, we *have* multithreading but didn't check the toggle-status yet
            // We assume that no multithreaded rank is expanded
            item->setIcon(*plusIcon);
        }

        item->setText(rankName);
        item->setToolTip(QString::fromStdString("node: "+rank.first->parent().name().str()));
        item->setSizeHint(QSize(0, this->ROW_HEIGHT+(rankOffsetMap->at(rankName)*this->ROW_HEIGHT*toggledRankMap->at(rankName))));
        item->setTextAlignment(Qt::AlignCenter);  
        this->addItem(item);
    }
}

void TimelineLabelList::mousePressEvent(QMouseEvent *event) {
    // First: determine where the click happend
    auto *item = this->itemAt(event->pos());  
    // Assuming we have clicked on a rank...
    if (item != nullptr) {
        QString rankName = item->text();
        auto settings = ViewSettings::getInstance();
        // We toggle the thread-view via a bool-flip 
        settings->getToggledRankMap()->insert_or_assign(rankName, !settings->getToggledRankMap()->at(rankName));
        Q_EMIT this->data->expansionEventHappend();
    }
    return;
}

void TimelineLabelList::mouseReleaseEvent(QMouseEvent *) {
    return;
}

void TimelineLabelList::mouseMoveEvent(QMouseEvent *) {
    return;
}
