#pragma once

#include <QList>
#include <QMap>
#include <QMargins>
#include <QObject>
#include <QRect>
#include <QString>

#include "Manager.h"
#include "panel/Panel.h"
#include "util/TextHelper.h"

class QRect;

/**
 * @brief Manages the list of panels and draws them inised the margin of the code
 * edit widget.
 */
class PanelManager: public QObject, public Manager {
  Q_OBJECT

public:
  using PanelMap = QMap<QString, Panel *>;

public:
  PanelManager(CodeEdit *editor);
  ~PanelManager();

  /**
   * @brief Installs a panel on the editor.
   * 
   * The ownership of the panel is transferred to the manager if the panel was
   * successfully added.
   * 
   * @return Returns true if the panel was successfully added.
   */
  bool append(Panel *, Panel::Position = Panel::Position::Left);

  /**
   * @brief Removes a panel by its name.
   * 
   * The ownership of the panel is transferred to the caller.
   * 
   * @return Returns nullptr if no panel with the given name was found.
   */
  Panel *remove(const QString &name);

  void clear();

  Panel *get(const QString &name) const;

  /**
   * @brief Gets the list of panels attached to the specified zone.
   */
  QList<Panel *> panelsForZone(Panel::Position) const;

  void refresh();

  void resize();

private slots:
  void updateViewportMargins();
  void update(const QRect &rect, int dy, bool forceUpdateMargins = false);

private:
  QRect computeZonesSizes() const;

private:
  PanelMap m_panels[4];
  CursorPosition m_cachedCursorPos;
  QMargins m_marginSizes;
};