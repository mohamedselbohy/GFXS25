#pragma once

#include "../ecs/world.hpp"
#include "../components/frog-movement.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include "../application.hpp"

namespace our
{

    // The movement system is responsible for moving every entity which contains a MovementComponent.
    // This system is added as a simple example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/movement.hpp"
    class FrogMovementSystem {
        float currentTime = 0;
        Application* app;
    public:
        void enter(Application* app){
            this->app = app;
        }
        // This should be called every frame to update all entities containing a MovementComponent. 
        void update(World* world, float deltaTime) {
            // For each entity in the world

            for(auto entity : world->getEntities()){
                // Get the movement component if it exists
                FrogMovementComponent* movement = entity->getComponent<FrogMovementComponent>();
                // If the movement component exists
                if(movement){
                    // Change the position and rotation based on the linear & angular velocity and delta time.
                    currentTime += deltaTime;
                    if(app->getKeyboard().isPressed(GLFW_KEY_W) && currentTime >= 0.25){ entity->localTransform.position += glm::vec3(0.0, 0.0, 1.0) * movement->linearVelocity; currentTime = 0;}
                    if(app->getKeyboard().isPressed(GLFW_KEY_S) && currentTime >= 0.25){ entity->localTransform.position += glm::vec3(0.0, 0.0, -1.0) * movement->linearVelocity; currentTime = 0;}
                    if(app->getKeyboard().isPressed(GLFW_KEY_A) && currentTime >= 0.25){ entity->localTransform.position += glm::vec3(-1.0, 0.0, 0.0) * movement->linearVelocity; currentTime = 0;}
                    if(app->getKeyboard().isPressed(GLFW_KEY_D) && currentTime >= 0.25){ entity->localTransform.position += glm::vec3(1.0, 0.0, 0.0) * movement->linearVelocity; currentTime = 0;}
                }
            }
        }

    };

}
