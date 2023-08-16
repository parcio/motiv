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
#include <utility>
#include <QString>
#include "./Slot.hpp"
#include "src/models/ColorMap.hpp"
#include "src/ui/ColorGenerator.hpp"
#include "src/ui/Constants.hpp"


ColorMap* colorMap_ = ColorMap::getInstance();

Slot::Slot(const otf2::chrono::duration &start, const otf2::chrono::duration &anEnd,
           otf2::definition::location* location, otf2::definition::region* region) :
    startTime(start),
    endTime(anEnd),
    location(location),
    region(region){
        QString function_ = QString::fromStdString(region->name().str());
        switch(this->getKind()){
            case ::MPI:
                this->color = colorMap_->getColor(function_);                
                if(!this->color.isValid()) {
                    this->color = colors::COLOR_SLOT_MPI;
                    colorMap_->addColor(function_, this->color);
                }
                this->priority = layers::Z_LAYER_SLOTS_MIN_PRIORITY + 2;
                break;
            case ::OpenMP:
                this->color = colorMap_->getColor(function_); 
                if(!this->color.isValid()) {
                    this->color = colors::COLOR_SLOT_OPEN_MP;
                    colorMap_->addColor(function_, this->color);
                }               
                this->priority = layers::Z_LAYER_SLOTS_MIN_PRIORITY + 1;
                break;
            case ::None:
            case ::Plain:
                if (!this->color.isValid ()) colorMap_->addColor(function_);
                this->color = colorMap_->getColor(function_);
                this->priority = layers::Z_LAYER_SLOTS_MIN_PRIORITY + 0;
                break;
        }
}

SlotKind Slot::getKind() const {
    auto regionName = this->region->name().str();
    if (regionName.starts_with("MPI_")) {
        return MPI;
    } else if (regionName.starts_with("!$omp")) {
        return OpenMP;
    } else {
        return Plain;
    }
}


types::TraceTime Slot::getStartTime() const {
    return startTime;
}

types::TraceTime Slot::getEndTime() const {
    return endTime;
}


void Slot::setColor(QColor color_){
    QString function_ = QString::fromStdString(region->name().str());    
    if(!color_.isValid()){
        if(colorMap_->getColor(function_).isValid()){
             this->color=colorMap_->getColor(function_);
             return;
        }
        switch(this->getKind()){
            case ::MPI:                
                this->color = colors::COLOR_SLOT_MPI;
                colorMap_->setColor(function_, this->color);                             
                break;
            case ::OpenMP:
                this->color = colors::COLOR_SLOT_OPEN_MP;
                colorMap_->setColor(function_, this->color);               
                break;
            case ::None:
            case ::Plain:                
                this->color = ColorGenerator::getInstance()->GetNewColor();
                colorMap_->setColor(function_,this->color);
                break;
        } 
    } else this->color = color_;
}

QColor Slot::getColor(){
    return this->color;
}
