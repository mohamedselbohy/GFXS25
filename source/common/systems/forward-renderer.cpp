#include "forward-renderer.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"
#include "../material/material.hpp"
#include <glm/glm.hpp>
#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

namespace our
{

    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json &config)
    {
        // First, we store the window size for later use
        this->windowSize = windowSize;

        // Then we check if there is a sky texture in the configuration
        if (config.contains("sky"))
        {
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));

            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram *skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link();

            // TODO: (Req 10) Pick the correct pipeline state to draw the sky
            //  Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            //  We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyPipelineState{};
            skyPipelineState.depthTesting.enabled = true;
            skyPipelineState.depthTesting.function = GL_LEQUAL;
            skyPipelineState.faceCulling.enabled = true;
            skyPipelineState.faceCulling.culledFace = GL_FRONT;
            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");
            Texture2D *skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky
            Sampler *skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if (config.contains("postprocess"))
        {
            // TODO: (Req 11) Create a framebuffer
            glGenFramebuffers(1, &postprocessFrameBuffer);
            // TODO: (Req 11) Create a color and a depth texture and attach them to the framebuffer
            //  Hints: The color format can be (Red, Green, Blue and Alpha components with 8 bits for each channel).
            //  The depth format can be (Depth component with 24 bits).
            colorTarget = new Texture2D();
            colorTarget->bind();
            GLsizei levels = (GLsizei)glm::floor(glm::log2((float)glm::max(windowSize.x, windowSize.y))) + 1;
            glTexStorage2D(GL_TEXTURE_2D, levels, GL_RGBA8, windowSize.x, windowSize.y);
            depthTarget = new Texture2D();
            depthTarget->bind();
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, windowSize.x, windowSize.y);
            glBindFramebuffer(GL_FRAMEBUFFER, postprocessFrameBuffer);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTarget->getOpenGLName(), 0);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getOpenGLName(), 0);
            // TODO: (Req 11) Unbind the framebuffer just to be safe
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler *postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram *postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            postprocessShader->link();

            // Create a post processing material
            postprocessMaterial = new TexturedMaterial();
            postprocessMaterial->shader = postprocessShader;
            postprocessMaterial->texture = colorTarget;
            postprocessMaterial->sampler = postprocessSampler;
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
        }
    }

    void ForwardRenderer::destroy()
    {
        // Delete all objects related to the sky
        if (skyMaterial)
        {
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if (postprocessMaterial)
        {
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
        }
    }

    void ForwardRenderer::render(World *world)
    {
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent *camera = nullptr;
        opaqueCommands.clear();
        transparentCommands.clear();
        lightComponents.clear();
        for (auto entity : world->getEntities())
        {
            if (entity->hidden)
                continue;
            // If we hadn't found a camera yet, we look for a camera in this entity
            if (!camera)
                camera = entity->getComponent<CameraComponent>();
            // If this entity has a mesh renderer component
            if (auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer)
            {
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list
                if (command.material->transparent)
                {
                    transparentCommands.push_back(command);
                }
                else
                {
                    // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }

            // If this entity has a light component
            if (auto light = entity->getComponent<LightComponent>(); light)
            {
                // We add it to the list of lights
                lightComponents.push_back(light);
            }
        }

        // If there is no camera, we return (we cannot render without a camera)
        if (camera == nullptr)
            return;

        // TODO: (Req 9) Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction [Done]
        //  HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
        glm::mat4 camera_local_to_world = camera->getOwner()->getLocalToWorldMatrix();

        glm::vec3 cameraPoint = camera_local_to_world * glm::vec4(0, 0, 0, 1);  // get the camera position with respect to the world
        glm::vec3 worldCenter = camera_local_to_world * glm::vec4(0, 0, -1, 1); // get world center value

        glm::vec3 cameraForward = glm::normalize(glm::vec3(camera_local_to_world[2]));
        std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand &first, const RenderCommand &second)
                  {
            //TODO: (Req 9) Finish this function [Done]
            // HINT: the following return should return true "first" should be drawn before "second".
            float distance_to_first = glm::dot(first.center, cameraForward);
            float distance_to_second = glm::dot(second.center, cameraForward);
            return distance_to_first < distance_to_second; });

        // TODO: (Req 9) Get the camera ViewProjection matrix and store it in VP [Done]
        glm::mat4 VP = camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();

        // TODO: (Req 9) Set the OpenGL viewport using viewportStart and viewportSize [Done]
        //  HINT: The viewportStart is the top left corner of the window and the viewportSize is the size of the window
        //  HINT: You can use the function getFrameBufferSize to get the size of the window
        glViewport(0, 0, windowSize.x, windowSize.y);

        // TODO: (Req 9) Set the clear color to black and the clear depth to 1 [Done]
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClearDepth(1.0);

        // TODO: (Req 9) Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer) [Done]
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);

        // If there is a postprocess material, bind the framebuffer
        if (postprocessMaterial)
        {
            // TODO: (Req 11) bind the framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
        }

        // TODO: (Req 9) Clear the color and depth buffers [Done]
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: (Req 9) Draw all the opaque commands [Done]
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (const auto &command : opaqueCommands)
        {
            command.material->setup();
            if (auto light_material = dynamic_cast<lightingMaterial *>(command.material); light_material)
            {
                command.material->shader->set("camera_position", cameraPoint);
                command.material->shader->set("VP", VP);
                command.material->shader->set("M", camera_local_to_world);
                command.material->shader->set("light_count", (int)lightComponents.size());
                command.material->shader->set("M_IT", glm::transpose(glm::inverse(command.localToWorld)));

                int i = 0;
                for (auto light : lightComponents)
                {
                    glm::mat4 position = light->getOwner()->getLocalToWorldMatrix();
                    glm::vec3 lightposition = position * glm::vec4(light->position, 1.0);
                    glm::vec3 lightdirection = glm::vec4(light->direction, 0.0);
                    light_material->shader->set("lights[" + std::to_string(i) + "].Typed", (int)light->lightType);
                    light_material->shader->set("lights[" + std::to_string(i) + "].color", light->color);
                    light_material->shader->set("lights[" + std::to_string(i) + "].specular", light->specular);

                    if (light->lightType == 0) // directional light
                    {
                        light_material->shader->set("lights[" + std::to_string(i) + "].direction", lightdirection);
                    }
                    else if (light->lightType == 1) // point light
                    {
                        light_material->shader->set("lights[" + std::to_string(i) + "].position", lightposition);
                        light_material->shader->set("lights[" + std::to_string(i) + "].attenuation", light->attenuation);
                    }
                    else
                    {
                        light_material->shader->set("lights[" + std::to_string(i) + "].position", lightposition);
                        light_material->shader->set("lights[" + std::to_string(i) + "].attenuation", light->attenuation);
                        light_material->shader->set("lights[" + std::to_string(i) + "].direction", lightdirection);
                        light_material->shader->set("lights[" + std::to_string(i) + "].cone_angles", light->cone_angles);
                    }
                    i++;
                }
            }
            else
            {
                glm::mat4 MVP = VP * command.localToWorld;
                command.material->shader->set("transform", MVP);
            }

            command.mesh->draw();
        }

        // If there is a sky material, draw the sky
        if (this->skyMaterial)
        {
            // TODO: (Req 10) setup the sky material
            this->skyMaterial->setup();
            // TODO: (Req 10) Get the camera position
            glm::vec3 cameraPosition = camera->getOwner()->localTransform.position;
            // TODO: (Req 10) Create a model matrix for the sy such that it always follows the camera (sky sphere center = camera pssosition)
            our::Transform skyTransformation;
            skyTransformation.position = cameraPosition;
            glm::mat4 skyModelMatrix = skyTransformation.toMat4();

            // TODO: (Req 10) We want the sky to be drawn behind everything (in NDC space, z=1)
            //  We can acheive the is by multiplying by an extra matrix after the projection but what values should we put in it?
            glm::mat4 alwaysBehindTransform = glm::mat4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 1.0f);
            // By converting z to w which is 1. we get this matrix.
            // 1   0   0   0
            // 0   1   0   0
            // 0   0   0   1
            // 0   0   0   1
            //
            // TODO: (Req 10) set the "transform" uniform
            glm::mat4 netTransform = alwaysBehindTransform * VP * skyModelMatrix;
            this->skyMaterial->shader->set("transform", netTransform);
            // TODO: (Req 10) draw the sky sphere
            this->skySphere->draw();
        }
        // TODO: (Req 9) Draw all the transparent commands [Done]
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (auto &command : transparentCommands)
        {
            command.material->setup();

            if (auto light_material = dynamic_cast<lightingMaterial *>(command.material); light_material)
            {
                // glm::vec3 sky_top = glm::vec3(0.01f, 0.01f, 0.01f);
                // glm::vec3 sky_middle = glm::vec3(0.01f, 0.01f, 0.01f);
                // glm::vec3 sky_bottom = glm::vec3(0.01f, 0.01f, 0.01f);
                // light_material->shader->set("sky.top", sky_top);
                // light_material->shader->set("sky.middle", sky_middle);
                // light_material->shader->set("sky.bottom", sky_bottom);

                command.material->shader->set("camera_postion", cameraPoint);
                command.material->shader->set("VP", VP);
                command.material->shader->set("M", command.localToWorld);
                command.material->shader->set("light_count", static_cast<int>(lightComponents.size()));
                glm::mat4 M_I = glm::inverse(command.localToWorld);
                glUniformMatrix4fv(command.material->shader->getUniformLocation("M_IT"), 1, GL_TRUE, &M_I[0][0]);
                int i = 0;
                for (auto light : lightComponents)
                {
                    auto position = light->getOwner()->getLocalToWorldMatrix();
                    glm::vec3 lightposition = glm::vec3(position * glm::vec4(light->position, 1.0f));
                    glm::vec3 lightdirection = lightposition - cameraPoint;
                    light_material->shader->set("lights[" + std::to_string(i) + "].type", (int)light->lightType);   
                    light_material->shader->set("lights[" + std::to_string(i) + "].color", light->color);
                    light_material->shader->set("lights[" + std::to_string(i) + "].specular", light->specular);

                    if (light->lightType == 0) // directional light
                    {
                        light_material->shader->set("lights[" + std::to_string(i) + "].direction", lightdirection);
                    }
                    else if (light->lightType == 1) // point light
                    {
                        light_material->shader->set("lights[" + std::to_string(i) + "].position", lightposition);
                        light_material->shader->set("lights[" + std::to_string(i) + "].attenuation", light->attenuation);
                    }
                    else
                    {
                        light_material->shader->set("lights[" + std::to_string(i) + "].position", lightposition);
                        light_material->shader->set("lights[" + std::to_string(i) + "].attenuation", light->attenuation);
                        light_material->shader->set("lights[" + std::to_string(i) + "].direction", lightdirection);
                        light_material->shader->set("lights[" + std::to_string(i) + "].cone_angles", light->cone_angles);
                    }
                }
            }
            else
            {
                glm::mat4 transform = VP * command.localToWorld;
                command.material->shader->set("transform", transform);
            }
            command.mesh->draw();
        }

        // If there is a postprocess material, apply postprocessing
        if (postprocessMaterial)
        {
            // TODO: (Req 11) Return to the default framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // TODO: (Req 11) Setup the postprocess material and draw the fullscreen triangle
            postprocessMaterial->setup();
            glBindVertexArray(postProcessVertexArray);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }

}