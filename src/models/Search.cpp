/*
 * Marvelous OTF2 Traces Interactive Visualizer (MOTIV)
 * Copyright (C) 2023 Jessica Lafontaine
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

#include <iterator>
#include <QSet>

#include "src/models/Search.hpp"

Search::Search(TraceDataProxy *data, QListWidget *itemList, QWidget *parent): data(data), itemList(itemList), QListWidget(parent){
    if(this->data == nullptr) throw std::invalid_argument("data (TraceDataProxy) is null");
    
    for(const auto &item : data->getFullTrace()->getSlots()){
        for(const auto &slot: item.second){
            QString name = QString::fromStdString(slot->region->name().str());
            QList<QListWidgetItem *> matches = this->itemList->findItems(name, Qt::MatchExactly);            
            if(matches.isEmpty()) this->itemList->addItem(name);
        }
    }
    
    this->itemList->sortItems(Qt::AscendingOrder);
}

void Search::findName(QString subname, QListWidget *itemList) {
    for (int i = 0; i < itemList->count(); i++) {
        QListWidgetItem *item = itemList->item(i);
        QString name = item->text();
        if (!name.contains(subname,Qt::CaseInsensitive)){
            item->setHidden(true);
        } else item->setHidden(false);
    }
}


std::list<TimedElement*> Search::createItemList(QString searchedName){
    std::list<TimedElement*> resultList;
    for(const auto &item : data->getFullTrace()->getSlots()){        
        for(const auto &slot: item.second){
            QString name = QString::fromStdString(slot->region->name().str());      
            if(name == searchedName){
                resultList.push_back(slot);
            }
        }
    }
    return resultList;
}

