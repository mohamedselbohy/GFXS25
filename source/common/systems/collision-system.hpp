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
            // std::cout << a->name << " Pos: " << apos.x << ", " << apos.y << ", " << apos.z << " | Size: "
            //           << asize.x << ", " << asize.y << ", " << asize.z << "\n";
            // std::cout << "player Pos: " << bpos.x << ", " << bpos.y << ", " << bpos.z << " | Size: "
            //           << bsize.x << ", " << bsize.y << ", " << bsize.z << "\n";
            // std::cout << "temp1: " << temp1 << ", temp2: " << temp2 << ", temp3: " << temp3 << "\n";
            // std::cout << "asize: " << asize.x + bsize.x << ", " << asize.y + bsize.y << ", " << asize.z + bsize.z << "\n";

            bool ok = (std::abs(apos.x - bpos.x)*2 < (asize.z + bsize.z)) &&
                      (std::abs(apos.y - bpos.y)*2 < (asize.y + bsize.y)) &&
                      (std::abs(apos.z - bpos.z)*2 < (asize.x + bsize.x));
            // std::cout << "Collision check: " << ok << "\n";
            return ok;
        }

        int update(World *world)
        {
            for (auto &entity : world->getEntities())
            {
                if (entity->hidden || entity == player)
                    continue;

                if (entity->name == "car1")
                {
                    std::string name = entity->name;
                    if (checkAABBCollision(entity, player))
                    {
                        std::cout << "Collision detected with entity: " << name << "\n";
                        player->hidden = true;                     
                    }
                } 
                if(entity->name == "water") {
                    if(checkAABBCollision(entity, player)) {
                        bool checkWoodCollision = false;
                        for(auto &e: world->getEntities()) {
                            if(e->name=="wood"&&checkAABBCollision(e, player)) {checkWoodCollision=true; break;}
                        }
                        if(!checkWoodCollision) player->hidden = true;
                    }
                }
            }
            return 1;
        }
    };
}