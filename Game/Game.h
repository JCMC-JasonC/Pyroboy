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
#include<glm/gtc/type_ptr.hpp>
#include<vector>
#include <map>
#include <memory>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
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
	std::shared_ptr<Mesh> createQuadMesh();
	void blurBrightPass();
	void brightPass();
	void switchUIToDraw(GameObject::Ptr, uiType);
	void drawHUD();
	void pauseMenu();
	void pauseUpdate();
	void initializeParticles();
	void drawFboAttachmentToBackBuffer(FrameBufferObject& fbo, int colorAttachment, glm::vec4 clearColor = glm::vec4(0.0));
	void drawScene();
	void drawSceneWithShadows(ShaderProgram &shader, bool isShadowMap);


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
	FrameBufferObject sceneBuffer, brightPassBuffer, blurBuffer, buffer
		, depthMapFBO;
	ParticleEmitterSoA emitter;

	Mesh treeMesh, playerMesh,backgroundMesh;
	GameObject tree, startupBack, gameOver, background, pauseback, monkey,arrow;

	std::map<std::string, GameObject::Ptr> playerUI, treeUI, otherUI;

	Player player;
	int player_health = 1500;

	glm::vec4 bulletLocation= glm::vec4(0.f);
	Mesh  insectMesh, trapMesh, run1,run2,run3,run4,run5,run6,run7,run8,run9,run10,run11,run12,run13,run14,run15,run16,run17,run18,run19
		,run20,run21,run22,run23,run24,run25,run26;
	Mesh bulletMesh, alienMesh, heartMesh, uiMesh;
	Texture fenceTex, p_healthTex, t_healthTex, uiTex, ui2Tex,smoke;

	GameStates state = GameStates::STARTUP;

	float healthCounter=0.f, bulletTime = 0.f, bloomThreshold= 0.01f;
	int numTraps = 0, counter, treeHeartCounter;
	Enemy enemy, enemy2, enemy3, enemy4, enemy5, enemy6;
	Enemy insect, insect2, insect3, insect4, insect5, insect6;

	std::vector<Enemy*> enemies;
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
	ShaderProgram depthMap;
	ShaderProgram shadows;


	VertexBufferObject vbo;

	std::map<std::string, std::shared_ptr<Texture>> textures;

	// Materials
	std::map<std::string, std::shared_ptr<ShaderProgram>> materials;

	std::map<std::string, std::shared_ptr<Mesh>> meshes;



	Camera camera;
	glm::mat4 emptyMat;
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

	float near_plane = 0.1f, far_plane = 1000.f;
	glm::mat4 lightProjection, lightView, lightSpaceMatrix;

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
