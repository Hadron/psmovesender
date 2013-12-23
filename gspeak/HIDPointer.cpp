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

#include <libNoodoo/PhysicalDisplay.h>
#include <libNoodoo/WindowedVisiFeld.h>

#include "HIDPointer.h"

using namespace oblong::noodoo;

HIDPointer::HIDPointer ()
{
}

HIDPointer::~HIDPointer ()
{
}

HIDSender &HIDPointer::LookupSender (const std::string s)
{
  return *m_senders[s];
}

void HIDPointer::UpdatePosition (HIDSender &s, const OEPointingEvent *e)
{
  Vect hit;
  SpaceFeld *feld = SpaceFeld::ClosestWhackedFeld (e->PhysOrigin(), e->PhysThrough(), &hit);
  if (! feld) { return; }

  WindowedVisiFeld *vf = dynamic_cast<WindowedVisiFeld *> (feld);
  if (! vf) { abort (); }

  Vect bl = vf->LocOf (-0.5, -0.5);
  
  float64 xf = (hit - bl).Dot (vf->Over ()) / vf->Width ();
  float64 yf = (hit - bl).Dot (vf->Up ()) / vf->Height ();

  unsigned int x = (int) round (xf * vf->PixelWidth () + vf->PixelOriginLeft ());
  unsigned int y = (int) round ((1.0 - yf) * vf->PixelHeight () + vf->PixelOriginTop ());

  s.SetPosition (x, y);
}

ObRetort HIDPointer::OEPointingAppear (OEPointingAppearEvent *e, Atmosphere *atm)
{
  OB_LOG_INFO ("OEPointingAppear: %s", e->Provenance ().utf8 ());
  std::string provenance (e->Provenance ().utf8 ());
  if (m_senders.count (provenance) != 0) { abort (); }
  
  std::shared_ptr <HIDSender> s (new HIDSender ("g-speak pointer " + provenance));

  int ret = s->Open ();
  if (ret != 0) { return ob_errno_to_retort (-ret); }

  ret = s->Process ();
  if (ret != 0) { return ob_errno_to_retort (-ret); }

  m_senders.insert (std::make_pair (provenance, s));
  return OB_OK;
}

ObRetort HIDPointer::OEPointingVanish (OEPointingVanishEvent *e, Atmosphere *atm)
{
  OB_LOG_INFO ("OEPointingVanish: %s", e->Provenance ().utf8 ());
  std::string provenance (e->Provenance ().utf8 ());
  if (m_senders.count (provenance) != 1) { abort (); }

  m_senders.erase (provenance);
  return OB_OK;
}

ObRetort HIDPointer::OEPointingMove (OEPointingMoveEvent *e, Atmosphere *atm)
{
  HIDSender &s = LookupSender (e->Provenance ().utf8 ());
  UpdatePosition (s, e);
  s.Process ();
  return OB_OK;
}

ObRetort HIDPointer::OEPointingHarden (OEPointingHardenEvent *e, Atmosphere *atm)
{
  OB_LOG_INFO ("OEPointingHarden: %s", e->Provenance ().utf8 ());
  HIDSender &s = LookupSender (e->Provenance ().utf8 ());

  for (int i = 0; i < e->PressureReportCount (); i++) {
    OEPointingEvent::PressureReport *p = e->NthPressureReport (i);
  }

  s.SetFlags (0x1);
  s.Process ();
  return OB_OK;
}

ObRetort HIDPointer::OEPointingSoften (OEPointingSoftenEvent *e, Atmosphere *atm)
{
  OB_LOG_INFO ("OEPointingSoften: %s", e->Provenance ().utf8 ());
  HIDSender &s = LookupSender (e->Provenance ().utf8 ());
  s.SetFlags (0x0);
  s.Process ();
  return OB_OK;
}

