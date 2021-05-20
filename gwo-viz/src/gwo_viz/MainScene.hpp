#ifndef GWOVIZ_MAIN_SCENE_HPP
#define GWOVIZ_MAIN_SCENE_HPP

#include <atomic>

#include <EASTL/vector.h>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include <corex/core/AssetManager.hpp>
#include <corex/core/Camera.hpp>
#include <corex/core/Scene.hpp>
#include <corex/core/Settings.hpp>
#include <corex/core/ds/Circle.hpp>
#include <corex/core/ds/LineSegments.hpp>
#include <corex/core/ds/NPolygon.hpp>
#include <corex/core/ds/Point.hpp>
#include <corex/core/ds/Polygon.hpp>
#include <corex/core/events/KeyboardEvent.hpp>
#include <corex/core/events/MouseButtonEvent.hpp>
#include <corex/core/events/MouseMovementEvent.hpp>
#include <corex/core/events/MouseScrollEvent.hpp>
#include <corex/core/events/sys_events.hpp>

namespace gwo_viz
{
  class MainScene : public corex::core::Scene
  {
  public:
    MainScene(entt::registry& registry,
              entt::dispatcher& eventDispatcher,
              corex::core::AssetManager& assetManager,
              corex::core::Camera& camera);

    void init() override;
    void update(float timeDelta) override;
    void dispose() override;

  private:
    float regionWidth;
    float regionHeight;
    cx::Point coordOrigin;
    cx::Point bestSol;
    cx::Scene::Entity ent_bestSol;

    int32_t numIterations;
    int32_t numWolves;

    int32_t currIterDisplayed;

    void flashBestSolPosition(float timeDelta);

    void buildControls();

    void handleWindowEvents(const corex::core::WindowEvent& e);
  };
}

#endif
