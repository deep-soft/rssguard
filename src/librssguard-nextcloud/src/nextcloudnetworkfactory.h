// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef NEXTCLOUDNETWORKFACTORY_H
#define NEXTCLOUDNETWORKFACTORY_H

#include <librssguard/core/message.h>
#include <librssguard/network-web/networkfactory.h>
#include <librssguard/services/abstract/rootitem.h>

#include <QDateTime>
#include <QIcon>
#include <QJsonObject>
#include <QNetworkReply>
#include <QString>

class NextcloudResponse {
  public:
    explicit NextcloudResponse(QNetworkReply::NetworkError response, const QString& raw_content = QString());
    virtual ~NextcloudResponse();

    bool isLoaded() const;
    QString toString() const;
    QNetworkReply::NetworkError networkError() const;

  protected:
    QNetworkReply::NetworkError m_networkError;
    QJsonObject m_rawContent;
    bool m_emptyString;
};

class NextcloudGetMessagesResponse : public NextcloudResponse {
  public:
    explicit NextcloudGetMessagesResponse(QNetworkReply::NetworkError response, const QString& raw_content = QString());
    virtual ~NextcloudGetMessagesResponse();

    QList<Message> messages() const;
};

class NextcloudStatusResponse : public NextcloudResponse {
  public:
    explicit NextcloudStatusResponse(QNetworkReply::NetworkError response, const QString& raw_content = QString());
    virtual ~NextcloudStatusResponse();

    QString version() const;
};

class RootItem;

class NextcloudGetFeedsCategoriesResponse : public NextcloudResponse {
  public:
    explicit NextcloudGetFeedsCategoriesResponse(QNetworkReply::NetworkError response,
                                                 QString raw_categories = QString(),
                                                 QString raw_feeds = QString());
    virtual ~NextcloudGetFeedsCategoriesResponse();

    // Returns tree of feeds/categories.
    // Top-level root of the tree is not needed here.
    // Returned items do not have primary IDs assigned.
    RootItem* feedsCategories(bool obtain_icons) const;

  private:
    QString m_contentCategories;
    QString m_contentFeeds;
};

class NextcloudNetworkFactory {
  public:
    explicit NextcloudNetworkFactory();
    virtual ~NextcloudNetworkFactory();

    QString url() const;
    void setUrl(const QString& url);

    bool forceServerSideUpdate() const;
    void setForceServerSideUpdate(bool force_update);

    QString authUsername() const;
    void setAuthUsername(const QString& auth_username);

    QString authPassword() const;
    void setAuthPassword(const QString& auth_password);

    // Gets/sets the amount of messages to obtain during single feed update.
    int batchSize() const;
    void setBatchSize(int batch_size);

    bool downloadOnlyUnreadMessages() const;
    void setDownloadOnlyUnreadMessages(bool dowload_only_unread_messages);

    // Operations.

    // Get version info.
    NextcloudStatusResponse status(const QNetworkProxy& custom_proxy);

    // Get feeds & categories (used for sync-in).
    NextcloudGetFeedsCategoriesResponse feedsCategories(const QNetworkProxy& custom_proxy);

    // Feed operations.
    bool deleteFeed(const QString& feed_id, const QNetworkProxy& custom_proxy);
    bool createFeed(const QString& url, int parent_id, const QNetworkProxy& custom_proxy);
    bool renameFeed(const QString& new_name, const QString& custom_feed_id, const QNetworkProxy& custom_proxy);

    // Get messages for given feed.
    NextcloudGetMessagesResponse getMessages(int feed_id, const QNetworkProxy& custom_proxy);

    // Misc methods.
    QNetworkReply::NetworkError triggerFeedUpdate(int feed_id, const QNetworkProxy& custom_proxy);

    NetworkResult markMessagesRead(RootItem::ReadStatus status,
                                   const QStringList& custom_ids,
                                   const QNetworkProxy& custom_proxy);

    NetworkResult markMessagesStarred(RootItem::Importance importance,
                                      const QStringList& feed_ids,
                                      const QStringList& guid_hashes,
                                      const QNetworkProxy& custom_proxy);

  private:
    QString m_url;
    QString m_fixedUrl;
    bool m_downloadOnlyUnreadMessages;
    bool m_forceServerSideUpdate;
    QString m_authUsername;
    QString m_authPassword;
    int m_batchSize;

    // Endpoints.
    QString m_urlUser;
    QString m_urlStatus;
    QString m_urlFolders;
    QString m_urlFeeds;
    QString m_urlMessages;
    QString m_urlFeedsUpdate;
    QString m_urlDeleteFeed;
    QString m_urlRenameFeed;
};

#endif // NEXTCLOUDNETWORKFACTORY_H
