#pragma once

/**
 * @file CodeEdit.h
 */

#include <QColor>
#include <QMimeType>
#include <QPlainTextEdit>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QTextBlock>
#include <QVector>
#include "utils.h"
#include "manager/BackendManager.h"
#include "manager/FileManager.h"
#include "manager/ModeManager.h"

class QPaintEvent;
class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
class QFocusEvent;
class QAction;
class QMenu;

struct VisibleBlock {
  int lineTopPos;
  int lineNum;
  QTextBlock block;
};

enum class IndentChar {
  Space,
  Tab
};

/**
  The editor widget is a simple extension to QPlainTextEdit.
  It adds a few utility signals/methods and introduces the concepts of
  **Managers, Modes and Panels**.
  A **mode/panel** is an editor extension that, once added to a CodeEdit
  instance, may modify its behaviour and appearance:
    * **Modes** are simple objects which connect to the editor signals to
      append new behaviours (such as automatic indentation, code completion,
      syntax checking,...)
    * **Panels** are the combination of a **Mode** and a **QWidget**.
      They are displayed in the CodeEdit's content margins.
      When you install a Panel on a CodeEdit, you can choose to install it in
      one of the four following zones:
          .. image:: _static/editor_widget.png
              :align: center
              :width: 600
              :height: 450
  A **manager** is an object that literally manage a specific aspect of
  :class:`pyqode.core.api.CodeEdit`. There are managers to manage the list of
  modes/panels, to open/save file and to control the backend:
      - :attr:`pyqode.core.api.CodeEdit.file`:
          File manager. Use it to open/save files or access the opened file
          attribute.
      - :attr:`pyqode.core.api.CodeEdit.backend`:
          Backend manager. Use it to start/stop the backend or send a work
          request.
      - :attr:`pyqode.core.api.CodeEdit.modes`:
          Modes manager. Use it to append/remove modes on the editor.
      - :attr:`pyqode.core.api.CodeEdit.panels`:
          Modes manager. Use it to append/remove panels on the editor.
  Starting from version 2.1, CodeEdit defines the
  :attr:`pyqode.core.api.CodeEdit.mimetypes` class attribute that can be used
  by IDE to determine which editor to use for a given mime type. This
  property is a list of supported mimetypes. An empty list means the
  CodeEdit is generic. **Code editors specialised for a specific language
  should define the mime types they support!**
 */
class CodeEdit: public QPlainTextEdit {
public:
  /**
    @param parent Parent widget
    @param createStandardActions true to create the action for the
        standard shortcuts (copy, paste, delete, undo, redo,...).
        Non-standard actions will always get created. If you would like
        to prevent the context menu from showing, just set the
        `showMenuEnabled` to false.
   */
  explicit CodeEdit(
    QWidget *parent = nullptr,
    bool createStandardActions = true);
  virtual ~CodeEdit();

public:
  /** @brief Returns the character for indentation. Default is Space. */
  IndentChar indentChar() const {
    return m_indentChar;
  }

  void setIndentChar(IndentChar c) {
    m_indentChar = c;
  }

  /** @brief Returns tab size in number of spaces. */
  int tabSize() const {
    return m_tabSize;
  }

  void setTabSize(int size);

  /** @brief Returns if the editor content is automatically saved on focus out. Default is false. */
  bool saveOnFocusOut() const {
    return m_saveOnFocusOut;
  }

  void setSaveOnFocusOut(bool save);

  /** @brief Returns if white space characters are shown. */
  bool showsWhitespaces() const {
    return m_showWhitespaces;
  }

  void setShowsWhiteSpaces(bool show);

  const QVector<QMimeType> &mimeTypes() const {
    return m_mimeTypes;
  }

  /** @brief Returns the editor font family name. */
  const QString &fontName() const {
    return m_fontFamily;
  }

  void setFontName(const QString &name);

  QVector<QMimeType> &mimeTypes() {
    return m_mimeTypes;
  }

  void resetStyleSheet();

  void addMenu(QMenu *);
  void removeMenu(QMenu *);

  /** @brief Sets show white spaces flag */
  void setWhitespaceFlags(bool show);

  void rehighlight();

private slots:
  void showContextMenu(const QPoint &);

private:
  void initActions();
  QMenu *createContextMenu(const QPoint &);

  void onTextChanged();
  void update();

  void initSettings();
  void initStyle();

signals:

  /**
   * @brief paint hook
   */
  void painted(const QPaintEvent &);

  /**
   * @brief Signal emitted when a new text is set on the widget
   */
  void newTextSet();

  /** @brief Signal emitted when the text is saved to file */
  void textSaved(const QString &);

  /** @brief Signal emitted before the text is saved to file */
  void textSaving(const QString &);

  /** @brief Signal emitted when the dirty state changed */
  void dirtyChanged(bool);

  /** @brief Signal emitted when a key is pressed */
  void keyPressed(QKeyEvent *);

  /** @brief Signal emitted when a key is released */
  void keyReleased(QKeyEvent *);

  /** @brief Signal emitted when a mouse button is pressed */
  void mousePressed(QMouseEvent *);

  /** @brief Signal emitted when a mouse button is released */
  void mouseReleased(QMouseEvent *);

  /** @brief Signal emitted when a mouse double click event occured */
  void mouseDoubleClicked(QMouseEvent *);

  /** @brief Signal emitted on a wheel event */
  void mouseWheelActivated(QWheelEvent *);

  /** @brief Signal emitted at the end of the key_pressed event */
  void postKeyPressed(QKeyEvent *);

  /** @brief Signal emitted when focusInEvent is is called */
  void focusedIn(QFocusEvent *);

  /** @brief Signal emitted when the mouse_moved */
  void mouseMoved(QMouseEvent *);

  /** @brief Signal emitted when the user press the TAB key */
  void indentRequested();

  /** @brief Signal emitted when the user press the BACK-TAB (Shift+TAB) key */
  void unindentRequested();

private:
  IndentChar m_indentChar;
  bool m_autoResetStyleSheet;
  bool m_closed;
  bool m_showCtxMenu;
  bool m_saveOnFocusOut;
  bool m_showWhitespaces;
  bool m_selectLineOnCopyEmpty;
  bool m_dirty;
  bool m_cleaning;
  bool m_createStandardActions;
  bool m_useStylesheet;
  int m_tabSize;
  int m_zoomLevel;
  int m_fontSize;
  int m_prevTooltipBlockNbr;
  // BackendManager m_backend;
  FileManager m_file;
  ModeManager m_modes;
  PanelManager m_panels;
  TextDecorationManager m_decorations;
  QVector<CodeEdit *> m_clones;
  QVector<char> m_wordSeparators;
  QColor m_selBackground;
  QColor m_selForeground;
  QColor m_foreground;
  QColor m_background;
  QColor m_whitespaceForeground;
  QString m_fontFamily;
  QVector<QMimeType> m_mimeTypes;
  QString m_originalText;
  QPoint m_lastMousePos;
  QSet<int> m_modifiedLines;
  DelayJobRunner m_tooltipRunner;
  QVector<QAction *> m_actions;
  QVector<QMenu *> m_menus;
  QMenu *m_ctxMenu;
  QVector<VisibleBlock> m_visibleBlocks;
};