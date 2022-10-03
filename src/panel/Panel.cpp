#include "panel/Panel.h"

#include <QApplication>
#include <QPaintEvent>
#include <QPainter>

#include "CodeEdit.h"

Panel::Panel(const char *name, bool isDynamic) :
  Mode(name),
  m_dynamic(isDynamic),
  m_orderInZone(-1),
  m_scrollable(false),
  m_position(Position::Left) {}

void Panel::setScrollable(bool scrollable) {
  m_scrollable = scrollable;
}

void Panel::onInstall(CodeEdit *editor) {
  Mode::onInstall(editor);
  setParent(editor);
  setPalette(qApp->palette());
  setFont(qApp->font());
  editor->m_panels.refresh();
  m_backgroundBrush = QBrush(QColor(palette().window().color()));
  m_foregroundPen = QPen(QColor(palette().windowText().color()));
}

void Panel::paintEvent(QPaintEvent *event) {
  if (isVisible()) {
    m_backgroundBrush = QBrush(QColor(palette().window().color()));
    m_foregroundPen = QPen(QColor(palette().windowText().color()));
    QPainter p(this);
    p.fillRect(event->rect(), m_backgroundBrush);
  }
}

void Panel::setVisible(bool visible) {
  QWidget::setVisible(visible);
  if (editor() != nullptr) {
    editor()->m_panels.refresh();
  }
}