#include "util/DelayJobRunner.h"

DelayJobRunner::DelayJobRunner(int delay) : m_delay(delay) {
  m_timer.connect(
    &m_timer,
    &QTimer::timeout,
    this,
    &DelayJobRunner::execRequestedJob);
}

void DelayJobRunner::requestJob(std::function<void()> job) {
  cancel();
  m_job = job;
  m_timer.start(m_delay);
}

void DelayJobRunner::cancel() {
  m_timer.stop();
  m_job = nullptr;
}

void DelayJobRunner::execRequestedJob() {
  m_timer.stop();
  m_job();
}
