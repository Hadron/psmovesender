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

#include "PSMoveWandManager.h"
#include "PSMoveWandController.h"

#include "quaternion.h"

#include <vector>

using oblong::plasma::Slaw;
using oblong::plasma::Pool;
using oblong::plasma::Protein;

typedef PSMoveController::Vector_3 Vector_3;
typedef PSMoveController::Point_3 Point_3;
typedef PSMoveController::Quaternion Quaternion;

PSMoveWandManager::PSMoveWandManager ()
{
  oblong::loam::ObRetort ret;

  OB_LOG_INFO ("writing to wands pool: '%s'", "wands");
  m_hose = Pool::Participate ("wands", Pool::MMAP_MEDIUM, &ret);
  if (ret.IsError ()) {
    fprintf (stderr, "Could not connect to controller.\n");
    abort ();
  }    

  Slaw s = Slaw::FromFile ("/etc/oblong/screen.protein");
  if (s.IsNull ()) {
    fprintf (stderr, "Could not parse screen config.\n");
    abort ();
  }
  Protein p (s);

  v3float64 cent, norm, over;
  float eye_dist;

  Slaw main = p.Ingests ().MapFind ("screens").MapFind ("main");
  bool bret = true;

  bret = bret && (main.MapFind ("cent").Into (cent));
  bret = bret && (main.MapFind ("norm").Into (norm));
  bret = bret && (main.MapFind ("over").Into (over));
  bret = bret && (main.MapFind ("eye-dist").Into (eye_dist));
  if (! bret) { OB_FATAL_BUG ("unable to parse screen configuration"); abort (); }
    
  m_cpos = Point_3 (cent.x, cent.y, cent.z);

  Vector_3 nnorm (norm.x, norm.y, norm.z);
  Vector_3 nover (over.x, over.y, over.z);

  m_corient = create (nnorm, nover);
  m_vpos = m_cpos + (eye_dist * nnorm);
}

Protein PSMoveWandManager::FrameProtein ()
{
  Slaw reports = Slaw::List ();
  
  for (auto w : m_wands) {
    PSMoveWandController *c = dynamic_cast<PSMoveWandController *> (w.second.controller);
    reports = reports.ListAppend (c->ToSlaw ());
  }

  struct timespec ts;
  clock_gettime (CLOCK_REALTIME, &ts);

  Slaw descrips = Slaw::List ("wandframe");
  Slaw ingests = Slaw::Map ();
  ingests = ingests.MapPut ("time", Slaw ((unt64) ts.tv_sec * 1000000 + ts.tv_nsec / 1000));
  ingests = ingests.MapPut ("wand-report", reports);
  
  return Protein (descrips, ingests);
}

void PSMoveWandManager::Heartbeat ()
{
  Protein p = FrameProtein ();
  m_hose->Deposit (p);
}
