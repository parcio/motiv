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

#include "ColorMap.hpp"
#include "src/models/AppSettings.hpp"
#include "src/ui/ColorGenerator.hpp"

ColorGenerator* colorgenerator = ColorGenerator::getInstance();

ColorMap* ColorMap::instance = nullptr;
ColorMap::ColorMap() {}

ColorMap* ColorMap::getInstance() {
    if (instance == nullptr) {
        instance = new ColorMap();
    }return instance;
}

void ColorMap::addColor(QString function, QColor color,bool fromConfig) {
    if (map.count(function) == 0) {
        if (color == nullptr) color = colorgenerator->GetNewColor();
        map[function] = color;
        if(!fromConfig) AppSettings::getInstance().colorConfigPush(function,color);    
    }
}

void ColorMap::setColor(QString function, QColor color){
    if (map.count(function) == 0) this->addColor(function,color);
    else map[function] = color;
    AppSettings::getInstance().colorConfigPush(function,color);
}

QColor ColorMap::getColor(QString function) {
    if (map.count(function) > 0) {
        return map[function];
    } else {
        return QColor();
    }
}

void ColorMap::clearColorMap(){
    this->map.clear();
}

std::unordered_map<QString, QColor> ColorMap::getMap(){
    return this->map;    
}
