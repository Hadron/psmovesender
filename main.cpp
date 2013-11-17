#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "PSMoveController.h"
#include "PSMoveWandManager.h"
#include "PSMoveHIDMouseManager.h"

int main (int argc, char *argv[])
{
  CameraGroup cg = ::parse ("/home/kdienes/test5.xml");

  for (auto i = cg.cameras.begin(); i != cg.cameras.end(); i++) {
    std::cout << i->second << std::endl;
  }

  PSMoveHIDMouseManager manager (cg);

  manager.ConnectAll ();
  manager.Loop ();

  return EXIT_SUCCESS;
}
