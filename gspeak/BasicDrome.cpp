#include <libNoodoo/WindowedVisiFeld.h>
#include <libNoodoo/VFFactory.h>

#include <libLoam/c/ob-log.h>

#include <tr1/functional>

#include "WindowedNullVisiFeld.h"

#include "BasicDrome.h"
#include "BasicApp.h"


static ObRetort SetupThunk (VFBunch *bunch, Atmosphere *atm, BasicDrome *drome)
{
  drome->Setup (bunch, atm);
  return OB_OK;
}

static WindowedVisiFeld *MakeWindowedNullVisiFeld ()
{
  return new WindowedNullVisiFeld ();
}

static VisiFeld *MakeNullVisiFeldFromSlaw (Slaw s)
{
  return new WindowedNullVisiFeld ();
}

BasicDrome::BasicDrome (const Str &str, int argc, char *argv[]) : VisiDrome (str, argc, argv)
{
  SetLogger (new LogLogger ());
  SetRespirePeriod (1.0 / 60.0);
  MakeSedatable ();

  AppendFeldSetupHook (FeldSetup, "feld-hook");

  VFFactory::MakeWindowedVisiFeld = MakeWindowedNullVisiFeld;
  VFFactory::MakeVisiFeldFromSlaw = MakeNullVisiFeldFromSlaw;

  FindVFBunch ()->AppendPostFeldInfoHook (SetupThunk, this, "post-feld-info-hook");

  m_app = NULL;
  m_g_optical = NULL;
  m_g_psmove = NULL;
}

ObRetort BasicDrome::FeldSetup (VisiDrome *drome, VisiFeld *vf, Atmosphere *atm)
{
  ObStyle::LoadStyle ("oblong");
  return OB_OK;
}

ObRetort BasicDrome::Setup (VFBunch *bunch, Atmosphere *atm)
{
  m_app = new BasicApp (this);
  AppendChild (m_app);

  return OB_OK;
}

void BasicDrome::SetupGestator ()
{
  m_g_optical = new Gestator ();
  m_g_optical->SetSenseCase (new DualCase (150));

  m_g_psmove = new Gestator ();
  m_g_psmove->SetSenseCase (new DualCase (150));
  
  const char *DEFAULT_GRIPES_POOL = "gripes";
  const char *DEFAULT_WANDS_POOL = "wands";
  const char *DEFAULT_PSMOVE_POOL = "wands";

  const char *gripes_pool = getenv ("GRIPES_POOL");
  if (gripes_pool == NULL) { gripes_pool = DEFAULT_GRIPES_POOL; }

  const char *wands_pool = getenv ("WANDS_POOL");
  if (wands_pool == NULL) { wands_pool = DEFAULT_WANDS_POOL; }

  const char *psmove_pool = getenv ("PSMOVE_POOL");
  if (psmove_pool == NULL) { wands_pool = DEFAULT_PSMOVE_POOL; }

  OB_LOG_INFO ("Using gripes pool: '%s'", gripes_pool);
  OB_LOG_INFO ("Using wands pool: '%s'", wands_pool);
  OB_LOG_INFO ("Using Playstation Move pool: '%s'", psmove_pool);

  m_g_optical->SetGripesPoolName (gripes_pool);
  m_g_optical->SetWandsPoolName (wands_pool);

  m_g_psmove->SetGripesPoolName (psmove_pool);
  m_g_psmove->SetWandsPoolName (psmove_pool);

  AppendChild (m_g_optical);
  AppendChild (m_g_psmove);
}
