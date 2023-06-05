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
#include "src/models/ColorList.hpp"
#include "src/ui/Constants.hpp"


ColorList* colorlist = ColorList::getInstance();

Slot::Slot(const otf2::chrono::duration &start, const otf2::chrono::duration &anEnd,
           otf2::definition::location* location, otf2::definition::region* region) :
    startTime(start),
    endTime(anEnd),
    location(location),
    region(region){
    switch(this->getKind()){
        case ::MPI:
            this->color = colors::COLOR_SLOT_MPI;
            this->priority = layers::Z_LAYER_SLOTS_MIN_PRIORITY + 2;
            break;
        case ::OpenMP:
            this->color = colors::COLOR_SLOT_OPEN_MP;
            this->priority = layers::Z_LAYER_SLOTS_MIN_PRIORITY + 1;
            break;
        case ::None:
        case ::Plain:
            if(this->getKind()==Plain){
                if (!this->color.isValid ()) colorlist->addColor(QString::fromStdString(region->name().str()));
                this->color = colorlist->getColor(QString::fromStdString(region->name().str()));
                this->priority = layers::Z_LAYER_SLOTS_MIN_PRIORITY + 0;
            }break;
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
