#include "Game.h"
#include<iostream>
#include<GL/glut.h>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include<random>
#include<ctime>

Light LightFollowPlayer;

Game::Game()
{

}
Game::~Game()
{
	delete updateTimer;

	noLight.unload();
	phongNoTexture.unload();
	phong.unload();

	delete[]&enemies;
	delete[] &bullets;
}
void Game::gameOverUpdate()
{
	updateTimer->tick();

	float deltaTime = updateTimer->getElapsedTimeSeconds();

	deltaTime = glm::clamp(deltaTime, 0.0f, 0.033f);
	dt += deltaTime;

	gameOver.transform = gameOver.translate * gameOver.rotate * glm::scale(glm::mat4(), glm::vec3(gameOver.scale*2.f, gameOver.scale, gameOver.scale));
}
void Game::gameOverMenu()
{
	glClearColor(0.5, 0.5, 0.5, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gameOver.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);

	glutSwapBuffers();
}
void Game::pauseUpdate()
{
	updateTimer->tick();

	float deltaTime = updateTimer->getElapsedTimeSeconds();

	deltaTime = glm::clamp(deltaTime, 0.0f, 0.033f);
	dt += deltaTime;

	pauseback.transform = pauseback.translate * pauseback.rotate * glm::scale(glm::mat4(), glm::vec3(pauseback.scale*2.f, pauseback.scale, pauseback.scale));
}
void Game::pauseMenu()
{
	glClearColor(0.5, 0.5, 0.5, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pauseback.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);

	glutSwapBuffers();
}
void Game::startUp()
{
	updateTimer = new Timer();
	glEnable(GL_DEPTH_TEST);
	if (!phong.load("shaders/Phong.vert", "shaders/Phong.frag"))
	{
		std::cout << "Shaders failed to initialize." << std::endl;
		system("pause");
		exit(0);
	}

	meshes["quad"] = createQuadMesh();

	materials["particles"] = std::make_shared<ShaderProgram>();
	materials["particles"]->load("shaders/passThru_v.glsl", "shaders/unlitTexture_f.glsl", "shaders/particles_g.glsl");

	materials["blur"] = std::make_shared<ShaderProgram>();
	materials["blur"]->load("shaders/default_v.glsl", "shaders/gaussianBlur_f.glsl");

	materials["default"] = std::make_shared<ShaderProgram>();
	materials["default"]->load("shaders/default_v.glsl", "shaders/default_f.glsl");

	materials["bright"] = std::make_shared<ShaderProgram>();
	materials["bright"]->load("shaders/default_v.glsl", "shaders/bright_f.glsl");

	materials["bloom"] = std::make_shared<ShaderProgram>();

	materials["bloom"]->load("shaders/default_v.glsl", "shaders/bloomComposite_f.glsl");
	//LIGHTING
	Light light1;

	light1.positionOrDirection = glm::vec4(0.f, 10.f, -1.f, 1.f);
	light1.originalPosition = light1.positionOrDirection;
	light1.ambient = glm::vec3(0.2f);
	light1.diffuse = glm::vec3(0.9f);
	light1.specular = glm::vec3(0.9f);
	light1.specularExponent = 50.f;
	light1.constantAttenuation = 1.f;
	light1.linearAttenuation = 0.1f;
	light1.quadraticAttenuation = 0.01f;

	pointLights.push_back(light1);

	directionalLight.positionOrDirection = glm::vec4(-1.f, -1.f, 11.f, 0.f);
	directionalLight.ambient = glm::vec3(0.08f);
	directionalLight.diffuse = glm::vec3(0.7f);
	directionalLight.specular - glm::vec3(1.0f);
	directionalLight.specularExponent = 50.f;

	startupBack.loadMesh("meshes/background.obj");
	startupBack.loadTexture(TextureType::Diffuse, "textures/Model Textures/testtitle.png");
	startupBack.loadTexture(TextureType::Specular, "textures/noSpecular.png");
	startupBack.scale = 4.f;

	//startupBack.rotate = glm::rotate(background.rotate, glm::pi<float>() / 2.f, glm::vec3(0.0f, 1.f, 0.f));
	startupBack.rotate = glm::rotate(background.rotate, glm::pi<float>() *1.5f, glm::vec3(1.0f, 0.f, 0.f));
	//startupBack.rotate = glm::rotate(background.rotate, glm::pi<float>() /2.f, glm::vec3(0.0f, 0.f, 1.f));

	cameraTransform = glm::lookAt(glm::vec3(0.f, 0.f, -25.f),
		glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f,-1.f,0.f));
	originalCameraTransform = cameraTransform;

	cameraProjection = glm::perspective(90.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.f);
	glViewport(0, 0, 20.f, 20.f);

	//state = GameStates::MAIN_MENU;
}
void Game::startDraw()
{

	glClearColor(0.5, 0.5, 0.5, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	startupBack.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	//brightPass(); // Implement this function!
	//blurBrightPass(); // Implement this function!

	//sceneBuffer.bindTextureForSampling(0, GL_TEXTURE1);
	//blurBuffer.bindTextureForSampling(0, GL_TEXTURE0);


	////FrameBufferObject::unbindFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
	////FrameBufferObject::clearFrameBuffer(glm::vec4(0));
	//materials["bloom"]->bind();
	//materials["bloom"]->sendUniformMat4("u_mvp", glm::value_ptr(emptyMat), false);

	//meshes["quad"]->draw();

	////////////////////////////////////////////////////////////////////////////
	//// UNBIND TEXTURES
	////////////////////////////////////////////////////////////////////////////
	//sceneBuffer.unbindTexture(GL_TEXTURE1);
	//blurBuffer.unbindTexture(GL_TEXTURE0);
	
	glutSwapBuffers();
}
void Game::startUpdate()
{
	updateTimer->tick();

	float deltaTime = updateTimer->getElapsedTimeSeconds();

	deltaTime = glm::clamp(deltaTime, 0.0f, 0.033f);
	dt += deltaTime;

	if (dt >= 1.f)
		state = GameStates::INIT;

	startupBack.transform = startupBack.translate * startupBack.rotate * glm::scale(glm::mat4(), glm::vec3(startupBack.scale*1.5f, startupBack.scale, startupBack.scale*0.60f));
	
}
void Game::mainMenu()
{
	updateTimer = new Timer();
	glEnable(GL_DEPTH_TEST);

	if (!noLight.load("shaders/noLight.vert", "shaders/noLight.frag"))
	{
		std::cout << "Shaders failed to initialize." << std::endl;
		system("pause");
		exit(0);
	}
	if (!phongNoTexture.load("shaders/Phong.vert", "shaders/PhongNoTexture.frag"))
	{
		std::cout << "Shaders failed to initialize." << std::endl;
		system("pause");
		exit(0);
	}
	if (!phong.load("shaders/Phong.vert", "shaders/Phong.frag"))
	{
		std::cout << "Shaders failed to initialize." << std::endl;
		system("pause");
		exit(0);
	}
	if (!lutShader.load("shaders/default_v.glsl", "shaders/lut_f.glsl"))
	{
		std::cout << "Shaders failed to initialize." << std::endl;
		system("pause");
		exit(0);
	} 
	monkey.loadMesh("meshes/background.obj");
	monkey.loadTexture(TextureType::Diffuse,"textures/Model Textures/menu.PNG");
	monkey.loadTexture(TextureType::Specular, "textures/noSpecular.png");

	monkey.scale = 4.f;
	monkey.rotate = glm::rotate(monkey.rotate, glm::pi<float>()* 1.5f, glm::vec3(0.0f, 0.f, 1.f));
	monkey.rotate = glm::rotate(monkey.rotate, glm::pi<float>() / 2.f, glm::vec3(1.0f,0.f,0.f));

	arrow.loadMesh("meshes/arrow.obj");
	arrow.loadTexture(TextureType::Diffuse, "textures/Health.png");
	arrow.loadTexture(TextureType::Specular, "textures/noSpecular.png");
	arrow.scale = 1.f;

	arrow.position =glm::vec3(-30.f, -1.f, 0.f);
	arrow.translate = glm::translate(arrow.translate,arrow.position); //(-30f,-1f,0.f), 10,25
	arrow.rotate = glm::rotate(arrow.rotate, glm::pi<float>()*0.f, glm::vec3(0.f,0.f,1.f));
	
	
}
void Game::initializeParticles()
{
	// Init particle emitter
	// Set the emitter properties
	emitter.lifeRange = glm::vec3(1.0f, 2.0f, 0.0f);
	emitter.initialForceMin = glm::vec3(-3.0f, 5.0f, -4.0f);
	emitter.initialForceMax = glm::vec3(4.0f, 5.0f, -10.0f);
	emitter.initialPosition = glm::vec3(93.f, -50.f, -13.f);

	emitter.material = materials["particles"];
	emitter.texture = textures["smoke"];
	emitter.initialize(10000);
	emitter.play();
}

void Game::initUIObjects()
{
	// Player
	std::shared_ptr<GameObject> playerUI10 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> playerUI9 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> playerUI8 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> playerUI7 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> playerUI6 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> playerUI5 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> playerUI4 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> playerUI3 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> playerUI2 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> playerUI1 = std::make_shared<GameObject>();

	// Tree
	std::shared_ptr<GameObject> treeUI10 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> treeUI9 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> treeUI8 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> treeUI7 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> treeUI6 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> treeUI5 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> treeUI4 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> treeUI3 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> treeUI2 = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> treeUI1 = std::make_shared<GameObject>();

	// Tutorial thing
	std::shared_ptr<GameObject> protect = std::make_shared<GameObject>();
	protect->loadMesh("meshes/UI.obj");
	protect->loadTexture(TextureType::Diffuse, "textures/UI/protect.png");
	protect->loadTexture(TextureType::Specular, "textures/noSpecular.png");
	otherUI["protect"] = protect;
	otherUI["protect"]->shouldDraw = true;

	// UI OBJECTS
	playerUI10->loadMesh("meshes/UI.obj");
	playerUI10->loadTexture(TextureType::Diffuse, "textures/UI/newPHP10.png");
	playerUI10->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	playerUI9->loadMesh("meshes/UI.obj");
	playerUI9->loadTexture(TextureType::Diffuse, "textures/UI/newPHP9.png");
	playerUI9->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	playerUI8->loadMesh("meshes/UI.obj");
	playerUI8->loadTexture(TextureType::Diffuse, "textures/UI/newPHP8.png");
	playerUI8->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	playerUI7->loadMesh("meshes/UI.obj");
	playerUI7->loadTexture(TextureType::Diffuse, "textures/UI/newPHP7.png");
	playerUI7->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	playerUI6->loadMesh("meshes/UI.obj");
	playerUI6->loadTexture(TextureType::Diffuse, "textures/UI/newPHP6.png");
	playerUI6->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	playerUI5->loadMesh("meshes/UI.obj");
	playerUI5->loadTexture(TextureType::Diffuse, "textures/UI/newPHP5.png");
	playerUI5->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	playerUI4->loadMesh("meshes/UI.obj");
	playerUI4->loadTexture(TextureType::Diffuse, "textures/UI/newPHP4.png");
	playerUI4->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	playerUI3->loadMesh("meshes/UI.obj");
	playerUI3->loadTexture(TextureType::Diffuse, "textures/UI/newPHP3.png");
	playerUI3->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	playerUI2->loadMesh("meshes/UI.obj");
	playerUI2->loadTexture(TextureType::Diffuse, "textures/UI/newPHP2.png");
	playerUI2->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	playerUI1->loadMesh("meshes/UI.obj");
	playerUI1->loadTexture(TextureType::Diffuse, "textures/UI/newPHP1.png");
	playerUI1->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	// Tree Objects
	treeUI10->loadMesh("meshes/UI.obj");
	treeUI10->loadTexture(TextureType::Diffuse, "textures/UI/newTreeHP10.png");
	treeUI10->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	treeUI9->loadMesh("meshes/UI.obj");
	treeUI9->loadTexture(TextureType::Diffuse, "textures/UI/newTreeHP9.png");
	treeUI9->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	treeUI8->loadMesh("meshes/UI.obj");
	treeUI8->loadTexture(TextureType::Diffuse, "textures/UI/newTreeHP8.png");
	treeUI8->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	treeUI7->loadMesh("meshes/UI.obj");
	treeUI7->loadTexture(TextureType::Diffuse, "textures/UI/newTreeHP7.png");
	treeUI7->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	treeUI6->loadMesh("meshes/UI.obj");
	treeUI6->loadTexture(TextureType::Diffuse, "textures/UI/newTreeHP6.png");
	treeUI6->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	treeUI5->loadMesh("meshes/UI.obj");
	treeUI5->loadTexture(TextureType::Diffuse, "textures/UI/newTreeHP5.png");
	treeUI5->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	treeUI4->loadMesh("meshes/UI.obj");
	treeUI4->loadTexture(TextureType::Diffuse, "textures/UI/newTreeHP4.png");
	treeUI4->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	treeUI3->loadMesh("meshes/UI.obj");
	treeUI3->loadTexture(TextureType::Diffuse, "textures/UI/newTreeHP3.png");
	treeUI3->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	treeUI2->loadMesh("meshes/UI.obj");
	treeUI2->loadTexture(TextureType::Diffuse, "textures/UI/newTreeHP2.png");
	treeUI2->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	treeUI1->loadMesh("meshes/UI.obj");
	treeUI1->loadTexture(TextureType::Diffuse, "textures/UI/newTreeHP1.png");
	treeUI1->loadTexture(TextureType::Specular, "textures/noSpecular.png");
	// 

	//playerUI10->transform = playerUI10->translate * playerUI10->rotate * glm::scale(glm::mat4(), glm::vec3(playerUI10->scale));
	//treeUI10->transform = treeUI10->translate * treeUI10->rotate * glm::scale(glm::mat4(), glm::vec3(treeUI10->scale));

	// map that holds UI objects so we can reference by name
	playerUI["playerHP10"] = playerUI10;
	playerUI["playerHP9"] = playerUI9;
	playerUI["playerHP8"] = playerUI8;
	playerUI["playerHP7"] = playerUI7;
	playerUI["playerHP6"] = playerUI6;
	playerUI["playerHP5"] = playerUI5;
	playerUI["playerHP4"] = playerUI4;
	playerUI["playerHP3"] = playerUI3;
	playerUI["playerHP2"] = playerUI2;
	playerUI["playerHP1"] = playerUI1;

	// Don't draw any until HP is checked
	playerUI["playerHP10"]->shouldDraw = false;
	playerUI["playerHP9"]->shouldDraw = false;
	playerUI["playerHP8"]->shouldDraw = false;
	playerUI["playerHP7"]->shouldDraw = false;
	playerUI["playerHP6"]->shouldDraw = false;
	playerUI["playerHP5"]->shouldDraw = false;
	playerUI["playerHP4"]->shouldDraw = false;
	playerUI["playerHP3"]->shouldDraw = false;
	playerUI["playerHP2"]->shouldDraw = false;
	playerUI["playerHP1"]->shouldDraw = false;

	// map for tree ui objects as well
	treeUI["treeHP10"] = treeUI10;
	treeUI["treeHP9"] = treeUI9;
	treeUI["treeHP8"] = treeUI8;
	treeUI["treeHP7"] = treeUI7;
	treeUI["treeHP6"] = treeUI6;
	treeUI["treeHP5"] = treeUI5;
	treeUI["treeHP4"] = treeUI4;
	treeUI["treeHP3"] = treeUI3;
	treeUI["treeHP2"] = treeUI2;
	treeUI["treeHP1"] = treeUI1;

	treeUI["treeHP10"]->shouldDraw = false;
	treeUI["treeHP9"]->shouldDraw = false;
	treeUI["treeHP8"]->shouldDraw = false;
	treeUI["treeHP7"]->shouldDraw = false;
	treeUI["treeHP6"]->shouldDraw = false;
	treeUI["treeHP5"]->shouldDraw = false;
	treeUI["treeHP4"]->shouldDraw = false;
	treeUI["treeHP3"]->shouldDraw = false;
	treeUI["treeHP2"]->shouldDraw = false;
	treeUI["treeHP1"]->shouldDraw = false;

	// set transform for all ui objects
	for (auto itr = playerUI.begin(); itr != playerUI.end(); itr++)
	{
		itr->second->transform =
		glm::translate(glm::mat4(1.0f), glm::vec3(100.f, float(WINDOW_HEIGHT) - 25.0f, -1.0f))
		* glm::scale(glm::mat4(1.0f), glm::vec3(200.0f, 50.f, 1.f));
	}
	for (auto itr = treeUI.begin(); itr != treeUI.end(); itr++)
	{
		itr->second->transform =
			glm::translate(glm::mat4(1.0f), glm::vec3(float(WINDOW_WIDTH) - 100.0f, float(WINDOW_HEIGHT) - 25.0f, -1.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(200.0f, 50.f, 1.f));
	}
	protect->transform = 
		glm::translate(glm::mat4(1.0f), glm::vec3(float(WINDOW_WIDTH) / 2, float(WINDOW_HEIGHT) / 2, -1.0f))
		* glm::scale(glm::mat4(1.0f), glm::vec3(300.0f, 150.f, 1.f));


	//treeUI["treeHP10"]->transform =
	//	glm::translate(glm::mat4(1.0f), glm::vec3(float(WINDOW_WIDTH) - 75.0f, float(WINDOW_HEIGHT) - 50.0f, -1.0f))
	//	* glm::scale(glm::mat4(1.0f), glm::vec3(150.0f, 100.f, 1.f));
}

std::shared_ptr<Mesh> Game::createQuadMesh()
{
	std::shared_ptr<Mesh> quadMesh = std::make_shared<Mesh>();

	// Triangle 1
	quadMesh->vertices.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
	quadMesh->textureCoordinates.push_back(glm::vec2(1.0f, 1.0f));

	quadMesh->vertices.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));
	quadMesh->textureCoordinates.push_back(glm::vec2(0.0f, 1.0f));

	quadMesh->vertices.push_back(glm::vec3(-1.0, -1.0, 0.0f));
	quadMesh->textureCoordinates.push_back(glm::vec2(0.0f, 0.0f));

	// Triangle 2
	quadMesh->vertices.push_back(glm::vec3(1.0, 1.0, 0.0f));
	quadMesh->textureCoordinates.push_back(glm::vec2(1.0f, 1.0f));

	quadMesh->vertices.push_back(glm::vec3(-1.0, -1.0, 0.0f));
	quadMesh->textureCoordinates.push_back(glm::vec2(0.0f, 0.0f));

	quadMesh->vertices.push_back(glm::vec3(1.0, -1.0, 0.0f));
	quadMesh->textureCoordinates.push_back(glm::vec2(1.0f, 0.0f));

	quadMesh->createVBO();

	return quadMesh;
}
//load everything in here
void Game::brightPass()
{
	//////////////////////////////////////////////////////////////////////////
	// IMPLEMENT BRIGHT PASS HERE
	// - Bind the appropriate shader and the texture that contains the rendered 
	//   scene and render a full screen quad to the appropriate fbo
	////////////////////////////////////////////////////////////////////////// 
	brightPassBuffer.bindFrameBufferForDrawing();

	sceneBuffer.bindTextureForSampling(0, GL_TEXTURE0);


	materials["bright"]->sendUniformMat4("u_mvp", glm::value_ptr(emptyMat),false);
	materials["bright"]->sendUniform("u_bloomThreshold", bloomThreshold);

	materials["bright"]->bind();

	meshes["quad"]->draw();

	sceneBuffer.unbindTexture(GL_TEXTURE0);
}

void Game::blurBrightPass()
{
	//////////////////////////////////////////////////////////////////////////
	// BLUR BRIGHT PASS HERE
	//	- Bind the appropriate shader, the texture that contains the bright pass
	//   and render a full screen quad to the appropriate fbo
	////////////////////////////////////////////////////////////////////////// 

	blurBuffer.bindFrameBufferForDrawing();
	brightPassBuffer.bindTextureForSampling(0, GL_TEXTURE0);

	materials["blur"]->bind();
	materials["blur"]->sendUniformMat4("u_mvp", glm::value_ptr(emptyMat), false);
	materials["blur"]->sendUniform("u_texelSize", glm::vec4(1.0 / (float)blurBuffer.getWidth(), 1.0 / (float)blurBuffer.getHeight(), 0.f, 0.f));

	meshes["quad"]->draw();

	int pass = 300; // pass 30 times
	for (int i = 0; i < pass; i++) {
		buffer.bindFrameBufferForDrawing();
		blurBuffer.bindTextureForSampling(0, GL_TEXTURE0);
		meshes["quad"]->draw();
	}

	blurBuffer.bindFrameBufferForDrawing();
	buffer.bindTextureForSampling(0, GL_TEXTURE0);

	meshes["quad"]->draw();

}
void Game::initializeGame()
{
	//se.Init();
	//result = se.system->createSound("sounds/testmusic.mp3", FMOD_3D, 0, &sound);
	//FmodErrorCheck(result);
	//result = sound->set3DMinMaxDistance(0.0f, 30000.0f);
	//FmodErrorCheck(result);
	//result = sound->setMode(FMOD_LOOP_NORMAL);
	//FmodErrorCheck(result);
	//result = se.system->playSound(sound, 0, false, &channel);
	//FmodErrorCheck(result);

	updateTimer = new Timer();

	glEnable(GL_DEPTH_TEST);

	// LUT bind
	//lutObj.loadLUT(file);
	//lutObj.bind();

	if (!noLight.load("shaders/noLight.vert","shaders/noLight.frag"))
	{
		std::cout << "Shaders failed to initialize."<< std::endl;
		system("pause");
		exit(0);
	}
	if (!phongNoTexture.load("shaders/Phong.vert", "shaders/PhongNoTexture.frag"))
	{
		std::cout << "Shaders failed to initialize." << std::endl;
		system("pause");
		exit(0);
	}
	if (!phong.load("shaders/Phong.vert", "shaders/Phong.frag"))
	{
		std::cout << "Shaders failed to initialize." << std::endl;
		system("pause");
		exit(0);
	}
	if (!lutShader.load("shaders/default_v.glsl", "shaders/lut_f.glsl"))
	{
		std::cout << "Shaders failed to initialize." << std::endl;
		system("pause");
		exit(0);
	}

	emptyMat = glm::mat4();

	// MAKE SURE TO RUN AT x86
	// LOADING MODELS
	// Create an object of type Enemy for any enemies you want to load in in Game.h
	// Create an object of type GameObject for any objects you want to load in Game.h
	// Keep all objs in the meshes folder. Make sure to match the obj with the object you want.
	// Make sure to initialize the position of the objects below where I commented 'POSITIONS'
	// Pushback all the info to every vector.
	// If it works, but looks weird, dont worry. I just need to change code for the rotation for each enemy.
	// Dont need to worry about drawing the enemies, as long as you add it to the vector of enemies.

	textures["smoke"] = std::make_shared<Texture>();
	textures["smoke"]->load("textures/smoke_256_dm.png");
	sceneBuffer.createFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 1, true);
	brightPassBuffer.createFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 1, true); 
	buffer.createFrameBuffer(WINDOW_WIDTH / 16.f, WINDOW_HEIGHT / 16.f, 1, true);
	blurBuffer.createFrameBuffer(WINDOW_WIDTH / 16.f, WINDOW_HEIGHT / 16.f, 1, true);

	initializeParticles();

	tree.loadMesh("meshes/GreenTree.obj");
	tree.loadTexture(TextureType::Diffuse, "textures/Model Textures/theWholeTree.png");
	tree.loadTexture(TextureType::Specular, "textures/noSpecular.png");

	float ar[2];
	ar[0] = 25.f;
	ar[1] = -37.f;
	int x = 0;

	background.loadMesh("meshes/map.obj");
	background.loadTexture(TextureType::Diffuse, "textures/Model Textures/Background.png");
	background.loadTexture(TextureType::Specular, "textures/noSpecular.png");

	gameOver.loadMesh("meshes/background.obj");
	gameOver.loadTexture(TextureType::Diffuse, "textures/Model Textures/gameovertemp.png");
	gameOver.loadTexture(TextureType::Specular, "textures/noSpecular.png"); 

	gameOver.rotate = glm::rotate(gameOver.rotate, glm::pi<float>() *1.5f, glm::vec3(1.0f, 0.f, 0.f));
	gameOver.scale = 3.f;
	gameOver.transform = gameOver.translate * gameOver.rotate * glm::scale(glm::mat4(), glm::vec3(gameOver.scale));

	pauseback.loadMesh("meshes/background.obj");
	pauseback.loadTexture(TextureType::Diffuse, "textures/Model Textures/pause.png");
	pauseback.loadTexture(TextureType::Specular, "textures/noSpecular.png");

	bulletMesh.loadFromFile("meshes/cube.obj");
		player.morph.loadMesh("meshes/PyroboyAnim1.obj");
		player.morph.loadTexture(TextureType::Diffuse, "textures/Model Textures/pyroboy_flipped.png");
		player.morph.loadTexture(TextureType::Specular, "textures/noSpecular.png");
		 run1.loadFromFile("meshes/PyroboyAnim1.obj");
		 run2.loadFromFile("meshes/PyroboyAnim2.obj");
		 run3.loadFromFile("meshes/PyroboyAnim3.obj");
		 run4.loadFromFile("meshes/PyroboyAnim4.obj");
		 run5.loadFromFile("meshes/PyroboyAnim5.obj");
		 run6.loadFromFile("meshes/PyroboyAnim6.obj");
		 run7.loadFromFile("meshes/PyroboyAnim7.obj");
		 run8.loadFromFile("meshes/PyroboyAnim8.obj");
		 run9.loadFromFile("meshes/PyroboyAnim9.obj");
		run10.loadFromFile("meshes/PyroboyAnim10.obj");
		run11.loadFromFile("meshes/PyroboyAnim11.obj");
		run12.loadFromFile("meshes/PyroboyAnim12.obj");
		run13.loadFromFile("meshes/PyroboyAnim13.obj");
		run14.loadFromFile("meshes/PyroboyAnim14.obj");
		run15.loadFromFile("meshes/PyroboyAnim15.obj");
		run16.loadFromFile("meshes/PyroboyAnim16.obj");
		run17.loadFromFile("meshes/PyroboyAnim17.obj");
		run18.loadFromFile("meshes/PyroboyAnim18.obj");
		run19.loadFromFile("meshes/PyroboyAnim19.obj");
		run20.loadFromFile("meshes/PyroboyAnim20.obj");
		run21.loadFromFile("meshes/PyroboyAnim21.obj");
		run22.loadFromFile("meshes/PyroboyAnim22.obj");
		run23.loadFromFile("meshes/PyroboyAnim23.obj");
		run24.loadFromFile("meshes/PyroboyAnim24.obj");
		run25.loadFromFile("meshes/PyroboyAnim25.obj");
		run26.loadFromFile("meshes/PyroboyAnim26.obj");
		
	//player.loadMesh("meshes/GDW_Pyroboy.obj");
	player.playerMesh.push_back(run1 );
	player.playerMesh.push_back(run2 );
	player.playerMesh.push_back(run3 );
	player.playerMesh.push_back(run4 );
	player.playerMesh.push_back(run5 );
	player.playerMesh.push_back(run6 );
	player.playerMesh.push_back(run7 );
	player.playerMesh.push_back(run8 );
	player.playerMesh.push_back(run9 );
	player.playerMesh.push_back(run10);
	player.playerMesh.push_back(run11);
	player.playerMesh.push_back(run12);
	player.playerMesh.push_back(run13);
	player.playerMesh.push_back(run14);
	player.playerMesh.push_back(run15);
	player.playerMesh.push_back(run16);
	player.playerMesh.push_back(run17);
	player.playerMesh.push_back(run18);
	player.playerMesh.push_back(run19);

	player.loadTexture(TextureType::Diffuse, "textures/Model Textures/pyroboy_flipped.png");
	player.loadTexture(TextureType::Specular, "textures/noSpecular.png");

	//player_health = 1500; // player starting health
	player.position = glm::vec3(0.f, -10.f, 0.f);
	player.speed = 1.5f;

	player.originalPosition = player.position;
	player.translate = glm::translate(player.translate, player.position);

	player.rotX = glm::pi<float>() / -3.f;
	player.rotY = glm::pi<float>()*2.f;
	player.rotZ = glm::pi<float>() *2.f;

	player.rotate = glm::rotate(player.rotate, player.rotY, glm::vec3(0.f, 1.f, 0.f));
	player.rotate = glm::rotate(player.rotate, player.rotX, glm::vec3(1.f, 0.f, 0.f));
	player.rotate = glm::rotate(player.rotate, player.rotZ, glm::vec3(0.f, 0.f, 1.f));

	player.scale = 1.f;
	player.direction = up;
	isUp = true;

	player.transform = player.translate * player.rotate *glm::scale(glm::mat4(), glm::vec3(player.scale));

	heartMesh.loadFromFile("meshes/Health.obj");
	p_healthTex.load("textures/Health.png");
	t_healthTex.load("textures/Model Textures/testuv1.png");

	for (int i = 0; i < 15; i++)
	{
		hearts.push_back(new UIGameObjects(heartMesh));
	}
	for (int i = 0; i < 20; i++)
	{
		treeHearts.push_back(new UIGameObjects(heartMesh));
	}
	//hearts.push_back(&HealthSprite);
	//hearts.push_back(&HealthSprite2);
	//hearts.push_back(&HealthSprite3);
	//hearts.push_back(&HealthSprite4);
	//hearts.push_back(&HealthSprite5);
	//hearts.push_back(&HealthSprite6);

	float heartpos[2];
	heartpos[0] = 38.0f;
	heartpos[1] = 36.0f;
	for (int i = 0; i < hearts.size()-1; i++)
	{
	//	hearts[i]->loadMesh("meshes/Health.obj");
		hearts[i]->loadTexture(TextureType::Diffuse, "textures/Health.png");
		hearts[i]->loadTexture(TextureType::Specular, "textures/noSpecular.png");

		if(i <=(hearts.size() - 1)/2)
		hearts[i]->position=glm::vec3(-18.0f+(1.0f*i), heartpos[0] + player.position.y, -5.0f);
		else
		hearts[i]->position = glm::vec3(-19.0f + (1.0f*(i- (hearts.size() - 1) / 2)), heartpos[1] + player.position.y, -5.0f);

		hearts[i]->translate = glm::translate(hearts[i]->translate, hearts[i]->position);
		hearts[i]->rotate = glm::rotate(hearts[i]->rotate, glm::pi<float>() / 2.0f, glm::vec3(1.f, 0.f, 0.f));
		hearts[i]->scale = 2.f;
		hearts[i]->transform = hearts[i]->translate * hearts[i]->rotate * glm::scale(glm::mat4(), glm::vec3(hearts[i]->scale));
		hearts[i]->originalTransform = hearts[i]->transform;
	}

	float treeHeartpos[2];
	treeHeartpos[0] = 38.0f;
	treeHeartpos[1] = 36.0f;
	for (int i = 0; i < treeHearts.size() - 1; i++)
	{
			//hearts[i]->loadMesh("meshes/Health.obj");
		treeHearts[i]->loadTexture(TextureType::Diffuse, "textures/Model Textures/testuv1.png");
		treeHearts[i]->loadTexture(TextureType::Specular, "textures/noSpecular.png");

		if (i <= (treeHearts.size() - 1) / 2)
			treeHearts[i]->position = glm::vec3(19.f - (1.0f*i), treeHeartpos[0] + player.position.y, -5.0f);
		else
			treeHearts[i]->position = glm::vec3(20.f - (1.0f*(i - (treeHearts.size() - 1) / 2)), treeHeartpos[1] + player.position.y, -5.0f);

		treeHearts[i]->translate = glm::translate(treeHearts[i]->translate, treeHearts[i]->position);
		treeHearts[i]->rotate = glm::rotate(treeHearts[i]->rotate, glm::pi<float>() / 2.0f, glm::vec3(1.f, 0.f, 0.f));
		treeHearts[i]->scale = 2.f;
		treeHearts[i]->transform = treeHearts[i]->translate * treeHearts[i]->rotate * glm::scale(glm::mat4(), glm::vec3(treeHearts[i]->scale));
		treeHearts[i]->originalTransform = treeHearts[i]->transform;
	}
	treeHeartCounter = treeHearts.size() - 1;
	counter = hearts.size() - 1;

	/*ui.translate = glm::translate(ui.translate, glm::vec3(0.f,0.f,-5.f));
	ui2.translate = glm::translate(ui.translate, glm::vec3(0.f, 0.f, -5.f));*/

	// Cleaning up block of UI code
	initUIObjects();

	//ENEMY POSITIONS
	enemy.position = glm::vec3 (-400.f,  0.f,   -2.f);
	enemy2.position = glm::vec3( 100.f, -400.f, -2.f);
	enemy3.position = glm::vec3(-250.f,  100.f, -2.f);
	enemy4.position = glm::vec3( 0.f,   -350.f, -2.f);
	enemy5.position = glm::vec3( 400.f,  0.f,   -2.f);
	enemy6.position = glm::vec3( 0.f,    400.f, -2.f);
	insect.position = glm::vec3( 300.f,  100.f, -2.f);
	insect2.position = glm::vec3(-100.f, 300.f, -2.f);
	insect3.position = glm::vec3(250.f,  100.f, -2.f);
	insect4.position = glm::vec3(0.f,    350.f, -2.f);
	insect5.position = glm::vec3(-400.f, 0.f,   -2.f);
	insect6.position = glm::vec3(0.f,   -400.f, -2.f);

	//Vector of positions
	//pushback all positions
	positions.push_back(enemy.position);
	positions.push_back(enemy3.position);
	positions.push_back(enemy5.position);
	positions.push_back(enemy2.position);
	positions.push_back(enemy4.position);
	positions.push_back(enemy6.position);
	positions.push_back(insect.position);
	positions.push_back(insect2.position);
	positions.push_back(insect3.position);
	positions.push_back(insect4.position);
	positions.push_back(insect5.position);
	positions.push_back(insect6.position);

	//Vector of enemies
	//pushback all enemy objects
	enemies.push_back(&enemy);
	enemies.push_back(&enemy3);
	enemies.push_back(&enemy5);
	enemies.push_back(&enemy2);
	enemies.push_back(&enemy4);
	enemies.push_back(&enemy6);
	enemies.push_back(&insect);
	enemies.push_back(&insect2);
	enemies.push_back(&insect3);
	enemies.push_back(&insect4);
	enemies.push_back(&insect5);
	enemies.push_back(&insect6);

	alienMesh.loadFromFile("meshes/GDW_AlienNew.obj");
	insectMesh.loadFromFile("meshes/GDW_ExpInsect.obj");

	//set enemies health and damage, respectively
	//aliens
	for (int i = 0; i < (enemies.size()/2)/2; i++)
	{
		enemies[i]->loadMesh("meshes/GDW_AlienNew.obj");
		enemies[i]->loadTexture(TextureType::Diffuse, "textures/Model Textures/GDW_AlienUV.png");
		enemies[i]->loadTexture(TextureType::Specular, "textures/noSpecular.png");
		enemies[i]->scale = 1.8f;
		enemies[i]->setProperty(50.f, 1.f);
	}
	//explosive insect v1
	for (int i = (enemies.size() / 2) / 2; i < (enemies.size() / 2); i++)
	{
		enemies[i]->loadMesh("meshes/GDW_ExpInsect.obj");
		enemies[i]->loadTexture(TextureType::Diffuse, "textures/Health.png");
		enemies[i]->loadTexture(TextureType::Specular, "textures/noSpecular.png");
		enemies[i]->scale = 1.8f;
		enemies[i]->setProperty(50.f, 1.f);

	}
	//explosive insect v2
	for (int i = (enemies.size() / 2); i < enemies.size(); i++)
	{
		enemies[i]->loadMesh("meshes/InsectPrototype.obj");
		enemies[i]->loadTexture(TextureType::Diffuse, "textures/Model Textures/GDW_ExpInsectTexture.png");
		enemies[i]->loadTexture(TextureType::Specular, "textures/noSpecular.png");
		enemies[i]->scale=10.f;
		enemies[i]->setProperty(10.f, 1.f);
	}

	//load textures
	//FENCE POSITIONS

	// BG Stuff
	background.scale = 2.0f;
	background.position = glm::vec3(-2.0f * background.scale, -8.0f, 1.0f);
	background.translate = glm::translate(background.translate, background.position);
	background.rotate = glm::rotate(background.rotate, -glm::pi<float>() / 2.f, glm::vec3(1.f, 0.f, 0.f));
	background.rotate = glm::rotate(background.rotate, -0.002f, glm::vec3(1.f, 0.f, 0.f));
	//background.rotate = glm::rotate(background.rotate, -glm::pi<float>(), glm::vec3(0.f, 1.f, 0.f));
	//background.rotate = glm::rotate(background.rotate, glm::pi<float>() / 2.f, glm::vec3(1.f, 0.f, 0.f));
	background.transform = background.translate * background.rotate * glm::scale(glm::mat4(), glm::vec3(background.scale));
	 
	pauseback.scale = 10.f;
	pauseback.rotate = glm::rotate(pauseback.rotate, glm::pi<float>() *1.5f, glm::vec3(1.0f, 0.f, 0.f));
	pauseback.transform = pauseback.translate * pauseback.rotate * glm::scale(glm::mat4(), glm::vec3(pauseback.scale));

	tree.position = glm::vec3(0.f,0.f,0.f);
	tree_health = 3000.f; // tree starting health
	tree.scale = 2.f;
	tree.rotate = glm::rotate(tree.rotate, glm::pi<float>() /-2.f, glm::vec3(1.0f, 0.f, 0.f));
	tree.translate = glm::translate(tree.translate, tree.position);

	tree.transform = tree.translate * tree.rotate * glm::scale(glm::mat4(), glm::vec3(tree.scale));

	//timer is std::cout << "Total elapsed time: " << updateTimer->getCurrentTime() / 1000.f<<std::endl;
	// want to do so that at specific time intervals spawn a enemy
	// can do if updateTimer->getCurrentTime() / 1000.f % 100 = 0 create new enemy 
	// to create new enemy we either need to have an initialize enemy function
	//
	float i = updateTimer->getCurrentTime() / 1000.f;
	//i = glm::clamp(i, 0.0f, 10.f);

	srand(time(0));
	for (int i = 0; i < (enemies.size()); i++)
	{
		glm::vec3 playerLocation = glm::vec3(player.translate*
			glm::vec4(glm::vec3(1.f, 1.f, 1.f), 1.0));

		enemies[i]->translate = glm::translate(enemies[i]->translate,positions[i]);
		enemies[i]->rotate = glm::rotate(enemies[i]->rotate, (glm::pi<float>() / 1.f), glm::vec3(0.f, 1.f, 0.f));
		enemies[i]->rotate = glm::rotate(enemies[i]->rotate, (glm::pi<float>() / -2.f), glm::vec3(1.f, 0.f, 0.f));
		enemies[i]->rotate = glm::rotate(enemies[i]->rotate, (glm::pi<float>() / 1.f), glm::vec3(0.f, 0.f, 1.f));

		enemies[i]->transform = enemies[i]->transform * enemies[i]->rotate *glm::scale(glm::mat4(), glm::vec3(enemies[i]->scale));

		float randSpeed = 10.f+ rand()%30;
		enemies[i]->speed = randSpeed;
		enemies[i]->setProperty(30.f,1.f);
	}

	//cameraTransform = glm::translate(cameraTransform, glm::vec3(0.f, 10.f, -10.f));
	/*cameraTransform = glm::lookAt(cameraEye,
									cameraCtr,glm::vec3(0.f,-1.f,0.f));
	originalCameraTransform = cameraTransform;
	
	cameraProjection = glm::perspective(150.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.f);
	glViewport(0, 0, 20.f, 20.f);*/
	//cameraProjection = glm::ortho(-20.0f,20.f, 20.f,-20.f, 0.1f, 1000.0f);
	cameraOrtho = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, -1000.0f, 1000.0f);
	state = GameStates::MAIN_MENU;
	mainMenu();
}

void Game::TreeWasAttacked(Enemy* _x, glm::vec3 pos)
{
	if (_x->inArea(glm::vec3(0.f, 0.f, 0.f)))
	{
		tree_health -= _x->getAttack();
		//std::cout << tree_health << std::endl;
		//if ((int)tree_health %100==0)
		//{
			if (tree_health == 0) // we can just use the direct health value now
			{
				
					player.position = glm::vec3(0.f,-10.f,0.f);
					bulletTime = 0;
					t = 0;

					treeDead = true;
			}
				//exit(0);
			
			//if (!treeDead)
			//{
			//	treeHearts[treeHeartCounter]->active = false;
			//	treeHeartCounter--;
			//}
			
			//cameraTransform = originalCameraTransform * glm::mat4(0.f);
		//}
	}

	if (_x->inArea(pos)) // player taking damage
	{
		player_health -= _x->getAttack(); 
		//std::cout << player_health << std::endl;

		// simplified player death as well
		if (player_health == 0)
		{
				playerDead = true;
				
		}
	}
}
void Game::createBullet(glm::vec3 pos, glm::vec3 dir)
{
	/*Bullet bullet;
	bullet.loadMesh("meshes/cube.obj");
	bullet.loadTexture(TextureType::Diffuse, "textures/testuv1.png");
	bullet.loadTexture(TextureType::Specular, "textures/noSpecular.png");
	bullet.speed = 1.f;
	bullet.position = player.position;*/
	
	if (bulletTime < 0.01f)
	{
		bullets.push_back(new Bullet(bulletMesh,pos,1.f, 100.f, 15.f, dir));
	}
	else if(bulletTime > 0.4f)
	{
		bulletTime = 0.f;
		bullets.push_back(new Bullet(bulletMesh, pos, 1.f, 100.f, 15.f, dir));
		
	}
}

void Game::uiUpdate()
{
	updateTimer->tick();

	float deltaTime = updateTimer->getElapsedTimeSeconds();

	deltaTime = glm::clamp(deltaTime, 0.0f, 0.033f);
	t += deltaTime;
	dt += deltaTime;
	if (t > 1.f)
		t = 0.f;

	if (dt > 0.1f)
	{
		dt = 0.f;
		//(-30f,-1f,0.f), 10,25
		if (state == GameStates::MAIN_MENU)
		{

			if(wKeyDown||upKey||leftKey)
			{
				if (arrow.position.y == -1.f)
				{
					arrow.position = glm::vec3(-30.f, 25.f, 0.f);
					//arrow.position = Math::lerp(arrow.position, glm::vec3(30.f, -40.f, 0.f), t);
					arrow.translate = glm::translate(arrow.translate, arrow.position);

				}
				else if (arrow.position.y == 10.f)
				{
					arrow.position = glm::vec3(-30.f, -1.f, 0.f);
					//arrow.position = Math::lerp(arrow.position, glm::vec3(30.f, -1.f, 0.f), t);
					arrow.translate = glm::translate(arrow.translate, arrow.position);
				}
				else if (arrow.position.y == 25.f)
				{
					arrow.position= glm::vec3(-30.f, 10.f, 0.f);
					//arrow.position = Math::lerp(arrow.position, glm::vec3(30.f, -18.f, 0.f), t);
					arrow.translate = glm::translate(arrow.translate, arrow.position);
				}
			}
			if (sKeyDown || downKey||rightKey)
			{
				if (arrow.position.y == -1.f)
				{
					arrow.position = glm::vec3(-30.f, 10.f, 0.f);
					arrow.translate = glm::translate(arrow.translate, arrow.position);

				}
				else if (arrow.position.y == 10.f)
				{
					arrow.position = glm::vec3(-30.f, 25.f, 0.f);
					arrow.translate = glm::translate(arrow.translate, arrow.position);

				}
				else if (arrow.position.y == 25.f)
				{
					arrow.position = glm::vec3(-30.f, -1.f, 0.f);
					arrow.translate = glm::translate(arrow.translate, arrow.position);
				}
		

			}
				arrow.translate = glm::translate(glm::mat4(), arrow.position);
			}

			monkey.transform = monkey.translate * monkey.rotate * glm::scale(glm::mat4(), glm::vec3(monkey.scale));
			arrow.transform = arrow.translate * arrow.rotate * glm::scale(glm::mat4(), glm::vec3(arrow.scale));
		}

}
void Game::uiDraw()
{
	glClearColor(0.5, 0.5, 0.5, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	monkey.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	arrow.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);

	glutSwapBuffers();
}
void Game::update()
{
	background.transform = background.translate * background.rotate * glm::scale(glm::mat4(), glm::vec3(background.scale));
	if (!pause)
	{

		updateTimer->tick();

		float deltaTime = updateTimer->getElapsedTimeSeconds();

		deltaTime = glm::clamp(deltaTime, 0.0f, 0.033f);
		t += deltaTime;
		srand(time(0));

		if (shooting)
		{
			blast = se.system->createSound("sounds/blast.wav", FMOD_3D, 0, &sound);
			FmodErrorCheck(blast);
			blast = sound->set3DMinMaxDistance(0.0f, 30000.0f);
			FmodErrorCheck(blast);
			blast = se.system->playSound(sound, 0, false, &channel);
		}

		result = se.system->update();
		FmodErrorCheck(result);

		player.update(deltaTime);
		player.animate(deltaTime);

		if (t >= 8.f)
		{
			float ranPos = 1 + rand() % 5;

			float randSpeed = 10.f + rand() % 30;
			glm::mat4 rot;
			rot = glm::rotate(rot, (glm::pi<float>() / 1.f), glm::vec3(0.f, 1.f, 0.f));
			rot = glm::rotate(rot, (glm::pi<float>() / -2.f), glm::vec3(1.f, 0.f, 0.f));
			rot = glm::rotate(rot, (glm::pi<float>() / 1.f), glm::vec3(0.f, 0.f, 1.f));
			enemies.push_back(new Enemy(alienMesh, glm::vec3(50.f* ranPos, 50.f*ranPos, -2.f), rot, 2.f, randSpeed, 30.f + (3.f * healthCounter), 1.f));
			enemies.push_back(new Enemy(insectMesh, glm::vec3(-50.f* ranPos, 50.f*ranPos, -2.f), rot, 2.f, randSpeed, 30.f + (3.f * healthCounter), 1.f));
			t = 0.f;
			healthCounter++;
		}


		if (shouldRotate) {
			player.rotate = glm::rotate(player.rotate, deltaTime *(glm::pi<float>() / 4), glm::vec3(0.f, 1.f, 0.f));
		}

		glm::vec3 playerLocation = glm::vec3(player.translate*
			glm::vec4(glm::vec3(0.f, 0.f, 0.f), 1.0));
		if (wKeyDown) {
			/*std::cout << glm::to_string(player.transform) << std::endl;*/
			if (playerLocation.y >-80 * (background.scale/3.f)) {

				if (isDown)
				{
					player.rotate = glm::rotate(player.rotate, glm::pi<float>(), glm::vec3(0.f, 1.f, 0.f));
					isDown = false;
				}
				else if (isLeft)
				{
					player.rotate = glm::rotate(player.rotate, glm::pi<float>() / 2.f, glm::vec3(0.f, 1.f, 0.f));
					isLeft = false;
				}
				else if (isRight)
				{
					player.rotate = glm::rotate(player.rotate, glm::pi<float>() / -2.f, glm::vec3(0.f, 1.f, 0.f));
					isRight = false;
				}
				player.direction = up;
				player.nForce = player.direction * player.speed;
				player.originalPosition = player.nForce;
				player.translate = glm::translate(player.translate, player.nForce);
				player.originalTranslate = glm::translate(player.originalTranslate, player.originalPosition);
			}
		}
		if (sKeyDown) {
			if (playerLocation.y <120 * (background.scale/3.f)) {
				if (isUp)
				{
					player.rotate = glm::rotate(player.rotate, glm::pi<float>(), glm::vec3(0.f, 1.f, 0.f));
					isUp = false;
				}
				else if (isLeft)
				{
					player.rotate = glm::rotate(player.rotate, glm::pi<float>() / -2.f, glm::vec3(0.f, 1.f, 0.f));
					isLeft = false;
				}
				else if (isRight)
				{
					player.rotate = glm::rotate(player.rotate, glm::pi<float>() / 2.f, glm::vec3(0.f, 1.f, 0.f));
					isRight = false;
				}

				player.direction = down;

				player.nForce = player.direction * player.speed;
				player.originalPosition = player.nForce;
				player.translate = glm::translate(player.translate, player.nForce);
				player.originalTranslate = glm::translate(player.originalTranslate, player.originalPosition);
			}
		}
		if (aKeyDown) {
			/*std::cout << glm::to_string(player.transform) << std::endl;*/
			if (playerLocation.x < 100.f * (background.scale/3.f)) {
				if (isUp)
				{
					player.rotate = glm::rotate(player.rotate, glm::pi<float>() / -2.f, glm::vec3(0.f, 1.f, 0.f));
					isUp = false;
				}
				else if (isDown)
				{
					player.rotate = glm::rotate(player.rotate, glm::pi<float>() / 2.f, glm::vec3(0.f, 1.f, 0.f));
					isDown = false;
				}
				else if (isRight)
				{
					player.rotate = glm::rotate(player.rotate, glm::pi<float>(), glm::vec3(0.f, 1.f, 0.f));
					isRight = false;
				}
				player.direction = left;
				player.nForce = player.direction * player.speed;
				player.originalPosition = glm::vec3(-player.nForce.x, player.nForce.y, player.nForce.z);//opposite

				player.translate = glm::translate(player.translate, player.nForce);
				player.originalTranslate = glm::translate(player.originalTranslate, player.originalPosition);
			}
		}

		if (dKeyDown) {
			/*std::cout << glm::to_string(player.transform) << std::endl;*/
			if (playerLocation.x >-100.f * (background.scale/3.f)) {
				if (isUp)
				{
					player.rotate = glm::rotate(player.rotate, glm::pi<float>() / 2.f, glm::vec3(0.f, 1.f, 0.f));
					isUp = false;
				}
				else if (isDown)
				{
					player.rotate = glm::rotate(player.rotate, glm::pi<float>() / -2.f, glm::vec3(0.f, 1.f, 0.f));
					isDown = false;
				}
				else if (isLeft)
				{
					player.rotate = glm::rotate(player.rotate, glm::pi<float>(), glm::vec3(0.f, 1.f, 0.f));
					isLeft = false;
				}
				player.direction = right;
				player.nForce = player.direction * player.speed;
				player.originalPosition = glm::vec3(-player.nForce.x, player.nForce.y, player.nForce.z);//opposite
				player.translate = glm::translate(player.translate, player.nForce);
				player.originalTranslate = glm::translate(player.originalTranslate, player.originalPosition);
			}
		}
		//SCALE THE OBJECT ACCORDINGLY

		for (int j = 0; j < enemies.size(); j++) {
			glm::vec3 enemyLocation = glm::vec3(enemies[j]->translate*glm::vec4(glm::vec3(0.f, 0.f, 0.f), 1.0));
			//TreeWasAttacked(*enemies[j], playerLocation);
			for (int i = 0; i < bullets.size(); i++)
			{
				if ((bullets[i]->position.x > enemyLocation.x - 5.f && bullets[i]->position.x < enemyLocation.x + 5.f)
					&& (bullets[i]->position.y > enemyLocation.y - 5.f && bullets[i]->position.y < enemyLocation.y + 5.f))
				{

					enemies[j]->setProperty(enemies[j]->getHealth() - bullets[i]->getDamage(), enemies[j]->getAttack());
					bullets[i]->position = glm::vec3(100.f, 100.f, 0.f);
					bullets[i]->direction = glm::vec3(0.f, 1.f, 0.f);
					bullets.erase(bullets.begin() + i);


				}
			}
			if (enemies[j]->getHealth() <= 0)
			{
				enemies[j]->setBool(true);
				enemies[j]->position = glm::vec3(-100.f, -100.f, -2.f);
			}
		}
		if (upKey)
		{
			createBullet(playerLocation, up);
			bulletTime += deltaTime;
		}
		else if (downKey)
		{
			createBullet(playerLocation, down);
			bulletTime += deltaTime;
		}
		else if (leftKey)
		{
			createBullet(playerLocation, left);
			bulletTime += deltaTime;
		}
		else if (rightKey)
		{
			createBullet(playerLocation, right);
			bulletTime += deltaTime;
		}
		if (iKeyDown)
		{

		}
		for (int i = 0; i < bullets.size(); i++)
		{
			bullets[i]->update(deltaTime);
			//bullets[i]->velocity = bullets[i]->direction * bullets[i]->speed;
			bullets[i]->translate = glm::translate(glm::mat4(), bullets[i]->position);
			bullets[i]->transform = bullets[i]->translate *bullets[i]->rotate * glm::scale(glm::mat4(), glm::vec3(bullets[i]->scale));

			bulletLocation += bullets[i]->transform * glm::vec4(glm::vec3(0.f), 1.f);
			if ((bullets[i]->position.x > 100.f || bullets[i]->position.x < -100.f) || (bullets[i]->position.y > 100.f || bullets[i]->position.y < -100.f))
			{
				//std::cout << glm::to_string(bullets[i]->position) << std::endl;
				bullets.erase(bullets.begin() + i);
				bulletLocation = glm::vec4(0.f);
			}
		}

		//enemies movement
		for (int i = 0; i < enemies.size(); i++)
		{
			if (!enemies[i]->getBool())
				enemies[i]->update(deltaTime);
			enemies[i]->translate = glm::translate(glm::mat4(), enemies[i]->position);
			/*enemies[i]->seek(0.5f * glm::vec3(0.f, 0.f, 9.f) + 0.5f * glm::vec3(playerLocation));*/
			if (glm::length(enemies[i]->position - glm::vec3(0.f, 0.f, -2.f)) < 7.f)
			{
				//enemies[i]->seek(glm::vec3(0.f, 0.f, 0.f));
				enemies[i]->velocity = glm::normalize(glm::vec3(0.f, 0.f, -2.f) - enemies[i]->position) * enemies[i]->speed;
			}
			else
			{
				if (glm::length(enemies[i]->position - playerLocation) < 25.f)
				{
					//enemies[i]->seek(glm::vec3(playerLocation));
					enemies[i]->velocity = glm::normalize(playerLocation - enemies[i]->position) * enemies[i]->speed;

					//rotation for enemies
					if ((playerLocation.x - enemies[i]->position.x) > 0 &&
						(playerLocation.y - enemies[i]->position.y) >= 0)
					{

						enemies[i]->rotate = glm::rotate(glm::mat4(),
							(glm::pi<float>() - atan((playerLocation.y - enemies[i]->position.y) /
							(playerLocation.x - enemies[i]->position.x))), glm::vec3(0.f, 0.f, 1.f));
						enemies[i]->rotate = glm::rotate(glm::mat4(),
							(glm::pi<float>() - atan((playerLocation.y - enemies[i]->position.y) /
							(playerLocation.x - enemies[i]->position.x))), glm::vec3(0.f, 0.f, 1.f));
					}
					else if ((playerLocation.x - enemies[i]->position.x) >= 0 &&
						(playerLocation.y - enemies[i]->position.y) < 0)
					{
						enemies[i]->rotate = glm::rotate(glm::mat4(),
							(atan((-playerLocation.y - enemies[i]->position.y) /
							(playerLocation.x - enemies[i]->position.x))), glm::vec3(0.f, 0.f, 1.f));

					}
					else if ((playerLocation.x - enemies[i]->position.x) < 0 &&
						(playerLocation.y - enemies[i]->position.y) <= 0)
					{
						enemies[i]->rotate = glm::rotate(glm::mat4(),
							(glm::pi<float>()*1.f - atan((-playerLocation.y - enemies[i]->position.y) /
							(-playerLocation.x - enemies[i]->position.x))), glm::vec3(0.f, 0.f, 1.f));

					}
					else
					{
						enemies[i]->rotate = glm::rotate(glm::mat4(),
							(glm::pi<float>()*1.f + atan((playerLocation.y - enemies[i]->position.y) /
							(-playerLocation.x - enemies[i]->position.x))), glm::vec3(0.f, 0.f, 1.f));

					}
				}
				else
				{
					//enemies[i]->seek(glm::vec3(0.f, 0.f, 0.f));
					enemies[i]->velocity = glm::normalize(glm::vec3(0.f, 0.f, -2.f) - enemies[i]->position) * enemies[i]->speed;

				}
			}
			enemies[i]->transform = enemies[i]->translate * enemies[i]->rotate * glm::scale(glm::mat4(), glm::vec3(enemies[i]->scale));
		}

		glm::vec3 lookAtMouse = mousePositionFlipped - playerLocation;
		//cos theta = (a dot b)/ |a||b| // player is a, mouse is b
		float b = glm::length(lookAtMouse);
		float top = glm::dot(a, lookAtMouse);
		float bottom = glm::length(a) * b;
		float angle = glm::acos(top / bottom);
		glm::vec3 mouseDir = glm::normalize(lookAtMouse);
		//std::cout << glm::pi<float>()/angle << std::endl;

		if (mousePositionFlipped != glm::vec3(0.f))
		{
			//float rot = Math::lerp(player.rotY, angle, t);

			if (player.direction != mouseDir)
			{
				//fixAngle(angle / glm::pi<float>(), mouseDir);
			}

		}

		if (player.direction == up)
		{
			glm::vec3 point = glm::vec3(player.position.x, player.position.y + 10.f, 0.f);
			a = point - playerLocation;
		}
		if (player.direction == down)
		{
			glm::vec3 point = glm::vec3(player.position.x, player.position.y - 10.f, 0.f);
			a = point - playerLocation;
		}
		if (player.direction == left)
		{
			glm::vec3 point = glm::vec3(player.position.x - 10.f, player.position.y, 0.f);
			a = point - playerLocation;
		}
		if (player.direction == right)
		{
			glm::vec3 point = glm::vec3(player.position.x + 10.f, player.position.y, 0.f);
			a = point - playerLocation;
		}

		//std::cout << glm::to_string(player.translate)<< std::endl
		//TRANSFORMS

		background.transform = background.translate *background.rotate * glm::scale(glm::mat4(), glm::vec3(background.scale));

		/*for (int i = 0; i < fences.size(); i++)
		{
			fences[i]->transform = fences[i]->translate * fences[i]->rotate * glm::scale(glm::mat4(), glm::vec3(fences[i]->scale));
		}*/

		for (int i = 0; i < hearts.size(); i++)
		{
			hearts[i]->transform = player.translate * hearts[i]->originalTransform;
		}
		for (int i = 0; i < treeHearts.size(); i++)
		{
			treeHearts[i]->transform = player.translate * treeHearts[i]->originalTransform;
		}
		
		cameraTransform = glm::lookAt(cameraEye, cameraCtr, glm::vec3(0.f, -1.f, 0.f));
		originalCameraTransform = cameraTransform;

		player.transform = player.translate * player.rotate * glm::scale(glm::mat4(), glm::vec3(player.scale));

		// Updating the position of the UI elements
	//	ui.transform = player.originalTranslate * ui.originalTransform;

		cameraTransform = originalCameraTransform * player.originalTranslate;
		cameraTransform[1] = -cameraTransform[1];

		emitter.update(deltaTime);

		if (treeDead|| playerDead)
		{
			//tree_health = 3000.f;
			//player_health = 1500.f;
			treeHeartCounter = treeHearts.size() - 1;
			counter = hearts.size() - 1;

			for (int i = 0; i < treeHearts.size() - 1; i++)
			{
				treeHearts[i]->active = true;
			}
			for (int i = 0; i < hearts.size() - 1; i++)
			{
				hearts[i]->active = true;
			}

			cameraTransform = glm::lookAt(glm::vec3(0.f, 0.f, -25.f),
				glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f));
			originalCameraTransform = cameraTransform;

			for (int i = 0; i < enemies.size(); i++)
			{
				srand(time(0));

				float randPosX = (rand()%500) - 200;
				float randPosY = (rand() % 500) - 200;

				enemies[i]->position = glm::vec3(randPosX, randPosY,-2.f);
			}
			player.position = glm::vec3(0.f, -10.f, 0.f);
			player.translate = glm::translate(player.translate, player.position);
			player.transform = player.translate * player.rotate * glm::scale(glm::mat4(), glm::vec3(player.scale));
			cameraProjection = glm::perspective(90.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.f);
			state = GameStates::LOSE;
			treeDead = false;
		}
	}
}

void Game::fixAngle(float angle, glm::vec3 &dir)
{
	player.rotate = glm::rotate(player.rotate, angle/ glm::pi<float>(), glm::vec3(0.f, 1.f, 0.f));
	player.direction = dir;
}

void Game::switchUIToDraw(GameObject::Ptr curUI, uiType type)
{
	// Takes in uiObject we want to draw, sets all other uiObjects to not draw
	// Then sets desired UI object to draw true
	if (type == PLAYER) {
		for (auto itr = playerUI.begin(); itr != playerUI.end(); itr++)
		{
			if (itr->second != curUI)
			{
				itr->second->shouldDraw = false;
			}
		}
	}

	else if (type == TREE) {
		for (auto itr = treeUI.begin(); itr != treeUI.end(); itr++)
		{
			if (itr->second != curUI)
			{
				itr->second->shouldDraw = false;
			}
		}
	}
	curUI->shouldDraw = true;
}

void Game::drawHUD()
{
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); // position and size of viewport
	glEnable(GL_BLEND); // Allows Transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	cameraOrtho = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, -1000.0f, 1000.0f);

	//treeUI["treeHP10"]->draw(noLight, glm::mat4(1.0f), cameraOrtho, pointLights, directionalLight);

	// update hearts based on player health and output num value for it
	//std::cout << "players health: " << player_health << std::endl;
	if (player_health <= 150) { switchUIToDraw(playerUI["playerHP1"], PLAYER); }
	else if (player_health <= 300) { switchUIToDraw(playerUI["playerHP2"], PLAYER); }
	else if (player_health <= 450) { switchUIToDraw(playerUI["playerHP3"], PLAYER); }
	else if (player_health <= 600) { switchUIToDraw(playerUI["playerHP4"], PLAYER); }
	else if (player_health <= 750) { switchUIToDraw(playerUI["playerHP5"], PLAYER); }
	else if (player_health <= 900) { switchUIToDraw(playerUI["playerHP6"], PLAYER); }
	else if (player_health <= 1050) { switchUIToDraw(playerUI["playerHP7"], PLAYER); }
	else if (player_health <= 1200) { switchUIToDraw(playerUI["playerHP8"], PLAYER); }
	else if (player_health <= 1350) { switchUIToDraw(playerUI["playerHP9"], PLAYER); }
	else if (player_health <= 1500) { switchUIToDraw(playerUI["playerHP10"], PLAYER); }
	
	// same thing with tree
	//std::cout << "tree health: " << tree_health << std::endl;
	if (tree_health <= 300) { switchUIToDraw(treeUI["treeHP1"], TREE); }
	else if (tree_health <= 600) { switchUIToDraw(treeUI["treeHP2"], TREE); }
	else if (tree_health <= 900) { switchUIToDraw(treeUI["treeHP3"], TREE); }
	else if (tree_health <= 1200) { switchUIToDraw(treeUI["treeHP4"], TREE); }
	else if (tree_health <= 1500) { switchUIToDraw(treeUI["treeHP5"], TREE); }
	else if (tree_health <= 1800) { switchUIToDraw(treeUI["treeHP6"], TREE); }
	else if (tree_health <= 2100) { switchUIToDraw(treeUI["treeHP7"], TREE); }
	else if (tree_health <= 2400) { switchUIToDraw(treeUI["treeHP8"], TREE); }
	else if (tree_health <= 2700) { switchUIToDraw(treeUI["treeHP9"], TREE); }
	else if (tree_health <= 3000) { switchUIToDraw(treeUI["treeHP10"], TREE); }
	
	// draw ui that's set to true
	for (auto itr = playerUI.begin(); itr != playerUI.end(); itr++)
	{
		if (itr->second->shouldDraw)
			itr->second->draw(noLight, glm::mat4(1.0f), cameraOrtho, pointLights, directionalLight);
	}
	for (auto itr = treeUI.begin(); itr != treeUI.end(); itr++)
	{
		if (itr->second->shouldDraw)
			itr->second->draw(noLight, glm::mat4(1.0f), cameraOrtho, pointLights, directionalLight);
	}

	//std::cout << t << std::endl;
	if (t >= 5) { otherUI["protect"]->shouldDraw = false; }

	switch (otherUI["protect"]->shouldDraw) {
	case(true):
		otherUI["protect"]->draw(noLight, glm::mat4(1.0f), cameraOrtho, pointLights, directionalLight);
		break;
	case(false):
		break;
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}
void Game::draw()
{
	
	glClearColor(0.5, 0.5, 0.5, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	sceneBuffer.bindFrameBufferForDrawing();
	sceneBuffer.clearFrameBuffer(glm::vec4(0));

	// Set material properties
	materials["default"]->sendUniform("u_lightPos",cameraTransform * directionalLight.positionOrDirection);

	// draw the scene to the fbo
	//drawScene(playerCamera);'
	sceneBuffer.unbindFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

	background.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	tree.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	for (int i = 0; i < enemies.size(); i++)
	{
		if (!enemies[i]->getBool())
			enemies[i]->draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	}
	player.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);

	//for (int i = 0; i < hearts.size()-1; i++)
	//{
	//	if(hearts[i]->active)
	//	hearts[i]->draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	//}
	//for (int i = 0; i < treeHearts.size(); i++)
	//{
	//	if (treeHearts[i]->active)
	//		treeHearts[i]->draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	//}*/
	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	}
	drawHUD();
	emitter.draw(player.transform, cameraTransform, cameraProjection);


	glutSwapBuffers();

}

//Input callback funct
void Game::keyboardDown(unsigned char key, int mouseX, int mouseY) 
{
	glm::vec3 playerLocation = glm::vec3(player.translate*
		glm::vec4(glm::vec3(1.f, 1.f, 1.f), 1.0));


	switch (key) 
	{
	case 27://esc
	case'q':
		exit(1);
		break;
	case't':
		std::cout << "Total elapsed time: " << updateTimer->getCurrentTime() / 1000.f<<std::endl;
	
		break;
	case'g':
		if (arrow.position == glm::vec3(-30.f, -1.f, 0.f))
		{
			state = GameStates::PLAYING;
			background.scale = 2.f;

			cameraTransform = glm::lookAt(cameraEye,
				cameraCtr, glm::vec3(0.f, -1.f, 0.f));
			originalCameraTransform = cameraTransform;

			cameraProjection = glm::perspective(150.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.f);
			glViewport(0, 0, 20.f, 20.f);
		}

		if (arrow.position == glm::vec3(-30.f, 25.f, 0.f))
			exit(1);
		break;
	case 'w':

		wKeyDown = true;
		isUp = true;
		break;
	case 'a':
		aKeyDown = true;
		isLeft = true;
		break;
	case 's':
		sKeyDown = true;	
		isDown = true;
		break;

	case 'd':
		dKeyDown = true;
		isRight = true;
		break;
	case 'o':
		oKeyDown = true;
		break;
	case 'p':
		pKeyDown = true;
		break;
	case'b':
		pause = !pause;
		if (pause)
		{
			state = GameStates::PAUSE;
			pauseback.scale = 3.f;
			
			cameraTransform = glm::lookAt(glm::vec3(0.f, 0.f, -25.f),
				glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f,-1.f,0.f));
			originalCameraTransform = cameraTransform;

			cameraProjection = glm::perspective(90.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.f);
			glViewport(0, 0, 20.f, 20.f);
		}
		if (!pause)
		{
			state = GameStates::PLAYING;
			background.scale = 2.f;
			cameraTransform = glm::lookAt(cameraEye,
				cameraCtr, glm::vec3(0.f, -1.f, 0.f));
			originalCameraTransform = cameraTransform;

			cameraProjection = glm::perspective(150.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.f);
		}

		break;
	default:
		break;
	}
}
void Game::keyboardUp(unsigned char key, int mouseX, int mouseY)
{
	switch (key)
	{
	case 'r':
		shouldRotate = !shouldRotate;
		break;
	case 'w':
		wKeyDown = false;
		isUp = false;
		break;
	case 'a':
		aKeyDown = false;
		isLeft = false;
		break;
	case 's':
		sKeyDown = false;
		isDown = false;
		break;
	case 'd':
		dKeyDown = false;
		isRight = false;
		break;
	case 'o':
		oKeyDown = false;
		break;
	case 'p':
		pKeyDown = false;
		break;
	case' ':
		iKeyDown = false;
		break;
	default:
		break;
	}
}

void Game::SpecialInputCallbackFunctionDown(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		upKey = true;
		shooting = true;
		break;
	case GLUT_KEY_LEFT:
		leftKey = true;
		shooting = true;
		break;
	case GLUT_KEY_DOWN:
		downKey = true;
		shooting = true;
		break;
	case GLUT_KEY_RIGHT:
		rightKey = true;
		shooting = true;
		break;
	}
}

void Game::SpecialInputCallbackFunctionUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		upKey = false;
		shooting = false;
		break;
	case GLUT_KEY_LEFT:
		leftKey = false;
		shooting = false;
		break;
	case GLUT_KEY_DOWN:
		downKey = false;
		shooting = false;
		break;
	case GLUT_KEY_RIGHT:
		rightKey = false;
		shooting = false;
		break;
	}
}
void Game::mouseClicked(int button, int state, int x, int y)
{
	mousePosition.x = x;
	mousePosition.y = y;

	mousePositionFlipped.x = x;
	mousePositionFlipped.y = WINDOW_HEIGHT - y;
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:
			//handle left click
			break;
		case GLUT_RIGHT_BUTTON:
			//handle right click
			break;
		case GLUT_MIDDLE_BUTTON:
			//handle middle click
			break;
		default:
			break;
		}
	}
}
void Game::mouseMoved(int x, int y)
{

	mousePositionFlipped.x = x;
	mousePositionFlipped.y = WINDOW_HEIGHT - y;

}
