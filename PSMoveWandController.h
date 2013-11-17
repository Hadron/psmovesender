#ifndef HADRON_PSMOVE_WAND_CONTROLLER
#define HADRON_PSMOVE_WAND_CONTROLLER

#include <libPlasma/c++/Slaw.h>

#include "PSMoveController.h"

class PSMoveWandController : public PSMoveController
{
 public:
  PSMoveWandController (PSMove *m, int id) : PSMoveController (m, id) { }
  virtual oblong::plasma::Slaw ToSlaw ();
};

#endif /* HADRON_PSMOVE_WAND_CONTROLLER */
