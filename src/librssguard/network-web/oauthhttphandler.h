// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef OAUTHHTTPHANDLER_H
#define OAUTHHTTPHANDLER_H

#include <QObject>

#include <QTcpServer>
#include <QUrl>

class OAuthHttpHandler : public QObject {
  Q_OBJECT

  public:
    explicit OAuthHttpHandler(QObject* parent = nullptr);
    virtual ~OAuthHttpHandler();

    bool isListening() const;

    quint16 listenPort() const;
    QHostAddress listenAddress() const;
    QString listenAddressPort() const;

    void setListenAddressPort(const QString& full_uri);

  signals:
    void authRejected(const QString& error_description, const QString& state);
    void authGranted(const QString& auth_code, const QString& state);

  private slots:
    void clientConnected();

  private:
    void handleRedirection(const QVariantMap& data);
    void answerClient(QTcpSocket* socket, const QUrl& url);
    void readReceivedData(QTcpSocket* socket);

  private:
    struct QHttpRequest {
      bool readMethod(QTcpSocket* socket);
      bool readUrl(QTcpSocket* socket);
      bool readStatus(QTcpSocket* socket);
      bool readHeader(QTcpSocket* socket);

      enum class State {
        ReadingMethod,
        ReadingUrl,
        ReadingStatus,
        ReadingHeader,
        ReadingBody,
        AllDone
      } m_state = State::ReadingMethod;

      enum class Method {
        Unknown,
        Head,
        Get,
        Put,
        Post,
        Delete,
      } m_method = Method::Unknown;

      QString m_address;
      quint16 m_port = 0;
      QByteArray m_fragment;
      QUrl m_url;
      QPair<quint8, quint8> m_version;
      QMap<QByteArray, QByteArray> m_headers;
    };
    QMap<QTcpSocket*, QHttpRequest> m_connectedClients;
    QTcpServer m_httpServer;
    QHostAddress m_listenAddress;
    quint16 m_listenPort;
    QString m_listenAddressPort;
    QString m_text;
};

#endif // OAUTHHTTPHANDLER_H
