#include <ostream>

#include <CGAL/aff_transformation_tags.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "FGParser.h"

Window::Window (boost::property_tree::ptree pt_) : pt (pt_)
{
}

std::string Window::name () const
{
  return pt.get<std::string>("name");
}

Camera::Camera (CameraGroup &cg_, boost::property_tree::ptree pt_) : cg (cg_), pt (pt_)
{
  auto pd = pt.get_child ("physical-dimensions");

  physical_dimensions = parse_physical_dimensions ();
  viewport = parse_viewport ();
  offset = calculate_offset ();

  port = Port::dimensioned_centered (pd.get<double>("width"), pd.get<double>("height"));
  Port::Matrix m (CGAL::TRANSLATION, K::Vector_3 (offset));
  port.m = m * port.m;
}

std::string Camera::name () const
{
  auto n2 = pt.get_child ("physical-dimensions");
  auto n = pt.get_child ("name");
  return n.get_value<std::string>();
}

Port Port::dimensioned (double w, double h)
{
  Port p;
  p.m = Matrix (w, 0, 0, 0, h, 0, 0, 0, 1, 1);
  return p;
}

Port Port::dimensioned_centered (double w, double h)
{
  Port p;
  Matrix x (w, 0, 0, 0, h, 0, 0, 0, 1, 1);
  Matrix tr (CGAL::TRANSLATION, K::Vector_3 (-w/2., -h/2., 0));
  p.m = tr * x;
  return p;
}

std::pair<double, double> Port::project (K::Point_3 p) const
{
  K::Point_3 origin = point (0, 0);
  K::Vector_3 offset = p - origin;
  
  K::Vector_3 vover = over ();
  K::Vector_3 vup = up ();

  double dover = (offset * vover) / vover.squared_length ();
  double dup = (offset * vup) / vup.squared_length ();

  return std::pair<double, double> (dover, dup);
}

K::Point_3 Port::point (double over, double up) const
{
  return m.transform (K::Point_3 (over, up, 0.0));
}

K::Vector_3 Port::over () const 
{
  return m.transform (K::Vector_3 (1, 0, 0));
}

K::Vector_3 Port::up () const 
{
  return m.transform (K::Vector_3 (0, 1, 0));
}

K::Vector_3 Port::norm () const 
{
  return m.transform (K::Vector_3 (0, 0, 1));
}

Camera::Offset Camera::calculate_offset () const
{
  auto pd = pt.get_child_optional ("master-perspective");
  if (pd) {    
    return Offset (pd->get<double>("x-offset"), pd->get<double>("y-offset"), -pd->get<double>("eye-distance"));
  }

  auto pleft = pt.get_child_optional ("left-of-perspective");
  auto pright = pt.get_child_optional ("right-of-perspective");
  
  pd = pleft;
  if (! pd) { pd = pright; }

  if (pd) {

    auto name = pd->get<std::string>("parent-camera");
    auto i = cg.cameras.find (name);
    if (i == cg.cameras.end()) { throw std::out_of_range (name); }
    const Camera &parent = i->second;
    
    Offset o = parent.offset;
    
    PhysicalDimensions them = parent.physical_dimensions;
    PhysicalDimensions us = physical_dimensions;

    if (pright) {
      return Offset (o.x() +
		     (them.width / 2.) + them.right + 
		     (us.width / 2.) + us.left,
		     o.y(), o.z());
    }
    
    if (pleft) {
      return Offset (o.x() -
		     (them.width / 2.) - them.right - 
		     (us.width / 2.) - us.left,
		     o.y(), o.z());
    }
  }    

  return Offset (0, 0, 0);
}

Camera::PhysicalDimensions Camera::parse_physical_dimensions () const
{
  PhysicalDimensions p;
  p.width = pt.get<double>("physical-dimensions.width");
  p.height = pt.get<double>("physical-dimensions.height");
  p.right = pt.get<double>("physical-dimensions.bezel.right");
  p.left = pt.get<double>("physical-dimensions.bezel.left");
  p.top = pt.get<double>("physical-dimensions.bezel.top");
  p.bottom = pt.get<double>("physical-dimensions.bezel.bottom");
  return p;
}

Camera::Viewport Camera::parse_viewport () const
{
  Viewport p;
  p.width = pt.get<double>("viewport.width");
  p.height = pt.get<double>("viewport.height");
  p.x = pt.get<double>("viewport.x");
  p.y = pt.get<double>("viewport.y");
  return p;
}

CameraGroup::CameraGroup ()
{
}

void CameraGroup::parse (std::string fn)
{
  read_xml (fn, pt);

  auto p = pt.get_child ("PropertyList.sim.rendering.camera-group");

  auto wp = p.equal_range ("window");
  for (auto wi = wp.first; wi != wp.second; wi++) {
    Window w (wi->second);
    windows.insert (std::pair <std::string, Window>(w.name(), w));
  }

  auto cp = p.equal_range ("camera");
  for (auto ci = cp.first; ci != cp.second; ci++) {
    Camera c (*this, ci->second);
    cameras.insert (std::pair <std::string, Camera>(c.name(), c));
  }

  for (auto i = cameras.begin(); i != cameras.end(); i++) {
    Camera &c = i->second;
    c.push_facets (triangles);
  }

  m_tree = new Tree (triangles.begin(),triangles.end());
}

CameraGroup parse (std::string fn)
{
  CameraGroup g;
  g.parse (fn);
  return g;
}

Screen::Screen (const std::string name_, Point ul_, Point ur_, Point bl_, Point br_):
  name (name_), ul (ul_), ur (ur_), bl (bl_), br (br_)
{
}

void Camera::push_facets (std::vector<ScreenTriangle> &l)
{
  auto bl = port.point (0, 0);
  auto br = port.point (1, 0);
  auto ul = port.point (0, 1);
  auto ur = port.point (1, 1);
  l.push_back (ScreenTriangle (this, bl, ul, ur));
  l.push_back (ScreenTriangle (this, bl, ur, br));
}

std::ostream &operator << (std::ostream &out, const CameraGroup::Intersection &i)
{
  out << i.camera->name() << ": [" <<  i.location << "] (" << i.px << ", " << i.py << ")";
  return out;
}

std::ostream &operator << (std::ostream &out, const Camera &c)
{
  out << c.name () << ": "
      << "bl=" << c.port.point (0.0, 0.0)
      << ", ul=" << c.port.point (0.0, 1.0)
      << ", ur=" << c.port.point (1.0, 1.0)
      << ", br=" << c.port.point (1.0, 0.0);
  return out;
}
