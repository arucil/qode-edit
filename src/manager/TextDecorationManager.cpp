#include "manager/TextDecorationManager.h"

#include <algorithm>

#include "CodeEdit.h"

TextDecorationManager::TextDecorationManager(CodeEdit *editor) :
  Manager(editor) {}

TextDecorationManager::~TextDecorationManager() {
  clear();
}

bool TextDecorationManager::append(TextDecoration *decoration) {
  if (m_decorationSet.insert(decoration).second) {
    m_decorationList.insert(decoration);
    updateEditorSelections();
    return true;
  }
  return false;
}

bool TextDecorationManager::remove(TextDecoration *decoration) {
  if (m_decorationSet.erase(decoration) == 1) {
    for (auto i = m_decorationList.begin(); i != m_decorationList.end(); i++) {
      if (*i == decoration) {
        m_decorationList.erase(i);
        break;
      }
    }
    return true;
  }
  return false;
}

void TextDecorationManager::updateEditorSelections() {
  QList<QTextEdit::ExtraSelection> decorations;
  for (auto i = m_decorationList.cbegin(); i != m_decorationList.cend(); i++) {
    decorations.append({(*i)->cursor, (*i)->format});
  }
  editor()->setExtraSelections(decorations);
}

void TextDecorationManager::clear() {
  for (auto i = m_decorationList.cbegin(); i != m_decorationList.cend(); i++) {
    delete *i;
  }
  m_decorationList.clear();
  m_decorationSet.clear();
}