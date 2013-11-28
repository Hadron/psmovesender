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

#ifndef HADRON_PSMOVE_MANAGER
#define HADRON_PSMOVE_MANAGER

#include "PSMoveController.h"

#include <unordered_map>
#include <stdint.h>

#include "hidapi.h"

class PSMoveController;

class PSMoveManager
{
 protected:

  PSMoveController::Point_3 m_cpos;
  PSMoveController::Point_3 m_vpos;
  PSMoveController::Quaternion m_corient;

  typedef size_t BTAddr;

  struct MoveRecord 
  {
    PSMoveController *controller;
    bool flag;
    int id;
    MoveRecord () : controller (NULL), flag (false), id (0) { }
  };
  
  std::unordered_map <uint64_t, MoveRecord> m_wands;

  int AvailableID ();
  virtual PSMoveController *CreateController (PSMove *m, int id) = 0;
  void SetupController (PSMoveController *c);
  MoveRecord &FindOrCreateRecord (uint64_t);

public:
  PSMoveManager ();
  void ConnectAll ();

  virtual void Process ();
  virtual void Loop ();

  virtual void Heartbeat ();
};

#endif /* HADRON_PSMOVE_MANAGER */
