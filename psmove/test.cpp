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

#include "FGParser.h"

int main()
{
  CameraGroup cg = parse ("/home/kdienes/test5.xml");

  for (auto i = cg.cameras.begin(); i != cg.cameras.end(); i++) {
    std::cout << i->second << std::endl;
  }

  for (int x = -600; x <= 600; x += 10) {

    K::Ray_3 rq (Point (0.0, 0.0, 200.0), Point (x * 1, 0.0, -200.0));
    std::vector<CameraGroup::Intersection> i;

    cg.Intersections (rq, std::back_inserter (i));
    for (auto p = i.begin(); p != i.end(); p++) {
      std::cout << *p << std::endl;
    }
  }

  return EXIT_SUCCESS;
}
