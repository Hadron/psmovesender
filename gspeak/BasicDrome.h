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

#pragma once

#include <libNoodoo/VisiDrome.h>
#include <libNoodoo/ObStyle.h>
#include <libBasement/LogLogger.h>
#include <libAfferent/Gestator.h>
#include <libAfferent/DualCase.h>

#include <libBasement/TWrangler.h>
#include <libBasement/RWrangler.h>
#include <libBasement/SWrangler.h>
#include <libBasement/LinearFloat.h>

class BasicApp;

using namespace oblong::afferent;
using namespace oblong::noodoo;

class BasicDrome : public VisiDrome
{
private:
  
  BasicApp *m_app;

  Gestator *m_g_optical;
  Gestator *m_g_psmove;

 public:
  BasicDrome (const Str &str, int argc, char *argv[]);

  void SetupGestator ();
  static ObRetort FeldSetup (VisiDrome *drome, VisiFeld *vf, Atmosphere *atm);
  ObRetort Setup (VFBunch *bunch, Atmosphere *atm);

  Gestator *OpticalGestator () { return m_g_optical; }
  Gestator *PSMoveGestator () { return m_g_psmove; }
};
