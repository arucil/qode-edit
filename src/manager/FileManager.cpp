#include "manager/FileManager.h"
#include "Cache.h"

#include <QDir>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QTextCodec>

FileManager::FileManager(CodeEdit *editor, bool replaceTabsBySpaces) :
  Manager(editor),
  m_replaceTabsBySpaces(replaceTabsBySpaces),
  m_sizeLimit(10000000),
  m_encoding(QTextCodec::codecForLocale()),
  m_opening(false),
  m_saving(true),
  m_safeSave(true),
  m_deleteTrailingWhitespaces(true),
  m_restoreCursor(true),
  m_preferredEol(Eol::System),
  m_eol(eolString(Eol::System)),
  m_autoDetectEol(true) {}

const QString &FileManager::path() const {
  return m_path;
}

QString FileManager::baseName() const {
  return m_path.isEmpty() ? m_path : QFileInfo(m_path).baseName();
}

QString FileManager::extension() const {
  return m_path.isEmpty() ? m_path : QFileInfo(m_path).completeSuffix();
}

QString FileManager::dirName() const {
  return m_path.isEmpty() ? m_path : QFileInfo(m_path).dir().absolutePath();
}

void FileManager::setAutoDetectEol(bool autoDetect) {
  m_autoDetectEol = autoDetect;
  if (!autoDetect) {
    m_eol = eolString(m_preferredEol);
  }
}

void FileManager::setPreferredEol(Eol eol) {
  m_preferredEol = eol;
  if (!m_autoDetectEol) {
    m_eol = eolString(m_preferredEol);
  }
}

QIcon FileManager::icon() const {
  return m_path.isEmpty() ? QIcon()
                          : QFileIconProvider().icon(QFileInfo(m_path));
}

QMimeType FileManager::getMimeType(const QString &filePath) {
  QMimeDatabase db;
  auto mime = db.mimeTypeForFile(filePath, QMimeDatabase::MatchExtension);
  if (mime.name() == "application/octet-stream") {
    return db.mimeTypeForName("text/plain");
  } else {
    return mime;
  }
}

bool FileManager::open(const QString &filePath, QTextCodec *encoding, bool useCachedEncoding) {
  m_opening = true;
  Cache settings;
  QFileInfo fileInfo(filePath);
  if (!fileInfo.exists()) {
    return false;
  }

  auto path = QFileInfo(fileInfo.absoluteFilePath()).canonicalFilePath();
  m_path = path;

  if (encoding == nullptr) {
    encoding = QTextCodec::codecForLocale();
  }

        if (useCachedEncoding) {
          auto cachedEncoding = settings.getFileEncoding(m_path, encoding);
          if (cachedEncoding) {
            encoding = cachedEncoding;
          }
        }

  auto enableModes = fileInfo.size() < m_sizeLimit;
        for m in self.editor.modes:
            if m.enabled:
                m.enabled = enable_modes
        # open file and get its content
        try:
            with open(path, 'Ur', encoding=encoding) as file:
                content = file.read()
                if self.autodetect_eol:
                    self._eol = file.newlines
                    if isinstance(self._eol, tuple):
                        self._eol = self._eol[0]
                    if self._eol is None:
                        # empty file has no newlines
                        self._eol = self.EOL.string(self.preferred_eol)
                else:
                    self._eol = self.EOL.string(self.preferred_eol)
        except (UnicodeDecodeError, UnicodeError) as e:
            try:
                from pyqode.core.panels import EncodingPanel
                panel = self.editor.panels.get(EncodingPanel)
            except KeyError:
                raise e  # panel not found, not automatic error management
            else:
                panel.on_open_failed(path, encoding)
        else:
            # success! Cache the encoding
            settings.set_file_encoding(path, encoding)
            self._encoding = encoding
            # replace tabs by spaces
            if self.replace_tabs_by_spaces:
                content = content.replace("\t", " " * self.editor.tab_length)
            # set plain text
            self.editor.setPlainText(
                content, self.get_mimetype(path), self.encoding)
            self.editor.setDocumentTitle(self.editor.file.name)
            ret_val = True
            _logger().debug('file open: %s', path)
        self.opening = False
        if self.restore_cursor:
            self._restore_cached_pos()
        self._check_for_readonly()
        return ret_val
}

QString eolString(Eol eol) {
  switch (eol) {
    case Eol::System:
#ifdef Q_OS_WIN
      return "\r\n";
#elif defined(Q_OS_UNIX)
      return "\n";
#else
  #error "this platform is not supported"
#endif
    case Eol::Unix:
      return "\n";
  }
}
