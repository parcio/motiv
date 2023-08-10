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
#include "ScrollSynchronizer.hpp"

#include <QScrollBar>

// Debug #todo: remove when no longer needed
#include <QDebug>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QAbstractItemView>

ScrollSynchronizer::ScrollSynchronizer(QObject *parent) : QObject(parent) {

}

void ScrollSynchronizer::addWidget(QAbstractScrollArea *newWidget) {
    if (this->widgets.contains(newWidget)) {
        return;
    }
    for (const auto &widget : this->widgets) {
        // Classic
        //connect(widget->verticalScrollBar(), &QScrollBar::valueChanged, newWidget->verticalScrollBar(), &QScrollBar::setValue);
        //connect(newWidget->verticalScrollBar(), &QScrollBar::valueChanged, widget->verticalScrollBar(), &QScrollBar::setValue);

        // Variant 2
        connect(widget->verticalScrollBar(), SIGNAL(valueChanged(int)), newWidget->verticalScrollBar(), SLOT(setValue(int))); 
        connect(newWidget->verticalScrollBar(), SIGNAL(valueChanged(int)), widget->verticalScrollBar(), SLOT(setValue(int)));
        qInfo() << "scroll areas connected :)";
    }
    this->widgets.push_back(newWidget);
}

void setScrollBarValueToOne(QScrollBar* scrollBar)
{
    scrollBar->setValue(1);
}