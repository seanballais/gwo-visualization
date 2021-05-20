#ifndef GWOVIZ_GWO_HPP
#define GWOVIZ_GWO_HPP

#include <vector>

#include <corex/core/ds/Point.hpp>

namespace gwo_viz
{
  class GWO
  {
  private:
    std::vector<std::vector<cx::Point>> solutions;
  };
}

#endif
