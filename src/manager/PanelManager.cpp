#include "manager/PanelManager.h"

#include <QPlainTextEdit>
#include <algorithm>

#include "CodeEdit.h"

PanelManager::PanelManager(CodeEdit *editor) :
  Manager(editor),
  m_cachedCursorPos(-1, -1) {
  connect(
    editor,
    &QPlainTextEdit::blockCountChanged,
    this,
    &PanelManager::updateViewportMargins);
  connect(editor, &QPlainTextEdit::updateRequest, this, &PanelManager::update);
  // except AttributeError:
  //   # QTextEdit
  //   editor.document().blockCountChanged.connect(
  //       self._update_viewport_margins)
}

PanelManager::~PanelManager() {
  clear();
}

bool PanelManager::append(Panel *panel, Panel::Position position) {
  auto ix = static_cast<int>(position);
  panel->m_orderInZone = static_cast<int>(m_panels[ix].size());
  if (m_panels[ix].count(panel->name()) == 0) {
    m_panels[ix][panel->name()] = panel;
    panel->m_position = position;
    panel->onInstall(editor());
    return true;
  } else {
    return false;
  }
}

Panel *PanelManager::remove(const QString &name) {
  PanelMap::iterator it;
  Panel *panel = nullptr;
  for (int i = 0; i < 4; i++) {
    it = m_panels[0].find(name);
    if (it != m_panels[0].end()) {
      panel = *it;
      break;
    }
  }
  if (panel == nullptr) {
    return nullptr;
  }
  panel->onUninstall();
  panel->hide();
  panel->setParent(nullptr);
  m_panels[static_cast<int>(panel->m_position)].erase(it);
  return panel;
}

Panel *PanelManager::get(const QString &name) const {
  for (int i = 0; i < 4; i++) {
    auto it = m_panels[0].find(name);
    if (it != m_panels[0].end()) {
      return *it;
    }
  }
  return nullptr;
}

void PanelManager::clear() {
  for (int i = 0; i < 4; i++) {
    for (auto j = m_panels[i].begin(); j != m_panels[i].end(); j++) {
      auto panel = *j;
      panel->setParent(nullptr);
      panel->deleteLater();
    }
  }
}

QList<Panel *> PanelManager::panelsForZone(Panel::Position position) const {
  return m_panels[static_cast<int>(position)].values();
}

void PanelManager::refresh() {
  resize();
  update(editor()->contentsRect(), 0, true);
}

void PanelManager::resize() {
  auto crect = editor()->contentsRect();
  auto viewCrect = editor()->viewport()->contentsRect();
  auto sizes = computeZonesSizes();
  auto tw = sizes.left() + sizes.right();
  auto th = sizes.bottom() + sizes.top();
  auto wOffset = crect.width() - (viewCrect.width() + tw);
  auto hOffset = crect.height() - (viewCrect.height() + th);

  auto left = 0;
  auto panels = panelsForZone(Panel::Position::Left);
  std::sort(panels.begin(), panels.end(), [](const auto &a, const auto &b) {
    a.m_orderInZone > b.m_orderInZone;
  });
  for (auto panel : panels) {
    if (!panel->isVisible()) {
      continue;
    }
    panel->adjustSize();
    auto sizeHint = panel->sizeHint();
    panel->setGeometry(
      crect.left() + left,
      crect.top() + sizes.top(),
      sizeHint.width(),
      crect.height() - sizes.bottom() - sizes.top() - hOffset);
    left += sizeHint.width();
  }

  auto right = 0;
  panels = panelsForZone(Panel::Position::Right);
  std::sort(panels.begin(), panels.end(), [](const auto &a, const auto &b) {
    a.m_orderInZone > b.m_orderInZone;
  });
  for (auto panel : panels) {
    if (!panel->isVisible()) {
      continue;
    }
    auto sizeHint = panel->sizeHint();
    panel->setGeometry(
      crect.right() - right - sizeHint.width() - wOffset,
      crect.top() + sizes.top(),
      sizeHint.width(),
      crect.height() - sizes.bottom() - sizes.top() - hOffset);
    right += sizeHint.width();
  }

  auto top = 0;
  panels = panelsForZone(Panel::Position::Top);
  std::sort(panels.begin(), panels.end(), [](const auto &a, const auto &b) {
    a.m_orderInZone < b.m_orderInZone;
  });
  for (auto panel : panels) {
    if (!panel->isVisible()) {
      continue;
    }
    auto sizeHint = panel->sizeHint();
    panel->setGeometry(
      crect.left(),
      crect.top() + top,
      crect.width() - wOffset,
      sizeHint.height());
    top += sizeHint.height();
  }

  auto bottom = 0;
  panels = panelsForZone(Panel::Position::Bottom);
  std::sort(panels.begin(), panels.end(), [](const auto &a, const auto &b) {
    a.m_orderInZone < b.m_orderInZone;
  });
  for (auto panel : panels) {
    if (!panel->isVisible()) {
      continue;
    }
    auto sizeHint = panel->sizeHint();
    panel->setGeometry(
      crect.left(),
      crect.bottom() - bottom - sizeHint.height() - hOffset,
      crect.width() - wOffset,
      sizeHint.height());
    bottom += sizeHint.height();
  }
}

void PanelManager::update(const QRect &rect, int dy, bool foreceUpdateMargins) {
  TextHelper helper(editor());
  // if not self:
  //     return
  for (int i = 0; i < 4; i++) {
    if (
      i == static_cast<int>(Panel::Position::Top)
      || i == static_cast<int>(Panel::Position::Bottom)) {
      continue;
    }
    for (auto panel : m_panels[i]) {
      if (panel->isScrollable() && dy) {
        panel->scroll(0, dy);
      }
      auto pos = helper.cursorPosition();
      if (
        pos.line != m_cachedCursorPos.line
        || pos.column != m_cachedCursorPos.column || panel->isScrollable()) {
        panel->update(0, rect.y(), panel->width(), rect.height());
      }
      m_cachedCursorPos = helper.cursorPosition();
    }
  }
  if (rect.contains(editor()->viewport()->rect()) || foreceUpdateMargins) {
    updateViewportMargins();
  }
}