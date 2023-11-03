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

#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "src/models/Search.hpp"
#include "src/ui/windows/SearchPopup.hpp"

SearchPopup::SearchPopup(TraceDataProxy *data, InformationDock *information, QWidget *parent):data(data), information(information), QDialog(parent){
   this->buildWindow(parent);
}

void SearchPopup::buildWindow(QWidget *parent){
 // Build search window
    this->searchWindow = new QDialog(parent);
    searchWindow->setWindowFlags(Qt::Window);
    searchWindow->setWindowTitle("Search");

    auto layout = new QVBoxLayout(searchWindow);

    auto searchEdit = new QLineEdit(searchWindow);
    searchEdit->setPlaceholderText("search");
    layout->addWidget(searchEdit);

    auto itemList = new QListWidget(searchWindow);
    itemList->setSelectionMode(QAbstractItemView::NoSelection);

    
    auto search_ = new Search(data, itemList, parent);
    layout->addWidget(itemList);
    
    searchWindow->setLayout(layout);

    auto buttonLayout = new QHBoxLayout(searchWindow);
    layout->addLayout(buttonLayout);
    
    // Next and previous buttons
    auto prevButton = new QPushButton(tr("prev"), this);
    prevButton->setEnabled(false);
    buttonLayout->addWidget(prevButton);

    auto nextButton = new QPushButton(tr("next"),this);
    nextButton->setEnabled(false);
    buttonLayout->addWidget(nextButton);

    // Enables or disables the buttons
    connect(this, &SearchPopup::itemSelected, this, [this, prevButton, nextButton](bool selected) {
        if (selected){
            prevButton->setEnabled(true);
            nextButton->setEnabled(true);

            prevButton->setStyleSheet("");
            nextButton->setStyleSheet("");             
        } else {
            prevButton->setEnabled(false);
            nextButton->setEnabled(false);

            prevButton->setStyleSheet("background-color: #d3d3d3;");
            nextButton->setStyleSheet("background-color: #d3d3d3;");
        }
    });

    // Zooms into the previous found slot
    connect(prevButton, &QPushButton::clicked ,this, [this]{
        selectSlot(ButtonType::PREV);
        auto element_= *currentIterator;
        
        //Q_EMIT this->information->zoomToWindow((*currentIterator)->getStartTime(), (*currentIterator)->getEndTime());
        auto padding = element_->getDuration() / 10;
        Q_EMIT this->information->zoomToWindow(element_->getStartTime() - padding, element_->getEndTime() + padding);
    });

    // Zooms into the next found slot   
    connect(nextButton,&QPushButton::clicked ,this, [this]{
        selectSlot(ButtonType::NEXT);
        auto element_= *currentIterator;
        
        //Q_EMIT this->information->zoomToWindow((*currentIterator)->getStartTime(), (*currentIterator)->getEndTime());
        auto padding = element_->getDuration() / 10;
        Q_EMIT this->information->zoomToWindow(element_->getStartTime() - padding, element_->getEndTime() + padding);
    });  


    // Connects    
    // Opens the search popup window
    connect(this, SIGNAL(searchButtonPressed()), this, SLOT(openSearchWindow()));
    
    // Updates the list of found elements based on the input
    connect(searchEdit, &QLineEdit::textChanged, this, [searchEdit, itemList, search_] {
        QString text = searchEdit->text();
        search_->findName(text, itemList);
    });

    // Handles highlighting of selected element
    connect(itemList, &QListWidget::itemPressed, this, [this, itemList]{
        if(this->selectedItem_ == nullptr || itemList->currentItem()->text()!=selectedItem_->text()){           
            if(this->selectedItem_!= nullptr) this->selectedItem_->setForeground(Qt::black);            
            this->selectedItem_ = itemList->currentItem();
            this->selectedItem_->setForeground(Qt::blue);

            ViewSettings::getInstance()->setSearchName(itemList->currentItem()->text());
            this->firstNextUse=true;

            auto search_ = new Search(data, itemList, this);
            this->slotList = search_->createItemList(itemList->currentItem()->text());
            this->currentIterator=slotList.begin();
            this->selectSlot();
            Q_EMIT this->data->refreshButtonPressed();
        }
        else{
            this->selectedItem_->setForeground(Qt::black);     
            this->selectedItem_ = nullptr;
            this->information->setElement(nullptr);
            ViewSettings::getInstance()->setSearchName("");
            Q_EMIT this->data->refreshButtonPressed();
            Q_EMIT this->itemSelected(false);
        }
    });

    // Handles highlighting with "Enter" key in search box
    connect(searchEdit,  &QLineEdit::returnPressed, this, [this, searchEdit]{        
        ViewSettings::getInstance()->setSearchName(searchEdit->text());
        Q_EMIT this->data->refreshButtonPressed();
     });

    // Remove highlighting when the search window is closed
    connect(searchWindow, &QDialog::finished, this, [this]{
        ViewSettings::getInstance()->setSearchName("");
        Q_EMIT this->data->refreshButtonPressed();
        Q_EMIT this->itemSelected(false);
    });
}

void SearchPopup::selectSlot(ButtonType buttenType){
        switch(buttenType){
            //none
            case ButtonType::NONE: 
                information->setElement(*currentIterator);
                Q_EMIT itemSelected(true);
                break;

            //prev
            case ButtonType::PREV:
                if(currentIterator!=slotList.begin()) --currentIterator;              
                else currentIterator=std::prev(slotList.end());
                information->setElement(*currentIterator);
                Q_EMIT itemSelected(true);                
                break;

            //next
            case ButtonType::NEXT:                
                if(currentIterator!=std::prev(slotList.end()) && firstNextUse!=true) ++currentIterator;
                else currentIterator=slotList.begin();
                information->setElement(*currentIterator);
                firstNextUse=false;
                Q_EMIT itemSelected(true);                
                break;
        }   
}

void SearchPopup::openSearchWindow(){
    this->searchWindow->show();
    this->searchWindow->activateWindow();
}