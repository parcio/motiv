/*
 * Marvelous OTF2 Traces Interactive Visualizer (MOTIV)
 * Copyright (C) 2024 Jessica Lafontaine
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

#include "NodeIndicator.hpp"

#include <QPen>
#include <QGraphicsSceneMouseEvent>


#include "src/ui/Constants.hpp"

NodeIndicator::NodeIndicator(Slot *representedSlot, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent)
    : GenericIndicator<Slot, QGraphicsEllipseItem>(representedSlot, parent) {    
    this->setRect(x, y, w, h);
    }
