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
