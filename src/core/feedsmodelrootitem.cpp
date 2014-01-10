#include "core/feedsmodelrootitem.h"

#include "qtsingleapplication/qtsingleapplication.h"

#include <QVariant>


FeedsModelRootItem::FeedsModelRootItem(FeedsModelRootItem *parent_item)
  : m_kind(FeedsModelRootItem::RootItem),
    m_parentItem(parent_item) {
}

FeedsModelRootItem::~FeedsModelRootItem() {
  qDebug("Destroying FeedsModelRootItem instance.");
  qDeleteAll(m_childItems);
}

FeedsModelRootItem *FeedsModelRootItem::parent() const {
  return m_parentItem;
}

void FeedsModelRootItem::setParent(FeedsModelRootItem *parent_item) {
  m_parentItem = parent_item;
}

FeedsModelRootItem::Kind FeedsModelRootItem::kind() const {
  return m_kind;
}

QIcon FeedsModelRootItem::icon() const {
  return m_icon;
}

FeedsModelRootItem *FeedsModelRootItem::child(int row) {
  return m_childItems.value(row);
}

void FeedsModelRootItem::appendChild(FeedsModelRootItem *child) {
  m_childItems.append(child);
  child->setParent(this);
}

int FeedsModelRootItem::columnCount() const {
  // FeedsModel offers exactly two columns.
  return 2;
}

int FeedsModelRootItem::row() const {
  if (m_parentItem) {
    return m_parentItem->m_childItems.indexOf(const_cast<FeedsModelRootItem*>(this));
  }
  else {
    // This item has no parent. Therefore, its row index is 0.
    return 0;
  }
}

int FeedsModelRootItem::childCount() const {
  return m_childItems.size();
}

QVariant FeedsModelRootItem::data(int column, int role) const {
  Q_UNUSED(column)
  Q_UNUSED(role)

  // Do not return anything for the root item.
  return QVariant();
}

int FeedsModelRootItem::countOfAllMessages() const {
  int total_count = 0;

  foreach (FeedsModelRootItem *child_item, m_childItems) {
    total_count += child_item->countOfAllMessages();
  }

  return total_count;
}

int FeedsModelRootItem::countOfUnreadMessages() const {
  int total_count = 0;

  foreach (FeedsModelRootItem *child_item, m_childItems) {
    total_count += child_item->countOfUnreadMessages();
  }

  return total_count;
}

void FeedsModelRootItem::setIcon(const QIcon &icon) {
  m_icon = icon;
}

int FeedsModelRootItem::id() const {
  return m_id;
}

void FeedsModelRootItem::setId(int id) {
  m_id = id;
}

QString FeedsModelRootItem::title() const {
  return m_title;
}

void FeedsModelRootItem::setTitle(const QString &title) {
  m_title = title;
}

QList<FeedsModelRootItem *> FeedsModelRootItem::childItems() const {
  return m_childItems;
}

void FeedsModelRootItem::clearChilds() {
  m_childItems.clear();
}

bool FeedsModelRootItem::removeChild(int index) {
  if (index >= 0 && index < m_childItems.size()) {
    m_childItems.removeAt(index);
    return true;
  }
  else {
    return false;
  }
}



QDateTime FeedsModelRootItem::creationDate() const {
  return m_creationDate;
}

void FeedsModelRootItem::setCreationDate(const QDateTime &creation_date) {
  m_creationDate = creation_date;
}

QString FeedsModelRootItem::description() const {
  return m_description;
}

void FeedsModelRootItem::setDescription(const QString &description) {
  m_description = description;
}


bool FeedsModelRootItem::isEqual(FeedsModelRootItem *lhs,
                                 FeedsModelRootItem *rhs) {
  return (lhs->kind() == rhs->kind()) && (lhs->id() == rhs->id());
}

bool FeedsModelRootItem::lessThan(FeedsModelRootItem *lhs,
                                  FeedsModelRootItem *rhs) {
  if (lhs->kind() == rhs->kind()) {
    return lhs->id() < rhs->id();
  }
  else {
    return false;
  }
}
