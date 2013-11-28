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

#ifndef HADRON_PSMOVE_CONTROLLER
#define HADRON_PSMOVE_CONTROLLER

#include <psmoveapi/psmove.h>
#include <psmoveapi/psmove_tracker.h>
#include <psmoveapi/psmove_fusion.h>

#include <CGAL/Simple_cartesian.h>

#include <boost/math/quaternion.hpp>

struct PSMoveManager;

class PSMoveController
{
 public:

  typedef CGAL::Simple_cartesian<double> K;
  typedef K::Vector_3 Vector_3;
  typedef K::Point_3 Point_3;
  typedef boost::math::quaternion<double> Quaternion;

 protected:
  
  PSMoveManager *m_manager;
  PSMove *move;
  PSMoveTracker *m_tracker;
  PSMoveFusion *m_fusion;
  int m_id;

  bool m_calibrated;

  /* All positions are in g-speak coordinates. */

  /* The position and orientation of the camera zero-point itself. */
  Point_3 m_cpos;
  Quaternion m_corient;
  bool m_cset;

  /* The width and height of the camera imaging plane, in pixels. */
  int m_imgw, m_imgh;
  /* The notional distance of the camera imaging plane, in pixels.
     This is computed from the FOV parameters. */
  float m_imgd;

  /* The point to which the wand should be pointing when reset. */
  Point_3 m_zeropoint;
  bool m_zeropointset;

  /* The known position of the wand in world-space when it was
     zeroized. */
  Point_3 m_zeropos;
  /* The known orientation of the wand in world-space when it was
     zeroized (as read from psmove_get_orientation_q). */
  Quaternion m_zeroq;
  /* True only if the wand has been zeroized at least once. */
  bool m_zeroqset;

  /* The current orientation of the wand (as reported by
     psmove_get_orientation_q). */
  Quaternion m_curq;

  /* The current world position of the ball of the PSMove, as calculated by the tracker. */
  Point_3 m_wpos;
  bool m_poslock;
  bool m_postrack;

  /* Cached data from the PSMove tracker, in pixel-space. */
  float m_wx, m_wy;
  float m_wradius;

  /* The computed distance from the PSMove tracker, in g-speak units. */
  float m_wdistance;

  unsigned long long m_nframes;

  void UpdateOrientation ();
  void UpdatePosition ();

  virtual void RecomputeTransforms ();
  virtual void RecomputeHand ();

 public:

  PSMoveController (PSMove *m, int id);

  virtual void Bind (PSMoveManager *m);

  virtual void SetCamera (Point_3 v, Quaternion q);
  virtual void SetZeroPoint (Point_3 v);

  virtual void LockPosition (Point_3 v);
  virtual void TrackPosition ();

  virtual ~PSMoveController ();

  virtual void Process ();

  virtual Quaternion GetHand ();
  
  virtual int FileHandle ();
  virtual void NotifyChanged ();
};

#endif /* HADRON_PSMOVE_CONTROLLER */
