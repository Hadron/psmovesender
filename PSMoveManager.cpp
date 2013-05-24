#include "PSMoveManager.h"

PSMoveManager::PSMoveManager ()
{
  m_move = NULL;
  psmove_set_remote_config (PSMove_OnlyLocal);
}

void PSMoveManager::ConnectAll ()
{
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

    m_move = new PSMoveController (move);

    Vect v (0., 1000., -700.);
    Quat q = Quat::QRotFromNormOver (Vect (0., 0., 1.), Vect (1., 0., 0.));
    m_move->SetCamera (v, q);

    m_move->LockPosition (Vect (0., 1000., -400.));

    m_move->SetZeroPoint (v);
  }
}

void PSMoveManager::Loop ()
{
  for (;;) {
    m_move->Process ();
  }
}

