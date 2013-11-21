/* Copyright (c) 2012-2013 Hadron Industries, Inc. All rights reserved. */

#include "BasicDrome.h"

#include "gspeak-uhid-options.h"

int main (int argc, char *argv[])
{
  gengetopt_args_info args_info;

  if (cmdline_parser (argc, argv, &args_info) != 0)
    exit (EXIT_FAILURE) ;

  BasicDrome *base = new BasicDrome ("ghid", argc, argv);

  base->SetupGestator ();

  base->Respire ();
  base->Delete ();

  return (EXIT_SUCCESS);
}
