#pragma once

#include "Manager.h"

/**
  @brief The backend controller takes care of controlling the client-server
  architecture.
  It is responsible of starting the backend process and the client socket and
  exposes an API to easily control the backend:
      - start
      - stop
      - send_request
 */
class BackendManager : public Manager {
public:
  BackendManager(CodeEdit *editor);
  ~BackendManager();

private:
};
