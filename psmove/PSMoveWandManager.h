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

#ifndef HADRON_PSMOVE_WAND_MANAGER
#define HADRON_PSMOVE_WAND_MANAGER

#include <libPlasma/c++/Pool.h>
#include <libPlasma/c++/Hose.h>

#include "PSMoveManager.h"
#include "PSMoveWandController.h"

class PSMoveWandManager : public PSMoveManager
{
 protected:
  oblong::plasma::Hose *m_hose;

 public:
  PSMoveWandManager ();

  virtual PSMoveController *CreateController (PSMove *m, int id)
  { return new PSMoveWandController (m, id); }

  oblong::plasma::Protein FrameProtein ();

  virtual void Heartbeat ();
};

#endif /* HADRON_PSMOVE_WAND_MANAGER */
