/*
 * Marvelous OTF2 Traces Interactive Visualizer (MOTIV)
 * Copyright (C) 2023 Florian Gallrein, Björn Gehrke, Jessica Lafontaine
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
#include "AppSettings.hpp"
#include "src/models/ColorMap.hpp"

#define SET_AND_EMIT(key) \
    settings->setValue(#key, key##_); \
    Q_EMIT key##Changed(key##_);

AppSettings::AppSettings() {
    this->recentlyOpenedFiles_ = this->settings->value("recentlyOpenedFiles").toStringList();
}

const QStringList &AppSettings::recentlyOpenedFiles() const {
    return recentlyOpenedFiles_;
}

void AppSettings::recentlyOpenedFilesPush(const QString &newFile) {
    // Make sure most recent files are always at the back.
    recentlyOpenedFiles_.removeAll(newFile);
    recentlyOpenedFiles_.push_back(newFile);
    SET_AND_EMIT(recentlyOpenedFiles)
}

void AppSettings::recentlyOpenedFilesRemove(const QString &filePath) {
    recentlyOpenedFiles_.removeAll(filePath);
    SET_AND_EMIT(recentlyOpenedFiles)
}

void AppSettings::recentlyOpenedFilesClear(QString filepath) {
    this->recentlyOpenedFiles_.clear();
    this->recentlyOpenedFiles_.push_back(filepath);
    SET_AND_EMIT(recentlyOpenedFiles)
}

void AppSettings::setColorConfigName(QString &filePath){
    this->colorConfigName_ = "Motiv/colors/";
    size_t pos1 = filePath.lastIndexOf("/");
    size_t pos2 = filePath.lastIndexOf("/",pos1 - 1);
    this->colorConfigName_.append(filePath.mid(pos2 + 1, pos1 - pos2 - 1));
    this->colorSettings = new QSettings (this->colorConfigName_);
}

void AppSettings::loadColorConfigs () {
    ColorMap *colorMap_ = ColorMap::getInstance ();
    QList<QSettings*> settingsList = {this->colorSettings, this->globalColorSettings};
    for (QSettings *settings : settingsList) {
        if (!settings->contains ("Colors")) {
            settings->setValue ("Colors", "");
            continue; 
        }
        QStringList functions = settings->allKeys ();
        for (const QString &function : functions) {
            QColor color = settings->value (function).value<QColor> ();
            colorMap_->addColor (function, color, true);
        }
    }
}

void AppSettings::saveAsGlobalColors(){
    ColorMap *colorMap_ = ColorMap::getInstance ();
    for (auto& [key, value]: colorMap_->getMap()){
        this->globalColorSettings->setValue(key,value);
    }this->globalColorSettings->sync();
}

void AppSettings::colorConfigPush(QString function, QColor color){
    this->colorSettings->setValue (function, color);    
    this->colorSettings->sync();
    
    if(this->useGlobalColorConfig){
        this->globalColorSettings->setValue(function, color);
        this->globalColorSettings->sync();
    }
}

void AppSettings::loadGlobalColors(){
    QStringList functions = globalColorSettings->allKeys ();
    ColorMap *colorMap_ = ColorMap::getInstance ();
    for (const QString &function : functions) {
        QColor color = globalColorSettings->value (function).value<QColor> ();
        colorMap_->setColor(function, color);
        this->colorSettings->setValue(function,color);
    }
    this->colorSettings->sync();
}

void AppSettings::clearColorConfig(){
     this->colorSettings->clear();
     this->colorSettings->sync();
}

void AppSettings::toggleGlobalColorConfig(bool checked)
{
    this->useGlobalColorConfig = checked;
}

bool AppSettings::getuseGlobalColorConfig(){
    return this->useGlobalColorConfig;
}

void AppSettings::setMode(Mode mode){
    this->mode = mode;
}

Mode AppSettings::getMode(){
    return this->mode;
}