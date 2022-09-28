#include "Mode.h"

Mode::Mode(const char *name) :
  m_name(name),
  m_enabled(false),
  m_editor(nullptr) {}

void Mode::setEnabled(bool enabled) {
  if (m_enabled != enabled) {
    m_enabled = enabled;
    onStateChanged(enabled);
  }
}

void Mode::onInstall(CodeEdit *editor) {
  m_editor = editor;
  m_enabled = true;
}

void Mode::onUninstall() {
  m_editor = nullptr;
  m_enabled = false;
}

void Mode::onStateChanged(bool) {}

void Mode::cloneSettings(Mode *) {}