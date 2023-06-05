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

#ifndef MOTIV_COLORLIST_HPP
#define MOTIV_COLORLIST_HPP

#include <unordered_map>
#include <QColor>
#include <QString>

/**
 * @brief Singleton class for managing a list of unique colors associated with function names.
 */
class ColorList {
private:    
    static ColorList* instance;  
    ColorList();  
    ColorList(const ColorList& obj) = delete;
   
    std::unordered_map<QString, QColor> map;

public:
    static ColorList* getInstance();  
    QColor getColor(QString);
    void addColor(QString);
};

#endif //MOTIV_COLORLIST_HPP