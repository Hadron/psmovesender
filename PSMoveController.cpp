#include <psmoveapi/psmove.h>
#include <psmoveapi/psmove_tracker.h>
#include <psmoveapi/psmove_fusion.h>

#include <opencv2/highgui/highgui_c.h>

#include <stdio.h>
#include <poll.h>

#include <libPlasma/c++/Slaw.h>
#include <libPlasma/c++/Protein.h>

#include "PSMoveController.h"

using namespace oblong::plasma;

static inline Quat psmove_get_orientation_q (PSMove *move, Quat &q)
{
    float w, x, y, z;
    psmove_get_orientation (move, &w, &x, &y, &z);
    q.a = w; q.i = x; q.j = y; q.k = z;
    q.NormSelf ();
}

PSMoveController::PSMoveController (PSMove *m, int id)
{
  move = m;
  m_tracker = NULL;
  m_fusion = NULL;
  m_id = id;

  m_calibrated = false;

  m_cset = false;

  m_imgw = -1;
  m_imgh = -1;
  m_imgd = 0.;

  m_zeropoint = Vect (0., 0., 0.);
  m_zeropointset = false;

  m_zeroq = Quat::QRotFromNormOver (Vect (0., 0., -1.), Vect (1., 0., 0.));
  m_zeroqset = false;
  
  m_poslock = false;
  m_postrack = false;

  m_wx = 0.;
  m_wy = 0.;
  m_wradius = 0.;
  m_wdistance = 0.;

  m_nframes = 0;

  if (move == NULL) {
    abort ();
  }

  psmove_enable_orientation (move, PSMove_True);
  psmove_reset_orientation (move);
}

PSMoveController::~PSMoveController ()
{
  if (m_fusion != NULL) {
    psmove_fusion_free (m_fusion);
    m_fusion = NULL;
  }
  if (m_tracker != NULL) {
    psmove_tracker_free (m_tracker);
    m_tracker = NULL;
  }
  psmove_disconnect (move);
  move = NULL;
}

void PSMoveController::UpdatePosition ()
{
  psmove_tracker_update_image (m_tracker);
  psmove_tracker_update (m_tracker, NULL);

  if (! psmove_tracker_get_status (m_tracker, move) == Tracker_TRACKING) {
    return;
  }

  psmove_tracker_get_position (m_tracker, move, &m_wx, &m_wy, &m_wradius);

  m_wdistance = psmove_tracker_distance_from_radius (m_tracker, m_wradius) * 10.0;
  
  /* x = over; y = up, as facing the camera */
  float xrel = (m_imgw / 2) - m_wx;
  float yrel = (m_imgh / 2) - m_wy;

  Vect imgpt (xrel, yrel, m_imgd);
  Vect unit = imgpt.Norm ();
  
  m_wpos = m_cpos + m_corient.QuatRotVect (unit * m_wdistance);
}

int PSMoveController::FileHandle ()
{
  /* If you're gonna go ugly, may as well go big. */
  int fd = *((int *) *(((void **) (move)) + 1));
  return fd;
}

void PSMoveController::Process ()
{
  if (!m_postrack && !m_poslock) {
    abort ();
  }
  if (!m_cset) {
    abort ();
  }

  if (m_postrack && (! m_calibrated)) {

    m_tracker = psmove_tracker_new ();
    m_fusion = psmove_fusion_new (m_tracker, .01, 1000);

    PSMoveTracker_Status ret = psmove_tracker_enable (m_tracker, move);
    if (ret != Tracker_CALIBRATED) {
      abort ();
    }
    m_calibrated = true;

    psmove_tracker_get_size (m_tracker, &m_imgw, &m_imgh);

    /* The half-angle to the horizontal field of view limit. */
    float xfov = PSEYE_FOV_BLUE_DOT * M_PI / 360.;
    /* tan[xfov] = opposite (m_imgw / 2.) / adjacent (m_imgd) */
    m_imgd = (m_imgw / 2.) / tan (xfov);
  }

  auto lastframe = m_nframes;
  while (psmove_poll (move)) {
    m_nframes++;
    if (psmove_get_buttons (move) & Btn_MOVE) {
      m_zeropos = m_wpos;
      psmove_get_orientation_q (move, m_zeroq);
      RecomputeTransforms ();
    }
  }
  if (m_nframes == lastframe) { return; }

  psmove_get_orientation_q (move, m_curq);
  RecomputeHand ();

  if (m_postrack) {
    UpdatePosition ();
  }
}

void PSMoveController::RecomputeTransforms ()
{
}

void PSMoveController::RecomputeHand ()
{
}

static inline Quat QuatFromVectors (Vect from, Vect to)
{
  Vect axis = to.Cross (from);
  float64 angle = from.AngleWith (to, axis);
  return Quat::QRotFromAxisAngle (axis, angle);
}

Slaw PSMoveController::ToSlaw ()
{
  /* m_zerovec is the worldspace aim vector of the hand when the wand
     was zeroized. */
  Vect m_zerovec = m_zeropoint - m_zeropos;

  Quat m_hzero = QuatFromVectors (Vect (0, 0, -1), m_zerovec);
  Quat m_q = m_hzero.Invert () * m_zeroq;
  Quat m_curhand = m_curq * m_q.Invert (); 

  Slaw m = Slaw::Map ();
  m = m.MapPut ("loc", m_wpos);

  m = m.MapPut ("name", Str ().Format ("wand-%d", m_id));
  m = m.MapPut ("wand-q", m_curq);
  m = m.MapPut ("info-aim", m_curhand.QuatRotVect (Vect (0., 0., -1.)).Norm ());
  m = m.MapPut ("norm", m_curhand.QuatRotVect (Vect (0., 1., 0.)).Norm ());
  m = m.MapPut ("over", m_curhand.QuatRotVect (Vect (1., 0., 0.)).Norm ());

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

void PSMoveController::LockPosition (Vect v)
{
  m_wpos = v;
  m_poslock = true;
  m_postrack = false;
}

void PSMoveController::TrackPosition ()
{
  m_poslock = false;
  m_postrack = true;
}

void PSMoveController::SetCamera (Vect v, Quat q)
{
  m_cpos = v;
  m_corient = q;
  m_cset = true;
}

void PSMoveController::SetZeroPoint (Vect v)
{
  m_zeropoint = v;
  m_zeropointset = true;
}
