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

#ifndef HADRON_PSMOVE_HID_MOUSE_CONTROLLER
#define HADRON_PSMOVE_HID_MOUSE_CONTROLLER

#include "PSMoveController.h"
#include "HIDSender.h"
#include "FGParser.h"

class PSMoveHIDMouseController : public PSMoveController
{
 private:
  CameraGroup &m_cg;
  HIDSender m_sender;

 public:
  PSMoveHIDMouseController (CameraGroup &cg, PSMove *m, int id);

  virtual void NotifyChanged ();
};

#endif /* HADRON_PSMOVE_HID_MOUSE_CONTROLLER */
