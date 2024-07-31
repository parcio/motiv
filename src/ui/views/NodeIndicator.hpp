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

#ifndef MOTIV_NODEINDICATOR_HPP
#define MOTIV_NODEINDICATOR_HPP

#include <QGraphicsEllipseItem>

#include "src/ui/TraceDataProxy.hpp"
#include "src/models/Slot.hpp"
#include "src/types.hpp"
#include "GenericIndicator.hpp"


/**
 * @brief Indicator for collective communications
 *
 * A slot is indicated by a rectangle.
 */
class NodeIndicator : public GenericIndicator<Slot, QGraphicsEllipseItem> {
public: // constructors
    /**
     * @brief Creates a new instance of the SlotIndicator class
     * @param rect The rect the slot should be rendered
     * @param representedSlot The Slot object the indicator is representing
     * @param parent The parent QGraphicsItem
     */
    NodeIndicator(Slot *representedSlot, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = nullptr);
};


#endif //MOTIV_NODEINDICATOR_HPP