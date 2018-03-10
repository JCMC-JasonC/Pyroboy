#pragma once
#include"Mesh.h"
#include"Enemy.h"
#include "Timer.h"
#include"Trap.h"
#include"ShaderProgram.h"
#include"AnimationMath.h"
#include "Texture.h"
#include "Obstacles.h"
#include"UIObjects.h"
#include"Bullet.h"
#include"Player.h"
#include"Soundheader.h"
#include "LUT.h"
#include "FrameBufferObject.h"
#include"ParticleEmitterSoA.h"
#include<vector>
#include <map>
#include <memory>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 432
#define FRAME_PER_SECOND 60

enum GameStates {
	MAIN_MENU
	,INIT
	,STARTUP
	, PLAYING
	, PAUSE
	, LOSE
	,EXIT
};
class Game {
public:
	Game();
	~Game();

	void initializeGame();
	void startUp();
	void gameOverMenu();
	void gameOverUpdate();
	void startDraw();
	void startUpdate();
	void mainMenu();
	void initUIObjects();
	void uiUpdate();
	void uiDraw();
	void update();
	void draw();
	void switchUIToDraw(GameObject::Ptr, uiType);
	void drawHUD();
	void pauseMenu();
	void pauseUpdate();
	GameStates getState() { return state; }

	//Input callback funct
	void keyboardDown(unsigned char key, int mouseX, int mouseY);
	void keyboardUp(unsigned char key, int mouseX, int mouseY);

	void SpecialInputCallbackFunctionDown(int key, int x, int y);
	void SpecialInputCallbackFunctionUp(int key, int x, int y);
	void mouseClicked(int button, int state, int x, int y);
	void mouseMoved(int x, int y);
	void fixAngle(float angle, glm::vec3 &dir);
	void createBullet(glm::vec3 pos, glm::vec3 dir);
	void TreeWasAttacked(Enemy* _x, glm::vec3 pos);
	float tree_health;
	Timer* updateTimer = nullptr;
	
	std::string file = "textures/LUT.cube";
	LUT lutObj;
	FrameBufferObject buffer;
	ParticleEmitterSoA emitter;

	Mesh treeMesh, playerMesh,backgroundMesh;
	GameObject tree, startupBack, gameOver, background, pauseback, monkey,arrow;

	std::map<std::string, GameObject::Ptr> playerUI, treeUI, otherUI;

	Player player;
	int player_health = 1500;

	glm::vec4 bulletLocation= glm::vec4(0.f);
	Trap trap1, trap2, trap3, trap4, trap5,
		trap6, trap7, trap8, trap9, trap10, trap11, trap12, trap13;
	Mesh  insectMesh, trapMesh, run1,run2,run3,run4,run5,run6,run7,run8,run9,run10,run11,run12,run13,run14,run15,run16,run17,run18,run19
		,run20,run21,run22,run23,run24,run25,run26;
	Mesh bulletMesh, alienMesh, fenceMesh, rockMesh, chestMesh, heartMesh, uiMesh;
	Texture fenceTex, p_healthTex, t_healthTex, uiTex, ui2Tex;

	Obstacles fence, fence2, fence3, fence4, fence5, fence6, fence7, fence8, fence9, fence10, rock, rock2, rock3,
		rock4, rock5, chest, chest2;
	UIGameObjects WeaponSprite;

	GameStates state = GameStates::STARTUP;

	float healthCounter=0.f, bulletTime = 0.f;
	int numTraps = 0, counter, treeHeartCounter;
	Enemy enemy, enemy2, enemy3, enemy4, enemy5, enemy6;
	Enemy insect, insect2, insect3, insect4, insect5, insect6;

	std::vector<Trap*> trap;
	std::vector<Enemy*> enemies;
	std::vector<Obstacles*> fences;
	std::vector<Obstacles*> chests;
	std::vector<Obstacles*> rocks;
	std::vector<UIGameObjects*> hearts;
	std::vector<UIGameObjects*> treeHearts;
	std::vector<Bullet*> bullets;

	std::vector<glm::vec3> positions;
	std::vector<Light> pointLights;
	Light directionalLight;

	ShaderProgram noLight;
	ShaderProgram phongNoTexture;
	ShaderProgram phong;
	ShaderProgram lutShader;
	ShaderProgram geoShader;

	glm::mat4 originalCameraTransform;
	glm::mat4 cameraTransform;
	glm::mat4 cameraProjection;
	glm::mat4 cameraOrtho;
	glm::vec3 cameraEye = glm::vec3(-0.5f, -56.f, -30.f);
	glm::vec3 cameraCtr = glm::vec3(0.f);

	glm::vec3 mousePosition; // x, y, 0
	glm::vec3 mousePositionFlipped; // x, windowHeight - y, 0
	
	glm::vec3 up = glm::vec3(0.f, -1.f, 0.f);
	glm::vec3 down = glm::vec3(0.f, 1.f, 0.f);
	glm::vec3 right = glm::vec3(-1.f, 0.f, 0.f);
	glm::vec3 left = glm::vec3(1.f, 0.f, 0.f);

	bool shooting = false, pause = false, treeDead = false, playerDead = false;

	SoundEngine se;
	FMOD_RESULT      result,blast;
	FMOD::Sound     *sound;
	FMOD::Channel   *channel = 0;
	FMOD_VECTOR soundpos = { 0.0f, 0.0f, 0.0f };
	FMOD_VECTOR soundvel = { 0.0f, 0.0f, 0.0f };

	glm::vec3 a;
	int temp = 0;
private:
	bool shouldRotate = false;
	bool wKeyDown = false;
	bool aKeyDown = false;
	bool sKeyDown = false;
	bool dKeyDown = false;
	bool upKey=false, downKey=false, leftKey=false, rightKey=false;
	bool oKeyDown = false;
	bool pKeyDown = false;
	bool iKeyDown = false;
	bool isUp = false , isDown= false, isLeft = false, isRight = false;

	
	float t=0,dt;
};
