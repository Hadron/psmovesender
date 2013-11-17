#include <libLoam/c++/Quat.h>
#include <libLoam/c++/Vect.h>
#include <libLoam/c++/Str.h>

#include <libPlasma/c++/Protein.h>

#include "PSMoveWandController.h"

#include "quaternion.h"

using namespace oblong::plasma;

static Vect ToPlasma (const PSMoveController::Vector_3 &v)
{
  return Vect (v.x(), v.y(), v.z());
}

static Vect ToPlasma (const PSMoveController::Point_3 &v)
{
  return Vect (v.x(), v.y(), v.z());
}

static Quat ToPlasma (const PSMoveController::Quaternion &q)
{
  return Quat (0, 0, 0, 0);
}

Slaw PSMoveWandController::ToSlaw ()
{
  Quat curhand = ToPlasma (GetHand ());

  Slaw m = Slaw::Map ();
  m = m.MapPut ("loc", ToPlasma (m_wpos));

  m = m.MapPut ("name", Str ().Format ("wand-%d", m_id));
  m = m.MapPut ("info-aim", curhand.QuatRotVect (Vect (0., 0., -1.)).Norm ());
  m = m.MapPut ("norm", curhand.QuatRotVect (Vect (0., 1., 0.)).Norm ());
  m = m.MapPut ("over", curhand.QuatRotVect (Vect (1., 0., 0.)).Norm ());

  switch (psmove_connection_type (move)) {
  case Conn_Bluetooth:
    m = m.MapPut ("connection-type", "bluetooth"); break;
  case Conn_USB:
    m = m.MapPut ("connection-type", "usb"); break;
  default:
    m = m.MapPut ("connection-type", "unknown"); break;
  }

  m = m.MapPut ("serial", Slaw (Str (psmove_get_serial (move))));

  m = m.MapPut ("temperature", Slaw ((float32) psmove_get_temperature_in_celsius (move)));

  int bflags = psmove_get_battery (move);
  int battery = 0;

  switch (bflags) {
  case Batt_MIN: battery = 0; break;
  case Batt_20Percent: battery = 20; break;
  case Batt_40Percent: battery = 40; break;
  case Batt_60Percent: battery = 60; break;
  case Batt_80Percent: battery = 80; break;
  case Batt_MAX: battery = 100; break;
  case Batt_CHARGING: battery = 0; break;
  case Batt_CHARGING_DONE: battery = 100; break;
  default:
    battery = -1; break;
  }

  Slaw blist = Slaw::List ();

  m = m.MapPut ("battery-level", battery);
  if (bflags == Batt_CHARGING) {
    blist = blist.ListAppend ("charging");
  } else if (bflags == Batt_CHARGING_DONE) {
    blist = blist.ListAppend ("charged");
  }
    
  m = m.MapPut ("battery-flags", blist);

  Slaw buttons = Slaw::List ();

  const char *psmove_button_names[] = { 
    "L2", "R2", "L1", "R1",
    "Triangle", "Circle", "Cross", "Square",
    "Select", "L3", "R3", "Start",
    "Up", "Right", "Down", "Left",
    "PS", "Unknown", "Unknown", "Move",
    "Trigger"
  };

  unsigned int bval = psmove_get_buttons (move);
  for (int i = 0; i < sizeof (psmove_button_names) / sizeof (char *); i++) {
    if (bval & (1 << i)) {
      buttons = buttons.ListAppend (psmove_button_names[i]);
    }
  }

  m = m.MapPut ("bitflag", Slaw (buttons.Count () > 0 ? 1 : 0));
  m = m.MapPut ("buttons", buttons);
  return m;
}
