#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/frog-movement.hpp>
#include <asset-loader.hpp>
#include <systems/collision-system.hpp>
#include <systems/spawn-entities.hpp>
#include <ecs/entity.hpp>
#include <components/mesh-renderer.hpp>

// This state shows how to use the ECS framework and deserialization.
class Playstate: public our::State {
    int gameState = 1; // 1 = running, 0 = lost, 2 = won
    double endTime = 0.0; // Time to wait before resetting
    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::FrogMovementSystem frogMovementSystem;
    our::CollisionSystem collisionSystem;
    our::SpawnEntities spawnEntities;
    public:
    void onInitialize() override {
        spawnEntities.cars.clear();
        spawnEntities.woodAuras.clear();
        spawnEntities.woods.clear();
        spawnEntities.carSpawnTimer = 0;
        spawnEntities.woodSpawnTimer = 0;
        // our::FreeCameraControllerSystem::total_time = 0.0f;
        // First of all, we get the scene configuration from the app config
        auto& config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if(config.contains("assets")){
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if(config.contains("world")){
            world.deserialize(config["world"]);
        }
        // We initialize the camera controller system since it needs a pointer to the app
        cameraController.enter(getApp());
        frogMovementSystem.enter(getApp());
        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
        for(auto entity: world.getEntities()) {
            if(entity->name == "frog") {
                collisionSystem.setPlayer(entity);
                break;
            }
        }
        // std::cout<<"intitialize play state finished"<<'\n';
    }

    // void onDraw(double deltaTime) override {
    //     // Here, we just run a bunch of systems to control the world logic
    //     spawnEntities.Update((float)deltaTime, &world);
    //     movementSystem.update(&world, (float)deltaTime);
    //     frogMovementSystem.update(&world, (float)deltaTime);
    //     cameraController.update(&world, (float)deltaTime);
    //     // And finally we use the renderer system to draw the scene
    //     int collisionResult = collisionSystem.update(&world, spawnEntities.getWoods());
    //     renderer.render(&world);

    //     // Get a reference to the keyboard object
    //     auto& keyboard = getApp()->getKeyboard();
    //     if(keyboard.justPressed(GLFW_KEY_ESCAPE)){
    //         // If the escape  key is pressed in this frame, go to the play state
    //         getApp()->changeState("menu");
    //     }
    // }
    void onDraw(double deltaTime) override {
        if (gameState == 1) {
            spawnEntities.Update((float)deltaTime, &world);
            movementSystem.update(&world, (float)deltaTime);
            frogMovementSystem.update(&world, (float)deltaTime);
            cameraController.update(&world, (float)deltaTime);
    
            gameState = collisionSystem.update(&world, spawnEntities.getWoods());
    
            if (gameState == 2) {
                // set timer to show win object for 3 seconds
                endTime = glfwGetTime() + 3.0;
            } else if (gameState == 0) {
                // Lose the game immediately reset the game
                getApp()->changeState("menu");
                gameState = 1;
                return;
            }
        }
    
        renderer.render(&world);
    
        if (gameState == 2) {
            // render win obj
            our::Entity* win = (&world)->add();
            win->localTransform.position = {0, 5, -35};
            win->localTransform.rotation = {0, 0, 0};
            win->localTransform.scale = {0.3, 0.3, 0.3};
            win->name = "win";

            auto* mesh = win->addComponent<our::MeshRendererComponent>();
            mesh->material = our::AssetLoader<our::Material>().get("win");
            mesh->material->transparent = true;
            mesh->mesh = our::AssetLoader<our::Mesh>().get("win");

    
            if (glfwGetTime() > endTime) {
                getApp()->changeState("menu");
                gameState = 1;
                return;
            }
        }
    
        auto& keyboard = getApp()->getKeyboard();
        if (keyboard.justPressed(GLFW_KEY_ESCAPE)) {
            getApp()->changeState("menu");
            gameState = 1;
            return;
        }
    }
    
    

    void onDestroy() override {
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraController.exit();
        // Clear the world
        world.clear();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
    }
};