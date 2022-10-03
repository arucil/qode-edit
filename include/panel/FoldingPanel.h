#pragma once

#include "panel/Panel.h"

/**
 * @brief Displays the document outline and lets the user collapse/expand blocks.
 * 
 * The data represented by the panel come from the text block user state and
 * is set by the SyntaxHighlighter mode.
 * 
 * The panel does not expose any function that you can use directly. To
 * interact with the fold tree, you need to modify text block fold level or
 * trigger state using TextBlockHelper or :mod:`api.folding`
 *                                        TODO folding module ?
 */
class FoldingPanel: public Panel {
  Q_OBJECT
public:
  const char *NAME = "FoldingPanel";
};