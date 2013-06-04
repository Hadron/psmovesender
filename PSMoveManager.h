#include "PSMoveController.h"

#include <unordered_map>
#include <stdint.h>

#include "hidapi.h"

#include <libPlasma/c++/Pool.h>
#include <libPlasma/c++/Hose.h>

class PSMoveController;

class PSMoveManager
{
 protected:

  oblong::plasma::Hose *m_hose;

  struct MoveRecord 
  {
    PSMoveController *controller;
    bool flag;
    int id;
    MoveRecord () : controller (NULL), flag (false), id (0) { }
  };
  
  std::unordered_map <uint64_t, MoveRecord> m_wands;

  int AvailableID ();
  void SetupController (PSMoveController *c);
  MoveRecord &FindOrCreateRecord (uint64_t);
  oblong::plasma::Protein FrameProtein ();

public:
  PSMoveManager ();
  void ConnectAll ();
  void Loop ();
};
