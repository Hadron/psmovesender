#include "PSMoveHIDMouseController.h"
#include "PSMoveHIDMouseManager.h"

#include "FGParser.h"

#include "quaternion.h"

void PSMoveHIDMouseController::NotifyChanged ()
{
  auto q = GetHand ();

  Point_3 from = m_wpos;
  Vector_3 to = rotate (q, Vector_3 (0, 0, -10000));
  
  K::Ray_3 rq (from, to);
  std::vector<CameraGroup::Intersection> i;

  m_cg.Intersections (rq, std::back_inserter (i));

  auto p = i.begin();
  if (p != i.end()) {
    m_sender.SetPosition (p->px, p->py);
  }

  bool set = psmove_get_buttons (move) & (~(PSMove_Button::Btn_MOVE));

  m_sender.SetFlags (set);
  m_sender.Process ();
}

PSMoveHIDMouseController::PSMoveHIDMouseController (CameraGroup &cg, PSMove *m, int id)
  : m_cg (cg), PSMoveController (m, id), m_sender ("PSMove pointer " + std::to_string (id))
{
  m_sender.Open ();
}
