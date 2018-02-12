#include "RenderSystem.hpp"

#include <iostream>

#include "IO/Window.hpp"
#include "ThirdParty/imgui/imgui.h"

RenderSystem::RenderSystem(std::vector<Component *> & components) :
    System(components)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

bool RenderSystem::addShader(const std::string & name, std::unique_ptr<Shader> shader) {
    auto it(m_shaders.find(name));
    if (it != m_shaders.end()) {
        return true;
    }
    if (shader->init()) {
        m_shaders[name] = std::move(shader);
        return true;
    }
    else {
        std::cerr << "Failed to initialize shader [" << name << "]" << std::endl;
        return false;
    }
}

void RenderSystem::update(float dt) {
    /* Reset rendering display */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.4f, 1.f);

    /* Loop through active shaders */
    for (auto &shader : m_shaders) {
        shader.second->bind();
        ///////////////////////////  TODO  ///////////////////////////
        // pass a list of render components that are specific       //
        // to this shader -- right now we are passing the entire    //
        // list and expecting each shader to filter through         //
        //////////////////////////////////////////////////////////////
        shader.second->render(shader.first, m_components);
        shader.second->unbind();
    }

    /* ImGui */
    if (Window::isImGuiEnabled()) {
        ImGui::Render();
    }
}