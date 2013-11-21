/* Copyright (c) 2012-2013, Hadron Industries, Inc. */

#include "PSMoveController.h"

#include <opencv2/highgui/highgui_c.h>

#include <stdio.h>
#include <poll.h>

#include "quaternion.h"

typedef PSMoveController::Quaternion Quaternion;
typedef PSMoveController::Vector_3 Vector_3;
typedef PSMoveController::Point_3 Point_3;

static inline Quaternion psmove_get_orientation_q (PSMove *move, Quaternion &q)
{
  float w, x, y, z;
  psmove_get_orientation (move, &w, &x, &y, &z);

  q = normalize (Quaternion (w, x, y, z));
}

void PSMoveController::Bind (PSMoveManager *m)
{
  m_manager = m;
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

  m_zeropoint = Point_3 (0., 0., 0.);
  m_zeropointset = false;

  m_zeroq = Quaternion (1, 0, 0, 0);
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

  Vector_3 imgpt (xrel, yrel, m_imgd);
  Vector_3 unit = imgpt / sqrt (imgpt.squared_length ());
  
  m_wpos = m_cpos + rotate (m_corient, (unit * m_wdistance));
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

  NotifyChanged ();
}

void PSMoveController::RecomputeTransforms ()
{
}

Quaternion PSMoveController::GetHand ()
{
  /* m_zerovec is the worldspace aim vector of the hand when the wand
     was zeroized. */
  Vector_3 m_zerovec = m_zeropoint - m_zeropos;

  Quaternion m_hzero = from_to (Vector_3 (0, 0, -1), m_zerovec);
  Quaternion m_q = m_zeroq * inverse (m_hzero);
  Quaternion m_curhand = inverse (m_zeroq) * m_curq;

  return m_curhand;
}

void PSMoveController::RecomputeHand ()
{
}

void PSMoveController::LockPosition (Point_3 v)
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

void PSMoveController::SetCamera (Point_3 v, Quaternion q)
{
  m_cpos = v;
  m_corient = q;
  m_cset = true;
}

void PSMoveController::SetZeroPoint (Point_3 v)
{
  m_zeropoint = v;
  m_zeropointset = true;
}

void PSMoveController::NotifyChanged ()
{
}

