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
  PSMoveHIDMouseController (CameraGroup &cg, PSMove *m, int id) : m_cg (cg), PSMoveController (m, id), m_sender ("hello") { }

  virtual void NotifyChanged ();
};

#endif /* HADRON_PSMOVE_HID_MOUSE_CONTROLLER */
