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
#ifndef MOTIV_SETTINGSPOPUP_HPP
#define MOTIV_SETTINGSPOPUP_HPP

#include "src/ui/TraceDataProxy.hpp"

#include <QDialog>
#include <QGroupBox>
#include <QCheckBox>

/**
 * @brief A simple popup showing performance options.
 *
 * In this popup, all possible performance settings can be changed. The relevant data is stored in ViewSettings, which than is considered during the render process in TimelineView or FlamegraphView.
 */
class SettingsPopup : public QDialog {
Q_OBJECT

public: // constructors
    explicit SettingsPopup(TraceDataProxy *data, QWidget *parent = nullptr);
    void checkSettings();

private: // widgets
    //QDialog *PerformancePopupWindow = nullptr;
    TraceDataProxy *data = nullptr;

    // Main window performance settings
    QCheckBox *countIndicatorsREG = nullptr;
    QCheckBox *countIndicatorsP2P = nullptr;
    QCheckBox *countIndicatorsCCM = nullptr;

    // Main window general settings
    QCheckBox *useRealWidthMainWindow = nullptr;

    // Flamegraph performance settings
    QCheckBox *countIndicatorDetailsFlamegraph = nullptr;
    QCheckBox *useThresholdFlamegraph = nullptr;

    // Flamegraph general settings
    QCheckBox *useRealWidthFlamegraph = nullptr;

    // Overview
    QCheckBox *useBorderOverview = nullptr;
    QCheckBox *usePriorityOverview = nullptr;

    // Misc
    QCheckBox *colorCodingTimeRecords = nullptr;
};


#endif //MOTIV_SETTINGSPOPUP_HPP
