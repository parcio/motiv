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

#include "ColorList.hpp"
#include "src/models/AppSettings.hpp"
#include "src/ui/ColorGenerator.hpp"

ColorGenerator* colorgenerator = ColorGenerator::getInstance();

ColorList* ColorList::instance = nullptr;
ColorList::ColorList() {}

ColorList* ColorList::getInstance() {
    if (instance == nullptr) {
        instance = new ColorList();
    }return instance;
}

void ColorList::addColor(QString function, QColor color,bool fromConfig) {
    if (map.count(function) == 0) {
        if (color == nullptr) color = colorgenerator->GetNewColor();
        map[function] = color;
        if(!fromConfig) AppSettings::getInstance().colorConfigPush(function,color);
    
    }
}

void ColorList::setColor(QString function, QColor color){
    if (map.count(function) == 0) this->addColor(function,color);
    else map[function] = color;
    AppSettings::getInstance().updateColorConfig(function,color);
}

QColor ColorList::getColor(QString function) {
    if (map.count(function) > 0) {
        return map[function];
    } else {
        return QColor();
    }
}

void ColorList::clearColorList(){
    this->map.clear();
}
