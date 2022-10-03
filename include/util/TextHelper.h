#pragma once

#include <QTextCursor>
#include <QTimer>
#include <functional>

class CodeEdit;

struct CursorPosition {
  CursorPosition(int line, int column) : line(line), column(column) {}

  /**
   * @brief Line number, 0 based.
   */
  int line;

  /**
   * @brief Column number, 0 base.
   */
  int column;
};

/**
 * @brief Text helper helps you manipulate the content of CodeEdit and extends the
 * Qt text api for an easier usage.
 */
class TextHelper {
public:
  TextHelper(CodeEdit *);

  /**
     * @brief Moves the text cursor to the specified position.
     * 
     * @param line Number of the line to go to (0 based)
     * @param column Optional column number. Default is 0 (start of line).
     * @param move true to move the cursor. false will return the cursor
     *              without setting it on the editor.
     */
  QTextCursor gotoLine(int line, int column = 0, bool move = true);

  QString selectedText() const;

  /**
   * @brief Gets the word under cursor using the separators defined by
   * CodeEdit::wordSeparators.
   * 
   * .. note: Instead of returning the word string, this function returns
   *     a QTextCursor, that way you may get more information than just the
   *     string. To get the word, just call ``selectedText()`` on the returned
   *     value.
   * @param selectWholeWord If set to true the whole word is selected,
   *  else the selection stops at the cursor position.
   * @return The QTextCursor that contains the selected word.
   */
  QTextCursor wordUnderCursor(bool selectWholeWord = false) const;

  /**
   * @brief Gets the word under cursor using the separators defined by
   * CodeEdit::wordSeparators.
   * 
   * @param textCursor Optional custom text cursor (e.g. from a QTextDocument clone)
   */
  QTextCursor
  wordUnderCursor(QTextCursor textCursor, bool selectWholeWord = false) const;

  /**
   * @brief Selects the word under the **mouse** cursor.
   * @return A QTextCursor with the word under mouse cursor selected.
   */
  QTextCursor wordUnderMouseCursor() const;

  /**
   * @brief Returns the QTextCursor position.
   */
  CursorPosition cursorPosition() const;

  /**
   * @brief Returns the text cursor's line number.
   */
  int currentLineNbr() const;

  /**
   * @brief Returns the text cursor's column number.
   */
  int currentColumnNbr() const;

  /**
   * @brief  Returns the line count of the specified editor.
   */
  int lineCount() const;

  /**
   * @brief Gets the text of the specified line.
   * @param lineNbr The line number of the text to get.
   */
  QString lineText(int lineNbr) const;

  /**
   * @brief Gets the previous line text (relative to the current cursor pos).
   */
  QString prevLineText() const;

  /**
   * @brief Gets the next line text (relative to the current cursor pos).
   */
  QString nextLineText() const;

  /**
   * @brief Returns the text of the current line.
   */
  QString currentLineText() const;

  /**
   * @brief Replace an entire line with ``newText``.
   * 
   * @param lineNbr line number of the line to change.
   */
  void setLineText(int lineNbr, const QString &newText);

  void removeLastLine();

  /**
   * @brief Selects an entire line.
   * 
   * @param lineNbr Line number to select.
   * @param apply true to apply selection on the text editor
   * widget, false to just return the text cursor without setting it on the editor.
   */
  QTextCursor selectWholeLine(int lineNbr, bool apply = true);

  /**
   * @brief Selects an current line.
   * 
   * @param apply true to apply selection on the text editor
   * widget, false to just return the text cursor without setting it on the editor.
   */
  QTextCursor selectCurrentLine(bool apply = true);

  /**
   * @brief Selects entire lines between start and end line numbers.
   * 
   * This functions apply the selection and returns the text cursor that
   * contains the selection.
   * 
   * Optionally it is possible to prevent the selection from being applied
   * on the code editor widget by setting ``apply`` to false.
   * 
   * @param start Start line number (0 based)
   * @param end End line number (0 based). Use -1 to select up to the end of the document.
   * @param apply true to apply the selection before returning the QTextCursor.
   * @param selectBlocks true to operate on blocks rather than visual lines.
   * @return A QTextCursor that holds the requested selection
   */
  QTextCursor selectLines(
    int start = 0,
    int end = -1,
    bool apply = true,
    bool selectBlocks = false);

  QTextCursor moveCursorTo(int line);

private:
  CodeEdit *m_editor;
};
