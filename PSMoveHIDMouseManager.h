/* Copyright (c) 2012-2013, Hadron Industries, Inc. */

#ifndef HADRON_PSMOVE_HID_MOUSE_MANAGER
#define HADRON_PSMOVE_HID_MOUSE_MANAGER

#include "PSMoveManager.h"
#include "PSMoveHIDMouseController.h"
#include "FGParser.h"

class PSMoveHIDMouseManager : public PSMoveManager
{
 protected:
  CameraGroup &m_cg;

 public:
  PSMoveHIDMouseManager (CameraGroup &cg) : m_cg (cg), PSMoveManager () { }

  virtual PSMoveController *CreateController (PSMove *m, int id)
  { return new PSMoveHIDMouseController (m_cg, m, id); }
};

#endif /* HADRON_PSMOVE_HID_MOUSE_MANAGER */
