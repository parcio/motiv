/*
 * Marvelous OTF2 Traces Interactive Visualizer (MOTIV)
 * Copyright (C) 2023 Tomas Cirkov
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

#ifndef MOTIV_FlamegraphPopup_HPP
#define MOTIV_FlamegraphPopup_HPP

#include "src/ui/views/FlamegraphView.hpp"
#include "src/ui/TraceDataProxy.hpp"
#include "src/ui/widgets/TimelineHeader.hpp"

#include <QDialog>
#include <QString>
#include <QStatusBar>

class FlamegraphPopup : public QDialog{
Q_OBJECT

public:
    FlamegraphPopup(TraceDataProxy *data, QWidget *parent = nullptr);
    void openFlamegraphWindow();
    void updateStatusbar();

private:
    QDialog *flamegraphWindow = nullptr;
    TraceDataProxy *data = nullptr;
    TimelineHeader *header = nullptr;
    FlamegraphView *view = nullptr;
    QStatusBar *infoBar = nullptr;
};

#endif //MOTIV_FlamegraphPopup_HPP