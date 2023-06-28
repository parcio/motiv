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

#include "src/models/AppSettings.hpp"
#include "src/models/ColorList.hpp"
#include "src/ui/ColorSynchronizer.hpp"


ColorSynchronizer *ColorSynchronizer::instance = nullptr_t();

ColorSynchronizer* ColorSynchronizer::getInstance(){
    if (instance == nullptr) instance = new ColorSynchronizer(); 
    return instance;
}

void ColorSynchronizer::synchronizeColors(const std::string& function, const QColor& color){
    if(!data_) return;

        for (const auto &item: data_->getFullTrace()->getSlots()) {  
            for (const auto &slot: item.second) {                
                if(slot->region->name().str()==function) slot->setColor(color);
            }
        }     
    data_->colorChanged();
}

void ColorSynchronizer::synchronizeColors(const QColor& color, bool all){
    if(!data_) return;
    
        for (const auto &item: data_->getFullTrace()->getSlots()) {     
            for (const auto &slot: item.second) {               
                if(slot->getKind()==Plain||slot->getKind()==None||all) slot->setColor(color);
            }
        } 
    data_->colorChanged();        
}


void ColorSynchronizer::synchronizeColors(){
    if(!data_) return;
    
        for (const auto &item: data_->getFullTrace()->getSlots()) {     
            for (const auto &slot: item.second) {
                QString function_= QString::fromStdString(slot->region->name().str());
                QColor color_ = ColorList::getInstance()->getColor(function_);
                slot->setColor(color_);
                AppSettings::getInstance().updateColorConfig(function_, color_);
            }
        }    
    data_->colorChanged();
}


void ColorSynchronizer::setData(TraceDataProxy* data) {
    data_ = data; 
}


ColorSynchronizer::ColorSynchronizer() : data_(nullptr) {}
ColorSynchronizer::~ColorSynchronizer() {}
