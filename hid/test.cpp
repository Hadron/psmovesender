#include <libevdev.h>
#include <libevdev-uinput.h>

int main ()
{
  int err;
  struct libevdev *dev;
  struct libevdev_uinput *uidev;
  dev = libevdev_new();
  libevdev_set_name(dev, "test device");
  libevdev_enable_event_type(dev, EV_REL);
  libevdev_enable_event_code(dev, EV_REL, REL_X);
  libevdev_enable_event_code(dev, EV_REL, REL_Y);
  libevdev_enable_event_type(dev, EV_KEY);
  libevdev_enable_event_code(dev, EV_KEY, BTN_LEFT);
  libevdev_enable_event_code(dev, EV_KEY, BTN_MIDDLE);
  libevdev_enable_event_code(dev, EV_KEY, BTN_RIGHT);
  err = libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);
  if (err != 0)
    return err;
  libevdev_uinput_destroy(uidev);
}
