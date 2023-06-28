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

#ifndef MOTIV_COLORSYNCHRONIZER_HPP
#define MOTIV_COLORSYNCHRONIZER_HPP

#include "src/ui/widgets/infostrategies/InformationDockTraceStrategy.hpp"
#include "src/ui/TraceDataProxy.hpp" 

/**
 * @brief Synchronizes the colors of slots that represent the same function
 */
class ColorSynchronizer {

public:
    /**
     * @brief Returns the singleton instance of the ColorSynchronizer class
     */
    static ColorSynchronizer* getInstance(); 

    /**
     * @brief Updates the color of all slots that represent the given function to the given color
     */
    void synchronizeColors(const std::string& function, const QColor& color);
    
    /**
     * @brief Changes the color of all slots (except MPI and OpenMP slots) to the given color.
     * 
     * The function takes a bool parameter that defaults to false. If set to true, the function will change the color of all slots, including the previously excluded ones.
     */
    void synchronizeColors(const QColor&, bool = false);
    
    /**
     * @brief Recolors all slots based on the ColorList structure
     */   
    void synchronizeColors();

    /**
     * @brief Sets the data member, which is needed by synchronizeColors to access and update all slots
     */
    void setData(TraceDataProxy* data);

private: 
    static ColorSynchronizer* instance;    
     ColorSynchronizer();
    ~ColorSynchronizer(); 

     TraceDataProxy* data_;
};

#endif //MOTIV_COLORSYNCHRONIZER_HPP