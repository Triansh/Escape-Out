//
// Created by triansh on 25/03/21.
//

#include <iostream>
#include "Camera2D.h"
#include "constants.h"

using namespace std;

Camera2D::Camera2D(glm::vec2 focusPosition, float zoom) {
    this->focusPosition = focusPosition;
    this->zoom = zoom;
}

glm::mat4 Camera2D::GetProjection(glm::vec2 target) {

    focusPosition = target;
    float left = max(0.0f -50, focusPosition.x - SCREEN_WIDTH / 2.0f);
    float right = left + SCREEN_WIDTH;
    float top = max(0.0f - 50, focusPosition.y - SCREEN_HEIGHT / 2.0f);
    float bottom = top + SCREEN_HEIGHT;

    glm::mat4 ortho = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(zoom, zoom, zoom));

    return scale * ortho;
}

void Camera2D::SetZoom(float zoomIn) {
    zoom = zoomIn;
}

