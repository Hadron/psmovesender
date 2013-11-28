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
