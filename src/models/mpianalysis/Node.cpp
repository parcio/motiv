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

#include <iostream>
#include <QRegularExpression>


#include "Node.hpp"

Node::Node(Slot* slot): slot(slot){  
}

bool Node::hasCommunication(){
    if(communication == nullptr) return false;
    else return true;
}

bool Node::hasCollectiveCommunication(){
    if(collectiveCommunication == nullptr) return false;
    else return true;
}

void Node::setCommunication(const Communication *comm){
    this->communication = comm;
}

void Node::setCollectiveCommunication(const CollectiveCommunicationEvent *collectiveCommunication){
    this->collectiveCommunication = collectiveCommunication;
}

void Node::addConnectedNode(Node* node){
    // this->connectedNodes[node->getLocation()] = node;
    this->connectedNodes[node->getLocation()].push_back(node);
}

void Node::addConnectedNode(uint16_t location, std::vector<Node*> nodes){
    this->connectedNodes[location] = nodes;
}

 void Node::addConnectedNode(std::vector<Node*> nodes){
        for(auto node : nodes){
            if(node!=this) this->connectedNodes[node->getLocation()].push_back(node);
    }

 }

Slot* Node::getSlot(){
    return slot;
}

uint64_t Node::getLocation(){
    return this->slot->location->ref();
}

otf2::definition::string Node::getRegionName(){
    return slot->region->name();
}

const Communication* Node::getCommunication(){
   if(this->hasCommunication()) return this->communication;
   else throw std::runtime_error("No communication present");
}

const CollectiveCommunicationEvent* Node::getCollectiveCommunication(){
    if(this->hasCollectiveCommunication()) return this->collectiveCommunication;
    else{
    throw std::runtime_error("No collective communication present");
    }
}

CommunicationKind Node::getCommunicationKind(){
    if(this->hasCommunication()) return this->getOwnEvent()->getKind();
    else if(this->hasCollectiveCommunication()) return this->collectiveCommunication->getKind();
    else throw std::runtime_error("No communication present");
}

std::map<uint64_t, std::vector<Node*>>& Node::getConnectedNodes(){
    if(!connectedNodes.empty()) return connectedNodes;    
    else throw std::runtime_error("connectedNodes vector is empty");
}

uint64_t Node::getConnectedCommunicationRank(){
    if(!this->hasCommunication()) throw std::runtime_error("No p2p communication present");
    else if(communication->getStartEvent()->getLocation()->ref() != this->getLocation()) return communication->getStartEvent()->getLocation()->ref();
    else return communication->getEndEvent()->getLocation()->ref();
}

const CommunicationEvent* Node::getOwnEvent(){
    if(!this->hasCommunication()) throw std::runtime_error("No p2p communication present");
    else if(communication->getStartEvent()->getLocation()->ref() == this->getLocation()) return communication->getStartEvent();
    else return communication->getEndEvent();
}

const CommunicationEvent* Node::getConnectedEvent(){
    if(!this->hasCommunication()) throw std::runtime_error("No p2p communication present");
    else if(communication->getStartEvent()->getLocation()->ref() != this->getLocation()) return communication->getStartEvent();
    else return communication->getEndEvent();
}

// void Node::setColor(){
//     std::string str = this->slot->region->name().str();
//     if(communication != nullptr  || collectiveCommunication != nullptr) {
//        if((this->getCommunicationKind() & CommunicationKind::BlockingPointToPoint) || 
//             (this->getCommunicationKind() == CommunicationKind::Synchronizing)) this->color = Qt::red;
//        else this->color = Qt::blue;
//     }else {
//         QString regionName = QString::fromStdString(this->slot->region->name().str());
//         QString pattern = "(.*)(wait|probe|test)(.*)";
//         QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption);

//         if(regex.match(regionName).hasMatch()) this->color = Qt::darkGray;
//         else this->color = Qt::blue;
//     }
// }

// QColor Node::getColor(){
//     return this->color;
// }

bool Node::operator<(const Node& other) const {
    return slot->endTime < other.slot->endTime;
}