// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef COLORTOOLBUTTON_H
#define COLORTOOLBUTTON_H

#include <QToolButton>

class RSSGUARD_DLLSPEC ColorToolButton : public QToolButton {
    Q_OBJECT

  public:
    explicit ColorToolButton(QWidget* parent = nullptr);

    QColor color() const;
    void setColor(const QColor& color, bool inform_about_changes = true);

    QColor alternateColor() const;
    void setAlternateColor(const QColor& alt_color);

  public slots:
    void setRandomColor();

  signals:
    void colorChanged(const QColor& new_color);

  protected:
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void paintEvent(QPaintEvent* e);

  private:
    QColor m_color;
    QColor m_alternateColor;
};

#endif // COLORTOOLBUTTON_H
