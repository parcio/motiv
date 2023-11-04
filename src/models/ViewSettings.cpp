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


//#todo: delete later
//#include <QDebug>

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

int ViewSettings::getRowHeight() {
    return this->rowHeight;
}

void ViewSettings::setRowHeight(int height) {
    if(height>=15) this->rowHeight = height;
}

void ViewSettings::setSearchName(QString searchName) {
    this->searchName = searchName;
}

QString ViewSettings::getSearchName() {
    return this->searchName;
}

std::map< OTF2_StringRef, std::pair<bool, std::map<std::string, std::pair<int, std::vector<bool>>>>>* ViewSettings::getRankThreadMap() {
    return &rankThreadMap;
}

std::map< OTF2_StringRef, otf2::definition::location_group *>* ViewSettings::getRankAdrMap() {
    return &rankAdrMap;
}

QIcon* ViewSettings::getIcon(std::string key) {
    return &Icons_.at(key);
}

int ViewSettings::getFlamegraphRankRef() {
    return this->rankRef;
}

void ViewSettings::setFlamegraphRankRef(int newRankRef) {
    this->rankRef = newRankRef;
}


void ViewSettings::setCountIndicatorsREG(bool newState) {
    this->countIndicatorsREG = newState;
}

void ViewSettings::setCountIndicatorsP2P(bool newState) {
    this->countIndicatorsP2P = newState;
}

void ViewSettings::setCountIndicatorsCCM(bool newState) {
    this->countIndicatorsCCM = newState;
}

void ViewSettings::setCountIndicatorDetailsFlamegraph(bool newState) {
    this->countIndicatorDetailsFlamegraph = newState;
}

void ViewSettings::setPxThresholdFlamegraph(bool newState) {
    this->pxThresholdFlamegraph = newState;
}

void ViewSettings::setUseRealWidthFlamegraph(bool newState) {
    this->useRealWidthFlamegraph = newState;
}

void ViewSettings::setUseRealWidthMainWindow(bool newState) {
    this->useRealWidthMainWindow = newState;
}

void ViewSettings::setUseBorderOverview(bool newState) {
    this->useBorderOverview = newState;
}

void ViewSettings::setUsePriorityOverview(bool newState) {
    this->usePriorityOverview = newState;
}

void ViewSettings::setColorCodingTimeRecords(bool newState) {
    this->colorCodingTimeRecords = newState;
}

void ViewSettings::setActiveThresholdOV(double newVal) {
    this->activeThresholdOV = newVal;
}

void ViewSettings::setActiveThresholdREG(double newVal) {
    this->activeThresholdREG = newVal;
}

void ViewSettings::setActiveThresholdP2P(double newVal) {
    this->activeThresholdP2P = newVal;
}

void ViewSettings::setActiveThresholdCCM(double newVal) {
    this->activeThresholdCCM = newVal;
}

bool ViewSettings::getCountIndicatorsREG() {
    return this->countIndicatorsREG;
}

bool ViewSettings::getCountIndicatorsP2P() {
    return this->countIndicatorsP2P;
}

bool ViewSettings::getCountIndicatorsCCM() {
    return this->countIndicatorsCCM;
}

bool ViewSettings::getCountIndicatorDetailsFlamegraph() {
    return this->countIndicatorDetailsFlamegraph;
}

bool ViewSettings::getPxThresholdFlamegraph() {
    return this->pxThresholdFlamegraph;
}

bool ViewSettings::getUseRealWidthFlamegraph() {
    return this->useRealWidthFlamegraph;
}

bool ViewSettings::getUseRealWidthMainWindow() {
    return this->useRealWidthMainWindow;
}

bool ViewSettings::getUseBorderOverview() {
    return this->useBorderOverview;
}

bool ViewSettings::getUsePriorityOverview() {
    return this->usePriorityOverview;
}

bool ViewSettings::getColorCodingTimeRecords() {
    return this->colorCodingTimeRecords;
}

double ViewSettings::getActiveThresholdOV() {
    return this->activeThresholdOV;
}

double ViewSettings::getActiveThresholdREG() {
    return this->activeThresholdREG;
}

double ViewSettings::getActiveThresholdP2P() {
    return this->activeThresholdP2P;
}

double ViewSettings::getActiveThresholdCCM() {
    return this->activeThresholdCCM;
}