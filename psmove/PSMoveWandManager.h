/* Copyright (c) 2012-2013, Hadron Industries, Inc. */

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
