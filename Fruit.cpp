#include "Fruit.hpp"
#include "Math.h"
#include "Load.hpp"
#include "data_path.hpp"

Load< Sound::Sample > sound1(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("Engine.wav"));
});
Load< Sound::Sample > sound2(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("Engine2.wav"));
});
Load< Sound::Sample > sound3(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("Engine3.wav"));
});
Load< Sound::Sample > sound4(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("Incoming.wav"));
});

Fruit::Fruit(Scene::Drawable &_drawable)
{
    drawable = &_drawable;
    rotationDirection = glm::quat(glm::vec3(0, 0,  Noire::Math::Random(-90, 90)));
    drawable->transform->rotation = rotationDirection;

    float rng = Noire::Math::Random(0, 1);
    if (rng < 0.25){
        sfx = Sound::play_3D(*sound1, 2.0f, drawable->transform->position, 10.0f);
        velocityMultiplier = 2;
    }
    else if (rng < 0.5){
        sfx = Sound::play_3D(*sound2, 2.0f, drawable->transform->position, 10.0f);
        velocityMultiplier = 1;
    }
    else if (rng < 0.75){
        sfx = Sound::play_3D(*sound3, 2.0f, drawable->transform->position, 10.0f);
        velocityMultiplier = 3;
    }
    else{
        sfx = Sound::play_3D(*sound4, 2.0f, drawable->transform->position, 10.0f);
        velocityMultiplier = 4;
    }
        
}

void Fruit::Update(float deltaTime)
{
    currentVelocity += velocityMultiplier * deltaTime;
    drawable->transform->position -= glm::vec3(0, 0, deltaTime * currentVelocity);

    sfx->set_position(drawable->transform->position, 1.0f / 60.0f);
}