#include "light-component.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"
#include "../asset-loader.hpp"


namespace our {
    // Reads color & intensity from the given json object
    void LightComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;


        // Deserialize the light type
        std::string typeStr = data.value("lightType", "DIRECTIONAL");
        if (typeStr == "DIRECTIONAL") {
            type = DIRECTIONAL;
        } else if (typeStr == "POINT") {
            type = POINT;
        } else if (typeStr == "SPOT") {
            type = SPOT;
        } else {
            throw std::runtime_error("Invalid light type: " + typeStr);
        }
        
        color = data.value("color", color);
        intensity = data.value("intensity", intensity);
        
        // Deserialize the position, direction, cutOff, outerCutOff, constant, linear, quadratic, and range
        position = data.value("position", position);
        direction = data.value("direction", direction);
        
        cutOff = data.value("cutOff", cutOff);
        outerCutOff = data.value("outerCutOff", outerCutOff);
        
        constant = data.value("constant", constant);
        linear = data.value("linear", linear);
        quadratic = data.value("quadratic", quadratic);
        range = data.value("range", range);
        
    }
}