// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef NOTIFICATIONSEDITOR_H
#define NOTIFICATIONSEDITOR_H

#include "miscellaneous/notification.h"

#include "ui_notificationseditor.h"

#include <QWidget>

class QVBoxLayout;

class NotificationsEditor : public QWidget {
    Q_OBJECT

  public:
    explicit NotificationsEditor(QWidget* parent = nullptr);

    void loadNotifications(const QList<Notification>& notifications);

    QList<Notification> allNotifications() const;

  signals:
    void someNotificationChanged();

  private:
    Ui::NotificationsEditor m_ui;
    QVBoxLayout* m_layout;
};

#endif // NOTIFICATIONSEDITOR_H
