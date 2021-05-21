#ifndef GWOVIZ_GWO_RESULT_HPP
#define GWOVIZ_GWO_RESULT_HPP

#include <cstdlib>

#include <vector>

#include <corex/core/ds/Point.hpp>

namespace gwo_viz {
  using Solutions = std::vector<std::vector<cx::Point>>;
  struct GWOResult
  {
    Solutions solutions;
    std::vector<cx::Point> wolfPreys;
  };
}

#endif
