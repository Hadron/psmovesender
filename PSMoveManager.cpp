#include <wchar.h>

#include "PSMoveManager.h"

#include "psmove_private.h"
#include "hidapi.h"

struct MoveRecord 
{
  PSMove *move;
  bool flag;
  int id;
};

PSMoveManager::PSMoveManager ()
{
  m_move = NULL;
  psmove_set_remote_config (PSMove_OnlyLocal);
}

void PSMoveManager::ConnectAll ()
{
  struct hid_device_info *devs, *cur;
  devs = hid_enumerate (PSMOVE_VID, PSMOVE_PID);

  for (cur = devs; cur != NULL; cur = cur->next) {
    /* Only consider Bluetooth devices. */
    if ((cur->serial_number == NULL) || (wcslen (cur->serial_number) == 0)) {
      continue;
    }
    printf ("%ls %s\n", cur->serial_number, cur->path);
  }

#if 0
  int num = psmove_count_connected ();
  for (int i = 0; i < num; i++) {

    PSMove *move = psmove_connect_by_id (i);
    if (move == NULL) {
      abort ();
    }

    if (psmove_connection_type (move) != Conn_Bluetooth) {
      psmove_disconnect (move);
      continue;
    }

    if (m_move != NULL) {
      abort ();
    }

    m_move = new PSMoveController (move, i);

    Vect v (0., 1000., -700.);
    Quat q = Quat::QRotFromNormOver (Vect (0., 0., 1.), Vect (1., 0., 0.));
    m_move->SetCamera (v, q);

    m_move->LockPosition (Vect (0., 1000., -400.));

    m_move->SetZeroPoint (v);
  }
#endif
}

void PSMoveManager::Loop ()
{
  for (;;) {
    m_move->Process ();
  }
}

