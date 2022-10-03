#include "util/TextHelper.h"

#include "CodeEdit.h"
#include "panel/FoldingPanel.h"

TextHelper::TextHelper(CodeEdit *editor) : m_editor(editor) {}

QTextCursor TextHelper::gotoLine(int line, int column, bool move) {
  auto textCursor = moveCursorTo(line);
  if (column) {
    textCursor.movePosition(
      QTextCursor::Right,
      QTextCursor::MoveAnchor,
      column);
  }
  if (move) {
    auto block = textCursor.block();
    // unfold parent fold trigger if the block is collapsed
    auto foldingPanel = m_editor->m_panels.get(FoldingPanel::Name);
    if (foldingPanel) {
      if (!block.isVisible()) {
        auto block = FoldScope::findParentScope(block);
        if (TextBlockHelper::isCollapsed(block)) {
          foldingPanel->toggleFoldTrigger(block);
        }
      }
    }
    m_editor->setTextCursor(textCursor);
  }
  return textCursor;
}

QTextCursor TextHelper::wordUnderCursor(bool selectWholeWord) const {
  return wordUnderCursor(m_editor->textCursor(), selectWholeWord);
}

QTextCursor TextHelper::wordUnderCursor(
  QTextCursor textCursor,
  bool selectWholeWord) const {
  auto &wordSeps = m_editor->m_wordSeparators;
  auto startPos = textCursor.position();
  auto endPos = textCursor.position();
  // select char by char until we are at the original cursor position.
  while (!textCursor.atStart()) {
    textCursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1);
    auto text = textCursor.selectedText();
    if (text.isEmpty()) {
      // nothing selectable
      break;
    }
    if (
      text[0].isSpace()
      || wordSeps.contains(text[0]) && text != "n" && text != "t") {
      // start boundary found
      break;
    }
    startPos = textCursor.position();
    textCursor.setPosition(startPos);
  }
  if (selectWholeWord) {
    // select the rest of the word
    textCursor.setPosition(endPos);
    while (!textCursor.atEnd()) {
      textCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
      auto text = textCursor.selectedText();
      if (
        text[0].isSpace()
        || wordSeps.contains(text[0]) && text != "n" && text != "t") {
        // end boundary found
        break;
      }
      endPos = textCursor.position();
      textCursor.setPosition(endPos);
    }
  }
  // now that we have the boundaries, we can select the text
  textCursor.setPosition(startPos);
  textCursor.setPosition(endPos, QTextCursor::KeepAnchor);
  return textCursor;
}

QTextCursor TextHelper::wordUnderMouseCursor() const {
  auto textCursor = m_editor->cursorForPosition(m_editor->m_lastMousePos);
  auto textCursor = wordUnderCursor(textCursor, true);
  return textCursor;
}

CursorPosition TextHelper::cursorPosition() const {
  return CursorPosition(
    m_editor->textCursor().blockNumber(),
    m_editor->textCursor().columnNumber());
}

int TextHelper::currentLineNbr() const {
  return cursorPosition().line;
}

int TextHelper::currentColumnNbr() const {
  return cursorPosition().column;
}

int TextHelper::lineCount() const {
  return m_editor->document().blockCount();
}

QString TextHelper::lineText(int lineNbr) const {
  // TODO Under some (apparent) race conditions, this function can be called
  // with a None line number. This should be fixed in a better way, but
  // for now we return an empty string to avoid crashes.
  // if line_nbr is None:
  //   return ''
  auto doc = m_editor->document();
  auto block = doc->findBlockByNumber(lineNbr);
  return block.text();
}

QString TextHelper::prevLineText() const {
  if (currentLineNbr()) {
    return lineText(currentLineNbr() - 1);
  } else {
    return {};
  }
}

QString TextHelper::nextLineText() const {
  if (currentLineNbr() < lineCount() - 1) {
    return lineText(currentLineNbr() + 1);
  } else {
    return {};
  }
}

QString TextHelper::currentLineText() const {
  return lineText(currentLineNbr());
}

void TextHelper::setLineText(int lineNbr, const QString &newText) {
  auto textCursor = moveCursorTo(lineNbr);
  textCursor.select(QTextCursor::LineUnderCursor);
  textCursor.insertText(newText);
  m_editor->setTextCursor(textCursor);
}

void TextHelper::removeLastLine() {
  auto textCursor = m_editor->textCursor();
  textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
  textCursor.select(QTextCursor::LineUnderCursor);
  textCursor.removeSelectedText();
  textCursor.deletePreviousChar();
  m_editor->setTextCursor(textCursor);
}

QTextCursor TextHelper::selectWholeLine(int line, bool apply) {
  return selectLines(line, line, apply);
}

QTextCursor TextHelper::selectCurrentLine(bool apply) {
  return selectLines(line, line, apply);
}

QTextCursor
TextHelper::selectLines(int start, int end, bool apply, bool selectBlocks) {
  if (end == -1) {
    end = lineCount() - 1;
  }
  if (start < 0) {
    start = 0;
  }
  auto textCursor = moveCursorTo(start);
  QTextCursor::MoveOperation moveStart, moveEnd, moveUp, moveDown;
  if (selectBlocks) {
    moveStart = QTextCursor::StartOfBlock;
    moveEnd = QTextCursor::EndOfBlock;
    moveUp = QTextCursor::PreviousBlock;
    moveDown = QTextCursor::NextBlock;
  } else {
    moveStart = QTextCursor::StartOfLine;
    moveEnd = QTextCursor::EndOfLine;
    moveUp = QTextCursor::Up;
    moveDown = QTextCursor::Down;
  }
  if (end > start) {
    // going down
    textCursor.movePosition(moveDown, QTextCursor::KeepAnchor, end - start);
    textCursor.movePosition(moveEnd, QTextCursor::KeepAnchor)
  } else if (end < start) {
    // going up
    textCursor.movePosition(moveEnd, QTextCursor::MoveAnchor);
    textCursor.movePosition(moveUp, QTextCursor::KeepAnchor, start - end);
    textCursor.movePosition(moveStart, QTextCursor::KeepAnchor);
  } else {
    textCursor.movePosition(moveEnd, QTextCursor::KeepAnchor);
  }
  if (apply) {
    m_editor->setTextCursor(textCursor);
  }
  return textCursor;
}

QTextCursor TextHelper::moveCursorTo(int line) {
  auto cursor = m_editor->textCursor();
  auto block = m_editor->document()->findBlockByNumber(line);
  cursor.setPosition(block.position());
  return cursor;
}