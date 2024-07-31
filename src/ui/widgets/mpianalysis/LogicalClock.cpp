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

#include <QGraphicsEllipseItem>
#include <algorithm>

#include "LogicalClock.hpp"
#include "src/ui/Constants.hpp"
#include "src/ui/views/CommunicationIndicator.hpp"
#include "src/ui/views/NodeIndicator.hpp"

//DEBUG INCLUDE
#include <iomanip>
constexpr int X_OFFSET = 50;
constexpr int Y_OFFSET = 80;
constexpr int RADIUS = 15;

const QColor SEND_COLOR("#4A69A6");
const QColor RECV_COLOR("#508B52");
const QColor COLLECTIVES_COLOR("#C32727");
const QColor OTHERS_COLOR("#A9A7A7");

LogicalClock::LogicalClock(TraceDataProxy *data, std::map<uint64_t, std::vector<Node*>> nodes, QWidget *parent): QGraphicsView(parent),
    data(data), nodes(nodes){

    auto scene = new QGraphicsScene();
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setAutoFillBackground(false);
    this->setStyleSheet("background: transparent");
    this->setScene(scene);
    this->populateScene(scene);

}


// Index aliases for accessing elements in pendingCollectives tuples:
// IDX_MAX_NODE_COUNT: Represents the largest node count among members, used for X-axis offset.
// IDX_START_Y: Index for the starting Y-coordinate.
// IDX_END_Y: Index for the ending Y-coordinate.
// IDX_INITIAL_LOCATION: Represents the location of the first occurrence of a specific collective communication.
// IDX_FINISHED_MEMBERS: Used to avoid redundant iterations.

constexpr size_t IDX_MAX_NODE_COUNT = 0;
constexpr size_t IDX_START_Y = 1;
constexpr size_t IDX_END_Y = 2;
constexpr size_t IDX_INITIAL_LOCATION = 3;
constexpr size_t IDX_FINISHED_MEMBERS = 4;

std::map<uint64_t, std::pair<int, int>> DEBUG_COUNTER;

int DEBUG_CALL_COUNTER = 0;

void LogicalClock::populateScene(QGraphicsScene* scene) {
    
    // Build nodes_ map with finishing state for each node.
    std::map<uint64_t, std::vector<std::pair<Node*, bool>>> nodes_;
    std::pair<Node*, Node*> matchingNodes;

    for (auto &item : nodes) {
        for(auto node: item.second){
            auto location = node->getSlot()->location->ref();
            if(nodes_.find(location) == nodes_.end()) nodes_[location] = std::vector<std::pair<Node*, bool>>();
            nodes_[location].push_back(std::make_pair(node, false));
        }
    }

    // Map contains x-axis offset(first) and vector index(second) by location
    std::map<uint64_t, std::pair<int,int>> nodeCountMap;        
    // Tupel: (x, y, count)   
    std::map<const Communication*, std::tuple<int, int, int>> pendingEdges; 
    std::map<const CollectiveCommunicationEvent*, std::tuple<int, int, int, uint64_t, std::set<uint16_t>>> pendingCollectives;
    
    drawNodes(nodes_, nodeCountMap, pendingEdges, pendingCollectives, scene);

    for(size_t i = 0; i < nodeCountMap.size(); ++i){
        QGraphicsLineItem* line = scene->addLine(0, i * Y_OFFSET, 1080, i * Y_OFFSET);
        line->setZValue(-1); 

        QString label = QString("Rank: %1").arg(i);
        QGraphicsTextItem* textItem = new QGraphicsTextItem(label);
        textItem->setPos(-textItem->boundingRect().width() - 10, i * Y_OFFSET - textItem->boundingRect().height() / 2);
        textItem->setZValue(-1);
        scene->addItem(textItem);
    }

    for(const auto& collectivCommunication: pendingCollectives){        
        if(!(collectivCommunication.first->getKind() == CommunicationKind::Synchronizing)) continue;
        int x = (std::get<IDX_MAX_NODE_COUNT>(collectivCommunication.second) * X_OFFSET);
        int yStart = std::get<IDX_START_Y>(collectivCommunication.second);
        int yEnd = std::get<IDX_END_Y>(collectivCommunication.second);

        QGraphicsLineItem* line = scene->addLine(x, yStart, x, yEnd);
        line->setPen(QPen(Qt::red));
    }
    
    // for(const auto& item: DEBUG_COUNTER){
    //     std::cout << "Location: " << item.first 
    //             << " P2P: " << item.second.first 
    //             << " Collectives: " << item.second.second << std::endl;
    // }
}

void LogicalClock::drawNodes(std::map<uint64_t, std::vector<std::pair<Node*, bool>>>& nodes_,
    std::map<uint64_t, std::pair<int,int>>& nodeCountMap,
    std::map<const Communication*, std::tuple<int, int, int>>& pendingEdges,
    std::map<const CollectiveCommunicationEvent*, std::tuple<int, int, int, uint64_t, std::set<uint16_t>>>& pendingCollectives,
    QGraphicsScene* scene,
    uint64_t location_,
    std::pair<Node*,Node*>* matchingNodes)
    {
    DEBUG_CALL_COUNTER++;
    auto onTimedElementSelected = [this](TimedElement *element) { this->data->setTimeElementSelection(element); };
   
    // Begin Iteration at specific location, default = 0
    auto item = nodes_.lower_bound(location_);
    for(; item!= nodes_.end(); ++item){
        auto location = item->first;
        if(nodeCountMap.find(location)==nodeCountMap.end()){ 
            nodeCountMap[location].first = 1;
            nodeCountMap[location].second = 0;
        }
        int& count = nodeCountMap[location].first;     

        // Continue node vector iteration at last position 
        for(size_t j = nodeCountMap[location].second; j < item->second.size(); ++j){
            auto& node = item->second[j].first;
            auto nodeFinishState = item->second[j].second;
            if(nodeFinishState) continue;
            QColor color = OTHERS_COLOR;

            auto region = QString::fromStdString(node->getRegionName().str());            
            auto DEBUG_REGION = region.toStdString();
            if(region.contains("Init") || region.contains("Comm") || region.contains("Group") || region.contains("Finalize")) {
                nodeFinishState = true;
                nodeCountMap[location].second++;
                continue; //workaround
                }

            if ((region.contains("send", Qt::CaseInsensitive) || region.contains("recv", Qt::CaseInsensitive)) && !(node->hasCommunication())) color = Qt::red;
            int y = (location * Y_OFFSET) - RADIUS;
            int x = (count * X_OFFSET) - RADIUS;

            
            // Node is P2P communication
            if(node->hasCommunication()){
                /* DEBUG INFO DELETE
                auto commStartTime = std::chrono::duration<double>(node->getOwnEvent()->getStartTime()).count();                
                auto commEndTime = std::chrono::duration<double>(node->getOwnEvent()->getEndTime()).count(); 
                auto slotStartTime = std::chrono::duration<double>(node->getSlot()->getStartTime()).count();
                auto slotEndTime =  std::chrono::duration<double>(node->getSlot()->getEndTime()).count();               
               
                std::cout << location << " " << node->getRegionName().str() << "     ";
                    std::cout << std::fixed << std::setprecision(6) << slotStartTime << "     ";
                    std::cout << std::fixed << std::setprecision(6) << slotEndTime << "     ";
                    std::cout << std::fixed << std::setprecision(6) << commStartTime << "     ";
                    std::cout << std::fixed << std::setprecision(6) << commEndTime <<  std::endl;             
                */
               
                // Set node color
                if(region.contains("send", Qt::CaseInsensitive)) color = SEND_COLOR;               
                else color = RECV_COLOR; 

                // Preparing offset for node
                uint64_t connectedNodeRank = node->getConnectedCommunicationRank();
                
                if(nodeCountMap.find(connectedNodeRank)==nodeCountMap.end()){
                    nodeCountMap[connectedNodeRank].first = 1;
                    nodeCountMap[connectedNodeRank].second = 0;
                }
                
                if(pendingEdges.find(node->getCommunication()) == pendingEdges.end()){
                    // Blocking communication needs a specific x-axis-offset
                    if(node->getCommunicationKind() & CommunicationKind::BlockingPointToPoint) {
                        // node define offset because is start event
                        if(node->getOwnEvent() == node->getCommunication()->getStartEvent()) {
                            // nodeCountMap[connectedNodeRank].first = std::max(nodeCountMap[connectedNodeRank].first, (count+1));
                            pendingEdges[node->getCommunication()] = std::make_tuple(x, y, std::max(nodeCountMap[connectedNodeRank].first, (count+1)));
                        }                        
                        // Node has to wait for offset from connected node
                        else{
                            if(!node->getConnectedNodes().empty()){
                                auto matchingNodes = std::make_pair(node, node->getConnectedNodes()[connectedNodeRank][0]);
                                drawNodes(nodes_, nodeCountMap, pendingEdges, pendingCollectives, scene, connectedNodeRank, &matchingNodes); 
                            }
                            //TODO WARNUNG PRINTEN
                            else drawNodes(nodes_, nodeCountMap, pendingEdges, pendingCollectives, scene, connectedNodeRank);                            
                        }
                        if(nodeFinishState) continue;

                        // overwrite x, maybe count has changed!                       
                        x = (count * X_OFFSET) - RADIUS;
                    }
                    
                    if(pendingEdges.find(node->getCommunication()) == pendingEdges.end()) pendingEdges[node->getCommunication()] = std::make_tuple(x, y, std::max(nodeCountMap[connectedNodeRank].first, (count+1)));
                }

                // Preparing coordinates for edges             
                // if(node->getOwnEvent() == node->getCommunication()->getEndEvent() && !(nodeFinishState)){
                
                // Calculate Offset for Blocking-Endevent
                if(node->getCommunicationKind() & CommunicationKind::BlockingPointToPoint) {                       
                    if(node->getOwnEvent() == node->getCommunication()->getEndEvent()) {
                        count = std::max(count, std::get<2>(pendingEdges[node->getCommunication()]));
                    }
                }

                uint64_t pendingY = std::get<1>(pendingEdges[node->getCommunication()]);
                uint64_t pendingLocation = (pendingY + RADIUS)/Y_OFFSET;
                if(pendingLocation != location || connectedNodeRank == location){
                    x = (nodeCountMap[location].first * X_OFFSET) - RADIUS;
                    
                    auto nonConstCommunication = const_cast<Communication*>(node->getCommunication());
                    qreal fromX = x + RADIUS;
                    qreal fromY = y + RADIUS;

                    qreal toX = std::get<0>(pendingEdges[node->getCommunication()]) + RADIUS;
                    qreal toY = std::get<1>(pendingEdges[node->getCommunication()]) + RADIUS;

                    if(!region.contains("send", Qt::CaseInsensitive)){
                        fromX = toX;
                        fromY = toY;

                        toX = x + RADIUS;
                        toY = y + RADIUS;
                    }

                    auto arrow = new CommunicationIndicator(nonConstCommunication, fromX, fromY, toX, toY);
                    arrow->setZValue(layers::Z_LAYER_P2P_COMMUNICATIONS);
                    arrow->setPen(QPen(Qt::black));
                    arrow->setOnSelected(onTimedElementSelected);
                    scene->addItem(arrow);                 
                }

                if(matchingNodes != nullptr){
                    auto initialNodeRegion = QString::fromStdString(matchingNodes->first->getRegionName().str());
                    if(initialNodeRegion.contains("wait",Qt::CaseInsensitive)) {
                        auto initialNodeLocation = matchingNodes->first->getLocation();
                        nodeCountMap[initialNodeLocation].first = std::max(count+1, nodeCountMap[initialNodeLocation].first);
                    }
                }
            }
            
            
            // Node is collective communication 
            else if(node->hasCollectiveCommunication()){
                color = COLLECTIVES_COLOR;          
                const auto collectiveCommunication = node->getCollectiveCommunication();
                int lineLength = 2*RADIUS;
                int yStart = y;
                int yEnd = yStart + lineLength;

                if(pendingCollectives.find(collectiveCommunication) == pendingCollectives.end()) pendingCollectives[collectiveCommunication] = std::make_tuple(count, yStart, yEnd, location, std::set<uint16_t>());
                std::set<uint16_t>& membersSet = std::get<IDX_FINISHED_MEMBERS>(pendingCollectives[collectiveCommunication]);

                std::get<IDX_MAX_NODE_COUNT>(pendingCollectives[collectiveCommunication]) = std::max(count, std::get<IDX_MAX_NODE_COUNT>(pendingCollectives[collectiveCommunication]));          
                std::get<IDX_START_Y>(pendingCollectives[collectiveCommunication]) = std::min(yStart, std::get<IDX_START_Y>(pendingCollectives[collectiveCommunication]));            
                std::get<IDX_END_Y>(pendingCollectives[collectiveCommunication]) = std::max(yEnd, std::get<IDX_END_Y>(pendingCollectives[collectiveCommunication]));
                
                membersSet.insert(location);

                // auto collectiveType = collectiveCommunication->getOperation();
                // if(collectiveType == otf2::collective_type::reduce){
                //     std::cout << "DEBUG HERE" << std::endl;
                // }
                
                bool isBlockingRoot = ((!region.contains("MPI_I")) && location == collectiveCommunication->getRoot());
                if(node->getCommunicationKind()==CommunicationKind::Synchronizing || isBlockingRoot ){
                    if(location_ == std::get<IDX_INITIAL_LOCATION>(pendingCollectives[node->getCollectiveCommunication()]) || isBlockingRoot){
                        auto members = collectiveCommunication->getMembers();

                        // std::sort(members.begin(), members.end(), 
                        //     [](const CollectiveCommunicationEvent::Member* a, const CollectiveCommunicationEvent::Member* b) {
                        //         return a->getStart() < b->getStart();
                        //     });

                        for(auto &member: members){
                            auto memberLocation = member->getLocation()->ref();
                            if(membersSet.find(memberLocation) == membersSet.end()){
                                if(!node->getConnectedNodes().empty()){                                
                                    auto matchingNodes = std::make_pair(node, node->getConnectedNodes()[memberLocation][0]);
                                    drawNodes(nodes_, nodeCountMap, pendingEdges, pendingCollectives, scene, memberLocation, &matchingNodes);
                                } else drawNodes(nodes_, nodeCountMap, pendingEdges, pendingCollectives, scene, memberLocation);
                            }                                 
                        }
                    }
                }
                
                std::get<IDX_MAX_NODE_COUNT>(pendingCollectives[collectiveCommunication]) = std::get<IDX_MAX_NODE_COUNT>(pendingCollectives[collectiveCommunication]);                 
                std::get<IDX_START_Y>(pendingCollectives[collectiveCommunication]) = std::min(yStart, std::get<IDX_START_Y>(pendingCollectives[collectiveCommunication]));                
                std::get<IDX_END_Y>(pendingCollectives[collectiveCommunication]) = std::max(yEnd, std::get<IDX_END_Y>(pendingCollectives[collectiveCommunication]));             
                                
                count = std::get<IDX_MAX_NODE_COUNT>(pendingCollectives[node->getCollectiveCommunication()]);
                x = (count * X_OFFSET) - RADIUS;
                count++;

            }
           
            // else if(region.contains("wait", Qt::CaseInsensitive)){
            //     for(auto waitingNode : node->getConnectedNodes()[location]){
            //         auto waitingNodeRegion_ = waitingNode->getRegionName().str();
            //         std::cout << "[" << waitingNode->getLocation() << "] "<< waitingNodeRegion_ << std::endl;
            //         // Wait refers to P2P communication
            //         if(waitingNode->hasCommunication()){
            //             auto connectedNodeRank = waitingNode->getConnectedCommunicationRank();
            //             auto connectedNode = waitingNode->getConnectedNodes()[connectedNodeRank][0];                       

            //             // if(waitingNode->getOwnEvent()->getEndTime() > waitingNode->getConnectedEvent()->getEndTime()) {
            //             // if(waitingNode->getOwnEvent() == waitingNode->getCommunication()->getStartEvent()){
            //             //     nodeCountMap[connectedNodeRank].first = std::max(nodeCountMap[connectedNodeRank].first, (count+1));
            //             // } else{
            //             //     auto matchingNodes = std::make_pair(node, connectedNode);
            //             //     drawNodes(nodes_, nodeCountMap, pendingEdges, pendingCollectives, scene, connectedNodeRank, &matchingNodes);
            //             // }

            //             if(waitingNode->getOwnEvent() == waitingNode->getCommunication()->getStartEvent()) {
            //                 auto matchingNodes = std::make_pair(node, connectedNode);
            //                 drawNodes(nodes_, nodeCountMap, pendingEdges, pendingCollectives, scene, connectedNodeRank, &matchingNodes);                            
            //             }

            //             count = std::max(count,nodeCountMap[location].first);
            //             // overwrite x, maybe count has changed!                       
            //             x = (count * X_OFFSET) - RADIUS;
            //         }
            //     }

            // }
            
            if(nodeFinishState) continue;
                        
            auto ellipseItem = new NodeIndicator(node->getSlot(), x, y, 2*RADIUS, 2*RADIUS);
            //ellipseItem->setOnDoubleClick(onTimedElementDoubleClicked);
            ellipseItem->setOnSelected(onTimedElementSelected);

            // QBrush brush(node->getColor());
            QBrush brush(color);
            ellipseItem->setBrush(brush);

            QString hoverText = region;
            ellipseItem->setToolTip(hoverText);
            ellipseItem->setAcceptHoverEvents(true);

            if(node->hasCommunication() && (node->getCommunicationKind() & CommunicationKind::BlockingPointToPoint)){
                QPen pen(Qt::red);
                // pen.setWidth(2);
                ellipseItem->setPen(pen);
            }

            // if(node->hasCommunication() && (node->getCommunicationKind() & CommunicationKind::NonBlockingPointToPoint)){
            //     QGraphicsEllipseItem* coverPattern = new QGraphicsEllipseItem(x, y, 2*RADIUS, 2*RADIUS);
            //     QPixmap patternPixmap(16, 16); 
            //     patternPixmap.fill(color); 

            //     QBrush patternBrush(patternPixmap);
            //     patternBrush.setStyle(Qt::DiagCrossPattern);
            //     coverPattern->setBrush(patternBrush);
            //     scene->addItem(coverPattern);
            //     coverPattern->setZValue(5);
            // }           

            nodeFinishState = true;
            nodeCountMap[location].second++;
            if(node->hasCollectiveCommunication()){
                if(!(node->getCommunicationKind()==CommunicationKind::Synchronizing)) scene->addItem(ellipseItem);

                if(matchingNodes !=nullptr){
                    auto initialLocation = std::get<IDX_INITIAL_LOCATION>(pendingCollectives[node->getCollectiveCommunication()]);
                    if(matchingNodes->first->hasCollectiveCommunication()){                        
                        if(node == matchingNodes->second && initialLocation != location_) return;    
                    }                
                }

            }else{
                if(matchingNodes !=nullptr){
                    scene->addItem(ellipseItem);
                   
                    if(node == matchingNodes->second && location_ != 0){
                        count++;
                        return;
                    }
                }else scene->addItem(ellipseItem);
            count++;
            }    
        }
    }

}