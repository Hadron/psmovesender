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

#include "PSMoveManager.h"

#include <poll.h>

#include <unordered_map>
#include <vector>
#include <array>

#include <wchar.h>
#include <stdint.h>

// #include "psmove_private.h"

/* Vendor ID and Product ID of PS Move Controller */
#define PSMOVE_VID 0x054c
#define PSMOVE_PID 0x03d5

extern "C" PSMove *psmove_connect_internal(wchar_t *serial, char *path, int id);

const uint64_t INVALID_BTADDR = (uint64_t) -1;

inline unsigned char hex_to_int (const wchar_t c)
{
  if (c >= 'a' && c <= 'f') { return c - 'a' + 10; }
  if (c >= 'A' && c <= 'F') { return c - 'A' + 10; }
  if (c >= '0' && c <= '9') { return c - '0'; }
  return 0xff;
}

static inline uint64_t parse_btaddr (const wchar_t *s)
{
  uint64_t addr = 0;

  for (int i = 0; i < 18; i++) {
    
    if (i == 17) { if (s[i] != '\0') { return INVALID_BTADDR; } else continue; }
    if ((i % 3) == 2) { if (s[i] != ':') { return INVALID_BTADDR; } else continue; }
    
    unsigned char ret = hex_to_int (s[i]);
    if (ret == 0xff) { return INVALID_BTADDR; }

    addr <<= 4;
    addr |= ret;
  }

  return addr;
}

PSMoveManager::PSMoveManager ()
{
  psmove_set_remote_config (PSMove_OnlyLocal);
}

int PSMoveManager::AvailableID ()
{
  int max = m_wands.size ();

  std::vector <int> v (max + 1);
  int i = 0;
  v[i++] = 0;
  for (auto w : m_wands) {
    v[i++] = w.second.id;
  }
  
  std::sort (v.begin (), v.end ());

  for (i = 1; i < max; i++) {
    if ((v[i] > 0) && (v[i] > (v[i-1] + 1))) {
      return v[i-1] + 1;
    }
  }

  /* We already added one to the size earlier. */
  return v[max] + 1;
}

PSMoveManager::MoveRecord &PSMoveManager::FindOrCreateRecord (uint64_t addr)
{
  if (m_wands.count (addr) > 0) {
    return m_wands[addr];
  }

  m_wands.insert (std::make_pair (addr, MoveRecord ()));
  return m_wands[addr];
}

void PSMoveManager::SetupController (PSMoveController *c)
{
  c->SetCamera (m_cpos, m_corient);
  c->LockPosition (m_vpos);
  c->SetZeroPoint (m_cpos);
}

void PSMoveManager::ConnectAll ()
{
  struct hid_device_info *devs, *cur;
  devs = hid_enumerate (PSMOVE_VID, PSMOVE_PID);

  for (auto w : m_wands) {
    w.second.flag = false;
  }

  uint64_t addr;

  for (cur = devs; cur != NULL; cur = cur->next) {
    /* Only consider Bluetooth devices. */
    
    if ((cur->serial_number == NULL) || (wcslen (cur->serial_number) == 0)) {
      continue;
    }

    uint64_t addr = parse_btaddr (cur->serial_number);
    if (addr == INVALID_BTADDR) { continue; }

    MoveRecord &record = FindOrCreateRecord (addr);
    record.flag = true;

    if (record.controller != NULL) { continue; }

    record.id = AvailableID ();
    PSMove *move = psmove_connect_internal (cur->serial_number, cur->path, record.id);
    if (move == NULL) { abort (); }
    record.controller = CreateController (move, record.id);
    record.controller->Bind (this);
    record.flag = true;

    SetupController (record.controller);
  }

  hid_free_enumeration (devs);

  auto w = m_wands.begin ();
  while (w != m_wands.end ()) {
    auto cur = w++;
    if (cur->second.flag) { continue; }
    delete cur->second.controller;
    m_wands.erase (cur);
  }
}

void PSMoveManager::Process ()
{
  std::vector<struct pollfd> fdvec;

  for (auto w : m_wands) {
    struct pollfd p;
    p.fd = w.second.controller->FileHandle();
    p.events = POLLIN;
    p.revents = 0;
    fdvec.push_back (p);
  }

  auto fdptr = &(*fdvec.begin());

  int ret = poll (fdptr, fdvec.size(), 10);
  
  if (ret < 0) {
    throw (ret);
  }

  if (ret != 0) {
    for (auto w : m_wands) {
      w.second.controller->Process ();
    }
  }

  Heartbeat ();
}

void PSMoveManager::Loop ()
{
  for (;;) {
    Process ();
  }
}

void PSMoveManager::Heartbeat ()
{
}
