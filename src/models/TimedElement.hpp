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
#ifndef MOTIV_TIMEDELEMENT_HPP
#define MOTIV_TIMEDELEMENT_HPP

#include "src/types.hpp"

/**
 * @brief A base class for all elements with a start and end time
 */
class TimedElement {
public:
    virtual ~TimedElement() = default;

public:
    /**
     * @brief Returns the start time of the current object.
     *
     * This pure virtual function returns the start time of the current object. The function has to be implemented in the derived classes.
     *
     * @return The start time of the current object.
     */
    [[nodiscard]] virtual types::TraceTime getStartTime() const = 0;

    /**
     * @brief Returns the end time of the current object.
     *
     * This pure virtual function returns the end time of the current object. The function has to be implemented in the derived classes.
     *
     * @return The end time of the current object.
     */
    [[nodiscard]] virtual types::TraceTime getEndTime() const = 0;

    /**
     * @brief Returns the duration of the current object.
     *
     * This function calculates and returns the duration of the current object, which is the difference between its end time and start time. The function can be overridden in the derived classes if necessary.
     *
     * @return The duration of the current object.
     */
    [[nodiscard]] virtual types::TraceTime getDuration() const { return getEndTime() - getStartTime(); }
};

#endif //MOTIV_TIMEDELEMENT_HPP
