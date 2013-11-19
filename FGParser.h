#ifndef HADRON_PSMOVE_CAMERA_GROUP
#define HADRON_PSMOVE_CAMERA_GROUP

#include <ostream>

#include <boost/math/quaternion.hpp>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <CGAL/Aff_transformation_3.h>
#include <CGAL/Simple_cartesian.h>

#include <map>

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Point;

class Screen;
class CameraGroup;
class Camera;

class ScreenTriangle : public K::Triangle_3
{
public:
  ScreenTriangle (const Camera *camera_, Point p1, Point p2, Point p3)
    : K::Triangle_3 (p1, p2, p3), camera (camera_) { }
  const Camera *camera;
};

class Screen
{
public:
  std::string name;
  Point ul, ur, bl, br;
  Screen (const std::string name_, Point ul_, Point ur_, Point bl_, Point br_);
};

typedef std::vector<ScreenTriangle>::const_iterator Iterator;

struct ScreenTriangle_primitive {
public:
  typedef const ScreenTriangle *Id;
  typedef K::Point_3 Point;
  typedef K::Triangle_3 Datum;
private:
  Id m_pt;
public:
  ScreenTriangle_primitive() {}
  ScreenTriangle_primitive(Iterator it) : m_pt(&(*it)) {}
  const Id& id() const { return m_pt; }
  Datum datum() const { return Datum(m_pt->vertex(0), m_pt->vertex(1), m_pt->vertex(2)); }
  Point reference_point() const { return m_pt->vertex(0); }
};

typedef CGAL::AABB_traits<K, ScreenTriangle_primitive> My_AABB_traits;
typedef CGAL::AABB_tree<My_AABB_traits> Tree;

class Viewport
{
  size_t x, y, width, height;
};

class Window
{
 public:
  Window (boost::property_tree::ptree pt_);
  std::string name () const;
 protected:
  boost::property_tree::ptree pt;
};

class Port
{
 public:
  typedef CGAL::Aff_transformation_3<K> Matrix;
  static Port dimensioned (double w, double h);
  static Port dimensioned_centered (double w, double h);
  Port () { }

  std::pair<double, double> project (K::Point_3 p) const;

  K::Point_3 point (double over, double up) const;

  K::Vector_3 over () const;
  K::Vector_3 up () const;
  K::Vector_3 norm () const;

 public:
  Matrix m;
};

class Camera
{
 public:

  struct PhysicalDimensions {
    double width, height, left, right, bottom, top;
  };

  struct Viewport {
    double width, height, x, y;
  };
  
  typedef K::Vector_3 Offset;

 protected:
  
  PhysicalDimensions parse_physical_dimensions () const;
  Viewport parse_viewport () const;
  Offset calculate_offset () const;

 public:

  friend std::ostream &operator << (std::ostream &out, const Camera &c);

  Camera (CameraGroup &cg_, boost::property_tree::ptree pt_);
  std::string name () const;

  PhysicalDimensions physical_dimensions;
  Viewport viewport;
  Offset offset;

  void push_facets (std::vector<ScreenTriangle> &l);

  CameraGroup &cg;
  boost::property_tree::ptree pt;
  Port port;
};

typedef boost::math::quaternion<double> Quaternion;

class CameraGroup
{
 public:

  CameraGroup ();
  void parse (std::string fn);

 public:

  struct Intersection {
    const Camera *camera;
    K::Point_3 location;
    double px, py;
    Intersection (const Camera *camera_, K::Point_3 location_, double px_, double py_)
    : camera (camera_), location (location_), px (px_), py (py_) { }
    friend std::ostream &operator << (std::ostream &out, const Intersection &i);
  };

  template <typename OutputIterator> void Intersections (K::Ray_3, OutputIterator dest) const;

  boost::property_tree::ptree pt;
  std::map<std::string, Window> windows;
  std::map<std::string, Camera> cameras;
  Tree *m_tree;
};

template <typename OutputIterator> void CameraGroup::Intersections (K::Ray_3 ray, OutputIterator dest) const
{
  std::list<Tree::Object_and_primitive_id> i;
  m_tree->all_intersections (ray, std::back_inserter (i));

  for (auto p = i.begin(); p != i.end(); p++) {

    Tree::Object_and_primitive_id op = *p; 
    CGAL::Object object = op.first; 
    Tree::Primitive_id id = op.second; 

    K::Point_3 point; 
    K::Segment_3 segment; 

    if (CGAL::assign (point, object)) {

      std::pair<double, double> loc = id->camera->port.project (point);

      const Camera::Viewport &v = id->camera->viewport;
      double nx = v.x + (loc.first * v.width);
      double ny = v.y + (loc.second * v.height);
      
      Intersection s (id->camera, point, nx, ny);
      *dest++ = s;

    } else if (CGAL::assign (segment, object)) {
      ; // std::cout << "intersection object is a segment" << std::endl; 
    } else {
      ; // std::cout << "intersection object has nothing" << std::endl;
    }
  }
}

CameraGroup parse (std::string fn);

#endif /* HADRON_PSMOVE_CAMERA_GROUP */
