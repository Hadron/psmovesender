#ifndef HADRON_PSMOVE_CONTROLLER
#define HADRON_PSMOVE_CONTROLLER

#include <psmoveapi/psmove.h>
#include <psmoveapi/psmove_tracker.h>
#include <psmoveapi/psmove_fusion.h>

#include <opencv2/highgui/highgui_c.h>

#include <libLoam/c++/Quat.h>
#include <libLoam/c++/Vect.h>
#include <libLoam/c++/Str.h>

#include <libPlasma/c++/Pool.h>
#include <libPlasma/c++/Hose.h>

using namespace oblong::loam;

class PSMoveController
{
 protected:
  
  PSMove *move;
  PSMoveTracker *m_tracker;
  PSMoveFusion *m_fusion;
  int m_id;

  bool m_calibrated;

  /* All positions are in g-speak coordinates. */

  /* The position and orientation of the camera zero-point itself. */
  Vect m_cpos;
  Quat m_corient;
  bool m_cset;

  /* The width and height of the camera imaging plane, in pixels. */
  int m_imgw, m_imgh;
  /* The notional distance of the camera imaging plan, in pixels.
     This is computed from the FOV parameters. */
  float m_imgd;

  /* The point to which the wand should be pointing when reset. */
  Vect m_zeropoint;
  bool m_zeropointset;

  /* The known orientation of the wand in world-space when it was
     zeroized. */
  Quat m_zeroq;
  bool m_zeroqset;

  /* The current orientation of the wand as reported by the PSMove,
     relative to the last zeroization. */
  Quat m_wq;
  
  /* The current world position of the ball of the PSMove, as calculated by the tracker. */
  Vect m_wpos;
  bool m_poslock;
  bool m_postrack;

  /* Cached data from the PSMove tracker, in pixel-space. */
  float m_wx, m_wy;
  float m_wradius;

  /* The computed distance from the PSMove tracker, in g-speak units. */
  float m_wdistance;

  unsigned long long m_nframes;
  unsigned long long m_lastframe;

  void UpdateOrientation ();
  void UpdatePosition ();

 public:

  PSMoveController (PSMove *m, int id);

  virtual void SetCamera (Vect v, Quat q);
  virtual void SetZeroPoint (Vect v);

  virtual void LockPosition (Vect v);
  virtual void TrackPosition ();

  virtual ~PSMoveController ();

  virtual void Process ();

  virtual int FileHandle ();

  virtual oblong::plasma::Slaw ToSlaw ();
};

#endif /* HADRON_PSMOVE_CONTROLLER */
