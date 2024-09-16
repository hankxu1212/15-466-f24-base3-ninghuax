#include "Fruit.hpp"
#include "Math.h"
#include "Load.hpp"
#include "data_path.hpp"

Load< Sound::Sample > hankNoise(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("Noise.wav"));
});

Fruit::Fruit(Scene::Drawable &_drawable)
{
    drawable = &_drawable;
    rotationDirection = glm::quat(glm::vec3(0, 0,  Noire::Math::Random(-90, 90)));
    drawable->transform->rotation = rotationDirection;
    sfx = Sound::play_3D(*hankNoise, 1.0f, drawable->transform->position, 10.0f);
}

void Fruit::Update(float deltaTime)
{
    currentVelocity += 3 * deltaTime;
    drawable->transform->position -= glm::vec3(0, 0, deltaTime * currentVelocity);

    sfx->set_position(drawable->transform->position, 1.0f / 60.0f);
}