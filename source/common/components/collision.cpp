#include "collision.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {

void CollisionComponent::deserialize(const nlohmann::json& data) {
    if (!data.is_object() || !data.contains("size") || !data["size"].is_array() || data["size"].size() != 3) {
        return; 
    }

    auto& size_array = data["size"];
    size.x = size_array[0].get<float>();
    size.y = size_array[1].get<float>();
    size.z = size_array[2].get<float>();

    isTrigger = data.value("isTrigger", isTrigger);
}
CollisionComponent::~CollisionComponent() {
    
}

}