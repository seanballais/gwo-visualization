#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <corex/core/math_functions.hpp>
#include <corex/core/ds/Point.hpp>

#include <gwo_viz/GWO.hpp>

namespace gwo_viz
{
  GWO::GWO()
    : numItersPerformed(0) {}

  GWO::Solutions GWO::optimize(int32_t numIterations,
                               int32_t numWolves,
                               cx::Point bestSolution,
                               cx::Point minPt,
                               cx::Point maxPt)
  {
    GWO::Solutions solutions;

    std::vector<cx::Point> pack;
    for (int32_t i = 0; i < numWolves; i++) {
      float x = cx::getRandomRealUniformly(minPt.x, maxPt.x);
      float y = cx::getRandomRealUniformly(minPt.y, maxPt.y);

      pack.emplace_back(x, y);
    }

    std::sort(
      pack.begin(),
      pack.end(),
      [&bestSolution](const cx::Point& a, const cx::Point& b) {
        float aFitness = std::fabs(cx::distance2D(bestSolution, a));
        float bFitness = std::fabs(cx::distance2D(bestSolution, b));

        return aFitness < bFitness;
      });

    solutions.push_back(pack);

    this->numItersPerformed = 0;

    float a = 2.f;
    for (int32_t t = 0; t < numIterations; t++) {
      cx::Point alphaWolf = pack[0];
      cx::Point betaWolf = pack[1];
      cx::Point deltaWolf = pack[2];

      std::array<cx::Point, 3> Al;
      std::array<cx::Point, 3> Cl;
      std::array<cx::Point, 3> r1l;
      std::array<cx::Point, 3> r2l;

      for (int32_t n = 0; n < 3; n++) {
        r1l[n].x = cx::getRandomRealUniformly(0.f, 1.f);
        r1l[n].y = cx::getRandomRealUniformly(0.f, 1.f);
        r2l[n].x = cx::getRandomRealUniformly(0.f, 1.f);
        r2l[n].y = cx::getRandomRealUniformly(0.f, 1.f);

        Al[n].x = (2 * a * r1l[n].x) - a;
        Al[n].y = (2 * a * r1l[n].y) - a;
        Cl[n].x = 2 * r2l[n].x;
        Cl[n].y = 2 * r2l[n].y;
      }

      for (int32_t j = 0; j < numWolves; j++) {
        // Generate the coefficient values.
        cx::Point Aw;
        cx::Point Cw;
        cx::Point r1w;
        cx::Point r2w;

        r1w.x = cx::getRandomRealUniformly(0.f, 1.f);
        r1w.y = cx::getRandomRealUniformly(0.f, 1.f);
        r2w.x = cx::getRandomRealUniformly(0.f, 1.f);
        r2w.y = cx::getRandomRealUniformly(0.f, 1.f);

        Aw.x = (2 * a * r1w.x) - a;
        Aw.y = (2 * a * r1w.y) - a;
        Cw.x = 2 * r2w.x;
        Cw.y = 2 * r2w.y;

        auto wolf = pack[j];

        auto Da = cx::vec2Abs(cx::pairwiseMult(Cl[0], alphaWolf) - wolf);
        auto Db = cx::vec2Abs(cx::pairwiseMult(Cl[1], betaWolf) - wolf);
        auto Dd = cx::vec2Abs(cx::pairwiseMult(Cl[2], deltaWolf) - wolf);

        auto X1 = alphaWolf - cx::pairwiseMult(Al[0], Da);
        auto X2 = betaWolf - cx::pairwiseMult(Al[1], Db);
        auto X3 = deltaWolf - cx::pairwiseMult(Al[2], Dd);

        pack[j] = (X1 + X2 + X3) / 3;
      }

      std::sort(
        pack.begin(),
        pack.end(),
        [&bestSolution](const cx::Point& a, const cx::Point& b) {
          float aFitness = std::fabs(cx::distance2D(bestSolution, a));
          float bFitness = std::fabs(cx::distance2D(bestSolution, b));

          return aFitness < bFitness;
        });
      solutions.push_back(pack);

      a = 2.f - (2.f * (static_cast<float>(t) / numIterations));

      this->numItersPerformed++;
    }

    return solutions;
  }

  int32_t GWO::getNumItersPerformed()
  {
    return this->numItersPerformed;
  }
}
