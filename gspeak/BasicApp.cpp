#include "BasicApp.h"
#include "BasicDrome.h"

#include <iostream>

#include <libAfferent/WandPointingRod.h>
#include <libAfferent/OneHandPointingRod.h>
#include <libAfferent/OneHandBlurtRod.h>
#include <libAfferent/TwoHandBlurtRod.h>
#include <libBasement/ob-logging-macros.h>
#include <libPlasma/c++/PlasmaStreams.h>
#include <libPlasma/c++/Slaw.h>

#include "HIDPointer.h"

using namespace oblong::noodoo;
using namespace oblong::impetus;
using namespace oblong::plasma;

BasicApp::BasicApp (BasicDrome *ndrome)
{
  m_drome = ndrome;
  Setup ();
}

void BasicApp::Setup ()
{
  m_hid = new HIDPointer;
  AppendChild (~m_hid);

  OneHandPointingRod *r;
  r = new OneHandPointingRod;  
  r->AppendEntryGripe ("**x|-:**");
  r->AppendEntryGripe ("**^|-:**");
  r->AppendHardenGripe ("**x||:**");
  r->AppendHardenGripe ("**^||:**");
  r->SetGenus ("one-finger");
  r->SetPressureName ("one");
  m_optical_glove_rod = r;

  m_optical_wand_rod = new WandPointingRod;
  (~m_optical_wand_rod)->SetNumButtons (2);

  m_psmove_wand_rod = new WandPointingRod;
  (~m_psmove_wand_rod)->SetNumButtons (20);

  m_psmove_wand_glimpser = new RoddedGlimpser ();
  (~m_psmove_wand_glimpser)->SetProtoRod (~m_psmove_wand_rod);
  (~m_psmove_wand_glimpser)->AppendEventTarget (~m_hid);

  m_optical_wand_glimpser = new RoddedGlimpser ();
  (~m_optical_wand_glimpser)->SetProtoRod (~m_optical_wand_rod);
  (~m_optical_wand_glimpser)->AppendEventTarget (~m_hid);

  m_optical_glove_glimpser = new RoddedGlimpser ();
  (~m_optical_glove_glimpser)->SetProtoRod (~m_optical_glove_rod);
  (~m_optical_glove_glimpser)->AppendEventTarget (~m_hid);

  (~m_drome)->OpticalGestator()->AppendGlimpser (~m_optical_glove_glimpser);
  (~m_drome)->OpticalGestator()->AppendGlimpser (~m_optical_wand_glimpser);
  (~m_drome)->PSMoveGestator()->AppendGlimpser (~m_psmove_wand_glimpser);
}

ObRetort BasicApp::Travail (Atmosphere *atm)
{  
  ObRetort ret = KneeObject::Travail (atm);
  assert (ret == OB_OK);

  return OB_OK;
}

ObRetort BasicApp::Exhale (Atmosphere *atm)
{
  return OB_OK;
}
