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

#ifndef MOTIV_ColorMap_HPP
#define MOTIV_ColorMap_HPP

#include <unordered_map>
#include <QColor>
#include <QString>

/**
 * @brief Singleton class for managing a list of unique colors associated with function names.
 */
class ColorMap {
private:    
    static ColorMap* instance;  
    ColorMap();  
    ColorMap(const ColorMap& obj) = delete;
   
    std::unordered_map<QString, QColor> map;

public:
    static ColorMap* getInstance();  
    
    QColor getColor(QString);
    
    /**
    *@brief Adds a new color to a map with QString as the key
    *
    *@param QString The key for the map
    *@param QColor The color to be added
    *@param fromConfig A flag indicating whether the color was read from the config file or not (default false)
    *
    *If no color is given, it uses the ColorGenerator to create a new color. 
    *If the color was not read from the config file, it adds the tuple of QString and QColor to the config file.
    */
    void addColor(QString, QColor = nullptr, bool fromConfig = false);    
    
    /**
    *@brief Sets the color for a given key in the map and the config file.
    *
    *@note This function updates the color for the given key in the map and also in the config file.
    */
    void setColor(QString, QColor);

    void clearColorMap();

    std::unordered_map<QString, QColor> getMap();

};

#endif //MOTIV_ColorMap_HPP