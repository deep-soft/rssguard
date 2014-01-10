#include "core/localization.h"

#include "core/defs.h"
#include "core/settings.h"
#include "qtsingleapplication/qtsingleapplication.h"

#include <QTranslator>
#include <QDir>
#include <QFileInfoList>


Localization::Localization() {
}

QList<Language> Localization::installedLanguages() {
  QList<Language> languages;
  QDir file_dir(APP_LANG_PATH);
  QTranslator translator;

  // Iterate all found language files.
  foreach (const QFileInfo &file, file_dir.entryInfoList(QStringList() << "rssguard_*.qm",
                                                         QDir::Files,
                                                         QDir::Name)) {
    if (translator.load(file.absoluteFilePath())) {
      Language new_language;
      new_language.m_name = translator.translate("QObject", "LANG_NAME");
      new_language.m_code = translator.translate("QObject", "LANG_ABBREV");
      new_language.m_version = translator.translate("QObject", "LANG_VERSION");
      new_language.m_author = translator.translate("QObject", "LANG_AUTHOR");
      new_language.m_email = translator.translate("QObject", "LANG_EMAIL");

      languages << new_language;
    }
  }
  return languages;
}
