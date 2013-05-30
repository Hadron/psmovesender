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
  m_lastframe = 0;

  oblong::loam::ObRetort ret;
  m_hose = Pool::Participate ("wands", Pool::MMAP_MEDIUM, &ret);
  if (ret.IsError ()) {
    fprintf (stderr, "Could not connect to controller.\n");
    abort ();
  }    

  if (move == NULL) {
    abort ();
  }

  psmove_enable_orientation (move, PSMove_True);
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

  {
    /* If you're gonna go ugly, may as well go big. */

    struct pollfd fds;
    int fd = *((int *) *(((void **) (move)) + 1));

    fds.fd = fd;
    fds.events = POLLIN;
    fds.revents = 0;
    int ret = poll (&fds, 1, 1000000);
    if (ret == -1 || ret == 0)
      return;
    if (fds.revents & (POLLERR | POLLHUP | POLLNVAL))
      return;
  }

  while (psmove_poll (move)) {

    m_nframes++;

    if (psmove_get_buttons (move) & Btn_MOVE) {

      psmove_reset_orientation (move);

      /* The negative one in the z-axis is because this whole section
	 of code actually broken, and I probably need to do some kind
	 of T Q T^(-1) shenanigans.  But it'll do until I take a more
	 thorough look. */

      /* 'from', in model space, is the vector we want to transform to
	 'to' in worldspace. */
      psmove_get_orientation_q (move, m_wq);
      Vect from = m_wq.QuatRotVect (Vect (0., 0., -1.)).Norm ();

      Vect to = m_cpos - m_wpos;

      Vect axis = to.Cross (from);
      float64 angle = from.AngleWith (to, axis);
      
      m_zeroq = Quat::QRotFromAxisAngle (axis, angle);
      m_zeroqset = true;
    }
  }

  if (m_nframes == m_lastframe) {
    return;
  }
  m_lastframe = m_nframes;

  Quat m_nq;
  psmove_get_orientation_q (move, m_nq);

  /* Just a very stupid low-pass filter.  The psmoveapi code should be
     doing this for us automatically, so I'm reluctant to do it
     here. */
  m_wq.UnitInterpSelf (0.35, m_nq);

  if (m_postrack) {
    UpdatePosition ();
  }

  // Quat wq = m_zeroq * m_wq;
  Quat wq = m_wq * m_zeroq ;
  
  Slaw m = Slaw::Map ();
  m = m.MapPut ("loc", m_wpos);

  m = m.MapPut ("name", Str ().Format ("wand-%d", m_id));
  m = m.MapPut ("info-aim", wq.QuatRotVect (Vect (0., 0., 1.)).Norm ());
  m = m.MapPut ("norm", wq.QuatRotVect (Vect (0., 1., 0.)).Norm ());
  m = m.MapPut ("over", wq.QuatRotVect (Vect (1., 0., 0.)).Norm ());

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
    abort ();
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

  struct timespec ts;
  clock_gettime (CLOCK_REALTIME, &ts);

  m = m.MapPut ("buttons", buttons);

  Slaw descrips = Slaw::List ("wandframe");
  Slaw ingests = Slaw::Map ();
  ingests = ingests.MapPut ("time", Slaw ((unt64) ts.tv_sec * 1000000 + ts.tv_nsec / 1000));
  ingests = ingests.MapPut ("wand-report", Slaw::List (m));
  
  Protein p = Protein (descrips, ingests);

  m_hose->Deposit (p);
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
