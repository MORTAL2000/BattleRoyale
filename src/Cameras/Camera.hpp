/* Parent Camera class
 * This camera can be used as a floating camera with no bounds */
#pragma once
#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include "glm/glm.hpp"
#include "Toolbox/Toolbox.hpp"

#define LOOK_SPEED 0.005f
#define MOVE_SPEED 0.5f

class Camera {
    public:
        /* Position in 3-D world */
        glm::vec3 position;
        /* Position looking at in 3-D world -- located 1u away from position */
        glm::vec3 lookAt;

        /* Constructors */
        Camera(const glm::vec3);
        Camera() : Camera(glm::vec3(0)) { }

        /* Update */
        virtual void update();
        virtual void updateLookAt(const float, const float);

        /* Move according to UVW */
        virtual void moveForward();
        virtual void moveBackward();
        virtual void moveLeft();
        virtual void moveRight();
        virtual void moveUp();
        virtual void moveDown();

    protected:
        /* Used for look at calculation */
        double phi = 0.0;
        double theta = 0.0;
        /* UVW basis vectors */
        glm::vec3 u, v, w;
};

#endif