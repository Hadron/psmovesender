#include <string>

class HIDSender
{
 protected:

  int m_fd;
  std::string m_name;

  unsigned int m_x, m_y;
  unsigned long m_flags;

  bool m_dirty;

  int SendEvent (const struct uhid_event &ev);
  int Create ();
  void Destroy ();
  int ProcessEvent ();
  int ProcessPending ();

 public:

  HIDSender (std::string name);
  ~HIDSender ();

  int Open ();
  int SendReport ();

  void SetPosition (int x, int y);
  void SetFlags (unsigned long flags);
  void SetFlag (unsigned int flag);
  void ClearFlag (unsigned int flag);

  int Process ();
};

inline void HIDSender::SetPosition (int x, int y)
{ m_dirty = m_dirty || (m_x != x) || (m_y != y); m_x = x; m_y = y; }

inline void HIDSender::SetFlags (unsigned long flags)
{ m_dirty = m_dirty || (m_flags != flags); m_flags = flags; }

inline void HIDSender::SetFlag (unsigned int flag)
{ m_dirty = m_dirty || (! (m_flags & (1 << flag))); m_flags |= (1 << flag); }

inline void HIDSender::ClearFlag (unsigned int flag)
{ m_dirty = m_dirty || (!! (m_flags & (1 << flag))); m_flags &= ~(1 << flag); }
