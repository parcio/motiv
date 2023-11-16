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

#include "src/models/AppSettings.hpp"
#include "src/models/ColorMap.hpp"
#include "src/ui/ColorSynchronizer.hpp"
#include "src/ui/Constants.hpp"

#include <QProgressDialog>

ColorSynchronizer *ColorSynchronizer::instance = nullptr_t();

ColorSynchronizer* ColorSynchronizer::getInstance(){
    if (instance == nullptr) instance = new ColorSynchronizer(); 
    return instance;
}

void ColorSynchronizer::synchronizeColors(const std::string& function, const QColor& color){
    if(!data_) return;
        ColorMap *map = ColorMap::getInstance();
        map->setColor(QString::fromStdString(function), color);
        for (const auto &item: data_->getFullTrace()->getSlots()) {
            for (const auto &slot: item.second) {
                if(slot->region->name().str()==function) slot->setColor(color);                    
            }
        }
    Q_EMIT data_->colorChanged();
}

void ColorSynchronizer::synchronizeColors(const QColor& color, bool all){
    if(!data_) return;
     ColorMap *map = ColorMap::getInstance();

        for (const auto &item: data_->getFullTrace()->getSlots()) {     
            for (const auto &slot: item.second) {               
                if(slot->getKind()==Plain||slot->getKind()==None||all) {
                    slot->setColor(color);
                    QString function_= QString::fromStdString(slot->region->name().str());
                    map->setColor(function_, color);
                }
            }
        }
    Q_EMIT data_->colorChanged();
}


void ColorSynchronizer::synchronizeColors(){
    if(!data_) return;
    ColorMap *map = ColorMap::getInstance();

        for (const auto &item: data_->getFullTrace()->getSlots()) {     
            for (const auto &slot: item.second) {
                QString function_= QString::fromStdString(slot->region->name().str());             
                slot->setColor(map->getColor(function_));                
            }
        }    
    Q_EMIT data_->colorChanged();
}

void ColorSynchronizer::reCalculateColors(){
    if(!data_) return;

    ColorMap *map = ColorMap::getInstance();    
        for (const auto &item: data_->getFullTrace()->getSlots()) {     
            for (const auto &slot: item.second) {
                QString function_= QString::fromStdString(slot->region->name().str());
                QColor color_ = map->getColor(function_);
                if(color_.isValid()) slot->setColor(color_);
                else {        
                    switch(slot->getKind()){
                        case ::MPI:
                            slot->setColor(colors::COLOR_SLOT_MPI);
                            map->addColor(function_,slot->getColor());
                            break;
                        case ::OpenMP:
                            slot->setColor(colors::COLOR_SLOT_OPEN_MP);
                            map->addColor(function_,slot->getColor());
                            break;
                        case ::None:
                        case ::Plain:
                            map->addColor(function_, QColor());
                            slot->setColor(map->getColor(function_));
                            break;                      
                    }                   
                }
            }
        }
    Q_EMIT data_->colorChanged();
}


void ColorSynchronizer::setData(TraceDataProxy* data) {
    data_ = data; 
}


ColorSynchronizer::ColorSynchronizer() : data_(nullptr) {}
ColorSynchronizer::~ColorSynchronizer() {}
