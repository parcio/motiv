/*
 * Marvelous OTF2 Traces Interactive Visualizer (MOTIV)
 * Copyright (C) 2023 Florian Gallrein, Björn Gehrke, Jessica Lafontaine
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
#ifndef MOTIV_APPSETTINGS_HPP
#define MOTIV_APPSETTINGS_HPP


#include <QSettings>
#include <QString>

// Swap mode ID to run Motiv in alternative mode by default. 
// Also, update mode control in main.cpp.
enum Mode{
    Default = 0,
    MPI_Analysis = 1
};


/**
 * @brief Singleton holding persistent information.
 * 
 * All config files are stored in /home/.config/Motiv
 */
class AppSettings : public QObject {
Q_OBJECT
private:
    AppSettings();

public:
    /**
     * @return instance of AppSettings
     */
    static AppSettings &getInstance() {
        static AppSettings instance;
        return instance;
    }

    // Do not implement copy and copy assignment constructor.
    AppSettings(AppSettings const &) = delete;
    void operator=(AppSettings const &) = delete;


    /**
     * @brief Returns the recently opened files
     * @return The recently opened files
     */
    [[nodiscard]] const QStringList &recentlyOpenedFiles() const;

    /**
     * @brief Pushes a new file to the recently opened files list
     *
     * After the file is added to the list is saved.
     *
     * @param newFile The new opened file
     */
    void recentlyOpenedFilesPush(const QString &newFile);

    /**
     * @brief Removes a file from the recently opened files list
     *
     * After the file is removed the list is saved
     *
     * @param filePath The file to be removed
     */
    void recentlyOpenedFilesRemove(const QString &filePath);

    /**
     * @brief Clears the recently opened files list
     *
     * After the list is cleared it is saved
     */
    void recentlyOpenedFilesClear(const QString);

    /**
     * @brief Sets the color configfile name based on a file path
     */
    void setColorConfigName(QString&);

    /**
     * @brief Loads the colors from both the trace specific and the global config files or creates them if not found.
     * @note The trace specific config file is loaded first, followed by the global config file.
     */
    void loadColorConfigs();

    /**
     * @brief Saves the current color settings to the global color config
     */
    void saveAsGlobalColors();

    /**
     * @brief Adds a color-function pair to the config file
     */
    void colorConfigPush(QString, QColor);

    /**
     * @brief Overwrites the current color configuration with the global colors
     */
    void loadGlobalColors();

    /**
     * @brief Clears the color config file
     */
    void clearColorConfig();

    bool getuseGlobalColorConfig();

    void setMode(Mode mode);

    Mode getMode();
    
public: Q_SIGNALS:
    /**
     * @brief Signals a change in the recently opened files
     */
    void recentlyOpenedFilesChanged(QStringList);

public Q_SLOTS:
    /**
     * @brief A slot that toggles the global color option in the app settings
     *
     * @param checked True if the global checkbox is checked, false otherwise
     */
    void toggleGlobalColorConfig(bool);

private:
    QSettings *settings = new QSettings ("Motiv/Motiv");
    QSettings *globalColorSettings = new QSettings ("Motiv/colors/GlobalColors");
    QSettings *colorSettings;
    QString leastRecentDirectory_;
    QStringList recentlyOpenedFiles_;
    QString colorConfigName_;
    bool useGlobalColorConfig = false;
    Mode mode = Default;
};


#endif //MOTIV_APPSETTINGS_HPP
