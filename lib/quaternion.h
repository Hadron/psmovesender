/* Copyright (c) 2012-2013, Hadron Industries, Inc. */

#ifndef HADRON_QUATERION
#define HADRON_QUATERION

#include <CGAL/Simple_cartesian.h>

#include <boost/math/quaternion.hpp>

template <typename T>
boost::math::quaternion<T> inverse (const boost::math::quaternion<T> &q)
{
  return boost::math::quaternion<T> (1, 0, 0, 0) / q;
}

template <typename T>
boost::math::quaternion<T> normalize (const boost::math::quaternion<T> &q)
{
  double w = q.R_component_1();
  double x = q.R_component_2();
  double y = q.R_component_3();
  double z = q.R_component_4();
  double inv_len = 1.0f / std::sqrt (w * w + x * x + y * y + z * z);
  return q * inv_len;
}

template <typename T>
typename CGAL::Simple_cartesian<T>::Vector_3 rotate 
(const boost::math::quaternion<T> &q, const typename CGAL::Simple_cartesian<T>::Vector_3 &p)
{
  auto v = q * boost::math::quaternion<T> (0, p.x(), p.y(), p.z()) / q;
  return typename CGAL::Simple_cartesian<T>::Vector_3 
    (v.R_component_2(), v.R_component_3(), v.R_component_4());
}

inline boost::math::quaternion<double> rotation
(const typename CGAL::Simple_cartesian<double>::Vector_3 &axis, double theta)
{
  auto naxis = axis / sqrt (axis.squared_length ());
  double sin2 = sin (theta / 2.);
  return boost::math::quaternion<double> (cos (theta / 2.), naxis.x() * sin2, naxis.y() * sin2, naxis.z() * sin2);
}

inline boost::math::quaternion<double> from_to 
(const typename CGAL::Simple_cartesian<double>::Vector_3 &from,
 const typename CGAL::Simple_cartesian<double>::Vector_3 &to)
{
  auto axis = cross_product (to, from);
  double qangle = (to * from) / (sqrt (to.squared_length ()) * sqrt (from.squared_length ()));
  return normalize (rotation (axis, qangle));
}

#if 0
template <class T>
boost::math::quaternion<T> create
(const class CGAL::Simple_cartesian<T>::Vector_3 &norm,
 const class CGAL::Simple_cartesian<T>::Vector_3 &over)
{
  auto up = cross_product (norm, over);
  double q1 = 0.5 * sqrt (1 + over.x() - up.y() - norm.z());
  double q2 = (1 / (4 * q1)) * (over.y() + up.x());
  double q3 = (1 / (4 * q1)) * (over.z() + norm.x());
  double q4 = (1 / (4 * q1)) * (norm.y() + up.z());
  return boost::math::quaternion<T> (q1, q2, q3, q4);
}

#else

inline boost::math::quaternion<double> create
(const typename CGAL::Simple_cartesian<double>::Vector_3 &norm,
 const typename CGAL::Simple_cartesian<double>::Vector_3 &over)
{
  auto up = cross_product (norm, over);
  double q1 = 0.5 * sqrt (1 + over.x() - up.y() - norm.z());
  double q2 = (1 / (4 * q1)) * (over.y() + up.x());
  double q3 = (1 / (4 * q1)) * (over.z() + norm.x());
  double q4 = (1 / (4 * q1)) * (norm.y() + up.z());
  return boost::math::quaternion<double> (q1, q2, q3, q4);
}
#endif

#endif /* HADRON_QUATERION */
