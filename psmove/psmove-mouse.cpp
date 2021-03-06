/* Copyright (c) 2012-2013 Hadron Industries, Inc. */

/* This is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see
   <http://www.gnu.org/licenses/>. */

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
