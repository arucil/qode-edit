#pragma once

#include <QString>

class CodeEdit;

/**
 * @brief Base class for editor extensions.
 * 
 * An extension is a "thing" that can be installed on an editor to add new
 * behaviours or to modify its appearance.
 * 
 * A mode is added to an editor by using the ModeManager/PanelManager:
 *     - :meth:`CodeEdit.modes.append` or
 *     - :meth:`CodeEdit.panels.append`
 * 
 * Subclasses may/should override the following methods:
 *     - :meth:`Mode.onInstall`
 *     - :meth:`Mode.onUninstall`
 *     - :meth:`Mode.onStateChanged`
 * 
 * ..warning: The mode will be identified by its class name, this means that
 * **there cannot be two modes of the same type on the same editor instance!**
 */
class Mode {
public:
  Mode(const char *name);
  virtual ~Mode() = 0;

  CodeEdit *editor() const {
    return m_editor;
  }

  bool isEnabled() const {
    return m_enabled;
  }
  void setEnabled(bool enabled = true);

  const QString &name() const {
    return m_name;
  }

  /**
   * @brief Clone the settings from another mode (same class).
   * 
   * This method is called when splitting an editor widget.
   * 
   * .. note:: The base method does not do anything, you must implement
   *     this method for every new mode/panel (if you plan on using the
   *     split feature). You should also make sure any properties will be
   *     propagated to the clones.
   * 
   * @param original other mode (must be the same class).
   */
  virtual void cloneSettings(Mode *original);

protected:

  /**
   * @brief Installs the extension on the editor.
   * 
   * .. note:: This method is called by editor when you install a Mode.
   *           You should never call it yourself, even in a subclasss.
   * 
   * .. warning:: Don't forget to call **super** when subclassing
   * 
   * @param editor editor widget instance
   */
  virtual void onInstall(CodeEdit *editor);

  /**
   * @brief Uninstalls the mode from the editor.
   */
  virtual void onUninstall();

  /**
   * @brief Called when the enable state has changed.
   * 
   * This method does not do anything, you may override it if you need
   * to connect/disconnect to the editor's signals (connect when state is
   * true and disconnect when it is false).
   */
  virtual void onStateChanged(bool enabled);

private:
  friend class ModeManager;
  friend class PanelManager;

private:
  QString m_name;
  CodeEdit *m_editor;
  bool m_enabled;
};