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
