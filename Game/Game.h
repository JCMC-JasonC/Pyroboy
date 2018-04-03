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
#include <queue>
#include<imgui\imgui_impl.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1020
#define FRAME_PER_SECOND 60

enum GameStates {
	MAIN_MENU
	,INIT
	,STARTUP
	, PLAYING
	, PAUSE
	, LOSE
	,EXIT
	,TUTORIAL
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
	void updateAlerts();
	void drawHUD();
	void pauseMenu();
	void pauseUpdate();
	void initializeParticles();
	void drawFboAttachmentToBackBuffer(FrameBufferObject& fbo, int colorAttachment, glm::vec4 clearColor = glm::vec4(0.0));
	void drawScene();
	void drawSceneWithShadows(ShaderProgram &shader, bool isShadowMap);
	void drawHealth();


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

	bool enemy1 = false, test = true;
	float half_treehealth = 1500.f;

	int tree_start_health = 3000;
	int player_start_health = 1500;
	float player_health;
	float tree_health;
	bool bulletsound = false;
	Timer* updateTimer = nullptr;
	
	std::string file = "textures/LUT.cube";
	LUT lutObj;
	FrameBufferObject sceneBuffer, brightPassBuffer, blurBuffer, buffer
		, depthMapFBO;
	ParticleEmitterSoA emitter;

	Mesh treeMesh, playerMesh,backgroundMesh;
	GameObject tree, startupBack, gameOver, ash, barrel, bridge, brokentower, cactus, fseast, fsnortheast, fssoutheast,
		fwood, gate, hill, hroof, hsand, hwalls, hwindows, hwood, hpath1, hpath2, pipe, plane, planegrass, planeroad,
		rocks, torch, water, watertower, wirepole, lamp, background, pauseback, arrow;

	std::map<std::string, GameObject::Ptr> playerUI, treeUI, otherUI;

	Player player;

	glm::vec4 bulletLocation= glm::vec4(0.f);
	Mesh  insectMesh, trapMesh, run1,run2,run3,run4,run5,run6,run7,run8,run9,run10,run11,run12,run13,run14,run15,run16,run17,run18,run19
		,run20,run21,run22,run23,run24,run25,run26;

	// Enemy running meshes :<
	Mesh e_run1, e_run2, e_run3, e_run4, e_run5, e_run6, e_run7, e_run8, e_run9, e_run10, e_run11, e_run12, e_run13, e_run14, e_run15,
		e_run16, e_run17, e_run18, e_run19, e_run20, e_run21, e_run22, e_run23, e_run24, e_run25, e_run26, e_run27, e_run28,
		e_run29, e_run30, e_run31, e_run32, e_run33, e_run34, e_run35, e_run36, e_run37, e_run38, e_run39;

	// Enemy attack	 meshes :<
	Mesh e_attack1, e_attack2, e_attack3, e_attack4, e_attack5, e_attack6, e_attack7, e_attack8, e_attack9, e_attack10, e_attack11, e_attack12, 
		e_attack13, e_attack14, e_attack15, e_attack16, e_attack17, e_attack18, e_attack19, e_attack20, e_attack21, e_attack22, e_attack23,
		e_attack24, e_attack25, e_attack26, e_attack27, e_attack28, e_attack29, e_attack30, e_attack31, e_attack32, e_attack33, e_attack34, 
		e_attack35, e_attack36, e_attack37, e_attack38;

	Mesh bulletMesh, alienMesh, heartMesh, uiMesh;
	Texture fenceTex, p_healthTex, t_healthTex, uiTex, ui2Tex,smoke;

	GameStates state = GameStates::STARTUP;

	float healthCounter=0.f, bulletTime = 0.f, bloomThreshold= 0.01f;
	int numTraps = 0, counter, treeHeartCounter;
	int pass = 300;

	float alertTimer = 0.f;

	// End me
	Enemy enemy, enemy2, enemy3, enemy4, enemy5, enemy6, enemy7, enemy8, enemy9, enemy10, enemy11, enemy12, enemy13, enemy14;
	Enemy enemy15, enemy16, enemy17, enemy18, enemy19, enemy20, enemy21, enemy22, enemy23, enemy24, enemy25, enemy26, enemy27, enemy28;

	Enemy insect, insect2, insect3, insect4, insect5, insect6, insect7, insect8, insect9, insect10, insect11, insect12, insect13, insect14;
	Enemy insect15, insect16, insect17, insect18, insect19, insect20, insect21, insect22, insect23, insect24, insect25, insect26, insect27, insect28;

	std::vector<Enemy*> enemies;
	std::vector<UIGameObjects*> hearts;
	std::vector<UIGameObjects*> treeHearts;
	std::vector<Bullet*> bullets;

	std::queue<Bullet*> bulletQ;
	Bullet* tempBullet;

	std::vector<glm::vec3> positions;
	std::vector<Light> pointLights;
	Light directionalLight, originalP1, originalD;

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
	bool treeDamaged = false, playerDamaged = false;
	bool debug = false, light= true;

	bool curPUIShouldDraw = true; // current player ui should draw
	bool curTUIShouldDraw = true;

	SoundEngine se;
	FMOD_RESULT      dialogueresult, menu, result, attack, blast, scroll, selection, start1res, start2res, enemy1result, enemy2result, enemy3result, treefeedback1result, treefeedback2result;
	FMOD::Sound     *dialoguesound, *sound, *menumusic, *game, *scrollsound, *selectsound, *shot, *attacksound, *start1sound, *start2sound, *enemydialogue1, *enemydialogue2, *enemydialogue3, *treesound1, *treesound2;
	FMOD::Channel  *dialoguechannel = 0, *menuchan = 0, *channel = 0, *menusounds = 0, *shotchannel = 0, *enemychannel = 0;
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
	float  st, mt, bt, t1, t2, t3, t4;
};
