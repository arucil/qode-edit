#include "TextDecoration.h"

TextDecoration::TextDecoration(
  const QTextCursor &cursor,
  const TextDecorationRange &range,
  int drawOrder,
  const QString &tooltip,
  bool fullWidth) :
  QTextEdit::ExtraSelection {cursor},
  m_drawOrder(drawOrder),
  m_tooltip(tooltip) {
  init(range, fullWidth);
}

TextDecoration::TextDecoration(
  const QTextBlock &block,
  const TextDecorationRange &range,
  int drawOrder,
  const QString &tooltip,
  bool fullWidth) :
  QTextEdit::ExtraSelection {cursor},
  m_drawOrder(drawOrder),
  m_tooltip(tooltip) {
  init(range, fullWidth);
}

TextDecoration::TextDecoration(
  const QTextDocument &document,
  const TextDecorationRange &range,
  int drawOrder,
  const QString &tooltip,
  bool fullWidth) :
  QTextEdit::ExtraSelection {cursor},
  m_drawOrder(drawOrder),
  m_tooltip(tooltip) {
  init(range, fullWidth);
}

void TextDecoration::init(const TextDecorationRange &range, bool fullWidth) {
  if (fullWidth) {
    setFullWidth();
  }
  if (range.is<TextDecorationPositionRange>()) {
    auto range1 = range.get<TextDecorationPositionRange>();
    cursor.setPosition(range1.startPos);
    cursor.setPosition(range1.endPos, QTextCursor::KeepAnchor);
  } else if (range.is<TextDecorationLineRange>()) {
    auto range1 = range.get<TextDecorationLineRange>();
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    cursor.movePosition(
      QTextCursor::Down,
      QTextCursor::MoveAnchor,
      range1.startLine);
    cursor.movePosition(
      QTextCursor::Down,
      QTextCursor::KeepAnchor,
      range1.endLine - range1.startLine);
  }
}

void TextDecoration::setFullWidth(bool fullWidth, bool clear) {
  if (clear) {
    cursor.clearSelection();
  }
  format.setProperty(QTextFormat::FullWidthSelection, fullWidth);
}

bool TextDecoration::containsCursor(const QTextCursor &otherCursor) const {
  auto start = cursor.selectionStart();
  auto end = cursor.selectionEnd();
  if (otherCursor.atBlockEnd()) {
    end -= 1;
  }
  return start <= otherCursor.position() <= end;
}

void TextDecoration::setBold() {
  format.setFontWeight(QFont::Bold);
}

void TextDecoration::setUnderline(
  const QColor &color,
  QTextCharFormat::UnderlineStyle style) {
  format.setUnderlineStyle(style);
  format.setUnderlineColor(color);
}

void TextDecoration::setForeground(const QBrush &brush) {
  format.setForeground(brush);
}

void TextDecoration::setBackground(const QBrush &brush) {
  format.setBackground(brush);
}

void TextDecoration::setOutline(const QColor &color) {
  format.setProperty(QTextFormat::OutlinePen, QPen(color));
}

void TextDecoration::selectLine() {
  cursor.movePosition(QTextCursor::StartOfBlock);
  auto text = cursor.block().text();
  auto lindent = text.size() - text.lstrip().size();
  cursor.setPosition(cursor.block().position() + lindent);
  cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
}