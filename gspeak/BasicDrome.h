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
