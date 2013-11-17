/* Copyright (c) 2012-2013, Hadron Industries, Inc. */

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
  virtual PSMoveController *CreateController (PSMove *m, int id)
  { return new PSMoveController (m, id); }
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
