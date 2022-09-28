#pragma once

/**
 * @file Cache.h
 * 
 * @brief This module contains a class to access the pyQode settings (QSettings).
 * 
 * QSettings are used to cache some specific settings.
 * 
 * At the moment, we use it to store the lists of encoding that appears in
 * the encoding menu (to not have a too big encoding menu, user can choose which
 * encoding should be display, offering a small compact menu with all
 * its favorite encodings). We also cache encoding used to save or load a
 * file so that we can reuse it automatically next time the user want to
 * open the same file.
 * 
 * We also use this to cache some editor states (such as the last cursor position
 * for a specific file path)
 * 
 * We do not store editor styles and settings here. Those kind of settings are
 * better handled at the application level.
 */

#include <QSettings>
#include <QString>
#include <QVector>

class QTextCodec;

/**
 * @brief Provides an easy acces to the cache by exposing some wrapper properties
 * over QSettings.
 */
class Cache {
public:
  Cache(const QString &suffix = "");

  void clear();

  /**
   * @brief The list of user defined encodings, for display in the encodings menu/combobox.
   */
  QVector<QTextCodec *> preferredEncodings() const;

  void setPreferredEncodings(const QVector<QTextCodec *> &encodings);

  /**
   * @brief Gets an eventual cached encoding for file_path.
   * 
   * @return Returns nullptr if no encoding were cached for the specified file path.
   */
  QTextCodec *getFileEncoding(
    const QString &path,
    QTextCodec *preferredEncoding = nullptr) const;

  /**
   * @brief Cache encoding for the specified file path.
   */
  void setFileEncoding(const QString &path, QTextCodec *encoding);

  /**
   * @brief Gets the cached cursor position for file_path.
   */
  int getCursorPosition(const QString &path) const;

  /**
   * @brief Cache encoding for the specified file path.
   */
  void setCursorPosition(const QString &path, int pos);

private:
  QSettings m_settings;
};