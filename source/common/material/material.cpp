#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our {

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const {
        //TODO: (Req 7) Write this function
        pipelineState.setup();
        shader->use();
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;

        if(data.contains("pipelineState")){
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint 
    void TintedMaterial::setup() const {
        //TODO: (Req 7) Write this function
        Material::setup();
        shader->set("tint", tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex" 
    void TexturedMaterial::setup() const {
        //TODO: (Req 7) Write this function
        TintedMaterial::setup();
        shader->set("alphaThreshold", alphaThreshold);
        texture->bind();
        sampler->bind(0);
        shader->set("texture", texture->getOpenGLName());
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json& data){
        TintedMaterial::deserialize(data);
        if(!data.is_object()) return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

    void lightingMaterial::setup() const {

        TexturedMaterial::setup();

        if (emission != nullptr) {
            glActiveTexture(GL_TEXTURE0);
            this->emission->bind();
            this->sampler->bind(0);
            shader->set("emission", this->emission->getOpenGLName());
        }
        if (ambient_occlusion != nullptr) {
            glActiveTexture(GL_TEXTURE1);
            this->ambient_occlusion->bind();
            this->sampler->bind(1);
            shader->set("ambient_occlusion", this->ambient_occlusion->getOpenGLName());
        }
        if (specular != nullptr) {
            glActiveTexture(GL_TEXTURE2);
            this->specular->bind();
            this->sampler->bind(2);
            shader->set("specular", this->specular->getOpenGLName());
        }
        if (roughness != nullptr) {
            glActiveTexture(GL_TEXTURE3);
            this->roughness->bind();
            this->sampler->bind(3);
            shader->set("roughness", this->roughness->getOpenGLName());
        }
        if (albedo != nullptr) {
            glActiveTexture(GL_TEXTURE4);
            this->albedo->bind();
            this->sampler->bind(4);
            shader->set("albedo", this->albedo->getOpenGLName());
        }

    }

    void lightingMaterial::deserialize(const nlohmann::json& data) {
        TexturedMaterial::deserialize(data);
        if (!data.is_object()) return;
        ambient_occlusion = AssetLoader<Texture2D>::get(data.value("ambient_occlusion", ""));
        specular = AssetLoader<Texture2D>::get(data.value("specular", ""));
        roughness = AssetLoader<Texture2D>::get(data.value("roughness", ""));
        albedo = AssetLoader<Texture2D>::get(data.value("albedo", ""));
        emission = AssetLoader<Texture2D>::get(data.value("emission", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

}