#pragma once

#include <QString>
#include <QTextCursor>
#include <QTextEdit>

#include "mapbox/variant.hpp"

using namespace mapbox::util;

class QTextBlock;
class QTextDocument;
class QBrush;
class QColor;

struct TextDecorationLineRange {
  int startLine;
  int endLine;
};

struct TextDecorationPositionRange {
  int startPos;
  int endPos;
};

/**
 * @brief The range of the text decoration is based on the cursor.
 */
struct TextDecorationCursorRange {};

using TextDecorationRange = variant<
  TextDecorationLineRange,
  TextDecorationPositionRange,
  TextDecorationCursorRange>;

/**
  @brief Helper class to quickly create a text decoration.
  
  The text decoration is an utility class that adds a few utility methods to
  QTextEdit.ExtraSelection.

  In addition to the helper methods, a tooltip can be added to a decoration.
  (useful for errors markers and so on...)
 */
class TextDecoration: public QObject, public QTextEdit::ExtraSelection {
public:
  /**
    @brief Creates a text decoration.
    @param drawOrder The draw order of the selection, highest values will
        appear on top of the lowest values.
   */
  TextDecoration(
    const QTextCursor &cursor,
    const TextDecorationRange &range,
    int drawOrder = 0,
    const QString &tooltip = "",
    bool fullWidth = false);

  /**
    @brief Creates a text decoration.
    @param drawOrder The draw order of the selection, highest values will
        appear on top of the lowest values.
   */
  TextDecoration(
    const QTextBlock &block,
    const TextDecorationRange &range,
    int drawOrder = 0,
    const QString &tooltip = "",
    bool fullWidth = false);

  /**
    @brief Creates a text decoration.
    @param drawOrder The draw order of the selection, highest values will
        appear on top of the lowest values.
   */
  TextDecoration(
    const QTextDocument &document,
    const TextDecorationRange &range,
    int drawOrder = 0,
    const QString &tooltip = "",
    bool fullWidth = false);

  /**
    @brief Enables FullWidthSelection (the selection does not stops at after the
        character instead it goes up to the right side of the widget).

    @param fullWidth use full width selection.
    @param clear clear any previous selection. Default is true.
   */
  void setFullWidth(bool fullWidth = true, bool clear = true);

  const QString &tooltip() const {
    return m_tooltip;
  }

  int drawOrder() const {
    return m_drawOrder;
  }

  /**
    @brief Checks if the textCursor is in the decoration
   */
  bool containsCursor(const QTextCursor &cursor) const;

  void setBold();
  void setUnderline(const QColor &color, QTextCharFormat::UnderlineStyle style);

  void setForeground(const QBrush &);
  void setBackground(const QBrush &);
  void setOutline(const QColor &);

  /**
    @brief Select the entire line but starts at the first non whitespace character
        and stops at the non-whitespace character.
   */
  void selectLine();

private:
  void init(const TextDecorationRange &range, bool fullWidth);

signals:
  void clicked();

private:
  QString m_tooltip;
  int m_drawOrder;
};

struct TextDecorationCmp {
  bool operator()(TextDecoration *a, TextDecoration *b) const {
    return a->drawOrder() < b->drawOrder();
  }
};