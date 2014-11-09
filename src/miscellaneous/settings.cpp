// This file is part of RSS Guard.
//
// Copyright (C) 2011-2014 by Martin Rotter <rotter.martinos@gmail.com>
//
// RSS Guard is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RSS Guard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RSS Guard. If not, see <http://www.gnu.org/licenses/>.

#include "miscellaneous/settings.h"

#include "miscellaneous/application.h"
#include "miscellaneous/iofactory.h"

#include <QDebug>
#include <QDir>
#include <QPointer>
#include <QWebSettings>


// Feeds.
DKEY Feeds::ID                            = "feeds";

DKEY Feeds::UpdateTimeout                 = "feed_update_timeout";
DVALUE(int) Feeds::UpdateTimeoutDef       = DOWNLOAD_TIMEOUT;

DKEY Feeds::CountFormat                   = "count_format";
DVALUE(char*) Feeds::CountFormatDef       = "(%unread)";

DKEY Feeds::AutoUpdateInterval            = "auto_update_interval";
DVALUE(int) Feeds::AutoUpdateIntervalDef  = DEFAULT_AUTO_UPDATE_INTERVAL;

DKEY Feeds::AutoUpdateEnabled             = "auto_update_enabled";
DVALUE(bool) Feeds::AutoUpdateEnabledDef  = false;

DKEY Feeds::FeedsUpdateOnStartup            = "feeds_update_on_startup";
DVALUE(bool) Feeds::FeedsUpdateOnStartupDef = false;

// Messages.
DKEY Messages::ID                            = "messages";

DKEY Messages::UseCustomDate                 = "use_custom_date";
DVALUE(bool) Messages::UseCustomDateDef      = false;

DKEY Messages::CustomDateFormat              = "custom_date_format";
DVALUE(char*) Messages::CustomDateFormatDef  = "";

DKEY Messages::ClearReadOnExit               = "clear_read_on_exit";
DVALUE(bool) Messages::ClearReadOnExitDef    = false;

// GUI.
DKEY GUI::ID                                      = "gui";

DKEY GUI::SplitterFeeds                           = "splitter_feeds";
DVALUE(char*) GUI::SplitterFeedsDef               = "";

DKEY GUI::SplitterMessages                        = "splitter_messages";
DVALUE(char*) GUI::SplitterMessagesDef            = "";

DKEY GUI::ToolbarStyle                            = "toolbar_style";
DVALUE(Qt::ToolButtonStyle) GUI::ToolbarStyleDef  = Qt::ToolButtonIconOnly;

DKEY GUI::FeedsToolbarActions                     = "feeds_toolbar";
DVALUE(char*) GUI::FeedsToolbarActionsDef         = "m_actionUpdateAllFeeds,m_actionMarkAllFeedsRead";

// General.
DKEY General::ID                            = "main";

// Proxy.
DKEY Proxy::ID                              = "proxy";

// Database.
DKEY Database::ID                           = "database";

// Keyboard.
DKEY Keyboard::ID                           = "keyboard";

// Web browser.
DKEY Browser::ID                            = "browser";

DKEY Browser::GesturesEnabled               = "gestures_enabled";
DVALUE(bool) Browser::GesturesEnabledDef    = true;

DKEY Browser::JavascriptEnabled             = "enable_javascript";
DVALUE(bool) Browser::JavascriptEnabledDef  = true;

DKEY Browser::ImagesEnabled                 = "enable_images";
DVALUE(bool) Browser::ImagesEnabledDef      = true;

DKEY Browser::PluginsEnabled                = "enable_plugins";
DVALUE(bool) Browser::PluginsEnabledDef     = false;

DKEY Browser::CustomExternalBrowserEnabled                = "custom_external_browser";
DVALUE(bool) Browser::CustomExternalBrowserEnabledDef     = false;

DKEY Browser::CustomExternalBrowserExecutable             = "external_browser_executable";
DVALUE(char*) Browser::CustomExternalBrowserExecutableDef = "";

DKEY Browser::CustomExternalBrowserArguments              = "external_browser_arguments";
DVALUE(char*) Browser::CustomExternalBrowserArgumentsDef  = "%1";

// Categories.
DKEY Categories::ID                         = "categories_expand_states";

Settings::Settings(const QString &file_name, Format format,
                   const Type &status, QObject *parent)
  : QSettings(file_name, format, parent), m_initializationStatus(status) {
}

Settings::~Settings() {  
  checkSettings();
  qDebug("Deleting Settings instance.");
}

QSettings::Status Settings::checkSettings() {
  qDebug("Syncing settings.");

  sync();
  return status();
}

bool Settings::initiateRestoration(const QString &settings_backup_file_path) {
  return IOFactory::copyFile(settings_backup_file_path,
                             QFileInfo(fileName()).absolutePath() + QDir::separator() +
                             BACKUP_NAME_SETTINGS + BACKUP_SUFFIX_SETTINGS);
}

void Settings::finishRestoration(const QString &desired_settings_file_path) {
  QString backup_settings_file = QFileInfo(desired_settings_file_path).absolutePath() + QDir::separator() +
                                 BACKUP_NAME_SETTINGS + BACKUP_SUFFIX_SETTINGS;

  if (QFile::exists(backup_settings_file)) {
    qWarning("Backup settings file '%s' was detected. Restoring it.", qPrintable(QDir::toNativeSeparators(backup_settings_file)));

    if (IOFactory::copyFile(backup_settings_file, desired_settings_file_path)) {
      QFile::remove(backup_settings_file);
      qDebug("Settings file was restored successully.");
    }
    else {
      qCritical("Settings file was NOT restored due to error when copying the file.");
    }
  }
}

Settings *Settings::setupSettings(QObject *parent) {
  Settings *new_settings;

  // If settings file exists in executable file working directory
  // (in subdirectory APP_CFG_PATH), then use it (portable settings).
  // Otherwise use settings file stored in home path.
  QString relative_path = QDir::separator() + QString(APP_CFG_PATH) + QDir::separator() + QString(APP_CFG_FILE);
  QString app_path = qApp->applicationDirPath();
  QString app_path_file = app_path + relative_path;
  QString home_path = qApp->homeFolderPath() + QDir::separator() + QString(APP_LOW_H_NAME);
  QString home_path_file = home_path + relative_path;

  bool portable_settings_available = QFileInfo(app_path).isWritable();
  bool non_portable_settings_exist = QFile::exists(home_path_file);

  // We will use PORTABLE settings only and only if it is available and NON-PORTABLE
  // settings was not initialized before.
  bool will_we_use_portable_settings = portable_settings_available && !non_portable_settings_exist;

  // Check if portable settings are available.
  if (will_we_use_portable_settings) {
    finishRestoration(app_path_file);

    // Portable settings are available, use them.
    new_settings = new Settings(app_path_file, QSettings::IniFormat, Settings::Portable, parent);

    // Construct icon cache in the same path.
    QString web_path = app_path + QDir::separator() + QString(APP_DB_WEB_PATH);
    QDir(web_path).mkpath(web_path);
    QWebSettings::setIconDatabasePath(web_path);

    qDebug("Initializing settings in '%s' (portable way).", qPrintable(QDir::toNativeSeparators(app_path_file)));
  }
  else {
    finishRestoration(home_path_file);

    // Portable settings are NOT available, store them in
    // user's home directory.
    new_settings = new Settings(home_path_file, QSettings::IniFormat, Settings::NonPortable, parent);

    // Construct icon cache in the same path.
    QString web_path = home_path + QDir::separator() + QString(APP_DB_WEB_PATH);
    QDir(web_path).mkpath(web_path);
    QWebSettings::setIconDatabasePath(web_path);

    qDebug("Initializing settings in '%s' (non-portable way).", qPrintable(QDir::toNativeSeparators(home_path_file)));
  }

  return new_settings;
}
