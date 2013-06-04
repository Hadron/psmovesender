#include <unordered_map>
#include <vector>
#include <array>

#include <wchar.h>
#include <stdint.h>

#include "PSMoveManager.h"

#include "psmove_private.h"

using namespace oblong::plasma;

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
  oblong::loam::ObRetort ret;
  m_hose = Pool::Participate ("wands", Pool::MMAP_MEDIUM, &ret);
  if (ret.IsError ()) {
    fprintf (stderr, "Could not connect to controller.\n");
    abort ();
  }    

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

  m_wands.emplace (std::make_pair (addr, MoveRecord ()));
  return m_wands[addr];
}

void PSMoveManager::SetupController (PSMoveController *c)
{
  Vect v (0., 1000., -700.);
  Quat q = Quat::QRotFromNormOver (Vect (0., 0., 1.), Vect (1., 0., 0.));
  c->SetCamera (v, q);
  c->LockPosition (Vect (0., 1000., -400.));
  c->SetZeroPoint (v);
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
    record.controller = new PSMoveController (move, record.id);
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

Protein PSMoveManager::FrameProtein ()
{
  Slaw reports = Slaw::List ();
  
  for (auto w : m_wands) {
    reports = reports.ListAppend (w.second.controller->ToSlaw ());
  }

  struct timespec ts;
  clock_gettime (CLOCK_REALTIME, &ts);

  Slaw descrips = Slaw::List ("wandframe");
  Slaw ingests = Slaw::Map ();
  ingests = ingests.MapPut ("time", Slaw ((unt64) ts.tv_sec * 1000000 + ts.tv_nsec / 1000));
  ingests = ingests.MapPut ("wand-report", reports);
  
  return Protein (descrips, ingests);
}

void PSMoveManager::Loop ()
{
#if 0
  {
    struct pollfd fds;

    fds.fd = fd;
    fds.events = POLLIN;
    fds.revents = 0;
    int ret = poll (&fds, 1, 1000000);
    if (ret == -1 || ret == 0)
      return;
    if (fds.revents & (POLLERR | POLLHUP | POLLNVAL))
      return;
  }
#endif

  for (;;) {
    for (auto w : m_wands) {
      w.second.controller->Process ();
    }
    Protein p = FrameProtein ();
    m_hose->Deposit (p);
  }
}
