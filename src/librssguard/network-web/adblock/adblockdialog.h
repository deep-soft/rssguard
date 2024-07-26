// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef ADBLOCKDIALOG_H
#define ADBLOCKDIALOG_H

#include "ui_adblockdialog.h"

#include <QDialog>

class AdBlockManager;

class AdBlockDialog : public QDialog {
    Q_OBJECT

  public:
    explicit AdBlockDialog(QWidget* parent = nullptr);

  protected:
    virtual void hideEvent(QHideEvent* event);

  private slots:
    void saveOnClose();
    void enableAdBlock(bool enable);
    void onAdBlockEnabledChanged(bool enabled, const QString& message);
    void onAdBlockProcessTerminated();

  private:
    void loadDialog();

  private:
    AdBlockManager* m_manager;
    bool m_loaded;

    Ui::AdBlockDialog m_ui;
};

#endif // ADBLOCKDIALOG_H
