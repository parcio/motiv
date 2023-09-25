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

#include "src/ui/windows/FlamegraphPopup.hpp"

#include <QVBoxLayout>
//#include <QDebug>


FlamegraphPopup::FlamegraphPopup(TraceDataProxy *data, QWidget *parent):QDialog(parent), data(data){
    qInfo() << "FlamegraphPopup ... " << this;
    this->flamegraphWindow = new QDialog(parent);
    this->flamegraphWindow->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    this->flamegraphWindow->setWindowTitle("Flamegraph");
    this->header = new TimelineHeader(this->data, this);
    this->view = new FlamegraphView(this->data, this);
    this->infoBar = new QStatusBar(this);
    QFont font;
    font.setPointSize(10);
    this->infoBar->setFont(font);
    QPalette palette;
    palette.setColor(QPalette::WindowText, QColorConstants::Svg::slategray);
    this->infoBar->setPalette (palette);
    this->infoBar->setFixedHeight(12);
    this->setAttribute(Qt::WA_DeleteOnClose);
}

void FlamegraphPopup::openFlamegraphWindow(){
    qInfo() << "EXECUTING FlamegraphPopup::openFlamegraphWindow ... for " << this;
    auto * settings = ViewSettings::getInstance();
    auto ROW_HEIGHT = settings->getRowHeight();
    auto * rankThreadMap = settings->getRankThreadMap();
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setContentsMargins(0,0,0,0);
    QBrush backgroundPattern = QBrush(QColorConstants::Svg::silver, Qt::Dense7Pattern);

    // Prepare the Layout
    this->view->updateView();
    QVBoxLayout *popupLayout = new QVBoxLayout();
    popupLayout->addWidget(this->header);
    popupLayout->addWidget(this->view);
    this->view->setBackgroundBrush(backgroundPattern);
    popupLayout->addWidget(line);
    //popupLayout->addSpacing(1);
    popupLayout->addWidget(this->infoBar);
    connect(this->view, &FlamegraphView::statusChanged, this, &FlamegraphPopup::updateStatusbar);
    this->flamegraphWindow->setLayout(popupLayout);

    // Set the Window to the right size
    auto sceneHeight = this->view->getGlobalMaxHeight() + rankThreadMap->at(this->view->getRequestedRank()).second.size() * ROW_HEIGHT;
    auto sceneRect = this->view->rect();
    sceneRect.setHeight(sceneHeight);
    sceneRect.setWidth(sceneHeight*1.8);
    this->view->scene()->setSceneRect(sceneRect);

    // Show the Window
    this->flamegraphWindow->show();
}

void FlamegraphPopup::updateStatusbar(){
    qInfo() << "EXECUTING FlamegraphPopup::updateStatusbar ... for " << this;
    //QFontMetrics fm(this->infoBar->font());
    //QString elidedText = fm.elidedText(this->view->statusInfo, Qt::ElideRight, this->view->width()-30);
    this->infoBar->showMessage(this->view->statusInfo,0);
}