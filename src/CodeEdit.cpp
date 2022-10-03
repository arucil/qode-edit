#include "CodeEdit.h"

#include <QAction>
#include <QApplication>
#include <QFont>
#include <QFontDatabase>
#include <QList>
#include <QMenu>
#include <algorithm>
#include <cstdlib>
#include <cstring>

CodeEdit::CodeEdit(QWidget *parent, bool createStandardActions) :
  QPlainTextEdit(parent),
  m_indentChar(IndentChar::Space),
  m_autoResetStyleSheet(false),
  m_closed(false),
  m_showCtxMenu(true),
  m_saveOnFocusOut(false),
  m_showWhitespaces(false),
  m_selectLineOnCopyEmpty(true),
  m_dirty(false),
  m_cleaning(false),
  m_createStandardActions(createStandardActions),
  m_useStylesheet(false),
  // m_backend(new BackendManager(this)),
  m_tabSize(4),
  m_zoomLevel(0),
  m_fontSize(10),
  m_prevTooltipBlockNbr(-1),
  m_file(this),
  m_modes(this),
  m_panels(this),
  m_decorations(this),
  m_wordSeparators({'~', '!', '@', '#', '$', '%',  '^',  '&',  '*', '(', ')',
                    '+', '{', '}', '|', ':', '"',  '\'', '<',  '>', '?', ',',
                    '.', '/', ';', '[', ']', '\\', '\n', '\t', '=', '-', ' '}),
  m_tooltipRunner(700),
  m_ctxMenu(nullptr) {
  installEventFilter(this);
  connect(
    document(),
    &QTextDocument::modificationChanged,
    this,
    &CodeEdit::dirtyChanged);

  setFontName("");

  // setup context menu
  initActions();
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(
    this,
    &QWidget::customContextMenuRequested,
    this,
    &CodeEdit::showContextMenu);

  // init settings and styles from global settings/style modules
  initSettings();
  initStyle();

  // connect slots
  connect(this, &QPlainTextEdit::textChanged, this, &CodeEdit::onTextChanged);
  connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeEdit::update);
  connect(
    this,
    &QPlainTextEdit::cursorPositionChanged,
    this,
    &CodeEdit::update);
  connect(this, &QPlainTextEdit::selectionChanged, this, &CodeEdit::update);

  setMouseTracking(true);
  setCenterOnScroll(true);
  setLineWrapMode(QPlainTextEdit::NoWrap);
  setCursorWidth(2);
  m_autoResetStyleSheet = true;
  resetStyleSheet();
}

void CodeEdit::setTabSize(int size) {
  if (size < 2) {
    size = 2;
  }
  m_tabSize = size;
  for (auto c : m_clones) {
    c->setTabSize(size);
  }
}

void CodeEdit::setSaveOnFocusOut(bool save) {
  m_saveOnFocusOut = save;
  for (auto c : m_clones) {
    c->setSaveOnFocusOut(save);
  }
}

void CodeEdit::setShowsWhiteSpaces(bool show) {
  if (m_showWhitespaces != show) {
    m_showWhitespaces = show;
    setWhitespaceFlags(show);
    for (auto c : m_clones) {
      c->setShowsWhiteSpaces(show);
    }
    rehighlight();
  }
}

void CodeEdit::setFontName(const QString &name) {
  auto fontName = name.isEmpty()
    ? QFontDatabase::systemFont(QFontDatabase::FixedFont).family()
    : name;
  m_fontFamily = fontName;
  if (m_autoResetStyleSheet) {
    resetStyleSheet();
  }
  for (auto c : m_clones) {
    c->setFontName(fontName);
  }
}

void CodeEdit::resetStyleSheet() {
  // This function is called very often during initialization, which
  // impacts performance. This is a hack to avoid this.
  setFont(QFont(m_fontFamily, m_fontSize + m_zoomLevel));
  if (qApp->styleSheet() != "" || m_useStylesheet) {
    m_useStylesheet = true;
    // On Windows, if the application once had a stylesheet, we must
    // keep on using a stylesheet otherwise strange colors appear
    // see https://github.com/OpenCobolIDE/OpenCobolIDE/issues/65
    // Also happen on plasma 5

#ifdef Q_OS_WIN
    setStyleSheet(QString("QPlainTextEdit { background-color: %1; color: %2 }")
                    .arg(m_background.name(), m_foreground.name()));
#else
    const char *env = std::getenv("DESKTOP_SESSION");
    bool plasma = env != nullptr && std::strcmp(env, "plasma") == 0;
    if (plasma) {
      setStyleSheet(
        QString("QPlainTextEdit { background-color: %1; color: %2 }")
          .arg(m_background.name(), m_foreground.name()));
    } else {
      // on linux/osx we just have to set an empty stylesheet to
      // cancel any previous stylesheet and still keep a correct
      // style for scrollbars
      setStyleSheet("");
    }
#endif
  } else {
    auto p = palette();
    p.setColor(QPalette::Base, m_background);
    p.setColor(QPalette::Text, m_foreground);
    p.setColor(QPalette::Highlight, m_selBackground);
    p.setColor(QPalette::HighlightedText, m_selForeground);
    setPalette(p);
  }
  repaint();
}

/** @brief Init context menu action */
void CodeEdit::initActions() {
  // TODO
}

/**
  @brief Adds a sub-menu to the editor context menu.

  Menu are put at the bottom of the context menu.
  .. note:: to add a menu in the middle of the context menu, you can
      always add its menuAction().
  @param menu menu to add
 */
void CodeEdit::addMenu(QMenu *menu) {
  if (std::find(m_menus.cbegin(), m_menus.cend(), menu) == m_menus.cend()) {
    m_menus.append(menu);
    //self._menus = sorted(list(set(self._menus)), key=lambda x: x.title())
    for (auto action : menu->actions()) {
      action->setShortcutContext(Qt::WidgetShortcut);
    }
    addActions(menu->actions());
  }
}

QMenu *CodeEdit::createContextMenu(const QPoint &pos) {
  QMenu *mnu;
  if (m_createStandardActions) {
    mnu = createStandardContextMenu(pos);
  } else {
    mnu = new QMenu();
  }
  mnu->addActions(QList<QAction *>::fromVector(m_actions));
  mnu->addSeparator();
  for (auto menu : m_menus) {
    mnu->addMenu(menu);
  }
  return mnu;
}

/** @brief Shows the context menu */
void CodeEdit::showContextMenu(const QPoint &pos) {
  auto tc = textCursor();
  auto nc = cursorForPosition(pos);
  if (
    nc.position() < tc.selectionStart() || nc.position() >= tc.selectionEnd()) {
    setTextCursor(nc);
  }
  if (m_ctxMenu->actions().size() > 1 && m_showCtxMenu) {
    m_ctxMenu = createContextMenu(mapToGlobal(pos));
  }
}

void CodeEdit::initSettings() {
  m_showWhitespaces = false;
  m_tabSize = 4;
  m_indentChar = IndentChar::Space;
  setTabStopDistance(m_tabSize * fontMetrics().horizontalAdvance(" "));
  setWhitespaceFlags(m_showWhitespaces);
}

void CodeEdit::initStyle() {
  m_background = QColor(Qt::white);
  m_foreground = QColor(Qt::black);
  m_whitespaceForeground = QColor(Qt::lightGray);
  m_selBackground = qApp->palette().highlight().color();
  m_selForeground = qApp->palette().highlightedText().color();
  m_fontSize = 10;
  // TODO m_fontFamily ???
  // self.font_name = ""
}

void CodeEdit::setWhitespaceFlags(bool show) {
  auto doc = document();
  auto options = doc->defaultTextOption();
  if (show) {
    options.setFlags(options.flags() | QTextOption::ShowTabsAndSpaces);
  } else {
    options.setFlags(options.flags() & ~QTextOption::ShowTabsAndSpaces);
  }
  doc->setDefaultTextOption(options);
}