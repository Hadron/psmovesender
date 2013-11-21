#include "PSMoveWandManager.h"

#include "psmove-gspeak-options.h"

int main (int argc, char *argv[])
{
  gengetopt_args_info args_info;

  if (cmdline_parser (argc, argv, &args_info) != 0)
    exit (EXIT_FAILURE) ;

  PSMoveWandManager manager;

  manager.ConnectAll ();
  manager.Loop ();

  return EXIT_SUCCESS;
}
