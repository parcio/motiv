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
#include "ColorGenerator.hpp"

// Increment value for color updates, controls color diversity
constexpr int COLOR_INCREMENT = 50;

// Singleton instance
ColorGenerator* ColorGenerator::instance = nullptr_t();

ColorGenerator::ColorGenerator():red{255}, green {0}, blue{50}, min{0}, token {0}
{
}

ColorGenerator* ColorGenerator::getInstance()
{
    if (instance == nullptr) 
    {
        instance = new ColorGenerator(); 
    }
    return instance; 
}

// Update color value based on comparison color
int ColorGenerator::updateColor(int color, int comparison_color){
    if (token == 2 && this->red > 249 && this->green<min+6 && this->blue<min+6){
      if(min<100) this->min+=COLOR_INCREMENT;
      else{
        this->red=189;
        this->green=189;
        this->blue=189;
        this->token = 3;
        return 189;
      }
    }


// Increment or decrement color value based on comparison color
if (comparison_color==min){
    if ((color+COLOR_INCREMENT)<256) return color+=COLOR_INCREMENT;
    else {
    this->token = (token +1)%3;           
    return 255;
    }

}else{
    if ((color-COLOR_INCREMENT)>min-1) return color-=COLOR_INCREMENT;
    else{
    this->token = (token +1)%3;           
    return min;
    }
}
}


// Get new color and update internal state
QColor ColorGenerator::GetNewColor(){
    QColor color;      
    switch(token){
         // Update red value
        case 1:
        color = QColor(red,green,blue);
        this->red=updateColor(red,blue);
        if(token == 2) this->green=updateColor(green,red);
        break;
        

         // Update green value
        case 2:
        color = QColor(red,green,blue);
        this->green=updateColor(green,red);
        if(token==0) this->blue=updateColor(blue,green);
        break;

        // Update blue value
        case 0:
        color = QColor(red,green,blue);
        this->blue = updateColor(blue,green);
        if(token==1) this->red=updateColor(red,blue);
        break;
        
        default:
        color = QColor(189,189,189);
        break;
    }
    return color;
}


