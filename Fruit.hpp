#pragma once

#include "Scene.hpp"
#include "Sound.hpp"

struct Fruit
{
	Fruit(Scene::Drawable& drawable);
	~Fruit() = default;

	Scene::Drawable* drawable;
	void Update(float deltaTime);

    float currentVelocity;
    glm::quat rotationDirection;
	float velocityMultiplier = 1;

	float timeOfInstantiation;
    std::shared_ptr< Sound::PlayingSample > sfx;
};