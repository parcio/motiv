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

#ifndef MOTIV_NODE_HPP
#define MOTIV_NODE_HPP

#include <QColor>

#include "src/models/communication/CollectiveCommunicationEvent.hpp"
#include "src/models/communication/Communication.hpp"
#include "src/models/Slot.hpp"

class Node {
public:
    Node(Slot* slot);

public: //methods

// Slot related functions
Slot* getSlot();
uint64_t getLocation();
otf2::definition::string getRegionName();


// Communication related functions
bool hasCommunication();
bool hasCollectiveCommunication();

void setCommunication(const Communication *communication);
void setCollectiveCommunication(const CollectiveCommunicationEvent *collectiveCommunication);

const Communication* getCommunication();
const CollectiveCommunicationEvent* getCollectiveCommunication();
CommunicationKind getCommunicationKind();

// only for Nodes with P2P communication
uint64_t getConnectedCommunicationRank();
const CommunicationEvent* getOwnEvent();
const CommunicationEvent* getConnectedEvent();


// Node related functions
void addConnectedNode(Node* node);
void addConnectedNode(uint16_t location, std::vector<Node*> nodes);
void addConnectedNode(std::vector<Node*> nodes);

// void setColor();
// QColor getColor();

std::map<uint64_t, std::vector<Node*>>& getConnectedNodes();

// Custom comparison operator to sort Node objects in ascending order of endTime
bool operator<(const Node& other) const;

private: //data
    Slot *slot;
    // QColor color;

    std::map<uint64_t, std::vector<Node*>> connectedNodes;

    const Communication *communication = nullptr;

    // Exists only for collective communication Nodes
    const CollectiveCommunicationEvent *collectiveCommunication = nullptr;
};

#endif //MOTIV_NODE_HPP