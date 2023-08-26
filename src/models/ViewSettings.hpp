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
#ifndef MOTIV_VIEWSETTINGS_HPP
#define MOTIV_VIEWSETTINGS_HPP

#include "src/models/Slot.hpp"
#include "Filter.hpp"

#include <QIcon>
#include <QCoreApplication>

#define SETTINGS_DEFAULT_ZOOM_QUOTIENT 25
#define SETTINGS_DEFAULT_ROW_HEIGHT 30

/**
 * @brief The ViewSettings class encapsulates settings for the main view.
 *
 * It includes a filter to control which events are displayed and a zoom factor to control
 * the level of detail in the visualization.
 */
class ViewSettings {
public:
    static ViewSettings* getInstance();

    /**
     * @brief Returns the current filter.
     * @return A Filter object representing the current filter.
     */
    [[nodiscard]] Filter getFilter() const;

    /**
     * @brief Returns the reciprocal of the current zoom factor.
     *
     * For example, if the zoom view is zoomed to show 1/50=2%, this function will return 50.
     * @return An integer representing the reciprocal of the current zoom factor.
     */
    [[nodiscard]] int getZoomQuotient() const;

    /**
     * @brief Sets the current filter.
     * @param filter A Filter object representing the new filter.
     */
    void setFilter(Filter filter);

    /**
     * @brief Sets the current zoom factor.
     *
     * The zoom factor defines how much of the actual trace is shown.
     * It is set as the reciprocal. For example, if the view is
     * zoom to show only 2%=1/50 of the whole view, the zoom factor is 50
     *
     * @param zoomFactor An integer representing the new zoom factor.
     */
    void setZoomFactor(int zoomFactor);

    int getRowHeight();

    void setRowHeight(int height);

    void setSearchName(QString name);

    QString getSearchName();

    void setFullTimeTableSlots(std::map<std::string, std::map<otf2::chrono::clock::rep, std::pair<otf2::chrono::clock::rep, std::string>>> fullTimeTableSlots);

    std::map<std::string, std::map<otf2::chrono::clock::rep, std::pair<otf2::chrono::clock::rep, std::string>>>* getFullTimeTableSlots();

    std::map< OTF2_StringRef, std::pair<bool, std::map<std::string, std::pair<int, std::vector<bool>>>>>* getRankThreadMap();

    QIcon* getIcon(std::string key);

    int getFlamegraphRankRef();

    void setFlamegraphRankRef(int rankRef);

private:
    static ViewSettings* instance;    
    ViewSettings();
    ViewSettings(const ViewSettings& obj) = delete;

    /**
     * Backing field for the current zoom factor.
     */
    int zoomFactor_ = SETTINGS_DEFAULT_ZOOM_QUOTIENT;
    /**
     * Backing field for the current filter.
     * */
    Filter filter_;

    int rowHeight = SETTINGS_DEFAULT_ROW_HEIGHT;
    
    std::map< OTF2_StringRef, std::pair<bool, std::map<std::string, std::pair<int, std::vector<bool>>>>> rankThreadMap{};

    std::map<std::string, std::map<otf2::chrono::clock::rep, std::pair<otf2::chrono::clock::rep, std::string>>> fullTimeTableSlots;

    //std::map<> timeTable{};

    QString searchName = "";

    QString executablePath = QCoreApplication::applicationDirPath();

    std::map<std::string, QIcon> Icons_ {
        {std::pair<std::string, QIcon>{"plus", executablePath.chopped(6) + "/res/tango_icons_png/plus.png"}},
        {std::pair<std::string, QIcon>{"plus_grey", executablePath.chopped(6) + "/res/tango_icons_png/plus_grey.png"}},
        {std::pair<std::string, QIcon>{"minus", executablePath.chopped(6) + "/res/tango_icons_png/minus.png"}},
        {std::pair<std::string, QIcon>{"zoom_in", executablePath.chopped(6) + "/res/tango_icons_png/zoom_in.png"}},
        {std::pair<std::string, QIcon>{"zoom_out", executablePath.chopped(6) + "/res/tango_icons_png/zoom_out.png"}},
        {std::pair<std::string, QIcon>{"zoom_fit", executablePath.chopped(6) + "/res/tango_icons_png/zoom_fit.png"}},
        {std::pair<std::string, QIcon>{"search", executablePath.chopped(6)+ "/res/tango_icons_png/search.png"}},
        {std::pair<std::string, QIcon>{"book", executablePath.chopped(6) + "/res/tango_icons_png/book.png"}},
        {std::pair<std::string, QIcon>{"refresh", executablePath.chopped(6) + "/res/tango_icons_png/refresh.png"}},
        {std::pair<std::string, QIcon>{"maximize", executablePath.chopped(6) + "/res/tango_icons_png/maximize.png"}}
    };

    int rankRef;
};


#endif //MOTIV_VIEWSETTINGS_HPP
