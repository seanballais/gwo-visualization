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
    , bestSol(coordOrigin.x, coordOrigin.y)
    , ent_bestSol(entt::null)
    , numIterations(0)
    , numWolves(0)
    , currIterDisplayed(0)
    , gwo()
    , solutions()
    , solutionEntities()
    , isRunningGWO(false)
    , isNewSolutionGenerated(false)
    , isIterDisplayedChanged(false)
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
    if (this->isNewSolutionGenerated) {
      for (auto& entity : this->solutionEntities) {
        this->registry.destroy(entity);
      }

      this->solutionEntities.clear();

      SDL_Color solutionColour;
      auto& currIteration = this->solutions[this->currIterDisplayed];
      for (int32_t i = 0; i < this->numWolves; i++) {
        if (i == 0) {
          // Entity for the alpha wolf.
          solutionColour = SDL_Color{ 79, 10, 22, 255 };
        } else if (i == 1) {
          // Entity for the beta wolf.
          solutionColour = SDL_Color{ 82, 43, 15, 255 };
        } else if (i == 2) {
          // Entity for the delta wolf.
          solutionColour = SDL_Color{ 82, 78, 15, 255 };
        } else {
          solutionColour = SDL_Color{ 10, 41, 79, 255 };
        }

        this->solutionEntities.push_back(
          this->createCircleEntity(currIteration[i].x, currIteration[i].y,
                                   0.f, 5.f, true, solutionColour, 1)
        );
      }
    }

    if (this->isIterDisplayedChanged) {
      for (int32_t i = 0; i < this->numWolves; i++) {
        auto& wolfPos = this->getEntityComponent<cx::Position>(
          solutionEntities[i]);
        wolfPos.x = this->solutions[this->currIterDisplayed][i].x;
        wolfPos.y = this->solutions[this->currIterDisplayed][i].y;
      }

      this->isIterDisplayedChanged = false;
    }

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
      bestSol.x = newX;
      bestSol.y = newY;
      bestSolPos.x = newX;
      bestSolPos.y = newY;
    }

    ImGui::Separator();

    if (ImGui::InputInt("No. of Iterations", &this->numIterations)) {
      // We are not subtracting numIterations by 1 in consideration of the
      // initial random population.
      this->currIterDisplayed = cx::clamp(this->currIterDisplayed,
                                          0, this->numIterations);
    }

    ImGui::InputInt("No. of Wolves", &this->numWolves);

    if (this->isRunningGWO) {
      ImGui::Text("Iteration #%d of %d",
                  this->gwo.getNumItersPerformed(),
                  this->numIterations);
    } else {
      if (ImGui::Button("Generate Solutions")) {
        this->isRunningGWO = true;
        std::thread gwoThread{
          [this](int32_t numIterations,
                 int32_t numWolves,
                 cx::Point bestSolution,
                 cx::Point minPt,
                 cx::Point maxPt) {
            this->solutions = this->gwo.optimize(numIterations,
                                                 numWolves,
                                                 bestSolution,
                                                 minPt,
                                                 maxPt);
            this->isRunningGWO = false;
            this->isNewSolutionGenerated = true;
          },
          this->numIterations,
          this->numWolves,
          this->bestSol,
          this->coordOrigin,
          this->coordOrigin + cx::Point{ this->regionWidth, this->regionHeight }
        };

        gwoThread.detach();
      }
    }

    ImGui::Separator();

    ImGui::Text("Iteration Displayed");

    if (ImGui::Button("<")) {
      if (this->numIterations > 0) {
        // Only change the current iteration displayed if we have set a non-zero
        // number of iterations.
        //
        // We are increasing numIterations by 1 in consideration of the
        // initial random population.
        this->currIterDisplayed = cx::mod(this->currIterDisplayed - 1,
                                          this->numIterations + 1);
        this->isIterDisplayedChanged = true;
      }
    }

    ImGui::SameLine();

    if (ImGui::InputInt("", &this->currIterDisplayed)) {
      // We are not subtracting numIterations by 1 in consideration of the
      // initial random population.
      this->currIterDisplayed = cx::clamp(this->currIterDisplayed,
                                          0, this->numIterations);
    }

    ImGui::SameLine();

    if (ImGui::Button(">")) {
      if (this->numIterations > 0) {
        // Only change the current iteration displayed if we have set a non-zero
        // number of iterations.
        //
        // We are increasing numIterations by 1 in consideration of the
        // initial random population.
        this->currIterDisplayed = cx::mod(this->currIterDisplayed + 1,
                                          this->numIterations + 1);
        this->isIterDisplayedChanged = true;
      }
    };

    ImGui::Separator();

    ImGui::Text("Legend:");
    ImGui::Text("- Flashing Green Circle is the best position.");
    ImGui::Text("- Red circle is the alpha wolf.");
    ImGui::Text("- Orange circle is the beta wolf.");
    ImGui::Text("- Yellow circle is the alpha wolf.");

    ImGui::End();
  }

  void MainScene::handleWindowEvents(const corex::core::WindowEvent& e)
  {
    if (e.event.window.event == SDL_WINDOWEVENT_CLOSE) {
      this->setSceneStatus(corex::core::SceneStatus::DONE);
    }
  }
}
