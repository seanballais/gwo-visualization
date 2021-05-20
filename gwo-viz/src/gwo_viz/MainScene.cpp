#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>

#include <EASTL/vector.h>
#include <entt/entt.hpp>
#include <imgui.h>
#include <implot/implot.h>
#include <nlohmann/json.hpp>
#include <SDL2/SDL.h>

#include <corex/core/AssetManager.hpp>
#include <corex/core/CameraZoomState.hpp>
#include <corex/core/Scene.hpp>
#include <corex/core/math_functions.hpp>
#include <corex/core/utils.hpp>
#include <corex/core/components/Position.hpp>
#include <corex/core/components/Renderable.hpp>
#include <corex/core/components/RenderableType.hpp>
#include <corex/core/components/RenderCircle.hpp>
#include <corex/core/components/RenderLineSegments.hpp>
#include <corex/core/components/RenderRectangle.hpp>
#include <corex/core/components/RenderPolygon.hpp>
#include <corex/core/components/Text.hpp>
#include <corex/core/ds/Circle.hpp>
#include <corex/core/ds/Point.hpp>
#include <corex/core/events/KeyboardEvent.hpp>
#include <corex/core/events/MouseButtonEvent.hpp>
#include <corex/core/events/MouseMovementEvent.hpp>
#include <corex/core/events/MouseScrollEvent.hpp>
#include <corex/core/events/sys_events.hpp>
#include <corex/core/systems/KeyState.hpp>
#include <corex/core/systems/MouseButtonState.hpp>
#include <corex/core/systems/MouseButtonType.hpp>

#include <gwo_viz/MainScene.hpp>

namespace gwo_viz
{
  MainScene::MainScene(entt::registry& registry,
                       entt::dispatcher& eventDispatcher,
                       corex::core::AssetManager& assetManager,
                       corex::core::Camera& camera)
    : regionWidth(750.f)
    , regionHeight(550.f)
    , coordOrigin(15.f, 15.f)
    , bestSol()
    , ent_bestSol(entt::null)
    , numIterations(0)
    , numWolves(0)
    , currIterDisplayed(0)
    , corex::core::Scene(registry, eventDispatcher, assetManager, camera) {}

  void MainScene::init()
  {
    std::cout << "MainScene is being initialized..." << std::endl;

    this->eventDispatcher.sink<corex::core::WindowEvent>()
      .connect<&MainScene::handleWindowEvents>(this);

    auto axesPoints = eastl::vector<cx::Point>{
      { coordOrigin.x, coordOrigin.y + regionHeight },
      coordOrigin,
      { coordOrigin.x + regionWidth, coordOrigin.y }
    };
    SDL_Color axesColour{ 64, 64, 64, 255 };
    this->createLineSegmentsEntity(0.f, axesPoints, axesColour, 1);

    SDL_Color bestPositionColour{ 12, 104, 47, 255 };
    this->ent_bestSol = this->createCircleEntity(coordOrigin.x, coordOrigin.y,
                                                 0.f, 5.f, true,
                                                 bestPositionColour, 1);
  }

  void MainScene::update(float timeDelta)
  {
    this->flashBestSolPosition(timeDelta);

    this->buildControls();
  }

  void MainScene::dispose()
  {
    std::cout << "Disposing MainScene. Bleep, bloop, zzzz." << std::endl;
  }

  void MainScene::flashBestSolPosition(float timeDelta)
  {
    static float accumTime = 0.f;
    static bool isInPrimaryColour = false;
    static SDL_Color primaryColour{ 12, 104, 47, 255 };
    static SDL_Color secondaryColour{19, 148, 68, 255 };
    accumTime += timeDelta;

    if (accumTime > 0.5) {
      // After 500ms, switch colours.
      auto& bestSolCircle = this->getEntityComponent<cx::RenderCircle>(
        this->ent_bestSol);

      if (isInPrimaryColour) {
        // Switch to secondary.
        bestSolCircle.colour = secondaryColour;
        isInPrimaryColour = false;
      } else {
        // Switch to primary.
        bestSolCircle.colour = primaryColour;
        isInPrimaryColour = true;
      }

      accumTime = 0.f;
    }
  }

  void MainScene::buildControls()
  {
    ImGui::Begin("Controls");

    if (ImGui::Button("Generate Best Position")) {
      auto& bestSolPos = this->getEntityComponent<cx::Position>(
        this->ent_bestSol);

      float newX = cx::getRandomRealUniformly(coordOrigin.x, this->regionWidth);
      float newY = cx::getRandomRealUniformly(coordOrigin.y,
                                              this->regionHeight);
      bestSolPos.x = newX;
      bestSolPos.y = newY;
    }

    ImGui::Separator();

    if (ImGui::InputInt("No. of Iterations", &this->numIterations)) {
      this->currIterDisplayed = cx::clamp(this->currIterDisplayed,
                                          0, this->numIterations - 1);
    }

    ImGui::InputInt("No. of Wolves", &this->numWolves);

    ImGui::Button("Generate Solutions");

    ImGui::Separator();

    ImGui::Text("Iteration Displayed");

    if (ImGui::Button("<")) {
      if (this->numIterations > 0) {
        this->currIterDisplayed = cx::mod(this->currIterDisplayed - 1,
                                          this->numIterations);
      }
    }

    ImGui::SameLine();

    if (ImGui::InputInt("", &this->currIterDisplayed)) {
      this->currIterDisplayed = cx::clamp(this->currIterDisplayed,
                                          0, this->numIterations - 1);
    }

    ImGui::SameLine();

    if (ImGui::Button(">")) {
      if (this->numIterations > 0) {
        this->currIterDisplayed = cx::mod(this->currIterDisplayed + 1,
                                          this->numIterations);
      }
    };

    ImGui::End();
  }

  void MainScene::handleWindowEvents(const corex::core::WindowEvent& e)
  {
    if (e.event.window.event == SDL_WINDOWEVENT_CLOSE) {
      this->setSceneStatus(corex::core::SceneStatus::DONE);
    }
  }
}
