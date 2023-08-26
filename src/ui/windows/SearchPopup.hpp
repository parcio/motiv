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

#ifndef MOTIV_SearchPopup_HPP
#define MOTIV_SearchPopup_HPP

#include <QDialog>
#include <QListWidget>
#include <QMouseEvent>
#include <QString>

#include "src/ui/widgets/InformationDock.hpp"

enum ButtonType {
    NONE,
    PREV,
    NEXT
};

/**
 * @brief Manages a popup window for searching and highlighting slots in the user interface.
 */
class SearchPopup : public QDialog{
Q_OBJECT

public:
    SearchPopup(TraceDataProxy *data, InformationDock *information, QWidget *parent = nullptr);
   
public: Q_SIGNALS:

    /**
     * @brief Signal indicating that the search button in the main window was pressed
     */
   void searchButtonPressed();

    /**
     * @brief Signal indicating that the previous button in the search window was pressed
     */
   void prevButtonPressed();

    /**
     * @brief Signal indicating that the next button in the search window was pressed
     */
   void nextButtonPressed();

    /**
     * @brief Signal representing whether an item is selected or not.
     *
     * @param selected True if an item is selected
     * @note This signal manages the interactivity of the "next" and "prev" buttons.
     */
   void itemSelected(bool selected);

private Q_SLOTS:

    /**
     * @brief Displays the search popup window.
     */ 
    void openSearchWindow();

    /**
     * @brief Handles enabling or disabling "next" and "prev" buttons.
     */ 
    void selectSlot(ButtonType state = ButtonType::NONE);

private:

    /**
     * @brief Builds the search window. 
     */ 
    void buildWindow(QWidget* parent);


private:
    QDialog *searchWindow = nullptr;
    QListWidgetItem *selectedItem_ = nullptr;
    std::list<TimedElement*> slotList;
    std::list<TimedElement*>::iterator currentIterator;
    bool firstNextUse = true;
    
    TraceDataProxy *data;
    InformationDock *information;



    
};

#endif //MOTIV_SearchPopup_HPP