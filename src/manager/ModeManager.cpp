#include "manager/ModeManager.h"

#include "mode/Mode.h"

ModeManager::ModeManager(CodeEdit *editor) : Manager(editor) {}

ModeManager::~ModeManager() {
  clear();
}

bool ModeManager::append(Mode *mode) {
  if (m_modes.count(mode->name()) == 0) {
    m_modes.insert(mode->name(), mode);
    mode->onInstall(editor());
    return true;
  }
  return false;
}

Mode *ModeManager::get(const QString &name) const {
  auto i = m_modes.find(name);
  if (i != m_modes.cend()) {
    return i.value();
  } else {
    return nullptr;
  }
}

Mode *ModeManager::remove(const QString &name) {
  auto i = m_modes.find(name);
  if (i != m_modes.cend()) {
    m_modes.erase(i);
    auto mode = i.value();
    mode->onUninstall();
    return mode;
  } else {
    return nullptr;
  }
}

void ModeManager::clear() {
  for (auto i = m_modes.begin(); i != m_modes.end(); i++) {
    i.value()->onUninstall();
    // TODO deleteLater()
    delete i.value();
  }
  m_modes.clear();
}

ModeManager::ModeMap::const_iterator ModeManager::cbegin() const {
  return m_modes.cbegin();
}

ModeManager::ModeMap::const_iterator ModeManager::cend() const {
  return m_modes.cend();
}