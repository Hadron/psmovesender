#include "HIDSender.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <linux/uhid.h>

static unsigned char descriptor[] = {

  0x05, 0x01,         /*  Usage Page (Desktop),               */
  0x09, 0x02,         /*  Usage (Mouse),                      */
  0xA1, 0x01,         /*  Collection (Application),           */
  0x09, 0x01,         /*      Usage (Pointer),                */
  0xA0,               /*      Collection (Physical),          */
  0x05, 0x09,         /*          Usage Page (Button),        */
  0x19, 0x01,         /*          Usage Minimum (01h),        */
  0x29, 0x32,         /*          Usage Maximum (32h),        */
  0x14,               /*          Logical Minimum (0),        */
  0x25, 0x01,         /*          Logical Maximum (1),        */
  0x95, 0x20,         /*          Report Count (32),          */
  0x75, 0x01,         /*          Report Size (1),            */
  0x81, 0x02,         /*          Input (Variable),           */
  0x05, 0x01,         /*          Usage Page (Desktop),       */
  0x09, 0x30,         /*          Usage (X),                  */
  0x09, 0x31,         /*          Usage (Y),                  */
  0x14,               /*          Logical Minimum (0),        */
  0x26, 0x00, 0x20,   /*          Logical Maximum (8192),     */
  0x34,               /*          Physical Minimum (0),       */
  0x46, 0x00, 0x20,   /*          Physical Maximum (8192),    */
  0x75, 0x10,         /*          Report Size (16),           */
  0x95, 0x02,         /*          Report Count (2),           */
  0x81, 0x02,         /*          Input (Variable),           */
  0xC0,               /*      End Collection,                 */
  0xC0                /*  End Collection                      */
};

static const char *uhid_event_name (enum uhid_event_type t) 
{
  switch (t) {
  case UHID_CREATE: return "UHID_CREATE";
  case UHID_DESTROY: return "UHID_DESTROY";
  case UHID_START: return "UHID_START";
  case UHID_STOP: return "UHID_STOP";
  case UHID_OPEN: return "UHID_OPEN";
  case UHID_CLOSE: return "UHID_CLOSE";
  case UHID_OUTPUT: return "UHID_OUTPUT";
  case UHID_OUTPUT_EV: return "UHID_OUTPUT_EV";
  case UHID_INPUT: return "UHID_INPUT";
  case UHID_FEATURE: return "UHID_FEATURE";
  case UHID_FEATURE_ANSWER: return "UHID_FEATURE_ANSWER";
  default: return NULL;
  }
}

HIDSender::HIDSender (std::string name) : m_name (name)
{
  m_fd = -1;
  m_x = m_y = 0;
  m_dirty = true;
}

HIDSender::~HIDSender ()
{
  if (m_fd >= 0) { 
    Destroy ();
    close (m_fd); 
  }
  m_fd = -1;
}

int HIDSender::SendEvent (const struct uhid_event &ev)
{
  ssize_t ret;

  ret = write (m_fd, &ev, sizeof (ev));
  if (ret < 0) {
    fprintf (stderr, "unable to write to uhid: %s\n", strerror (errno));
    return -errno;
  } else if (ret != sizeof (ev)) {
    fprintf (stderr, "wrong size written to uhid: %ld != %lu\n", ret, sizeof (ev));
    return -EFAULT;
  } else {
    return 0;
  }
}

int HIDSender::Create ()
{
  struct uhid_event ev;

  memset (&ev, 0, sizeof (ev));
  ev.type = UHID_CREATE;
  strncpy ((char *) ev.u.create.name, m_name.c_str (), sizeof (ev.u.create.name));
  ev.u.create.name[sizeof (ev.u.create.name)] = '\0';
  ev.u.create.rd_data = descriptor;
  ev.u.create.rd_size = sizeof (descriptor);
  ev.u.create.bus = BUS_USB;
  ev.u.create.vendor = 0x0;
  ev.u.create.product = 0x0;
  ev.u.create.version = 0;
  ev.u.create.country = 0;

  return SendEvent (ev);
}

void HIDSender::Destroy ()
{
  struct uhid_event ev;

  memset (&ev, 0, sizeof (ev));
  ev.type = UHID_DESTROY;

  (void) SendEvent (ev);
}

int HIDSender::ProcessEvent ()
{
  struct uhid_event ev;
  ssize_t ret;

  memset (&ev, 0, sizeof (ev));
  ret = read (m_fd, &ev, sizeof (ev));
  if (ret == 0) {
    fprintf (stderr, "end-of-file from HID device\n");
    return -EFAULT;
  } else if (ret < 0) {
    fprintf (stderr, "unable to read from HID device: %s\n", strerror (errno));
    return -errno;
  } else if (ret != sizeof (ev)) {
    fprintf (stderr, "invalid size read from HID device: %ld != %lu\n",
	     ret, sizeof (ev));
    return -EFAULT;
  }

  /* fprintf (stderr, "%s\n", uhid_event_name ((uhid_event_type) ev.type)); */
  return 0;
}

int HIDSender::SendReport ()
{
  struct uhid_event ev;

  memset (&ev, 0, sizeof (ev));
  ev.type = UHID_INPUT;
  ev.u.input.size = 8;

  ev.u.input.data[0] = m_flags & 0xff;
  ev.u.input.data[1] = (m_flags >> 8) & 0xff;
  ev.u.input.data[2] = (m_flags >> 16) & 0xff;
  ev.u.input.data[3] = (m_flags >> 24) & 0xff;

  ev.u.input.data[4] = m_x & 0xff;
  ev.u.input.data[5] = (m_x >> 8) & 0xff;
  ev.u.input.data[6] = m_y & 0xff;
  ev.u.input.data[7] = (m_y >> 8) & 0xff;
  
  return SendEvent (ev);
}

int HIDSender::Open ()
{
  const char *path = "/dev/uhid";

  m_fd = open (path, O_RDWR | O_CLOEXEC);
  if (m_fd < 0) {
    fprintf (stderr, "unable to open '%s': %s\n", path, strerror (errno));
    return -errno;
  }

  return Create ();
}

int HIDSender::ProcessPending ()
{
  struct pollfd pfds[1];
  int ret;

  pfds[0].fd = m_fd;
  pfds[0].events = POLLIN;

  for (;;) {

    ret = poll (pfds, 1, 0);

    if (ret < 0) {
      fprintf (stderr, "unable to poll HID: %s\n", strerror (errno));
      return -errno;
    }

    if (pfds[0].revents & POLLHUP) {
      fprintf (stderr, "end of file from HID\n");
      return -EFAULT;
    }

    if (pfds[0].revents & POLLIN) {
      int nret = ProcessEvent ();
      if (nret != 0) { return nret; }
      continue;
    }

    return 0;
  }
}

int HIDSender::Process ()
{
  int ret = ProcessPending ();
  if (ret != 0) { return ret; } 

  if (! m_dirty) { return 0; }
  m_dirty = false;
  return SendReport ();
}
