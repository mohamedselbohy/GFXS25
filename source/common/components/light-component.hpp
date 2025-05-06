#pragma once

#include "../ecs/component.hpp"
#include <glm/glm.hpp>


namespace our {

    enum LightType {
        DIRECTIONAL,
        POINT,
        SPOT
    };

    // This component denotes that the LightComponent will be used to render the owning entity as a light source.
    // This component is added as a simple example for how use the ECS framework to implement logic.
    // For more information, see "common/systems/light.hpp"
    class LightComponent : public Component {
    public:

        // light type: 0: directional, 1: point, 2: spot
        LightType type = DIRECTIONAL; // The type of the light (DIRECTIONAL, POINT, SPOT)
        glm::vec3 color = {1, 1, 1}; // The color of the light
        float intensity = 1.0f; // The intensity of the light

        glm::vec3 position = {0, 0, 0}; // The position of the light (only for point and spot lights)
        glm::vec3 direction = {0, 0, -1}; // The direction of the light (only for directional and spot lights)

        float cutOff = 0.0f; // The cutoff angle of the light (only for spot lights)
        float outerCutOff = 0.0f; // The outer cutoff angle of the light (only for spot lights)

        float constant = 1.0f; // The constant attenuation factor (only for point lights)
        float linear = 0.09f; // The linear attenuation factor (only for point lights)
        float quadratic = 0.032f; // The quadratic attenuation factor (only for point lights)
        float range = 100.0f; // The range of the light (only for point lights)


        // The ID of this component type is "Light"
        static std::string getID() { return "Light"; }

        // Reads color & intensity from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}