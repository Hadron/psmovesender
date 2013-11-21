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
