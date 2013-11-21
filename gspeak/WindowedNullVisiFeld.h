#ifndef HADRON_WINDOWED_NULL_VISIFELD
#define HADRON_WINDOWED_NULL_VISIFELD

#include <libNoodoo/WindowedVisiFeld.h>

class WindowedNullVisiFeld : public oblong::noodoo::WindowedVisiFeld
{ 
  PATELLA_SUBCLASS (WindowedNullVisiFeld, WindowedVisiFeld);

 public:
  WindowedNullVisiFeld ();
  virtual ~WindowedNullVisiFeld ();
};

#endif /* HADRON_WINDOWED_NULL_VISIFELD */
