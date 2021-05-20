#ifndef GWOVIZ_APPLICATION_HPP
#define GWOVIZ_APPLICATION_HPP

#include <EASTL/string.h>

#include <corex/core/Application.hpp>

namespace gwo_viz
{
  class Application : public corex::core::Application
  {
  public:
    Application(const eastl::string& windowTitle);

    void init() override;
    void dispose() override;
  };
}

#endif

