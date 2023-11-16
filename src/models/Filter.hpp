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
#ifndef MOTIV_FILTER_HPP
#define MOTIV_FILTER_HPP

#define SLOT_FILTER_DEFAULT ((SlotKind) (SlotKind::MPI | SlotKind::OpenMP | SlotKind::Plain))
#define COMMUNICATION_FILTER_DEFAULT ((CommunicationKind) (CommunicationKind::PointToPoint | CommunicationKind::Collective | CommunicationKind::Synchronizing))

#include "Slot.hpp"
#include "src/models/communication/CommunicationKind.hpp"

/**
 * @brief Class containing options to filter the view.
 */
class Filter {
public: // constructors

public: // methods
    /**
     * @brief Returns the kinds of slots that should be rendered.
     * @return The kinds of slots that should be rendered.
     */
    [[nodiscard]] SlotKind getSlotKinds() const;

    /**
     * @brief Sets the slots that should be rendered.
     *
     * Note that SlotKind values can be used as a flag and be combined with a bitwise or.
     * @param slotKinds The kind of slots that should be rendered.
     */
    void setSlotKinds(SlotKind slotKinds);

    /**
     * @brief Returns the kinds of communication that should be rendered.
     * @return The kinds of communication that should be rendered.
     */
    [[nodiscard]] CommunicationKind getCommunicationKinds() const;
    
    /**
     * @brief Sets the communication that should be rendered.
     *
     * Note that CommunicationKind values can be used as a flag and be combined with a bitwise or.
     * @param communicationKinds The kind of communication that should be rendered.
     */
    void setCommunicationKind(CommunicationKind communicationKinds);

    
private: // fields
    SlotKind slotKinds_ = SLOT_FILTER_DEFAULT;
    CommunicationKind communicationKinds_ = COMMUNICATION_FILTER_DEFAULT;
};


#endif //MOTIV_FILTER_HPP
