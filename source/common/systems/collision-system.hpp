#pragma once
#include "../ecs/world.hpp"
#include "../components/movement.hpp"
#include "../components/mesh-renderer.hpp"
#include "../application.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <iostream>
#include <vector>
#include <string>

namespace our
{
    class CollisionSystem
    {
        Entity *player;

    public:
        void setPlayer(Entity *playerEntity)
        {
            this->player = playerEntity;
        }

        bool checkAABBCollision(Entity *a, Entity *b)
        {
            glm::vec3 apos = a->localTransform.position;
            glm::vec3 asize = a->size;
            glm::vec3 bpos = b->localTransform.position;
            glm::vec3 bsize = b->size;
            std::string name = (a->name != "") ? a->name : "nullptr";
            if (name == "nullptr")
                return 0;
            bool ok = (std::abs(apos.x - bpos.x)*2 < (asize.z + bsize.z)) &&
                      (std::abs(apos.y - bpos.y)*2 < (asize.y + bsize.y)) &&
                      (std::abs(apos.z - bpos.z)*2 < (asize.x + bsize.x));
            return ok;
        }

        int update(World *world, std::vector<Entity*> woods)
        {
            long long count = 0;
            for (auto &entity : world->getEntities())
            {
                if (entity->hidden || entity == player)
                    continue;

                if (entity->name == "car1")
                {
                    std::string name = entity->name;
                    if (checkAABBCollision(entity, player))
                    {
                        // std::cout << "Collision detected with carrrrrrr: " << name << "\n";
                        return 0;                   
                    }
                } 
                glm::vec3 ppos = player->localTransform.position;
                if((ppos.z<16.5&&ppos.z>6.5) || (ppos.z<-33.5&&ppos.z>-43.5)) {
                    bool checkWoodCollision = false;
                    for(auto &wood: woods) {
                        if(checkAABBCollision(wood, player)) {
                            player->getComponent<MovementComponent>()->linearVelocity = {5, 0, 0};
                            // std::cout<<"WOOD"<<'\n';
                            checkWoodCollision=true; 
                            break;
                        }
                        else {
                            player->getComponent<MovementComponent>()->linearVelocity = {0, 0, 0};
                        }
                    }
                    if(!checkWoodCollision) return 0;
                } else {
                    player->getComponent<MovementComponent>()->linearVelocity = {0, 0, 0};
                }
                if(entity->name == "star" && checkAABBCollision(entity, player)) {
                    // Winning 
                    return 2;
                }
            }
            return 1;
        }
    };
}