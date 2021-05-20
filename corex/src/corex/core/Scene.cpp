#include <cstdint>

#include <entt/entt.hpp>

#include <corex/core/AssetManager.hpp>
#include <corex/core/Camera.hpp>
#include <corex/core/Scene.hpp>
#include <corex/core/ds/LineSegments.hpp>
#include <corex/core/ds/Point.hpp>
#include <corex/core/components/Position.hpp>
#include <corex/core/components/Renderable.hpp>
#include <corex/core/components/RenderableType.hpp>
#include <corex/core/components/RenderCircle.hpp>
#include <corex/core/components/RenderLineSegments.hpp>

namespace corex::core
{
  Scene::Scene(entt::registry& registry,
               entt::dispatcher& eventDispatcher,
               AssetManager& assetManager,
               Camera& camera,
               float ppmRatio)
    : registry(registry)
    , eventDispatcher(eventDispatcher)
    , assetManager(assetManager)
    , camera(camera)
    , entities({})
    , ppmRatio(ppmRatio)
  {
    for (int i = 0; i < this->kNumLayers; i++) {
      this->layerAvailabilityStatuses[i] = true;
    }

    for (int i = 0; i < this->kNumSortingLayers; i++) {
      this->sortingLayerAvailabilityStatuses[i] = true;
    }
  }

  SceneStatus Scene::getStatus()
  {
    return this->status;
  }

  float Scene::getPPMRatio()
  {
    return this->ppmRatio;
  }

  Scene::Entity Scene::createSceneEntity()
  {
    Scene::Entity entity = this->registry.create();
    Scene::Layer defaultLayer = 1;
    Scene::SortingLayer defaultSortingLayer = 1;

    this->registry.emplace<Scene::EntitySceneProperty>(
      entity, defaultLayer, defaultSortingLayer
    );
    this->entities.push_back(entity);

    return entity;
  }

  void Scene::setEntityLayer(Scene::Entity entity, Scene::Layer layer)
  {
    this->registry.patch<Scene::EntitySceneProperty>(
      entity,
      [&layer](auto& props) {
        props.layer = layer;
      }
    );
  }

  Scene::Layer Scene::getEntityLayer(Scene::Entity entity)
  {
    const auto& props = this->registry.get<Scene::EntitySceneProperty>(entity);
    return props.layer;
  }

  void Scene::setEntitySortingLayer(Scene::Entity entity,
                                    Scene::SortingLayer layer)
  {
    this->registry.patch<Scene::EntitySceneProperty>(
      entity,
      [&layer](auto& props) {
        props.sortingLayer = layer;
      }
    );
  }

  Scene::SortingLayer Scene::getEntitySortingLayer(Scene::Entity entity)
  {
    const auto& props = this->registry.get<Scene::EntitySceneProperty>(entity);
    return props.layer;
  }

  void Scene::enableLayer(Scene::Layer layer)
  {
    this->layerAvailabilityStatuses[layer] = true;
  }

  void Scene::disableLayer(Scene::Layer layer)
  {
    this->layerAvailabilityStatuses[layer] = false;
  }

  bool Scene::isLayerEnabled(Scene::Layer layer)
  {
    return this->layerAvailabilityStatuses[layer];
  }

  void Scene::enableSortingLayer(Scene::SortingLayer layer)
  {
    this->sortingLayerAvailabilityStatuses[layer] = true;
  }

  void Scene::disableSortingLayer(Scene::SortingLayer layer)
  {
    this->sortingLayerAvailabilityStatuses[layer] = false;
  }

  bool Scene::isSortingLayerEnabled(Scene::SortingLayer layer)
  {
    return this->sortingLayerAvailabilityStatuses[layer];
  }

  void Scene::setSceneStatus(SceneStatus status)
  {
    this->status = status;
  }

  Scene::Entity Scene::createLineSegmentsEntity(
    float z,
    eastl::vector<Point> points,
    SDL_Color color,
    int8_t sortingLayerID)
  {
    Scene::Entity e = this->createSceneEntity();
    this->setEntityComponent<Position>(e, 0.f, 0.f, z, sortingLayerID);
    this->setEntityComponent<Renderable>(e, RenderableType::LINE_SEGMENTS);
    this->setEntityComponent<RenderLineSegments>(e, points, color);

    return e;
  }

  Scene::Entity Scene::createCircleEntity(float x,
                                          float y,
                                          float z,
                                          float radius,
                                          bool isFilled,
                                          SDL_Color colour,
                                          int8_t sortingLayerID)
  {
    Scene::Entity e = this->createSceneEntity();
    this->setEntityComponent<Position>(e, x, y, z, sortingLayerID);
    this->setEntityComponent<Renderable>(e, RenderableType::PRIMITIVE_CIRCLE);
    this->setEntityComponent<RenderCircle>(e, radius, colour, isFilled);

    return e;
  }
}
