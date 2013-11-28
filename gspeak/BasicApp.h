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

#include <libNoodoo/FlatThing.h>
#include <libNoodoo/VisiDrome.h>
#include <libNoodoo/TexQuad.h>
#include <libNoodoo/GlyphoString.h>
#include <libImpetus/OEBlurt.h>
#include <libGestation/MouseGTot.h>
#include <libGestation/KeyboardGTot.h>
#include <libAfferent/WandPointingRod.h>
#include <libAfferent/OneHandPointingRod.h>

#include "BasicDrome.h"
#include "HIDPointer.h"

using namespace oblong::noodoo;
using namespace oblong::basement;
using namespace oblong::gestation;


class BasicApp : public NestyThing, 
  public OEPointing::Evts
{
  PATELLA_SUBCLASS (BasicApp, NestyThing);

private:

  ObRef <BasicDrome *> m_drome;
  ObRef <HIDPointer *> m_hid;

  ObRef <RoddedGlimpser *> m_psmove_wand_glimpser;

  ObRef <RoddedGlimpser *> m_optical_wand_glimpser;
  ObRef <RoddedGlimpser *> m_optical_glove_glimpser;

  ObRef <WandPointingRod *> m_psmove_wand_rod;
  ObRef <WandPointingRod *> m_optical_wand_rod;
  ObRef <OneHandPointingRod *> m_optical_glove_rod;

public:
  BasicApp (BasicDrome *drome);
  
  virtual void Setup ();

  virtual ObRetort Travail (Atmosphere *atm);
  virtual ObRetort Exhale (Atmosphere *atm);
};
