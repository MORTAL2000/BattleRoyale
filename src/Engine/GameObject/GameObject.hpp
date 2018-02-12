/* Core game object 
 * Maintains a list of components and a system for messaging */
#pragma once
#ifndef _GAME_OBJECT_HPP_
#define _GAME_OBJECT_HPP_

#include <vector>

#include "Message.hpp"

class Component;
class SpatialComponent;

class GameObject {

    public:

        GameObject();
        ~GameObject();

        void init();

        template <typename T>
        T * getComponent();

        template <typename T>
        int componentCount() const;

        void addComponent(Component &);

        void sendMessage(Message &);

        const std::vector<Component *> & getComponents() const { return components; }

        int numComponents() const { return int(components.size()); }

        SpatialComponent * getSpatial() { return spatialComponent; }
        const SpatialComponent * getSpatial() const { return spatialComponent; }

    private:

        std::vector<Component *> components;
        SpatialComponent * spatialComponent;
};



// TEMPLATE IMPLEMENTATION /////////////////////////////////////////////////////



template <typename T>
T * GameObject::getComponent() {
    T * comp;
    for (auto c : components) {
        if (dynamic_cast<T *>(c)) {
            return static_cast<T *>(comp);
        }
    }
    return nullptr;
}

template <typename T>
int GameObject::componentCount() const {
    int n(0);
    for (auto c : components) {
        if (dynamic_cast<T *>(c)) {
            ++n;
        }
    }
    return n;
}



#endif