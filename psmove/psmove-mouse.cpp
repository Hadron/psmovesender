#include "PSMoveHIDMouseManager.h"

#include "psmove-mouse-options.h"

#include <stdlib.h>

int main (int argc, char *argv[])
{
  gengetopt_args_info args_info;

  if (cmdline_parser (argc, argv, &args_info) != 0)
    exit (EXIT_FAILURE);

  CameraGroup cg = ::parse (args_info.cavelayout_arg);

  for (auto i = cg.cameras.begin(); i != cg.cameras.end(); i++) {
    std::cout << i->second << std::endl;
  }

  PSMoveHIDMouseManager manager (cg);

  manager.ConnectAll ();
  manager.Loop ();

  return EXIT_SUCCESS;
}
