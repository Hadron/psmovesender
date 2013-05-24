#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "PSMoveController.h"
#include "PSMoveManager.h"

int main (int argc, char *argv[])
{
#ifdef __linux
  // moved_monitor *monitor = moved_monitor_new(on_monitor_update, server);
#endif

  PSMoveManager manager;
  manager.ConnectAll ();

  manager.Loop ();
}
