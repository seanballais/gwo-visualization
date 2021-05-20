#ifndef GWOVIZ_GWO_HPP
#define GWOVIZ_GWO_HPP

#include <cstdlib>

#include <vector>

#include <corex/core/ds/Point.hpp>

namespace gwo_viz
{
  class GWO
  {
  public:
    GWO();

    using Solutions = std::vector<std::vector<cx::Point>>;
    Solutions optimize(int32_t numIterations,
                       int32_t numWolves,
                       cx::Point bestSolution,
                       cx::Point minPt,
                       cx::Point maxPt);

    int32_t getNumItersPerformed();
  private:
    int32_t numItersPerformed;
    std::vector<std::vector<cx::Point>> solutions;
  };
}

#endif
