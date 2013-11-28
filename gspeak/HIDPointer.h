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

#ifndef HADRON_HID_POINTER_H
#define HADRON_HID_POINTER_H

#include <memory>
#include <unordered_map>

#include <libBasement/KneeObject.h>
#include <libAfferent/OneHandPointingRod.h>
#include <libImpetus/OEPointing.h>

#include "HIDSender.h"

using namespace oblong::impetus;

class HIDPointer : public KneeObject,
  public OEPointing::Evts
{
  PATELLA_SUBCLASS (HIDPointer, KneeObject);

 protected:
  std::unordered_map <std::string, std::shared_ptr <HIDSender> > m_senders;

  void UpdatePosition (HIDSender &s, const OEPointingEvent *e);
  HIDSender &LookupSender (const std::string s);

 public:

  HIDPointer ();
  ~HIDPointer ();

  ObRetort OEPointingAppear (OEPointingAppearEvent *e, Atmosphere *atm);
  ObRetort OEPointingVanish (OEPointingVanishEvent *e, Atmosphere *atm);
  ObRetort OEPointingMove (OEPointingMoveEvent *e, Atmosphere *atm);
  ObRetort OEPointingHarden (OEPointingHardenEvent *e, Atmosphere *atm);
  ObRetort OEPointingSoften (OEPointingSoftenEvent *e, Atmosphere *atm);
};

#endif /* HADRON_HID_POINTER_H */
