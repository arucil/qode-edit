#pragma once

#include <QTimer>
#include <functional>

/**
  @brief Utility class for running job after a certain delay.
  
  If a new request is made during this delay, the previous request is dropped
  and the timer is restarted for the new request.
  We use this to implement a cooldown effect that prevents jobs from being
  executed while the IDE is not idle.
  A job is a simple callable.
 */
class DelayJobRunner {
public:
  /**
    @param delay Delay (ms) to wait before running the job. This delay applies
    to all requests and cannot be changed afterwards.
   */
  DelayJobRunner(int delay = 500);

  /**
    @brief Request a job execution.
    
    The job will be executed after the delay specified in the DelayJobRunner
    contructor elapsed if no other job is requested until then.
    @param job job.
   */
  void requestJob(std::function<void ()> job);

  /** @brief Cancel pending request. */
  void cancel();

private slots:
  void execRequestedJob();

private:
  QTimer m_timer;
  int m_delay;
  std::function<void ()> m_job;
};