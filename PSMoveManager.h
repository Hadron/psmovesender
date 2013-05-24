#include "PSMoveController.h"

class PSMoveManager
{
 protected:
  PSMoveController *m_move;

 public:
  PSMoveManager ();
  void ConnectAll ();
  void Loop ();
};
