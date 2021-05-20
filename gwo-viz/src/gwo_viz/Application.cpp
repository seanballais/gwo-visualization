#include <iostream>

#include <EASTL/string.h>
#include <EASTL/unique_ptr.h>

#include <corex/core/Application.hpp>
#include <corex/core/Scene.hpp>

#include <gwo_viz/Application.hpp>
#include <gwo_viz/MainScene.hpp>

namespace gwo_viz
{
  Application::Application(const eastl::string& windowTitle)
    : corex::core::Application(windowTitle) {}

  void Application::init()
  {
    std::cout << "Initializing GWO Visualization..." << std::endl;
    auto& mainScene = this->sceneManager->addScene<gwo_viz::MainScene>();
    this->sceneManager->setRootScene(mainScene);
  }

  void Application::dispose()
  {
    std::cout << "Disposing GWO Visualization..." << std::endl;
  }
}

namespace corex
{
  eastl::unique_ptr<corex::core::Application> createApplication()
  {
    return eastl::make_unique<gwo_viz::Application>("GWO Visualization");
  }
}
