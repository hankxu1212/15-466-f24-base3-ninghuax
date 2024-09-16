#include "Mode.hpp"

#include "Scene.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include "Fruit.hpp"

#define MAX_LEVELS 100

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;
	virtual void Start() override;

	//----- game state -----
	float time = 0;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} space;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	std::unordered_map<std::string, glm::vec3> velocities;

	//camera:
	Scene::Camera *camera = nullptr;

	Scene::Transform *lightTransform = nullptr;

	std::vector<Fruit> fruits;
	void InstantiateFruit(const std::string& meshName, glm::vec3 position);
 
	void DestroyFruit(int index);

	int displayPoints = 0;
	std::string displayText = "0";

	std::string warnText = "Press SPACE to catch fruits!!";

	glm::u8vec4 c_red = glm::u8vec4(0xFF, 0x00, 0x00, 0x00);
	glm::u8vec4 c_green = glm::u8vec4(0x00, 0xFF, 0x00, 0x00);
	glm::u8vec4 c_white = glm::u8vec4(0xFF, 0xFF, 0xFF, 0x00);
	glm::u8vec4 warnTextColor = glm::u8vec4(0xFF, 0xFF, 0xFF, 0x00);

	void HandleSpacePressed();


	enum FruitType : uint8_t
	{		
		BANANA = 0,
		APPLE = 1,
		ORANGE = 2,
		COCONUT = 3
	};

	// spawning
	struct SpawnTimeStamp
	{
		float timeStamp;
		FruitType fruitType;
	};

	int currentSpawnIndex = 0;

	const SpawnTimeStamp spawnPattern[MAX_LEVELS] = 
	{
		{1,BANANA},
		{2,BANANA},
		{4,APPLE},
		{4,COCONUT},
		{5,COCONUT},
		{6,BANANA},
		{9,BANANA},
		{10,ORANGE},
		{13,ORANGE},
		{14,APPLE},
		{15,ORANGE},
		{19,APPLE},
		{23,ORANGE},
		{27,ORANGE},
		{31,ORANGE},
		{32,BANANA},
		{36,COCONUT},
		{37,ORANGE},
		{40,ORANGE},
		{44,COCONUT},
		{48,APPLE},
		{51,ORANGE},
		{54,COCONUT},
		{58,COCONUT},
		{61,COCONUT},
		{61,APPLE},
		{65,BANANA},
		{69,APPLE},
		{73,BANANA},
		{77,ORANGE},
		{79,COCONUT},
		{82,COCONUT},
		{86,COCONUT},
		{88,COCONUT},
		{90,BANANA},
		{92,COCONUT},
		{96,BANANA},
		{100,COCONUT},
		{104,APPLE},
		{107,APPLE},
		{107,COCONUT},
		{110,APPLE},
		{112,ORANGE},
		{115,COCONUT},
		{117,COCONUT},
		{117,ORANGE},
		{119,APPLE},
		{122,APPLE},
		{124,APPLE},
		{125,COCONUT},
		{128,BANANA},
		{132,ORANGE},
		{133,COCONUT},
		{133,BANANA},
		{137,ORANGE},
		{141,BANANA},
		{142,ORANGE},
		{142,BANANA},
		{146,BANANA},
		{148,COCONUT},
		{149,ORANGE},
		{152,APPLE},
		{154,ORANGE},
		{155,ORANGE},
		{157,ORANGE},
		{159,BANANA},
		{159,COCONUT},
		{159,APPLE},
		{160,COCONUT},
		{162,COCONUT},
		{165,ORANGE},
		{168,BANANA},
		{169,COCONUT},
		{169,ORANGE},
		{171,APPLE},
		{175,BANANA},
		{177,APPLE},
		{181,ORANGE},
		{182,COCONUT},
		{183,ORANGE},
		{183,ORANGE},
		{187,BANANA},
		{190,ORANGE},
		{194,ORANGE},
		{195,ORANGE},
		{196,BANANA},
		{198,ORANGE},
		{199,COCONUT},
		{202,ORANGE},
		{204,APPLE},
		{207,BANANA},
		{209,ORANGE},
		{209,APPLE},
		{212,ORANGE},
		{215,APPLE},
		{218,ORANGE},
		{219,BANANA},
		{223,BANANA},
		{226,ORANGE},
		{229,BANANA},
	};

	void UpdateSpawn();
};