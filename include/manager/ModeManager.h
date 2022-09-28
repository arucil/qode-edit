#pragma once

#include <QMap>
#include <QString>

#include "Manager.h"

class Mode;

class ModeManager: public Manager {
public:
  using ModeMap = QMap<QString, Mode *>;

public:
  ModeManager(CodeEdit *editor);
  ~ModeManager();

  /**
   * @brief Adds a mode to the editor.
   * 
   * The ownership of the mode is transferred to the manager if the mode was
   * successfully added.
   * 
   * @return Returns true if the mode was successfully added.
   */
  bool append(Mode *);

  /**
   * @brief Removes a mode by mode name.
   * 
   * The ownership of the mode is transferred to the caller.
   * 
   * @return Returns nullptr if no mode with the given name was found.
   */
  Mode *remove(const QString &name);

  void clear();

  Mode *get(const QString &name) const;

  ModeMap::const_iterator cbegin() const;
  ModeMap::const_iterator cend() const;

private:
  ModeMap m_modes;
};