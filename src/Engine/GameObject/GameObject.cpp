#include "GameObject.hpp"

#include "Component/Component.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "System/Systems.hpp"

GameObject::GameObject() :
    m_allComponents(),
    m_compsBySysT(),
    m_compsByCompT(),
    m_spatialComponent(nullptr)
{}

GameObject::~GameObject() {

}

/* Initialize all components */
void GameObject::init() {
    for (Component * c : m_allComponents) {
        c->init();
    }
}

/* Take in a message and pass it to all components that match the sender's 
 * desired type */
void GameObject::sendMessage(Message & msg) {
    for (Component * c : m_allComponents) {
        // TODO : how does message know which component to send to?
        // if (c != nullptr && c->type == msg->type) {
        //     c->receiveMessage(msg);
        // }
    }
}

const std::vector<Component *> & GameObject::getComponentsBySystem(SystemID sysID) const {
    static const std::vector<Component *> s_emptyList;

    auto it(m_compsBySysT.find(sysID));
    if (it != m_compsBySysT.end()) {
        return it->second;
    }
    return s_emptyList;
}

Component * GameObject::getComponentBySystem(SystemID sysID) {
    auto it(m_compsBySysT.find(sysID));
    if (it != m_compsBySysT.end() && it->second.size()) {
        return it->second.front();
    }
    return nullptr;
}
