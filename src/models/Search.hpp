/*
 * Marvelous OTF2 Traces Interactive Visualizer (MOTIV)
 * Copyright (C) 2023 Jessica Lafontaine
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

#ifndef MOTIV_Search_HPP
#define MOTIV_Search_HPP

#include <QListWidget>

#include "src/ui/TraceDataProxy.hpp"

/**
 * @brief A class for handling search functionality
 * @note It holds a distinct list of all slot function names.
 */ 
class Search : QListWidget {
Q_OBJECT
public:
    Search(TraceDataProxy *data, QListWidget *itemList, QWidget *parent=nullptr);
    
    /**
     * @brief Shortens the item list based on the provided string (hides all other items).
     * @param string The string to match
     * @param itemList Pointer to the QListWidget for modification
     */
    void findName(QString string, QListWidget *itemList);

    /**
     * @brief Creates an iteratable item list for "Next" and "Previous" actions (doubly linked).
     * @param name The name to search for
     * @return List of TimedElement pointers for iteration
     */    
    std::list<TimedElement*> createItemList(QString name);


private:
    QListWidget *itemList = nullptr;
    TraceDataProxy* data = nullptr;

};
#endif //MOTIV_Search_HPP