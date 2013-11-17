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
