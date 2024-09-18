#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "Math.h"
#include "Sound.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

const std::string FRUITS[4] =  { "Banana", "Apple", "Orange", "Coconut" }; 

GLuint myscene_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > myscene_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("myscene.pnct"));
	myscene_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Sound::Sample > ambSFX(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("Background.wav"));
});

Load< Sound::Sample > successSFX(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("TargetHit.wav"));
});
Load< Sound::Sample > failureSFX(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("TargetMiss.wav"));
});

Load< Scene > myscene_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("myscene.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = myscene_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = myscene_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

		drawable.fruitIndex = std::nullopt;
	});
});

void PlayMode::InstantiateFruit(const std::string& meshName, glm::vec3 position)
{
	Mesh const &mesh = myscene_meshes->lookup(meshName);
	
	Scene::Transform* newTransform = new Scene::Transform();
	newTransform->position = position;

	scene.drawables.emplace_back(newTransform);
	Scene::Drawable &drawable = scene.drawables.back();

	drawable.pipeline = lit_color_texture_program_pipeline;

	drawable.pipeline.vao = myscene_meshes_for_lit_color_texture_program;
	drawable.pipeline.type = mesh.type;
	drawable.pipeline.start = mesh.start;
	drawable.pipeline.count = mesh.count;

	int index = (int)fruits.size();
	fruits.emplace_back(drawable);

	drawable.fruitIndex = index;
	drawable.cleanUpTransform = true;

	fruits.back().timeOfInstantiation = time;
}

void PlayMode::DestroyFruit(int index)
{
	assert(index >= 0 && index < fruits.size());

	// remove from fruits list
	fruits.erase(fruits.begin() + index);
	
	// remove drawable
	scene.drawables.remove_if([&] (Scene::Drawable& d){ return d.fruitIndex.has_value() && d.fruitIndex.value() == index; });

	// make the rest of the fruits->drawables point to the right index
	for (int i=index; i < fruits.size(); i++)
	{
		assert(fruits[i].drawable->fruitIndex.has_value());
		fruits[i].drawable->fruitIndex = i;
	}
}

PlayMode::PlayMode() : scene(*myscene_scene) {
	//get pointers to leg for convenience:
	for (auto &transform : scene.transforms) {
		if (transform.name == "Sun")
			lightTransform = &transform;
		if (transform.name == "Bowl")
			std::cout << "FOUND BOWL TRANSFORM: " << transform.position.x << " " << transform.position.y << " " << transform.position.z << '\n';
	}

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

	Sound::loop_3D(*ambSFX, 1.0f, lightTransform->position, 10.0f);

	for (int i=0;i<MAX_LEVELS;++i)
		spawnPattern[i].timeStamp *= 2;
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) 
{
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_SPACE) {
			if (space.pressed)
				return true;
			HandleSpacePressed();
			space.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP)
	{
		if (evt.key.keysym.sym == SDLK_SPACE) {
			space.pressed = false;
			return true;
		} 
	}
	return false;
}

bool pairCMP (std::pair<float, int> i, std::pair<float, int> j) { return (i.first<j.first); }

void PlayMode::HandleSpacePressed()
{
	if (fruits.size() == 0)
		return;

	std::vector<std::pair<float, int>> distances;

	int i = 0;
	for(auto& fruit : fruits)
	{
		float d = fruit.drawable->transform->position.z + 2.059f;
		if (d >= 0)
			distances.emplace_back(std::make_pair(d, i));
		
		i++;
	}

	if (distances.size() == 0)
		return;

	std::sort(distances.begin(), distances.end(), pairCMP);

	std::cout << distances[0].first << "\n";

	if (distances[0].first > 0.5f)
	{
		warnText = "Too Early!";
		warnTextColor = c_red;
		
		Sound::play_3D(*failureSFX, 1.0f, glm::vec3(0, 0, -2), 10.0f);
	}
	else
	{
		warnText = "Perfect!";
		warnTextColor = c_green;

		displayPoints++;
		displayText = std::to_string(displayPoints);
		DestroyFruit(distances[0].second);

		Sound::play_3D(*successSFX, 1.0f, glm::vec3(0, 0, -2), 10.0f);

	}
	fruits[distances[0].second].canTriggerWarning = false;
}

void PlayMode::UpdateSpawn()
{
	if (currentSpawnIndex >= MAX_LEVELS)
		return;
	
	if (time > spawnPattern[currentSpawnIndex].timeStamp)
	{
		InstantiateFruit(FRUITS[spawnPattern[currentSpawnIndex].fruitType], glm::vec3(0, 0, 7));
		currentSpawnIndex++;
	}
}

void PlayMode::update(float elapsed) 
{
	time += elapsed;

	UpdateSpawn();

	for(auto& fruit : fruits)
	{
		fruit.Update(elapsed);
		if (fruit.drawable->transform->position.z < -4)
		{
			if (fruit.canTriggerWarning){
				std::cout << fruit.drawable->transform->position.z << " FOUND Z\n";
				warnText = "Too Slow!";
				warnTextColor = c_white;

				Sound::play_3D(*failureSFX, 1.0f, glm::vec3(0, 0, -2), 10.0f);
			}

			DestroyFruit(fruit.drawable->fruitIndex.value());
			displayPoints--;
			displayText = std::to_string(displayPoints);
		}
	}
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 2);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(lightTransform->rotation));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(2, 2,2)));
	glUseProgram(0);

	glClearColor(0, 0, 0, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	GL_ERRORS(); //print any errors produced by this setup code

	scene.draw(*camera);

	Sound::listener.set_position_right(glm::vec3(0,0,-2), glm::vec3(0,0,1), 1.0f / 60.0f);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.5f;
		lines.draw_text(displayText,
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text(displayText,
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));

		constexpr float H2 = 0.2f;
		lines.draw_text(warnText,
			glm::vec3(-aspect + 5 * H2, -1.0 + 2 * H2, 0.0),
			glm::vec3(H2, 0.0f, 0.0f), glm::vec3(0.0f, H2, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		lines.draw_text(warnText,
			glm::vec3(-aspect + 5 * H2 + ofs, -1.0 + 2 * H2 + ofs, 0.0),
			glm::vec3(H2, 0.0f, 0.0f), glm::vec3(0.0f, H2, 0.0f),
			warnTextColor);
	}
}

void PlayMode::Start()
{
}