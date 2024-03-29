#ifndef COREX_CORE_COMPONENTS_RENDERABLE_TYPE_HPP
#define COREX_CORE_COMPONENTS_RENDERABLE_TYPE_HPP

namespace corex::core
{
  enum class RenderableType
  {
    TEXT, SPRITE, PRIMITIVE_RECTANGLE, PRIMITIVE_POLYGON, LINE_SEGMENTS,
    PRIMITIVE_CIRCLE
  };
}

namespace cx
{
  using namespace corex::core;
}

#endif
