#pragma once

#include "Manager.h"
#include <QString>
#include <QMimeType>

class CodeEdit;
class QTextCodec;
class QIcon;

/**
 * @brief This enum enumerates the possible Eol conventions:
            - System: apply the system Eol
            - Unix: force the use of Unix Eol (\\n)
            - Windows: force the use of Windows Eol (\\r\\n)
 */
enum class Eol { System, Unix, Windows };

/**
  Helps manage file operations:
        - opening and saving files
        - providing file icon
        - detecting mimetype
 */
class FileManager: public Manager {
public:
  /**
   * @param editor Code edit instance to work on.
   * @param replaceTabsBySpaces true to replace tabs by spaces on load/save.
   */
  FileManager(CodeEdit *editor, bool replaceTabsBySpaces = true);

  ~FileManager();

  /* @brief true to replace tabs by spaces on load/save. */
  bool replacesTabsBySpacesOnSL() const {
    return m_replaceTabsBySpaces;
  }

  /** @brief Opening flag. Set to true during the opening of a file. */
  bool isOpening() const {
    return m_opening;
  }

  /** @brief Saving flag. Set to true while saving the editor content to a file. */
  bool isSaving() const {
    return m_saving;
  }

  /**
   * @brief If true, the file is saved to a temporary file first. If the save went fine,
   * the temporary file is renamed to the final filename.
   **/
  bool isSafeSave() const {
    return m_safeSave;
  }

  void setSafeSave(bool safe) {
    m_safeSave = safe;
  }

  /**
   * @brief true to clean trailing whitespaces of changed lines. Default is true.
   **/
  bool deletesTrailingWhitespacesOnSave() const {
    return m_deleteTrailingWhitespaces;
  }

  /**
   * @brief true to restore cursor position (if the document has already been opened once).
   **/
  bool restoresCursorOnLoad() const {
    return m_restoreCursor;
  }

  bool autoDetectsEol() const {
    return m_autoDetectEol;
  }

  void setAutoDetectEol(bool autoDetect) ;

  Eol preferredEol() const {
    return m_preferredEol;
  }

  void setPreferredEol(Eol eol);

  /**
   * @brief Returns the file size limit.
   * 
   * If the size of the file to open is superior to the limit, then we disabled
   * syntax highlighting, code folding,... to improve the load time and the
   * runtime performances.
   * 
   * Default is 10MB.
   */
  size_t fileSizeLimit() const {
    return m_sizeLimit;
  }

  const QString &path() const;
  QString baseName() const;
  QString extension() const;
  QString dirName() const;
  QIcon icon() const;

  QTextCodec *encoding() const {
    return m_encoding;
  }

  /**
   * @brief Open a file and set its content on the editor widget.
   * 
   * qode-edit does not try to guess encoding. It's up to the client code to
   * handle encodings. You can either use a charset detector to detect
   * encoding or rely on a settings in your application. It is also up to
   * you to handle UnicodeDecodeError, unless you've added
   * class:`pyqode.core.panels.EncodingPanel` on the editor.
   * 
   * qode-edit automatically caches file encoding that you can later reuse it
   * automatically.
   * 
   * @param filePath Path of the file to open.
   * @param encoding Default file encoding. Default is to use the locale encoding.
   * @param useCachedEncoding true to use the cached encoding instead
   *     of ``encoding``. Set it to true if you want to force reload with a
   *     new encoding. Uses the locale encoding if useCachedEncoding is false.
   * 
   * @return If the file was successfully open.
   */
  bool open(const QString &filePath, QTextCodec *encoding = nullptr, bool useCachedEncoding = true);

  /**
   * @brief Guesses the mime type of a file. If mime type cannot be detected,
   * plain text is assumed.
   */
  static QMimeType getMimeType(const QString &filePath);

private:
  bool m_replaceTabsBySpaces;
  bool m_opening;
  bool m_saving;
  bool m_safeSave;
  bool m_deleteTrailingWhitespaces;
  bool m_restoreCursor;
  bool m_autoDetectEol;

  size_t m_sizeLimit;

  /** @brief canonical absolute path */
  QString m_path;

  /**
   * @brief Preferred Eol convention. This setting will be used for saving the
   * document unless autoDetectEol is True.
   */
  Eol m_preferredEol;
  QString m_eol;

  /** @brief store the last file encoding used to open or save the file. */
  QTextCodec *m_encoding;
};

QString eolString(Eol eol);