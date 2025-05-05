#pragma once

#include "../ecs/component.hpp"
#include <glm/glm.hpp>

namespace our {

    
    class CollisionComponent : public Component {
    public:
        glm::vec3 size = {1.0f, 1.0f, 1.0f}; 
        bool isTrigger = false;              

        static std::string getID() { return "Collision"; }

        void deserialize(const nlohmann::json& data) override;
        ~CollisionComponent();
    };

}