#include <random>
#include <vector>
#include <chrono>
#include "../ecs/entity.hpp"
#include "../ecs/world.hpp"
#include "../components/mesh-renderer.hpp"
#include "../components/collision.hpp"
#include "../components/movement.hpp"
#include <asset-loader.hpp>
#include <glm/glm.hpp>

namespace our {
    const float CAR_SPAWN_INTERVAL = 1.0f; // seconds
    const float WOOD_SPAWN_INTERVAL = 0.5f;

    const float MIN_DISTANCE_BETWEEN_ENTITIES = 10.0f;

    const glm::vec3 CAR_INITIAL_POS1 = {-50, 0, -28};
    const glm::vec3 CAR_INITIAL_POS2 = {-50, 0, -7.5};
    const glm::vec3 WOOD_INITIAL_POS1 = {-50, -0.2f, -35.5};
    const glm::vec3 WOOD_INITIAL_POS2 = {-50, -0.2f, -40.5};

    // Separate RNGs
    std::mt19937 rngCar1(std::random_device{}());
    std::mt19937 rngWood(std::random_device{}());
    std::mt19937 rngCar2(std::random_device{}());
    std::uniform_int_distribution<int> rand01(0, 1);

    class SpawnEntities {
    public:
        std::vector<Entity*> cars;
        std::vector<Entity*> woods;
        std::vector<Entity*> woodAuras;

        float carSpawnTimer = 0;
        float woodSpawnTimer = 0;
        std::vector<Entity*> getWoods() {
            return woodAuras;
        }
        void Update(float deltaTime, World *world) {
            carSpawnTimer += deltaTime;
            woodSpawnTimer += deltaTime;

            if (carSpawnTimer >= CAR_SPAWN_INTERVAL) {
                carSpawnTimer = 0;

                if (rand01(rngCar1) && !isOccupied(CAR_INITIAL_POS1, 'c'))
                        spawnCar(CAR_INITIAL_POS1, world);
                if(rand01(rngCar2) && !isOccupied(CAR_INITIAL_POS2, 'c'))
                        spawnCar(CAR_INITIAL_POS2, world);
            }

            if (woodSpawnTimer >= WOOD_SPAWN_INTERVAL) {
                woodSpawnTimer = 0;

                if (rand01(rngWood)) {
                    if (!isOccupied(WOOD_INITIAL_POS1, 'w')) {
                        spawnWood(WOOD_INITIAL_POS1, world);
                        spawnWood(WOOD_INITIAL_POS2, world);
                    }
                }
            }
        }

        bool isOccupied(const glm::vec3& pos, char c) {
            if((c=='c'&&cars.empty()) || (c=='w'&&woods.empty())) return false;
            Entity* lastAdded;
            if(c=='c') lastAdded = cars[cars.size()-1];
            else  lastAdded = woods[woods.size()-1];
            glm::vec3 currentPos = lastAdded->localTransform.position;
            float dist = glm::distance(currentPos, pos);
            if (dist < MIN_DISTANCE_BETWEEN_ENTITIES) {
                return true; 
            }
            return false;
        }

        void spawnCar(const glm::vec3& pos, World *world) {
            Entity* car = world->add();
            car->localTransform.position = pos;
            car->localTransform.rotation = {0, 0, 0};
            car->localTransform.scale = {-1.8, 1.8, -1.8};
            car->name = "car1";

            auto* mesh = car->addComponent<MeshRendererComponent>();
            mesh->material = AssetLoader<Material>().get("car1");
            mesh->mesh = AssetLoader<Mesh>().get("car1");

            car->addComponent<CollisionComponent>();
            car->addComponent<MovementComponent>()->linearVelocity = {10, 0, 0};

            cars.push_back(car);
        }

        void spawnWood(const glm::vec3& pos, World *world) {
            Entity* wood = world->add();
            wood->localTransform.position = {pos.x, -4.8, pos.z};
            wood->localTransform.rotation = {0, 0, 0};
            wood->localTransform.scale = {2.5, 2.5, 2.5};
            wood->name = "wood";

            auto* mesh = wood->addComponent<MeshRendererComponent>();
            mesh->material = AssetLoader<Material>().get("wood");
            mesh->material->transparent = true;
            mesh->mesh = AssetLoader<Mesh>().get("wood");

            wood->addComponent<CollisionComponent>();
            wood->addComponent<MovementComponent>()->linearVelocity = {5, 0, 0};

            woods.push_back(wood);
            spawnWoodAura(pos, world);
            
        }
        void spawnWoodAura(const glm::vec3& pos, World *world) {
            Entity* aura = world->add();
            aura->localTransform.position = {pos.x, 0, pos.z};
            aura->localTransform.rotation = {0, 0, 0};
            aura->localTransform.scale = {2.5, 2.5, 2.5};
            aura->name = "woodAura";

            auto* mesha = aura->addComponent<MeshRendererComponent>();
            mesha->material = AssetLoader<Material>().get("woodAura");
            mesha->material->transparent = true;
            mesha->mesh = AssetLoader<Mesh>().get("wood");

            aura->addComponent<CollisionComponent>();
            aura->addComponent<MovementComponent>()->linearVelocity = {5, 0, 0};
            woodAuras.push_back(aura);
        }
    };
}
