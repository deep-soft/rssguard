// For license of this file, see <project-root-folder>/LICENSE.md.

#include "src/gui/ttrssaccountdetails.h"

#include "src/definitions.h"
#include "src/ttrssnetworkfactory.h"

#include <librssguard/definitions/definitions.h>
#include <librssguard/network-web/networkfactory.h>

TtRssAccountDetails::TtRssAccountDetails(QWidget* parent) : QWidget(parent) {
  m_ui.setupUi(this);

  m_ui.m_lblTestResult->label()->setWordWrap(true);
  m_ui.m_lblNewAlgorithm
    ->setHelpText(tr("If you select intelligent synchronization, then only not-yet-fetched "
                     "or updated articles are downloaded. Network usage is greatly reduced and "
                     "overall synchronization speed is greatly improved, but "
                     "first feed fetching could be slow anyway if your feed contains "
                     "huge number of articles.<br/><br/>"
                     "Also, make sure to install <a href=\"https://www.google.com\">api_newsplus</a> TT-RSS "
                     "plugin to your server instance."),
                  true,
                  true);
  m_ui.m_lblServerSideUpdateInformation
    ->setHelpText(tr("Leaving this option on causes that updates "
                     "of feeds will be probably much slower and may time-out often."),
                  true);
  m_ui.m_txtHttpUsername->lineEdit()->setPlaceholderText(tr("HTTP authentication username"));
  m_ui.m_txtHttpPassword->lineEdit()->setPlaceholderText(tr("HTTP authentication password"));
  m_ui.m_txtPassword->lineEdit()->setPlaceholderText(tr("Password for your TT-RSS account"));
  m_ui.m_txtUsername->lineEdit()->setPlaceholderText(tr("Username for your TT-RSS account"));
  m_ui.m_txtUrl->lineEdit()->setPlaceholderText(tr("URL of your TT-RSS instance WITHOUT trailing \"/api/\" string"));
  m_ui.m_lblTestResult->setStatus(WidgetWithStatus::StatusType::Information,
                                  tr("No test done yet."),
                                  tr("Here, results of connection test are shown."));

  setTabOrder(m_ui.m_txtUrl->lineEdit(), m_ui.m_checkDownloadOnlyUnreadMessages);
  setTabOrder(m_ui.m_checkDownloadOnlyUnreadMessages, m_ui.m_spinLimitMessages);
  setTabOrder(m_ui.m_spinLimitMessages, m_ui.m_cbNewAlgorithm);
  setTabOrder(m_ui.m_cbNewAlgorithm, m_ui.m_checkServerSideUpdate);
  setTabOrder(m_ui.m_checkServerSideUpdate, m_ui.m_txtUsername->lineEdit());
  setTabOrder(m_ui.m_txtUsername->lineEdit(), m_ui.m_txtPassword->lineEdit());
  setTabOrder(m_ui.m_txtPassword->lineEdit(), m_ui.m_gbHttpAuthentication);
  setTabOrder(m_ui.m_gbHttpAuthentication, m_ui.m_txtHttpUsername->lineEdit());
  setTabOrder(m_ui.m_txtHttpUsername->lineEdit(), m_ui.m_txtHttpPassword->lineEdit());
  setTabOrder(m_ui.m_txtHttpPassword->lineEdit(), m_ui.m_btnTestSetup);

  m_ui.m_txtHttpPassword->lineEdit()->setPasswordMode(true);
  m_ui.m_txtPassword->lineEdit()->setPasswordMode(true);

  connect(m_ui.m_txtPassword->lineEdit(), &BaseLineEdit::textChanged, this, &TtRssAccountDetails::onPasswordChanged);
  connect(m_ui.m_txtUsername->lineEdit(), &BaseLineEdit::textChanged, this, &TtRssAccountDetails::onUsernameChanged);
  connect(m_ui.m_txtHttpPassword->lineEdit(),
          &BaseLineEdit::textChanged,
          this,
          &TtRssAccountDetails::onHttpPasswordChanged);
  connect(m_ui.m_txtHttpUsername->lineEdit(),
          &BaseLineEdit::textChanged,
          this,
          &TtRssAccountDetails::onHttpUsernameChanged);
  connect(m_ui.m_txtUrl->lineEdit(), &BaseLineEdit::textChanged, this, &TtRssAccountDetails::onUrlChanged);
  connect(m_ui.m_gbHttpAuthentication, &QGroupBox::toggled, this, &TtRssAccountDetails::onHttpPasswordChanged);
  connect(m_ui.m_gbHttpAuthentication, &QGroupBox::toggled, this, &TtRssAccountDetails::onHttpUsernameChanged);

  onPasswordChanged();
  onUsernameChanged();
  onUrlChanged();
  onHttpPasswordChanged();
  onHttpUsernameChanged();
}

void TtRssAccountDetails::performTest(const QNetworkProxy& proxy) {
  TtRssNetworkFactory factory;

  factory.setUsername(m_ui.m_txtUsername->lineEdit()->text());
  factory.setPassword(m_ui.m_txtPassword->lineEdit()->text());
  factory.setUrl(m_ui.m_txtUrl->lineEdit()->text());
  factory.setAuthIsUsed(m_ui.m_gbHttpAuthentication->isChecked());
  factory.setAuthUsername(m_ui.m_txtHttpUsername->lineEdit()->text());
  factory.setAuthPassword(m_ui.m_txtHttpPassword->lineEdit()->text());
  factory.setForceServerSideUpdate(m_ui.m_checkServerSideUpdate->isChecked());
  factory.setBatchSize(m_ui.m_spinLimitMessages->value());

  TtRssLoginResponse result = factory.login(proxy);

  if (result.isLoaded()) {
    if (result.hasError()) {
      QString error = result.error();

      if (error == QSL(TTRSS_API_DISABLED)) {
        m_ui.m_lblTestResult->setStatus(WidgetWithStatus::StatusType::Error,
                                        tr("API access on selected server is not enabled."),
                                        tr("API access on selected server is not enabled."));
      }
      else if (error == QSL(TTRSS_LOGIN_ERROR)) {
        m_ui.m_lblTestResult->setStatus(WidgetWithStatus::StatusType::Error,
                                        tr("Entered credentials are incorrect."),
                                        tr("Entered credentials are incorrect."));
      }
      else {
        m_ui.m_lblTestResult->setStatus(WidgetWithStatus::StatusType::Error,
                                        tr("Other error occurred, contact developers."),
                                        tr("Other error occurred, contact developers."));
      }
    }
    else if (result.apiLevel() < TTRSS_MINIMAL_API_LEVEL) {
      m_ui.m_lblTestResult->setStatus(WidgetWithStatus::StatusType::Error,
                                      tr("Installed version: %1, required at least: %2.")
                                        .arg(QString::number(result.apiLevel()),
                                             QString::number(TTRSS_MINIMAL_API_LEVEL)),
                                      tr("Selected Tiny Tiny RSS server is running unsupported version of API."));
    }
    else {
      m_ui.m_lblTestResult->setStatus(WidgetWithStatus::StatusType::Ok,
                                      tr("Installed version: %1, required at least: %2.")
                                        .arg(QString::number(result.apiLevel()),
                                             QString::number(TTRSS_MINIMAL_API_LEVEL)),
                                      tr("Tiny Tiny RSS server is okay."));
    }
  }
  else if (factory.lastError() != QNetworkReply::NoError) {
    m_ui.m_lblTestResult
      ->setStatus(WidgetWithStatus::StatusType::Error,
                  tr("Network error: '%1'.").arg(NetworkFactory::networkErrorText(factory.lastError())),
                  tr("Network error, have you entered correct Tiny Tiny RSS API endpoint and password?"));
  }
  else {
    m_ui.m_lblTestResult->setStatus(WidgetWithStatus::StatusType::Error,
                                    tr("Unspecified error, did you enter correct URL?"),
                                    tr("Unspecified error, did you enter correct URL?"));
  }
}

void TtRssAccountDetails::onUsernameChanged() {
  const QString username = m_ui.m_txtUsername->lineEdit()->text();

  if (username.isEmpty()) {
    m_ui.m_txtUsername->setStatus(WidgetWithStatus::StatusType::Error, tr("Username cannot be empty."));
  }
  else {
    m_ui.m_txtUsername->setStatus(WidgetWithStatus::StatusType::Ok, tr("Username is okay."));
  }
}

void TtRssAccountDetails::onPasswordChanged() {
  const QString password = m_ui.m_txtPassword->lineEdit()->text();

  if (password.isEmpty()) {
    m_ui.m_txtPassword->setStatus(WidgetWithStatus::StatusType::Error, tr("Password cannot be empty."));
  }
  else {
    m_ui.m_txtPassword->setStatus(WidgetWithStatus::StatusType::Ok, tr("Password is okay."));
  }
}

void TtRssAccountDetails::onHttpUsernameChanged() {
  const bool is_username_ok =
    !m_ui.m_gbHttpAuthentication->isChecked() || !m_ui.m_txtHttpUsername->lineEdit()->text().isEmpty();

  m_ui.m_txtHttpUsername->setStatus(is_username_ok ? LineEditWithStatus::StatusType::Ok
                                                   : LineEditWithStatus::StatusType::Warning,
                                    is_username_ok ? tr("Username is ok or it is not needed.")
                                                   : tr("Username is empty."));
}

void TtRssAccountDetails::onHttpPasswordChanged() {
  const bool is_username_ok =
    !m_ui.m_gbHttpAuthentication->isChecked() || !m_ui.m_txtHttpPassword->lineEdit()->text().isEmpty();

  m_ui.m_txtHttpPassword->setStatus(is_username_ok ? LineEditWithStatus::StatusType::Ok
                                                   : LineEditWithStatus::StatusType::Warning,
                                    is_username_ok ? tr("Password is ok or it is not needed.")
                                                   : tr("Password is empty."));
}

void TtRssAccountDetails::onUrlChanged() {
  const QString url = m_ui.m_txtUrl->lineEdit()->text();

  if (url.isEmpty()) {
    m_ui.m_txtUrl->setStatus(WidgetWithStatus::StatusType::Error, tr("URL cannot be empty."));
  }
  else if (url.endsWith(QL1S("/api/")) || url.endsWith(QL1S("/api"))) {
    m_ui.m_txtUrl->setStatus(WidgetWithStatus::StatusType::Warning, tr("URL should NOT end with \"/api/\"."));
  }
  else {
    m_ui.m_txtUrl->setStatus(WidgetWithStatus::StatusType::Ok, tr("URL is okay."));
  }
}
