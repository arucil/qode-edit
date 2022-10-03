#pragma once

#include <QBrush>
#include <QPen>
#include <QWidget>

#include "mode/Mode.h"

class QPaintEvent;

/**
 * @brief Base class for editor panels.
 * 
 *  A panel is a mode and a QWidget.
 * 
 *  .. note:: Use enabled to disable panel actions and setVisible to change the
 *      visibility of the panel.
 */
class Panel: public QWidget, public Mode {
  Q_OBJECT
public:
  enum class Position { Top, Left, Right, Bottom };

public:
  /**
     * @param isDynamic Specifies whether the panel is dynamic.
     * A dynamic panel is a panel that will be shown/hidden depending on the context.
     * Dynamic panel should not appear in any GUI menu (e.g. no display
     * in the panels menu of the notepad example).
     */
  explicit Panel(const char *name, bool isDynamic = false);

  /**
     * @brief A scrollable panel will follow the editor's scroll-bars. Left and right
     * panels follow the vertical scrollbar. Top and bottom panels follow the
     * horizontal scrollbar.
     */
  bool isScrollable() const {
    return m_scrollable;
  }

  void setScrollable(bool);

  void setVisible(bool) override;

protected:
  void onInstall(CodeEdit *) override;

  void paintEvent(QPaintEvent *) override;

  friend class PanelManager;

private:
  bool m_scrollable;
  bool m_dynamic;
  /**
     * @brief order into the zone it is installed to.
     * 
     * This value is automatically set when installing the panel but it can be
     * changed later (negative values can also be used).
     */
  int m_orderInZone;
  Position m_position;
  QBrush m_backgroundBrush;
  QPen m_foregroundPen;
};