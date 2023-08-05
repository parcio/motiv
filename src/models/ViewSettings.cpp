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
#include "ViewSettings.hpp"

ViewSettings* ViewSettings::instance = nullptr_t();

ViewSettings::ViewSettings(){}

ViewSettings* ViewSettings::getInstance()
{
    if (instance == nullptr) instance = new ViewSettings(); 
    return instance; 
}

int ViewSettings::getZoomQuotient() const {
    return zoomFactor_;
}

void ViewSettings::setZoomFactor(int zoomFactor) {
    zoomFactor_ = zoomFactor;
}

Filter ViewSettings::getFilter() const {
    return filter_;
}

void ViewSettings::setFilter(Filter filter) {
    filter_ = filter;
}

int ViewSettings::getRowHeight(){
    return this->rowHeight;
}

void ViewSettings::setRowHeight(int height){
    if(height>=15) this->rowHeight = height;
}

std::map< OTF2_StringRef, std::pair<bool, std::map<std::string, int>>>* ViewSettings::getRankThreadMap() {
    return &rankThreadMap;
}

QIcon* ViewSettings::getIcon(std::string key) {
    return &Icons_.at(key);
}
