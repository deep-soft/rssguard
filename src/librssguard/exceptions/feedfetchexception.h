// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FEEDFETCHEXCEPTION_H
#define FEEDFETCHEXCEPTION_H

#include "exceptions/applicationexception.h"
#include "services/abstract/feed.h"

class RSSGUARD_DLLSPEC FeedFetchException : public ApplicationException {
  public:
    explicit FeedFetchException(Feed::Status feed_status, const QString& message = {});

    Feed::Status feedStatus() const;

  private:
    Feed::Status m_feedStatus;
};

#endif // FEEDFETCHEXCEPTION_H
