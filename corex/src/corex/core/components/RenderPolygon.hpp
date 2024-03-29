#ifndef COREX_CORE_COMPONENTS_RENDER_POLYGON_HPP
#define COREX_CORE_COMPONENTS_RENDER_POLYGON_HPP

#include <EASTL/vector.h>
#include <SDL2/SDL.h>

#include <corex/core/ds/Point.hpp>

namespace corex::core
{
  struct RenderPolygon
  {
    eastl::vector<Point> vertices;
    SDL_Color colour;
    bool isFilled;
  };
}

namespace cx
{
  using namespace corex::core;
}

#endif
