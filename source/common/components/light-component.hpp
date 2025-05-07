#pragma once

#include "../ecs/component.hpp"
#include <glm/glm.hpp>

namespace our
{

    // enum LightType {
    //     DIRECTIONAL,
    //     POINT,
    //     SPOT
    // };

    // This component denotes that the LightComponent will be used to render the owning entity as a light source.
    // This component is added as a simple example for how use the ECS framework to implement logic.
    // For more information, see "common/systems/light.hpp"
    class LightComponent : public Component
    {
    public:
        int lightType = 0; // 0: directional, 1: point, 2: spot
        glm::vec3 color = glm::vec3(0, 0, 0);
        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 direction = glm::vec3(0, 0, 0);
        glm::vec3 specular = glm::vec3(0, 0, 0);
        glm::vec2 cone_angles = glm::vec2(0.0f, 0.0f);
        glm::vec3 attenuation = glm::vec3(0.0f, 0.0f, 0.0f);

        static std::string getID() { return "lighting"; }
        // Reads color & intensity from the given json object
        void deserialize(const nlohmann::json &data) override;
    };

}