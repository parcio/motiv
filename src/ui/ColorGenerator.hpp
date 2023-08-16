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

#ifndef MOTIV_COLORGENERATOR_HPP
#define MOTIV_COLORGENERATOR_HPP

#include <QColor>
#include <stdio.h>


class ColorGenerator{
  private:
    int red, green, blue, min;
    // 0= blue, 1=red, 2=green, 4=full
    int token;
 

    static ColorGenerator* instance;    
    ColorGenerator();
    ColorGenerator(const ColorGenerator& obj) = delete;

  //Change basic values of red green blue
  int updateColor (int,int);

public:
  static ColorGenerator* getInstance(); 
  QColor GetNewColor();
  
  /**
   * @brief Resets all values to their default values
   */
  void setDefault();

};

#endif //MOTIV_COLORGENERATOR_HPP