#include "Cache.h"

#include <QFile>
#include <QSet>
#include <QString>
#include <QTextCodec>
#include <QTextStream>

const char *KEY_USER_DEFINED_ENCODINGS = "userDefinedEncodings";
const char *KEY_CACHED_ENCODINGS = "cachedFileEncodings";
const char *KEY_CACHED_CURSOR_POSITION = "cachedCursorPositions";

Cache::Cache(const QString &suffix) :
  m_settings("plodsoft", QString("qode.core%1").arg(suffix)) {}

void Cache::clear() {
  m_settings.clear();
}

QVector<QTextCodec *> Cache::preferredEncodings() const {
  auto localeEncoding = QTextCodec::codecForLocale();
  QList<QVariant> defaultEncodings = {localeEncoding->name()};
  if (localeEncoding->name() != "UTF-8") {
    defaultEncodings.append(QByteArray("UTF-8"));
  }
  auto encodings =
    m_settings.value(KEY_USER_DEFINED_ENCODINGS, defaultEncodings).toList();
  QVector<QTextCodec *> result;
  for (auto &x : encodings) {
    result.append(QTextCodec::codecForName(x.toByteArray()));
  }
  return result;
}

void Cache::setPreferredEncodings(const QVector<QTextCodec *> &encodings) {
  QList<QVariant> value;
  for (auto x : encodings) {
    value.append(x->name());
  }
  m_settings.setValue(KEY_USER_DEFINED_ENCODINGS, value);
}

QTextCodec *Cache::getFileEncoding(
  const QString &path,
  QTextCodec *preferredEncoding) const {
  auto encodings = m_settings.value(KEY_CACHED_ENCODINGS).toMap();
  auto it = encodings.find(path);
  if (it != encodings.end()) {
    return QTextCodec::codecForName(it->toByteArray());
  } else {
    auto encodings = preferredEncodings();
    if (preferredEncoding) {
      encodings.insert(0, preferredEncoding);
      for (auto encoding : encodings) {
        QFile f(path);
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        if (!f.isOpen()) {
          return {};
        }
        QTextStream stream(&f);
        stream.setCodec(encoding);
        stream.readAll();
        if (stream.status() == QTextStream::ReadPastEnd) {
          return encoding;
        }
      }
      return nullptr;
    }
  }
}

void Cache::setFileEncoding(const QString &path, QTextCodec *encoding) {
  auto encodings = m_settings.value(KEY_CACHED_ENCODINGS).toMap();
  encodings.insert(path, encoding->name());
  m_settings.setValue(KEY_CACHED_ENCODINGS, encodings);
}

int Cache::getCursorPosition(const QString &path) const {
  auto positions = m_settings.value(KEY_CACHED_CURSOR_POSITION).toMap();
  auto it = positions.find(path);
  if (it != positions.end()) {
    return it->toInt();
  } else {
    return 0;
  }
}

void Cache::setCursorPosition(const QString &path, int position) {
  auto positions = m_settings.value(KEY_CACHED_CURSOR_POSITION).toMap();
  positions.insert(path, position);
  m_settings.setValue(KEY_CACHED_CURSOR_POSITION, positions);
}