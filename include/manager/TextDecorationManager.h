#pragma once

#include <set>
#include <unordered_set>

#include "../TextDecoration.h"
#include "Manager.h"

/** @brief Manages the collection of TextDecoration that have been set on the editor widget.
 */
class TextDecorationManager: public Manager {
public:
  TextDecorationManager(CodeEdit *editor);
  ~TextDecorationManager();

  /**
     * @brief Adds a text decoration on a CodeEdit instance
     * 
     * The ownership of the decoration is transferred to the manager.
     * 
     * @return Returns false if the decoration was already in the manager.
     */
  bool append(TextDecoration *);

  /**
     * @brief Removes a text decoration from the editor.
     * 
     * The ownership of the decoration is transferred to the caller.
     * 
     * @return Returns false if the decoration did not exist.
     */
  bool remove(TextDecoration *);

  void clear();

private:
  void updateEditorSelections();

private:
  std::multiset<TextDecoration *, TextDecorationCmp> m_decorationList;
  std::unordered_set<TextDecoration *> m_decorationSet;
};