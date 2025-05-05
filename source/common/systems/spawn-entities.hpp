#include <random>
#include <vector>
#include <chrono>
#include "../ecs/entity.hpp"
#include "../ecs/world.hpp"
#include "../components/mesh-renderer.hpp"
#include "../components/collision.hpp"
#include "../components/movement.hpp"
#include <asset-loader.hpp>

namespace our {
    const float CAR_SPAWN_INTERVAL = 3.0f; // seconds
    const float WOOD_SPAWN_INTERVAL = 3.0f;
    
    const glm::vec3 CAR_INITIAL_POS = {30, 0, -28};
    const glm::vec3 WOOD_INITIAL_POS = {30, -0.2f, -38.5};

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> rand01(0, 1);
    class SpawnEntities {
        public:
        std::vector<Entity*> cars;
        std::vector<Entity*> woods;
        
        float carSpawnTimer = 0;
        float woodSpawnTimer = 0;
        // You can call this in the update() loop
        void Update(float deltaTime, World *world) {
            carSpawnTimer += deltaTime;
            woodSpawnTimer += deltaTime;
        
            if (carSpawnTimer >= CAR_SPAWN_INTERVAL) {
                carSpawnTimer = 0;
                if (rand01(rng)) {
                    spawnCar(CAR_INITIAL_POS, world);
                }
            }
        
            if (woodSpawnTimer >= WOOD_SPAWN_INTERVAL) {
                woodSpawnTimer = 0;
                if (rand01(rng)) {
                    spawnWood(WOOD_INITIAL_POS, world);
                }
            }
        }
        
        void spawnCar(const glm::vec3& pos, World *world) {
            Entity* car = world->add();
            car->localTransform.position=pos;
            car->localTransform.rotation = {0, 0, 0};
            car->localTransform.scale = {-1.5, 1.5, -1.5};
            car->name = "car1";
            car->addComponent<MeshRendererComponent>();
            car->getComponent<MeshRendererComponent>()->material = AssetLoader<Material>().get("car1");
            car->getComponent<MeshRendererComponent>()->mesh = AssetLoader<Mesh>().get("car1");
            car->addComponent<CollisionComponent>();
            car->addComponent<MovementComponent>(); 
            car->getComponent<MovementComponent>()->linearVelocity = {-3, 0, 0};
            cars.push_back(car);
        }
        
        void spawnWood(const glm::vec3& pos, World *world) {
            Entity* wood = world->add();
            wood->localTransform.position=pos;
            wood->localTransform.rotation = {0, 0, 0};
            wood->localTransform.scale = {-5, 5, -5};
            wood->name = "wood";
            wood->addComponent<MeshRendererComponent>();
            wood->getComponent<MeshRendererComponent>()->material = AssetLoader<Material>().get("wood");
            wood->getComponent<MeshRendererComponent>()->mesh = AssetLoader<Mesh>().get("wood");
            wood->addComponent<CollisionComponent>();
            wood->addComponent<MovementComponent>(); 
            wood->getComponent<MovementComponent>()->linearVelocity = {-3, 0, 0};
            woods.push_back(wood);
        }
    };     
}
