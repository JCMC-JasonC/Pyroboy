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

	materials["unlitTexture"] = std::make_shared<ShaderProgram>();
	materials["unlitTexture"]->load("shaders/default_v.glsl", "shaders/unlitTexture_f.glsl");

	materials["depthMap"] = std::make_shared<ShaderProgram>();
	materials["depthMap"]->load("shaders/depthMap_v.glsl", "shaders/depthMap_f.glsl");

	materials["shadows"] = std::make_shared<ShaderProgram>();
	materials["shadows"]->load("shaders/shadows_v.glsl", "shaders/shadows_f.glsl");
	
	//LIGHTING
	Light light1;

	light1.positionOrDirection = glm::vec4(0.f, 10.f, -1.f, 1.f);
	light1.originalPosition = light1.positionOrDirection;
	light1.ambient = glm::vec3(0.06f,0.1f,0.06f);
	light1.diffuse = glm::vec3(0.9f);
	light1.specular = glm::vec3(0.5f);
	light1.specularExponent = 50.f;
	light1.constantAttenuation = 1.f;
	light1.linearAttenuation = 0.1f;
	light1.quadraticAttenuation = 0.01f;

	originalP1.positionOrDirection = glm::vec4(0.f, 10.f, -1.f, 1.f);
	originalP1.originalPosition = light1.positionOrDirection;
	originalP1.ambient = glm::vec3(0.06f, 0.1f, 0.06f);
	originalP1.diffuse = glm::vec3(0.9f);
	originalP1.specular = glm::vec3(0.5f);
	originalP1.specularExponent = 50.f;
	originalP1.constantAttenuation = 1.f;
	originalP1.linearAttenuation = 0.1f;
	originalP1.quadraticAttenuation = 0.01f;

	pointLights.push_back(light1);

	directionalLight.positionOrDirection = glm::vec4(-1.f, -1.f, 11.f, 0.f);
	directionalLight.ambient = glm::vec3(0.08f);
	directionalLight.diffuse = glm::vec3(0.7f);
	directionalLight.specular - glm::vec3(1.0f);
	directionalLight.specularExponent = 50.f;

	originalD.positionOrDirection = glm::vec4(-1.f, -1.f, 11.f, 0.f);
	originalD.ambient = glm::vec3(0.08f);
	originalD.diffuse = glm::vec3(0.7f);
	originalD.specular - glm::vec3(1.0f);
	originalD.specularExponent = 50.f;

	lightProjection = glm::ortho(-10.f, 10.f, -10.f,10.f, near_plane, far_plane);
	//lightProjection = glm::perspective(150.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, near_plane, far_plane);
	lightView = glm::lookAt(glm::vec3(glm::vec4(1.f, 1.f, 300.f, 0.f)), glm::vec3(0.f), glm::vec3(0.f, -1.f, 0.f));//unsure if up is 1 or -1
	lightSpaceMatrix = lightProjection * lightView;

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

	cameraProjection = glm::perspective(90.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, near_plane, far_plane);
	glViewport(0, 0, 20.f, 20.f);

	//state = GameStates::MAIN_MENU;
}
void Game::startDraw()
{

	glClearColor(0.5, 0.5, 0.5, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	startupBack.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	
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
	background.loadMesh("meshes/background.obj");
	background.loadTexture(TextureType::Diffuse,"textures/Model Textures/menu.PNG");
	background.loadTexture(TextureType::Specular, "textures/noSpecular.png");

	background.scale = 4.f;
	background.rotate = glm::rotate(background.rotate, glm::pi<float>()* 1.5f, glm::vec3(0.0f, 0.f, 1.f));
	background.rotate = glm::rotate(background.rotate, glm::pi<float>() / 2.f, glm::vec3(0.0f, 1.f, 0.f));
	background.rotate = glm::rotate(background.rotate, glm::pi<float>() / 2.f, glm::vec3(1.0f, 0.f, 0.f));

	arrow.loadMesh("meshes/arrow.obj");
	arrow.loadTexture(TextureType::Diffuse, "textures/Health.png");
	arrow.loadTexture(TextureType::Specular, "textures/noSpecular.png");
	arrow.scale = 1.f;

	arrow.position =glm::vec3(-30.f, -1.f, 0.f);
	arrow.translate = glm::translate(arrow.translate,arrow.position); //(-30f,-1f,0.f), 10,25
	arrow.rotate = glm::rotate(arrow.rotate, glm::pi<float>()*0.f, glm::vec3(0.f,0.f,1.f));
}
void Game::drawFboAttachmentToBackBuffer(FrameBufferObject& fbo, int colorAttachment, glm::vec4 clearColor)
{
	FrameBufferObject::unbindFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
	FrameBufferObject::clearFrameBuffer(clearColor);
	static auto unlitMaterial = materials["unlitTexture"];
	fbo.bindTextureForSampling(colorAttachment, GL_TEXTURE0);
	unlitMaterial->bind();
	glm::mat4 temp = glm::mat4(1.0);
	unlitMaterial->sendUniformMat4("u_mvp", glm::value_ptr(temp), false);
	//unlitMaterial->sendUniforms();
	meshes["quad"]->draw();

	fbo.unbindTexture(GL_TEXTURE0);
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

	std::shared_ptr<GameObject> treeAlert = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> playerAlert = std::make_shared<GameObject>();

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

	//Alert UI Elements
	playerAlert->loadMesh("meshes/UI.obj");
	playerAlert->loadTexture(TextureType::Diffuse, "textures/UI/UI2.png");
	playerAlert->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	treeAlert->loadMesh("meshes/UI.obj");
	treeAlert->loadTexture(TextureType::Diffuse, "textures/UI/UI3.png");
	treeAlert->loadTexture(TextureType::Specular, "textures/noSpecular.png");

	// Put them into the other UI list
	otherUI["playerAlert"] = playerAlert;
	otherUI["playerAlert"]->shouldDraw = false;

	otherUI["treeAlert"] = treeAlert;
	otherUI["treeAlert"]->shouldDraw = false;

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

	playerAlert->transform =
		glm::translate(glm::mat4(1.0f), glm::vec3(float(WINDOW_WIDTH) / 4, float(WINDOW_HEIGHT) / 2, -1.0f))
		* glm::scale(glm::mat4(1.0f), glm::vec3(300.0f, 150.f, 1.f));

	treeAlert->transform =
		glm::translate(glm::mat4(1.0f), glm::vec3(float(WINDOW_WIDTH) * 0.75, float(WINDOW_HEIGHT) / 2, -1.0f))
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
void Game::drawSceneWithShadows(ShaderProgram &shader, bool isShadowMap) 
{
	//background.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	ash			.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	barrel		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	bridge		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	brokentower .drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	cactus		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	fseast		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	fsnortheast .drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	fssoutheast .drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	fwood		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	gate		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	hill		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	hroof		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	hsand		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	hwalls		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	hwindows	.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	hwood		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	hpath1		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	hpath2		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	lamp		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	pipe		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	plane		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	planegrass	.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	planeroad	.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	rocks		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	torch		.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	tree.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	for (int i = 0; i < enemies.size(); i++)
	{
		if (!enemies[i]->getBool())
			enemies[i]->drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	}
	player.drawWithShadows(shader, cameraTransform, cameraProjection, lightSpaceMatrix, isShadowMap);
	for (int i = 0; i < bulletQ.size(); i++)
	{
		tempBullet = bulletQ.front();
		tempBullet->draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);

		bulletQ.pop();
		bulletQ.push(tempBullet);
	}
	if (!isShadowMap)
	{
		shader.sendUniform("near_plane", near_plane);
		shader.sendUniform("far_plane", far_plane);
		sceneBuffer.bindDepthTextureForSampling(GL_TEXTURE0);
	}
	else
	{
		sceneBuffer.bindTextureForSampling(0, GL_TEXTURE0);
		sceneBuffer.bindDepthTextureForSampling(GL_TEXTURE1);
		shader.sendUniform("lightPos", glm::vec3(1.f, 1.f, 300.f));
		//shader.sendUniformMat4("view", glm::value_ptr(emptyMat), false);
	}
}
void Game::drawScene()
{
	ash.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	barrel.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	bridge.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	brokentower.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	cactus.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	fseast.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	fsnortheast.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	fssoutheast.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	fwood.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	gate.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	hill.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	hroof.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	hsand.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	hwalls.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	hwindows.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	hwood.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	hpath1.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	hpath2.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	lamp.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	pipe.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	plane.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	planegrass.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	planeroad.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	rocks.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	torch.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	tree.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	for (int i = 0; i < enemies.size(); i++)
	{
		if (!enemies[i]->getBool())
			enemies[i]->draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	}
	player.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	for (int i = 0; i < bulletQ.size(); i++)
	{
		tempBullet = bulletQ.front();
		tempBullet->draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);

		bulletQ.pop();
		bulletQ.push(tempBullet);
	}
	updateAlerts();
	drawHUD();
	//materials["particles"]->bind();
	emitter.draw(player.transform, cameraTransform, cameraProjection);
	//materials["particles"]->unbind();
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

	//player.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	//tree.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);

	//drawScene();
	//drawHUD();
	drawHealth();
	sceneBuffer.bindTextureForSampling(0, GL_TEXTURE0);


	materials["bright"]->sendUniformMat4("u_mvp", glm::value_ptr(emptyMat),false);
	materials["bright"]->sendUniformMat4("u_mv", glm::value_ptr(cameraTransform), false);

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
	materials["blur"]->sendUniformMat4("u_mv", glm::value_ptr(cameraTransform), false);
	materials["blur"]->sendUniform("u_texelSize", glm::vec4(1.0 / (float)blurBuffer.getWidth(), 1.0 / (float)blurBuffer.getHeight(), 0.f, 0.f));

	meshes["quad"]->draw();

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
	bool musicmenu = false;
	se.Init();

	treefeedback1result = se.system->createSound("sounds/MyTreeWillStand.wav", FMOD_3D, 0, &treesound1);
	FmodErrorCheck(treefeedback1result);
	treefeedback1result = treesound1->set3DMinMaxDistance(0.0f, 30000.0f);
	FmodErrorCheck(treefeedback1result);

	treefeedback2result = se.system->createSound("sounds/DefendHome.wav", FMOD_3D, 0, &treesound2);
	FmodErrorCheck(treefeedback2result);
	treefeedback2result = treesound2->set3DMinMaxDistance(0.0f, 30000.0f);
	FmodErrorCheck(treefeedback2result);

	enemy1result = se.system->createSound("sounds/NothingAsh.wav", FMOD_3D, 0, &enemydialogue1);
	FmodErrorCheck(enemy1result);
	enemy1result = enemydialogue1->set3DMinMaxDistance(0.0f, 30000.0f);
	FmodErrorCheck(enemy1result);


	enemy2result = se.system->createSound("sounds/Warning.wav", FMOD_3D, 0, &enemydialogue2);
	FmodErrorCheck(enemy2result);
	enemy2result = enemydialogue2->set3DMinMaxDistance(0.0f, 30000.0f);
	FmodErrorCheck(enemy2result);


	enemy3result = se.system->createSound("sounds/Wildfire.wav", FMOD_3D, 0, &enemydialogue3);
	FmodErrorCheck(enemy3result);
	enemy3result = enemydialogue3->set3DMinMaxDistance(0.0f, 30000.0f);
	FmodErrorCheck(enemy3result);


	blast = se.system->createSound("sounds/fireball.mp3", FMOD_3D, 0, &shot);
	FmodErrorCheck(blast);
	blast = shot->set3DMinMaxDistance(0.0f, 30000.0f);
	FmodErrorCheck(blast);

	start1res = se.system->createSound("sounds/InfernoBegins.wav", FMOD_3D, 0, &start1sound);
	FmodErrorCheck(start1res);
	start1res = start1sound->set3DMinMaxDistance(0.0f, 30000.0f);
	FmodErrorCheck(start1res);

	start2res = se.system->createSound("sounds/TheyAreComing.wav", FMOD_3D, 0, &start2sound);
	FmodErrorCheck(start2res);
	start2res = start2sound->set3DMinMaxDistance(0.0f, 30000.0f);
	FmodErrorCheck(start2res);

	result = se.system->createSound("sounds/gamemusic.mp3", FMOD_3D, 0, &game);
	FmodErrorCheck(result);
	result = game->set3DMinMaxDistance(0.0f, 100.0f);
	FmodErrorCheck(result);
	result = game->setMode(FMOD_LOOP_NORMAL);

	dialogueresult = se.system->createSound("sounds/test.mp3", FMOD_3D, 0, &dialoguesound);
	FmodErrorCheck(dialogueresult);
	dialogueresult = dialoguesound->set3DMinMaxDistance(0.0f, 100.0f);
	FmodErrorCheck(dialogueresult);
	// not in sound folder, is placeholder


	scroll = se.system->createSound("sounds/scroll.mp3", FMOD_3D, 0, &scrollsound);
	FmodErrorCheck(scroll);
	scroll = scrollsound->set3DMinMaxDistance(0.0f, 30000.0f);
	FmodErrorCheck(scroll);

	selection = se.system->createSound("sounds/selectSound.wav", FMOD_3D, 0, &selectsound);
	FmodErrorCheck(selection);
	selection = selectsound->set3DMinMaxDistance(0.0f, 30000.0f);
	FmodErrorCheck(selection);

	//
	attack = se.system->createSound("sounds/enemydamage.mp3", FMOD_3D, 0, &attacksound);
	FmodErrorCheck(attack);
	attack = attacksound->set3DMinMaxDistance(0.0f, 30000.0f);
	FmodErrorCheck(attack);

	menu = se.system->createSound("sounds/menumusicEditV2.wav", FMOD_3D, 0, &menumusic);
	FmodErrorCheck(menu);
	menu = menumusic->set3DMinMaxDistance(0.0f, 30000.0f);
	FmodErrorCheck(menu);
	menu = menumusic->setMode(FMOD_LOOP_NORMAL);
	FmodErrorCheck(menu);


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
	brightPassBuffer.createFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 1, false); 
	buffer.createFrameBuffer(WINDOW_WIDTH / 16.f, WINDOW_HEIGHT / 16.f, 1, false);
	blurBuffer.createFrameBuffer(WINDOW_WIDTH / 16.f, WINDOW_HEIGHT / 16.f, 1, false);
	//depthMapFBO.createDepthBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

	initializeParticles();

	tree.loadMesh("meshes/OBJ_Tree.obj");
	tree.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Tree_diffuse.png");
	tree.loadTexture(TextureType::Specular, "textures/noSpecular.png");
	tree.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Tree_normal_map.png");

	float ar[2];
	ar[0] = 25.f;
	ar[1] = -37.f;
	int x = 0;

	
	// New Models
	ash.loadMesh("meshes/OBJ_Ash.obj");
	ash.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Ash_diffuse.png");
	ash.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	ash.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Ash_normal_map.png");

	barrel.loadMesh("meshes/OBJ_Barrel.obj");
	barrel.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Barrel_diffuse.png");
	barrel.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	barrel.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Barrel_normal_map.png");

	bridge.loadMesh("meshes/OBJ_Bridge.obj");
	bridge.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Bridge_diffuse.png");
	bridge.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	bridge.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Bridge_normal_map.png");

	brokentower.loadMesh("meshes/OBJ_BrokenTower.obj");
	brokentower.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_BrokenTower_diffuse.png");
	brokentower.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	brokentower.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_BrokenTower_normal_map.png");

	cactus.loadMesh("meshes/OBJ_Cactus.obj");
	cactus.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Cactus_diffuse.png");
	cactus.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	cactus.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Cactus_normal_map.png");

	fseast.loadMesh("meshes/OBJ_FenceSteel_East.obj");
	fseast.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_FenceSteel_East_diffuse.png");
	fseast.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	fseast.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_FenceSteel_East_normal_map.png");

	fsnortheast.loadMesh("meshes/OBJ_FenceSteel_NorthEast.obj");
	fsnortheast.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_FenceSteel_NorthEast_diffuse.png");
	fsnortheast.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	fsnortheast.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_FenceSteel_NorthEast_normal_map.png");

	fssoutheast.loadMesh("meshes/OBJ_FenceSteel_SouthEast.obj");
	fssoutheast.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_FenceSteel_SouthEast_diffuse.png");
	fssoutheast.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	fssoutheast.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_FenceSteel_SouthEast_normal_map.png");

	fwood.loadMesh("meshes/OBJ_FenceWood.obj");
	fwood.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_FenceWood_diffuse.png");
	fwood.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	fwood.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_FenceWood_normal_map.png");

	gate.loadMesh("meshes/OBJ_Gate.obj");
	gate.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Gate_diffuse.png");
	gate.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	gate.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Gate_normal_map.png");

	hill.loadMesh("meshes/OBJ_Hill.obj");
	hill.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Hill_diffuse.png");
	hill.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	hill.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Hill_normal_map.png");

	hpath1.loadMesh("meshes/OBJ_Path1.obj");
	hpath1.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Path1_diffuse.png");
	hpath1.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	hpath1.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Path1_normal_map.png");

	hpath2.loadMesh("meshes/OBJ_Path2.obj");
	hpath2.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Path2_diffuse.png");
	hpath2.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	hpath2.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Path2_normal_map.png");

	hroof.loadMesh("meshes/OBJ_Roof.obj");
	hroof.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Roof_diffuse.png");
	hroof.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	hroof.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Roof_normal_map.png");

	hsand.loadMesh("meshes/OBJ_House_Sand.obj");
	hsand.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_House_Sand_diffuse.png");
	hsand.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	hsand.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_House_Sand_normal_map.png");

	hwalls.loadMesh("meshes/OBJ_House_Walls.obj");
	hwalls.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_House_Walls_diffuse.png");
	hwalls.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	hwalls.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_House_Walls_normal_map.png");

	hwindows.loadMesh("meshes/OBJ_House_Windows.obj");
	hwindows.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_House_Windows_diffuse.png");
	hwindows.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	hwindows.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_House_Windows_normal_map.png");

	hwood.loadMesh("meshes/OBJ_House_Wood.obj");
	hwood.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_House_Wood_diffuse.png");
	hwood.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	hwood.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_House_Wood_normal_map.png");

	lamp.loadMesh("meshes/OBJ_Lamp.obj");
	lamp.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Lamp_diffuse.png");
	lamp.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	lamp.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Lamp_normal_map.png");

	pipe.loadMesh("meshes/OBJ_Pipe.obj");
	pipe.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Pipe_diffuse.png");
	pipe.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	pipe.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Pipe_normal_map.png");

	plane.loadMesh("meshes/OBJ_Plane.obj");
	plane.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Plane_diffuse.png");
	plane.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	plane.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Plane_normal_map.png");

	planegrass.loadMesh("meshes/OBJ_Plane_Grass.obj");
	planegrass.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Plane_Grass_diffuse.png");
	planegrass.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	planegrass.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Plane_Grass_normal_map.png");

	planeroad.loadMesh("meshes/OBJ_Plane_Road.obj");
	planeroad.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Plane_Road_diffuse.png");
	planeroad.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	planeroad.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Plane_Road_normal_map.png");

	rocks.loadMesh("meshes/OBJ_Rocks.obj");
	rocks.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Rocks_diffuse.png");
	rocks.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	rocks.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Rocks_normal_map.png");

	torch.loadMesh("meshes/OBJ_Torch.obj");
	torch.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Torch_diffuse.png");
	torch.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	torch.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Torch_normal_map.png");

	water.loadMesh("meshes/OBJ_Water.obj");
	water.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_Water_diffuse.png");
	water.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	water.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_Water_normal_map.png");

	wirepole.loadMesh("meshes/OBJ_WirePole.obj");
	wirepole.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_WirePole_diffuse.png");
	wirepole.loadTexture(TextureType::Specular, "textures/noSpecular.png"); ///////////
	wirepole.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_WirePole_normal_map.png");

	watertower.loadMesh("meshes/OBJ_WaterTower.obj");
	watertower.loadTexture(TextureType::Diffuse, "textures/Model Textures/OBJ_WaterTower_diffuse.png");
	watertower.loadTexture(TextureType::Specular, "textures/noSpecular.png");
	watertower.loadTexture(TextureType::Normal, "textures/Model Textures/OBJ_WaterTower_normal_map.png"); ///////////

	//Set New Model Properties
	ash.transform = ash.translate * ash.rotate * glm::scale(glm::mat4(), glm::vec3(ash.scale));
	barrel.transform = barrel.translate * barrel.rotate * glm::scale(glm::mat4(), glm::vec3(barrel.scale));
	bridge.transform = bridge.translate * bridge.rotate * glm::scale(glm::mat4(), glm::vec3(bridge.scale));
	brokentower.transform = brokentower.translate * brokentower.rotate * glm::scale(glm::mat4(), glm::vec3(brokentower.scale));
	cactus.transform = cactus.translate * cactus.rotate * glm::scale(glm::mat4(), glm::vec3(cactus.scale));
	fseast.transform = fseast.translate * fseast.rotate * glm::scale(glm::mat4(), glm::vec3(fseast.scale));
	fsnortheast.transform = fsnortheast.translate * fsnortheast.rotate * glm::scale(glm::mat4(), glm::vec3(fsnortheast.scale));
	fssoutheast.transform = fssoutheast.translate * fssoutheast.rotate * glm::scale(glm::mat4(), glm::vec3(fssoutheast.scale));
	fwood.transform = fwood.translate * fwood.rotate * glm::scale(glm::mat4(), glm::vec3(fwood.scale));
	gate.transform = gate.translate * gate.rotate * glm::scale(glm::mat4(), glm::vec3(gate.scale));
	hill.transform = hill.translate * hill.rotate * glm::scale(glm::mat4(), glm::vec3(hill.scale));
	hroof.transform = hroof.translate * hroof.rotate * glm::scale(glm::mat4(), glm::vec3(hroof.scale));
	hsand.transform = hsand.translate * hsand.rotate * glm::scale(glm::mat4(), glm::vec3(hsand.scale));
	hwalls.transform = hwalls.translate * hwalls.rotate * glm::scale(glm::mat4(), glm::vec3(hwalls.scale));
	hwindows.transform = hwindows.translate * hwindows.rotate * glm::scale(glm::mat4(), glm::vec3(hwindows.scale));
	hwood.transform = hwood.translate * hwood.rotate * glm::scale(glm::mat4(), glm::vec3(hwood.scale));
	hpath1.transform = hpath1.translate * hpath1.rotate * glm::scale(glm::mat4(), glm::vec3(hpath1.scale));
	hpath2.transform = hpath2.translate * hpath2.rotate * glm::scale(glm::mat4(), glm::vec3(hpath2.scale));
	lamp.transform = lamp.translate * lamp.rotate * glm::scale(glm::mat4(), glm::vec3(lamp.scale));
	pipe.transform = pipe.translate * pipe.rotate * glm::scale(glm::mat4(), glm::vec3(pipe.scale));
	plane.transform = plane.translate * plane.rotate * glm::scale(glm::mat4(), glm::vec3(plane.scale));
	planegrass.transform = planegrass.translate * planegrass.rotate * glm::scale(glm::mat4(), glm::vec3(planegrass.scale));
	planeroad.transform = planeroad.translate * planeroad.rotate * glm::scale(glm::mat4(), glm::vec3(planeroad.scale));
	rocks.transform = rocks.translate * rocks.rotate * glm::scale(glm::mat4(), glm::vec3(rocks.scale));
	torch.transform = torch.translate * torch.rotate * glm::scale(glm::mat4(), glm::vec3(torch.scale));
	water.transform = water.translate * water.rotate * glm::scale(glm::mat4(), glm::vec3(water.scale));
	watertower.transform = watertower.translate * watertower.rotate * glm::scale(glm::mat4(), glm::vec3(watertower.scale));
	wirepole.transform = wirepole.translate * wirepole.rotate * glm::scale(glm::mat4(), glm::vec3(wirepole.scale));

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
	
	// Load in animation Frames
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
		
	//for (int i = 0; i <= 25; i++)
	//{
	//	std::string name = "player" + std::to_string(i);
	//	meshes[name] = std::make_shared<Mesh>();
	//	meshes[name]->loadFromFile("meshes/PyroboyAnim" + std::to_string(i + 1) + ".obj");
	//}

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
	player.playerMesh.push_back(run20);
	player.playerMesh.push_back(run21);
	player.playerMesh.push_back(run22);
	player.playerMesh.push_back(run23);
	player.playerMesh.push_back(run24);
	player.playerMesh.push_back(run25);
	player.playerMesh.push_back(run26);

	player.loadTexture(TextureType::Diffuse, "textures/Model Textures/pyroboy_flipped.png");
	player.loadTexture(TextureType::Specular, "textures/noSpecular.png");

	player_health = player_start_health; // player starting health
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

	// Cleaning up block of UI code
	initUIObjects();

	//ENEMY POSITIONS
	enemy.position = glm::vec3 (-400.f,  0.f,   0.f);
	enemy.position = glm::vec3 (-400.f,  0.f,   -2.f);
	enemy2.position = glm::vec3( 100.f, -400.f, -2.f);
	enemy3.position = glm::vec3(-250.f,  100.f, -2.f);
	enemy4.position = glm::vec3( 0.f,   -350.f, -2.f);
	enemy5.position = glm::vec3( 400.f,  0.f,   -2.f);
	enemy6.position = glm::vec3( 0.f,    400.f, -2.f);

	enemy7.position = glm::vec3(-400.f, 400.f, -2.f);
	enemy8.position = glm::vec3(200.f, 310.f, -2.f);
	enemy9.position = glm::vec3(-300.f, 0.f, -2.f);
	enemy10.position = glm::vec3(300.f, 0.f, -2.f);
	enemy11.position = glm::vec3(309.f, -400.f, -2.f);
	enemy12.position = glm::vec3(230.f, 180.f, -2.f);
	enemy13.position = glm::vec3(-180.f, 230.f, -2.f);
	enemy14.position = glm::vec3(-240.f, -400.f, -2.f);

	enemy15.position = glm::vec3(-800.f, 20.f, -2.f);
	enemy16.position = glm::vec3(1000.f, -880.f, -2.f);
	enemy17.position = glm::vec3(-950.f, 700.f, -2.f);
	enemy18.position = glm::vec3(0.f, -950.f, -2.f);
	enemy19.position = glm::vec3(755.f, 0.f, -2.f);
	enemy20.position = glm::vec3(0.f, 650.f, -2.f);

	enemy21.position = glm::vec3(-850.f, 0.f, -2.f);
	enemy22.position = glm::vec3(400.f, 610.f, -2.f);
	enemy23.position = glm::vec3(-740.f, 0.f, -2.f);
	enemy24.position = glm::vec3(430.f, 0.f, -2.f);
	enemy25.position = glm::vec3(30.f, -840.f, -2.f);
	enemy26.position = glm::vec3(10.f, 680.f, -2.f);
	enemy27.position = glm::vec3(-580.f, 30.f, -2.f);
	enemy28.position = glm::vec3(-740.f, -40.f, -2.f);

	// Insect
	insect.position = glm::vec3( 300.f,  100.f, -2.f);
	insect2.position = glm::vec3(-100.f, 300.f, -2.f);
	insect3.position = glm::vec3(250.f,  100.f, -2.f);
	insect4.position = glm::vec3(0.f,    350.f, -2.f);
	insect5.position = glm::vec3(-400.f, 0.f,   -2.f);
	insect6.position = glm::vec3(0.f,   -400.f, -2.f);

	insect7.position = glm::vec3(-100.f, -200.f, -2.f);
	insect8.position = glm::vec3(140.f, 140.f, -2.f);
	insect9.position = glm::vec3(-140.f, -140.f, -2.f);
	insect10.position = glm::vec3(190.f, 450.f, -2.f);
	insect11.position = glm::vec3(405.f, -300.f, -2.f);
	insect12.position = glm::vec3(450.f, 300.f, -2.f);
	insect13.position = glm::vec3(-360.f, -220.f, -2.f);
	insect14.position = glm::vec3(0.f, -350.f, -2.f);

	insect15.position = glm::vec3(-800.f, 0.f, -2.f);
	insect16.position = glm::vec3(500.f, -800.f, -2.f);
	insect17.position = glm::vec3(-750.f, 900.f, -2.f);
	insect18.position = glm::vec3(0.f, -850.f, -2.f);
	insect19.position = glm::vec3(600.f, 1000.f, -2.f);
	insect20.position = glm::vec3(0.f, 940.f, -2.f);

	insect21.position = glm::vec3(-800.f, 800.f, -2.f);
	insect22.position = glm::vec3(600.f, 610.f, -2.f);
	insect23.position = glm::vec3(-670.f, 770.f, -2.f);
	insect24.position = glm::vec3(859.f, 0.f, -2.f);
	insect25.position = glm::vec3(679.f, -740.f, -2.f);
	insect26.position = glm::vec3(730.f, 880.f, -2.f);
	insect27.position = glm::vec3(-680.f, 930.f, -2.f);
	insect28.position = glm::vec3(-1040.f, -980.f, -2.f);

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
	// Aliens
	enemies.push_back(&enemy);
	enemies.push_back(&enemy3);
	enemies.push_back(&enemy5);
	enemies.push_back(&enemy2);
	enemies.push_back(&enemy4);
	enemies.push_back(&enemy6);

	enemies.push_back(&enemy7);
	enemies.push_back(&enemy8);
	enemies.push_back(&enemy9);
	enemies.push_back(&enemy10);
	enemies.push_back(&enemy11);
	enemies.push_back(&enemy12);
	enemies.push_back(&enemy13);
	enemies.push_back(&enemy14);

	// Insects
	enemies.push_back(&insect);
	enemies.push_back(&insect2);
	enemies.push_back(&insect3);
	enemies.push_back(&insect4);
	enemies.push_back(&insect5);
	enemies.push_back(&insect6);

	enemies.push_back(&insect7);
	enemies.push_back(&insect8);
	enemies.push_back(&insect9);
	enemies.push_back(&insect10);
	enemies.push_back(&insect11);
	enemies.push_back(&insect12);
	enemies.push_back(&insect13);
	enemies.push_back(&insect14);

	// Frames
	e_run1.loadFromFile("meshes/Enemy/Alien (1).obj");
	e_run2.loadFromFile("meshes/Enemy/Alien (2).obj");
	e_run3.loadFromFile("meshes/Enemy/Alien (3).obj");
	e_run4.loadFromFile("meshes/Enemy/Alien (4).obj");
	e_run5.loadFromFile("meshes/Enemy/Alien (5).obj");
	e_run6.loadFromFile("meshes/Enemy/Alien (6).obj");
	e_run7.loadFromFile("meshes/Enemy/Alien (7).obj");
	e_run8.loadFromFile("meshes/Enemy/Alien (8).obj");
	e_run9.loadFromFile("meshes/Enemy/Alien (9).obj");
	e_run10.loadFromFile("meshes/Enemy/Alien (10).obj");
	e_run11.loadFromFile("meshes/Enemy/Alien (11).obj");
	e_run12.loadFromFile("meshes/Enemy/Alien (12).obj");
	e_run13.loadFromFile("meshes/Enemy/Alien (13).obj");
	e_run14.loadFromFile("meshes/Enemy/Alien (14).obj");
	e_run15.loadFromFile("meshes/Enemy/Alien (15).obj");
	e_run16.loadFromFile("meshes/Enemy/Alien (16).obj");
	e_run17.loadFromFile("meshes/Enemy/Alien (17).obj");
	e_run18.loadFromFile("meshes/Enemy/Alien (18).obj");
	e_run19.loadFromFile("meshes/Enemy/Alien (19).obj");
	e_run20.loadFromFile("meshes/Enemy/Alien (20).obj");
	e_run21.loadFromFile("meshes/Enemy/Alien (21).obj");
	e_run22.loadFromFile("meshes/Enemy/Alien (22).obj");
	e_run23.loadFromFile("meshes/Enemy/Alien (23).obj");
	e_run24.loadFromFile("meshes/Enemy/Alien (24).obj");
	e_run25.loadFromFile("meshes/Enemy/Alien (25).obj");
	e_run26.loadFromFile("meshes/Enemy/Alien (26).obj");
	e_run27.loadFromFile("meshes/Enemy/Alien (27).obj");
	e_run28.loadFromFile("meshes/Enemy/Alien (28).obj");
	e_run29.loadFromFile("meshes/Enemy/Alien (29).obj");
	e_run30.loadFromFile("meshes/Enemy/Alien (30).obj");
	e_run31.loadFromFile("meshes/Enemy/Alien (31).obj");
	e_run32.loadFromFile("meshes/Enemy/Alien (32).obj");
	e_run33.loadFromFile("meshes/Enemy/Alien (33).obj");
	e_run34.loadFromFile("meshes/Enemy/Alien (34).obj");
	e_run35.loadFromFile("meshes/Enemy/Alien (35).obj");
	e_run36.loadFromFile("meshes/Enemy/Alien (36).obj");
	e_run37.loadFromFile("meshes/Enemy/Alien (37).obj");
	e_run38.loadFromFile("meshes/Enemy/Alien (38).obj");
	e_run39.loadFromFile("meshes/Enemy/Alien (39).obj");

	// Frames
	e_attack1.loadFromFile("meshes/Enemy/Attack (1).obj");
	e_attack2.loadFromFile("meshes/Enemy/Attack (2).obj");
	e_attack3.loadFromFile("meshes/Enemy/Attack (3).obj");
	e_attack4.loadFromFile("meshes/Enemy/Attack (4).obj");
	e_attack5.loadFromFile("meshes/Enemy/Attack (5).obj");
	e_attack6.loadFromFile("meshes/Enemy/Attack (6).obj");
	e_attack7.loadFromFile("meshes/Enemy/Attack (7).obj");
	e_attack8.loadFromFile("meshes/Enemy/Attack (8).obj");
	e_attack9.loadFromFile("meshes/Enemy/Attack (9).obj");
	e_attack10.loadFromFile("meshes/Enemy/Attack (10).obj");
	e_attack11.loadFromFile("meshes/Enemy/Attack (11).obj");
	e_attack12.loadFromFile("meshes/Enemy/Attack (12).obj");
	e_attack13.loadFromFile("meshes/Enemy/Attack (13).obj");
	e_attack14.loadFromFile("meshes/Enemy/Attack (14).obj");
	e_attack15.loadFromFile("meshes/Enemy/Attack (15).obj");
	e_attack16.loadFromFile("meshes/Enemy/Attack (16).obj");
	e_attack17.loadFromFile("meshes/Enemy/Attack (17).obj");
	e_attack18.loadFromFile("meshes/Enemy/Attack (18).obj");
	e_attack19.loadFromFile("meshes/Enemy/Attack (19).obj");
	e_attack20.loadFromFile("meshes/Enemy/Attack (20).obj");
	e_attack21.loadFromFile("meshes/Enemy/Attack (21).obj");
	e_attack22.loadFromFile("meshes/Enemy/Attack (22).obj");
	e_attack23.loadFromFile("meshes/Enemy/Attack (23).obj");
	e_attack24.loadFromFile("meshes/Enemy/Attack (24).obj");
	e_attack25.loadFromFile("meshes/Enemy/Attack (25).obj");
	e_attack26.loadFromFile("meshes/Enemy/Attack (26).obj");
	e_attack27.loadFromFile("meshes/Enemy/Attack (27).obj");
	e_attack28.loadFromFile("meshes/Enemy/Attack (28).obj");
	e_attack29.loadFromFile("meshes/Enemy/Attack (29).obj");
	e_attack30.loadFromFile("meshes/Enemy/Attack (30).obj");
	e_attack31.loadFromFile("meshes/Enemy/Attack (31).obj");
	e_attack32.loadFromFile("meshes/Enemy/Attack (32).obj");
	e_attack33.loadFromFile("meshes/Enemy/Attack (33).obj");
	e_attack34.loadFromFile("meshes/Enemy/Attack (34).obj");
	e_attack35.loadFromFile("meshes/Enemy/Attack (35).obj");
	e_attack36.loadFromFile("meshes/Enemy/Attack (36).obj");
	e_attack37.loadFromFile("meshes/Enemy/Attack (37).obj");
	e_attack38.loadFromFile("meshes/Enemy/Attack (38).obj");

	//set enemies health and damage, respectively
	//aliens
	for (int i = 0; i < (enemies.size()/2)/2; i++)
	{
		enemies[i]->morph.loadMesh("meshes/Enemy/Alien (1).obj");
		enemies[i]->morph.loadTexture(TextureType::Diffuse, "textures/Model Textures/Enemy_Alien_Diffuse.png");
		enemies[i]->morph.loadTexture(TextureType::Specular, "textures/noSpecular.png");

		enemies[i]->enemyMesh.push_back(e_run1);
		enemies[i]->enemyMesh.push_back(e_run2);
		enemies[i]->enemyMesh.push_back(e_run3);
		enemies[i]->enemyMesh.push_back(e_run4);
		enemies[i]->enemyMesh.push_back(e_run5);
		enemies[i]->enemyMesh.push_back(e_run6);
		enemies[i]->enemyMesh.push_back(e_run7);
		enemies[i]->enemyMesh.push_back(e_run8);
		enemies[i]->enemyMesh.push_back(e_run9);
		enemies[i]->enemyMesh.push_back(e_run10);
		enemies[i]->enemyMesh.push_back(e_run11);
		enemies[i]->enemyMesh.push_back(e_run12);
		enemies[i]->enemyMesh.push_back(e_run13);
		enemies[i]->enemyMesh.push_back(e_run14);
		enemies[i]->enemyMesh.push_back(e_run15);
		enemies[i]->enemyMesh.push_back(e_run16);
		enemies[i]->enemyMesh.push_back(e_run17);
		enemies[i]->enemyMesh.push_back(e_run18);
		enemies[i]->enemyMesh.push_back(e_run19);
		enemies[i]->enemyMesh.push_back(e_run20);
		enemies[i]->enemyMesh.push_back(e_run21);
		enemies[i]->enemyMesh.push_back(e_run22);
		enemies[i]->enemyMesh.push_back(e_run23);
		enemies[i]->enemyMesh.push_back(e_run24);
		enemies[i]->enemyMesh.push_back(e_run25);
		enemies[i]->enemyMesh.push_back(e_run26);
		enemies[i]->enemyMesh.push_back(e_run27);
		enemies[i]->enemyMesh.push_back(e_run28);
		enemies[i]->enemyMesh.push_back(e_run29);
		enemies[i]->enemyMesh.push_back(e_run30);
		enemies[i]->enemyMesh.push_back(e_run31);
		enemies[i]->enemyMesh.push_back(e_run32);
		enemies[i]->enemyMesh.push_back(e_run33);
		enemies[i]->enemyMesh.push_back(e_run34);
		enemies[i]->enemyMesh.push_back(e_run35);
		enemies[i]->enemyMesh.push_back(e_run36);
		enemies[i]->enemyMesh.push_back(e_run37);
		enemies[i]->enemyMesh.push_back(e_run38);
		enemies[i]->enemyMesh.push_back(e_run39);

		enemies[i]->enemyAttackMesh.push_back(e_attack1);
		enemies[i]->enemyAttackMesh.push_back(e_attack2);
		enemies[i]->enemyAttackMesh.push_back(e_attack3);
		enemies[i]->enemyAttackMesh.push_back(e_attack4);
		enemies[i]->enemyAttackMesh.push_back(e_attack5);
		enemies[i]->enemyAttackMesh.push_back(e_attack6);
		enemies[i]->enemyAttackMesh.push_back(e_attack7);
		enemies[i]->enemyAttackMesh.push_back(e_attack8);
		enemies[i]->enemyAttackMesh.push_back(e_attack9);
		enemies[i]->enemyAttackMesh.push_back(e_attack10);
		enemies[i]->enemyAttackMesh.push_back(e_attack11);
		enemies[i]->enemyAttackMesh.push_back(e_attack12);
		enemies[i]->enemyAttackMesh.push_back(e_attack13);
		enemies[i]->enemyAttackMesh.push_back(e_attack14);
		enemies[i]->enemyAttackMesh.push_back(e_attack15);
		enemies[i]->enemyAttackMesh.push_back(e_attack16);
		enemies[i]->enemyAttackMesh.push_back(e_attack17);
		enemies[i]->enemyAttackMesh.push_back(e_attack18);
		enemies[i]->enemyAttackMesh.push_back(e_attack19);
		enemies[i]->enemyAttackMesh.push_back(e_attack20);
		enemies[i]->enemyAttackMesh.push_back(e_attack21);
		enemies[i]->enemyAttackMesh.push_back(e_attack22);
		enemies[i]->enemyAttackMesh.push_back(e_attack23);
		enemies[i]->enemyAttackMesh.push_back(e_attack24);
		enemies[i]->enemyAttackMesh.push_back(e_attack25);
		enemies[i]->enemyAttackMesh.push_back(e_attack26);
		enemies[i]->enemyAttackMesh.push_back(e_attack27);
		enemies[i]->enemyAttackMesh.push_back(e_attack28);
		enemies[i]->enemyAttackMesh.push_back(e_attack29);
		enemies[i]->enemyAttackMesh.push_back(e_attack30);
		enemies[i]->enemyAttackMesh.push_back(e_attack31);
		enemies[i]->enemyAttackMesh.push_back(e_attack32);
		enemies[i]->enemyAttackMesh.push_back(e_attack33);
		enemies[i]->enemyAttackMesh.push_back(e_attack34);
		enemies[i]->enemyAttackMesh.push_back(e_attack35);
		enemies[i]->enemyAttackMesh.push_back(e_attack36);
		enemies[i]->enemyAttackMesh.push_back(e_attack37);
		enemies[i]->enemyAttackMesh.push_back(e_attack38);

		enemies[i]->morph.scale = 1.2f;
		enemies[i]->setProperty(50.f, 1.f);
	}
	//explosive insect v1
	for (int i = (enemies.size() / 2) / 2; i < (enemies.size() / 2); i++)
	{
		enemies[i]->morph.loadMesh("meshes/GDW_ExpInsect.obj");
		enemies[i]->morph.loadTexture(TextureType::Diffuse, "textures/Health.png");
		enemies[i]->morph.loadTexture(TextureType::Specular, "textures/noSpecular.png");
		enemies[i]->morph.scale = 1.8f;
		enemies[i]->setProperty(50.f, 1.f);

	}
	//explosive insect v2
	for (int i = (enemies.size() / 2); i < enemies.size(); i++)
	{
		enemies[i]->morph.loadMesh("meshes/InsectPrototype.obj");
		enemies[i]->morph.loadTexture(TextureType::Diffuse, "textures/Model Textures/GDW_ExpInsectTexture.png");
		enemies[i]->morph.loadTexture(TextureType::Specular, "textures/noSpecular.png");
		enemies[i]->scale=10.f;
		enemies[i]->setProperty(10.f, 1.f);
	}

	//enemies[0]->loadTexture(TextureType::Diffuse, "textures/Model Textures/alien_PNG.png");
	//enemies[0]->loadTexture(TextureType::Specular, "textures/noSpecular.png");
	//enemies[0]->scale = 1.2f;
	//enemies[0]->setProperty(50.f, 1.f);

	// BG Stuff
	background.scale = 2.0f;
	background.position = glm::vec3(-2.0f * background.scale, -8.0f, 1.0f);
	background.translate = glm::translate(background.translate, background.position);
	background.rotate = glm::rotate(background.rotate, -glm::pi<float>() / 2.f, glm::vec3(1.f, 0.f, 0.f));
	background.rotate = glm::rotate(background.rotate, -0.002f, glm::vec3(1.f, 0.f, 0.f));
	background.transform = background.translate * background.rotate * glm::scale(glm::mat4(), glm::vec3(background.scale));
	 
	pauseback.scale = 10.f;
	pauseback.rotate = glm::rotate(pauseback.rotate, glm::pi<float>() *1.5f, glm::vec3(1.0f, 0.f, 0.f));
	pauseback.transform = pauseback.translate * pauseback.rotate * glm::scale(glm::mat4(), glm::vec3(pauseback.scale));

	tree.position = glm::vec3(0.0001f,-0.2f,0.f);
	tree_health = tree_start_health; // tree starting health
	tree.scale = 2.5f;
	tree.rotate = glm::rotate(tree.rotate, glm::pi<float>() /-2.f, glm::vec3(1.0f, 0.f, 0.f));
	tree.translate = glm::translate(tree.translate, tree.position);
	tree.transform = tree.translate * tree.rotate * glm::scale(glm::mat4(), glm::vec3(tree.scale));

	ash.position = glm::vec3(0.f, 0.f, 0.f);
	ash.scale = 2.f;
	ash.rotate = glm::rotate(ash.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	ash.translate = glm::translate(ash.translate, ash.position);
	ash.transform = ash.translate * ash.rotate * glm::scale(glm::mat4(), glm::vec3(ash.scale));

	barrel.position = glm::vec3(0.f, 0.f, 0.f);
	barrel.scale = 2.f;
	barrel.rotate = glm::rotate(barrel.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	barrel.translate = glm::translate(barrel.translate, barrel.position);
	barrel.transform = barrel.translate * barrel.rotate * glm::scale(glm::mat4(), glm::vec3(barrel.scale));

	bridge.position = glm::vec3(0.f, 0.f, 0.f);
	bridge.scale = 2.f;
	bridge.rotate = glm::rotate(bridge.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	bridge.translate = glm::translate(bridge.translate, bridge.position);
	bridge.transform = bridge.translate * bridge.rotate * glm::scale(glm::mat4(), glm::vec3(bridge.scale));

	brokentower.position = glm::vec3(0.f, 0.f, 0.f);
	brokentower.scale = 2.f;
	brokentower.rotate = glm::rotate(brokentower.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	brokentower.translate = glm::translate(brokentower.translate, brokentower.position);
	brokentower.transform = brokentower.translate * brokentower.rotate * glm::scale(glm::mat4(), glm::vec3(brokentower.scale));

	cactus.position = glm::vec3(0.f, 0.f, 0.f);
	cactus.scale = 2.f;
	cactus.rotate = glm::rotate(cactus.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	cactus.translate = glm::translate(cactus.translate, cactus.position);
	cactus.transform = cactus.translate * cactus.rotate * glm::scale(glm::mat4(), glm::vec3(cactus.scale));

	fseast.position = glm::vec3(0.f, 0.f, 0.f);
	fseast.scale = 2.f;
	fseast.rotate = glm::rotate(fseast.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	fseast.translate = glm::translate(fseast.translate, fseast.position);
	fseast.transform = fseast.translate * fseast.rotate * glm::scale(glm::mat4(), glm::vec3(fseast.scale));

	fsnortheast.position = glm::vec3(0.f, 0.f, 0.f);
	fsnortheast.scale = 2.f;
	fsnortheast.rotate = glm::rotate(fsnortheast.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	fsnortheast.translate = glm::translate(fsnortheast.translate, fsnortheast.position);
	fsnortheast.transform = fsnortheast.translate * fsnortheast.rotate * glm::scale(glm::mat4(), glm::vec3(fsnortheast.scale));

	fssoutheast.position = glm::vec3(0.f, 0.f, 0.f);
	fssoutheast.scale = 2.f;
	fssoutheast.rotate = glm::rotate(fssoutheast.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	fssoutheast.translate = glm::translate(fssoutheast.translate, fssoutheast.position);
	fssoutheast.transform = fssoutheast.translate * fssoutheast.rotate * glm::scale(glm::mat4(), glm::vec3(fssoutheast.scale));

	fwood.position = glm::vec3(0.f, 0.f, 0.f);
	fwood.scale = 2.f;
	fwood.rotate = glm::rotate(fwood.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	fwood.translate = glm::translate(fwood.translate, fwood.position);
	fwood.transform = fwood.translate * fwood.rotate * glm::scale(glm::mat4(), glm::vec3(fwood.scale));

	gate.position = glm::vec3(0.f, 0.f, 0.f);
	gate.scale = 2.f;
	gate.rotate = glm::rotate(gate.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	gate.translate = glm::translate(gate.translate, gate.position);
	gate.transform = gate.translate * gate.rotate * glm::scale(glm::mat4(), glm::vec3(gate.scale));

	hill.position = glm::vec3(0.f, 0.f, 0.f);
	hill.scale = 2.f;
	hill.rotate = glm::rotate(hill.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	hill.translate = glm::translate(hill.translate, hill.position);
	hill.transform = hill.translate * hill.rotate * glm::scale(glm::mat4(), glm::vec3(hill.scale));

	hroof.position = glm::vec3(0.f, 0.f, 0.f);
	hroof.scale = 2.f;
	hroof.rotate = glm::rotate(hroof.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	hroof.translate = glm::translate(hroof.translate, hroof.position);
	hroof.transform = hroof.translate * hroof.rotate * glm::scale(glm::mat4(), glm::vec3(hroof.scale));

	hsand.position = glm::vec3(0.f, 0.f, 0.f);
	hsand.scale = 2.f;
	hsand.rotate = glm::rotate(hsand.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	hsand.translate = glm::translate(hsand.translate, hsand.position);
	hsand.transform = hsand.translate * hsand.rotate * glm::scale(glm::mat4(), glm::vec3(hsand.scale));

	hwalls.position = glm::vec3(0.f, 0.f, 0.f);
	hwalls.scale = 2.f;
	hwalls.rotate = glm::rotate(hwalls.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	hwalls.translate = glm::translate(hwalls.translate, hwalls.position);
	hwalls.transform = hwalls.translate * hwalls.rotate * glm::scale(glm::mat4(), glm::vec3(hwalls.scale));

	hwindows.position = glm::vec3(0.f, 0.f, 0.f);
	hwindows.scale = 2.f;
	hwindows.rotate = glm::rotate(hwindows.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	hwindows.translate = glm::translate(hwindows.translate, hwindows.position);
	hwindows.transform = hwindows.translate * hwindows.rotate * glm::scale(glm::mat4(), glm::vec3(hwindows.scale));

	hwood.position = glm::vec3(0.f, 0.f, 0.f);
	hwood.scale = 2.f;
	hwood.rotate = glm::rotate(hwood.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	hwood.translate = glm::translate(hwood.translate, hwood.position);
	hwood.transform = hwood.translate * hwood.rotate * glm::scale(glm::mat4(), glm::vec3(hwood.scale));

	hpath1.position = glm::vec3(0.f, 0.f, 0.f);
	hpath1.scale = 2.f;
	hpath1.rotate = glm::rotate(hpath1.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	hpath1.translate = glm::translate(hpath1.translate, hpath1.position);
	hpath1.transform = hpath1.translate * hpath1.rotate * glm::scale(glm::mat4(), glm::vec3(hpath1.scale));

	hpath2.position = glm::vec3(0.f, 0.f, 0.f);
	hpath2.scale = 2.f;
	hpath2.rotate = glm::rotate(hpath2.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	hpath2.translate = glm::translate(hpath2.translate, hpath2.position);
	hpath2.transform = hpath2.translate * hpath2.rotate * glm::scale(glm::mat4(), glm::vec3(hpath2.scale));

	lamp.position = glm::vec3(0.f, 0.f, 0.f);
	lamp.scale = 2.f;
	lamp.rotate = glm::rotate(lamp.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	lamp.translate = glm::translate(lamp.translate, lamp.position);
	lamp.transform = lamp.translate * lamp.rotate * glm::scale(glm::mat4(), glm::vec3(lamp.scale));

	pipe.position = glm::vec3(0.f, 0.f, 0.f);
	pipe.scale = 2.f;
	pipe.rotate = glm::rotate(pipe.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	pipe.translate = glm::translate(pipe.translate, pipe.position);
	pipe.transform = pipe.translate * pipe.rotate * glm::scale(glm::mat4(), glm::vec3(pipe.scale));

	plane.position = glm::vec3(0.f, 0.f, 0.f);
	plane.scale = 2.f;
	plane.rotate = glm::rotate(plane.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	plane.translate = glm::translate(plane.translate, plane.position);
	plane.transform = plane.translate * plane.rotate * glm::scale(glm::mat4(), glm::vec3(plane.scale));

	planegrass.position = glm::vec3(0.f, 0.f, 0.f);
	planegrass.scale = 2.f;
	planegrass.rotate = glm::rotate(planegrass.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	planegrass.translate = glm::translate(planegrass.translate, planegrass.position);
	planegrass.transform = planegrass.translate * planegrass.rotate * glm::scale(glm::mat4(), glm::vec3(planegrass.scale));

	planeroad.position = glm::vec3(0.f, 0.f, 0.f);
	planeroad.scale = 2.f;
	planeroad.rotate = glm::rotate(planeroad.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	planeroad.translate = glm::translate(planeroad.translate, planeroad.position);
	planeroad.transform = planeroad.translate * planeroad.rotate * glm::scale(glm::mat4(), glm::vec3(planeroad.scale));

	rocks.position = glm::vec3(0.f, 0.f, 0.f);
	rocks.scale = 2.f;
	rocks.rotate = glm::rotate(rocks.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	rocks.translate = glm::translate(rocks.translate, rocks.position);
	rocks.transform = rocks.translate * rocks.rotate * glm::scale(glm::mat4(), glm::vec3(rocks.scale));

	torch.position = glm::vec3(0.f, 0.f, 0.f);
	torch.scale = 2.f;
	torch.rotate = glm::rotate(torch.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	torch.translate = glm::translate(torch.translate, torch.position);
	torch.transform = torch.translate * torch.rotate * glm::scale(glm::mat4(), glm::vec3(torch.scale));

	water.position = glm::vec3(0.f, 0.f, 0.f);
	water.scale = 2.f;
	water.rotate = glm::rotate(water.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	water.translate = glm::translate(water.translate, water.position);
	water.transform = water.translate * water.rotate * glm::scale(glm::mat4(), glm::vec3(water.scale));

	watertower.position = glm::vec3(0.f, 0.f, 0.f);
	watertower.scale = 2.f;
	watertower.rotate = glm::rotate(watertower.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	watertower.translate = glm::translate(watertower.translate, watertower.position);
	watertower.transform = watertower.translate * watertower.rotate * glm::scale(glm::mat4(), glm::vec3(watertower.scale));

	wirepole.position = glm::vec3(0.f, 0.f, 0.f);
	wirepole.scale = 2.f;
	wirepole.rotate = glm::rotate(wirepole.rotate, glm::pi<float>() / -2.f, glm::vec3(1.0f, 0.f, 0.f));
	wirepole.translate = glm::translate(wirepole.translate, wirepole.position);
	wirepole.transform = wirepole.translate * wirepole.rotate * glm::scale(glm::mat4(), glm::vec3(wirepole.scale));

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
		//enemies[i]->rotate = glm::rotate(enemies[i]->rotate, (glm::pi<float>() / 1.f), glm::vec3(0.f, 1.f, 0.f));
		//enemies[i]->rotate = glm::rotate(enemies[i]->rotate, (glm::pi<float>() / -2.f), glm::vec3(1.f, 0.f, 0.f));
		//enemies[i]->rotate = glm::rotate(enemies[i]->rotate, (glm::pi<float>() / 1.f), glm::vec3(0.f, 0.f, 1.f));

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
	if (_x->inArea(glm::vec3(0.f, 0.f, 0.), objType::T_OBJ))
	{
		_x->isAttacking = true;
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
	if ((tree_health < 1500.f) && test)
	{
		treefeedback2result = se.system->playSound(treesound2, 0, false, &dialoguechannel);
		half_treehealth = 0.0f;
		test = false;
	}
	if (_x->inArea(pos, objType::P_OBJ)) // player taking damage
	{
		_x->isAttacking = true;
		player_health -= _x->getAttack(); 
		//std::cout << player_health << std::endl;

		// simplified player death as well
		if (player_health == 0)
		{
				playerDead = true;
				
		}
	}
	if (!_x->inArea(pos, objType::P_OBJ) && !_x->inArea(glm::vec3(0.f, 0.f, 0.), objType::T_OBJ))
	{
		_x->isAttacking = false;
	}
}
void Game::createBullet(glm::vec3 pos, glm::vec3 dir)
{	
	if (bulletTime < 0.01f)
	{
		bulletQ.push(new Bullet(bulletMesh, pos, 1.f, 100.f, 15.f, dir));
		bulletsound = true;
		blast = se.system->playSound(shot, 0, false, &shotchannel);
	}
	else if(bulletTime > 0.4f)
	{
		bulletTime = 0.f;
		bulletQ.push(new Bullet(bulletMesh, pos, 1.f, 100.f, 15.f, dir));
		bulletsound = true;
		blast = se.system->playSound(shot, 0, false, &shotchannel);
		
	}
}

void Game::uiUpdate()
{
	updateTimer->tick();

	float deltaTime = updateTimer->getElapsedTimeSeconds();

	deltaTime = glm::clamp(deltaTime, 0.0f, 0.033f);
	t += deltaTime;
	dt += deltaTime;
	st += deltaTime;

	if (state = GameStates::MAIN_MENU) {
		t1 += deltaTime;
		t2 += deltaTime;
		t3 += deltaTime;
	}
	if (t > 1.f)
		t = 0.f;

	if (dt > 0.1f)
	{
		dt = 0.f;
		//(-30f,-1f,0.f), 10,25
		if (state == GameStates::MAIN_MENU)
		{
			if (st < 0.1f) {
				menu = se.system->playSound(menumusic, 0, true, &menuchan);
				menu = menuchan->setVolume(0.04f);
				menu = menuchan->setPaused(false);
			}
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

			arrow.transform = arrow.translate * arrow.rotate * glm::scale(glm::mat4(), glm::vec3(arrow.scale));
			background.transform = background.translate * background.rotate * glm::scale(glm::mat4(), glm::vec3(background.scale));
			ash.transform = ash.translate * ash.rotate * glm::scale(glm::mat4(), glm::vec3(ash.scale));
			barrel.transform = barrel.translate * barrel.rotate * glm::scale(glm::mat4(), glm::vec3(barrel.scale));
			bridge.transform = bridge.translate * bridge.rotate * glm::scale(glm::mat4(), glm::vec3(bridge.scale));
			brokentower.transform = brokentower.translate * brokentower.rotate * glm::scale(glm::mat4(), glm::vec3(brokentower.scale));
			cactus.transform = cactus.translate * cactus.rotate * glm::scale(glm::mat4(), glm::vec3(cactus.scale));
			fseast.transform = fseast.translate * fseast.rotate * glm::scale(glm::mat4(), glm::vec3(fseast.scale));
			fsnortheast.transform = fsnortheast.translate * fsnortheast.rotate * glm::scale(glm::mat4(), glm::vec3(fsnortheast.scale));
			fssoutheast.transform = fssoutheast.translate * fssoutheast.rotate * glm::scale(glm::mat4(), glm::vec3(fssoutheast.scale));
			fwood.transform = fwood.translate * fwood.rotate * glm::scale(glm::mat4(), glm::vec3(fwood.scale));
			gate.transform = gate.translate * gate.rotate * glm::scale(glm::mat4(), glm::vec3(gate.scale));
			hill.transform = hill.translate * hill.rotate * glm::scale(glm::mat4(), glm::vec3(hill.scale));
			hroof.transform = hroof.translate * hroof.rotate * glm::scale(glm::mat4(), glm::vec3(hroof.scale));
			hsand.transform = hsand.translate * hsand.rotate * glm::scale(glm::mat4(), glm::vec3(hsand.scale));
			hwalls.transform = hwalls.translate * hwalls.rotate * glm::scale(glm::mat4(), glm::vec3(hwalls.scale));
			hwindows.transform = hwindows.translate * hwindows.rotate * glm::scale(glm::mat4(), glm::vec3(hwindows.scale));
			hwood.transform = hwood.translate * hwood.rotate * glm::scale(glm::mat4(), glm::vec3(hwood.scale));
			hpath1.transform = hpath1.translate * hpath1.rotate * glm::scale(glm::mat4(), glm::vec3(hpath1.scale));
			hpath2.transform = hpath2.translate * hpath2.rotate * glm::scale(glm::mat4(), glm::vec3(hpath2.scale));
			lamp.transform = lamp.translate * lamp.rotate * glm::scale(glm::mat4(), glm::vec3(lamp.scale));
			pipe.transform = pipe.translate * pipe.rotate * glm::scale(glm::mat4(), glm::vec3(pipe.scale));
			plane.transform = plane.translate * plane.rotate * glm::scale(glm::mat4(), glm::vec3(plane.scale));
			planegrass.transform = planegrass.translate * planegrass.rotate * glm::scale(glm::mat4(), glm::vec3(planegrass.scale));
			planeroad.transform = planeroad.translate * planeroad.rotate * glm::scale(glm::mat4(), glm::vec3(planeroad.scale));
			rocks.transform = rocks.translate * rocks.rotate * glm::scale(glm::mat4(), glm::vec3(rocks.scale));
			torch.transform = torch.translate * torch.rotate * glm::scale(glm::mat4(), glm::vec3(torch.scale));
			water.transform = water.translate * water.rotate * glm::scale(glm::mat4(), glm::vec3(water.scale));
			watertower.transform = watertower.translate * watertower.rotate * glm::scale(glm::mat4(), glm::vec3(watertower.scale));
			wirepole.transform = wirepole.translate * wirepole.rotate * glm::scale(glm::mat4(), glm::vec3(wirepole.scale));
		}

}
void Game::uiDraw()
{
	glClearColor(0.5, 0.5, 0.5, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	background.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);
	arrow.draw(phong, cameraTransform, cameraProjection, pointLights, directionalLight);

	glutSwapBuffers();
}

void Game::updateAlerts()
{
	/*std::cout << "Tree: " << tree_health << std::endl;
	std::cout << "Player: " << player_health << std::endl;*/
	alertTimer += dt;
	if (playerDamaged == true)
	{
		if (alertTimer <= 0.5f)
		{
			curPUIShouldDraw = true;
		}

		else if (alertTimer <= 1.f)
		{
			curPUIShouldDraw = false;
		}

		else { alertTimer = 0.f; }
	}
	else { curPUIShouldDraw = true; }

	if (treeDamaged == true)
	{
		if (alertTimer <= 0.5f)
		{
			curTUIShouldDraw = true;
		}

		else if (alertTimer <= 1.f)
		{
			curTUIShouldDraw = false;
		}
		else { alertTimer = 0.f; }
	}
	else { curTUIShouldDraw = true; }
	//std::cout << alertTimer << std::endl;
	//else { alertTimer = 0.f; }
	//std::cout << alertTimer << std::endl;
}

void Game::update()
{
	
	if (!pause)
	{

		updateTimer->tick();

		float deltaTime = updateTimer->getElapsedTimeSeconds();

		deltaTime = glm::clamp(deltaTime, 0.0f, 0.033f); //
		t += deltaTime;
		srand(time(0));

		mt += deltaTime;
		bool audiocheck1 = false;
		if (t4 < 0.1f) {

			t4 += deltaTime;
		}
		else if (t4 > 0.1f) {
			t4 = 0.1f;
		}
		deltaTime = glm::clamp(deltaTime, 0.0f, 0.033f);
		if (state = GameStates::PLAYING) {
			mt += deltaTime;
		}
		//std::cout << mt << std::endl;
		srand(time(0));
		menu = menuchan->addFadePoint(deltaTime + 1.0f, 0.0f);
		//	menusounds->addFadePoint(deltaTime + 1.0f, 0.0f);
		if (mt < 0.1f) {
			//	start1res = se.system->playSound(start1sound, 0, false, &dialoguechannel);
			start2res = se.system->playSound(start2sound, 0, false, &dialoguechannel);
			result = se.system->playSound(game, 0, true, &channel);
			result = channel->setVolume(0.04f);
			result = channel->setPaused(false);
		}
		bt += deltaTime;


		player.update(deltaTime);
		if (wKeyDown || aKeyDown || sKeyDown || dKeyDown)
		{
			player.animate(deltaTime);
		}
		if (t >= 8.f)
		{
			float ranPos = 1 + rand() % 5;

			float randSpeed = 10.f + rand() % 30;
			glm::mat4 rot = glm::mat4();
			/*rot = glm::rotate(rot, (glm::pi<float>() / 1.f), glm::vec3(0.f, 1.f, 0.f));
			rot = glm::rotate(rot, (glm::pi<float>() / -2.f), glm::vec3(1.f, 0.f, 0.f));
			rot = glm::rotate(rot, (glm::pi<float>() / 1.f), glm::vec3(0.f, 0.f, 1.f));*/
			enemies.push_back(new Enemy(alienMesh, glm::vec3(50.f* ranPos, 50.f*ranPos, -2.f), rot, 1.2f, randSpeed, 30.f + (3.f * healthCounter), 1.f));
			enemies.push_back(new Enemy(insectMesh, glm::vec3(-50.f* ranPos, 50.f*ranPos, -2.f), rot, 2.f, randSpeed, 30.f + (3.f * healthCounter), 1.f));
			t = 0.f;
			healthCounter++;
		}


		if (shouldRotate) {
			player.rotate = glm::rotate(player.rotate, -deltaTime *(glm::pi<float>() * 20.f), glm::vec3(0.f, 1.f, 0.f));
		}

		glm::vec3 playerLocation = glm::vec3(player.translate*
			glm::vec4(glm::vec3(0.f, 0.f, 0.f), 1.0));
		if (wKeyDown) {
			/*std::cout << glm::to_string(player.transform) << std::endl;*/
			if (playerLocation.y >-80 * (plane.scale/3.f)) {
				// Set all other directions to false
				if (isDown) { isDown = false; }
				else if (isRight) { isRight = false; }
				else if (isLeft) { isLeft = false; }

				// Check if we're not already moving up, we only want to rotate once
				if (player.direction != up)
				{
					player.rotate = glm::mat4(); // Set rotation to identity
					player.rotX = glm::pi<float>() / -3.f;
					player.rotY = glm::pi<float>()*3.f; // give it new y rotation here
					player.rotZ = glm::pi<float>() *2.f;
					// Set the rotations using new Y and previous X/Z values
					player.rotate = glm::rotate(player.rotate, player.rotX, glm::vec3(1.f, 0.f, 0.f));
					player.rotate = glm::rotate(player.rotate, player.rotY, glm::vec3(0.f, 1.f, 0.f));
					player.rotate = glm::rotate(player.rotate, player.rotZ, glm::vec3(0.f, 0.f, 1.f));
				}
				player.direction = up; // Set direction to up
				player.nForce = player.direction * player.speed;
				player.originalPosition = player.nForce;
				player.translate = glm::translate(player.translate, player.nForce);
				player.originalTranslate = glm::translate(player.originalTranslate, player.originalPosition);
			}
		}
		if (sKeyDown) {
			if (playerLocation.y <120 * (plane.scale/3.f)) {
				if (isUp) { isUp = false; }
				else if (isRight) { isRight = false; }
				else if (isLeft) { isLeft = false; }
				if (player.direction != down)
				{
					player.rotate = glm::mat4();
					player.rotX = glm::pi<float>() / -3.f;
					player.rotY = glm::pi<float>()*2.f;
					player.rotZ = glm::pi<float>() *2.f;
					player.rotate = glm::rotate(player.rotate, player.rotX, glm::vec3(1.f, 0.f, 0.f));
					player.rotate = glm::rotate(player.rotate, player.rotY, glm::vec3(0.f, 1.f, 0.f));
					player.rotate = glm::rotate(player.rotate, player.rotZ, glm::vec3(0.f, 0.f, 1.f));
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
			if (playerLocation.x < 100.f * (plane.scale/3.f)) {
				if (isUp) { isUp = false; }
				else if (isDown) { isDown = false; }
				else if (isRight) { isRight = false; }
				if (player.direction != left)
				{
					player.rotate = glm::mat4();
					player.rotX = glm::pi<float>() / -3.f;
					player.rotY = glm::pi<float>() * 2.5;
					player.rotZ = glm::pi<float>() *2.f;
					player.rotate = glm::rotate(player.rotate, player.rotX, glm::vec3(1.f, 0.f, 0.f));
					player.rotate = glm::rotate(player.rotate, player.rotY, glm::vec3(0.f, 1.f, 0.f));
					player.rotate = glm::rotate(player.rotate, player.rotZ, glm::vec3(0.f, 0.f, 1.f));
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
			if (playerLocation.x >-100.f * (plane.scale/3.f)) {
				if (isUp) {	isUp = false; }
				else if (isDown) { isDown = false; }
				else if (isLeft) { isLeft = false; }

				if (player.direction != right)
				{
					player.rotate = glm::mat4();
					player.rotX = glm::pi<float>() / -3.f;
					player.rotY = glm::pi<float>()*1.5f;
					player.rotZ = glm::pi<float>() *2.f;
					player.rotate = glm::rotate(player.rotate, player.rotX, glm::vec3(1.f, 0.f, 0.f));
					player.rotate = glm::rotate(player.rotate, player.rotY, glm::vec3(0.f, 1.f, 0.f));
					player.rotate = glm::rotate(player.rotate, player.rotZ, glm::vec3(0.f, 0.f, 1.f));
				}
				player.direction = right;
				player.nForce = player.direction * player.speed;
				player.originalPosition = glm::vec3(-player.nForce.x, player.nForce.y, player.nForce.z);//opposite
				player.translate = glm::translate(player.translate, player.nForce);
				player.originalTranslate = glm::translate(player.originalTranslate, player.originalPosition);
			}
		}
		//SCALE THE OBJECT ACCORDINGLY

		// Store current health values and then loop through all enemies
		float previousTreeHealth = tree_health;
		float previousPlayerHealth = player_health;
		for (int j = 0; j < enemies.size(); j++) {
			glm::vec3 enemyLocation = glm::vec3(enemies[j]->translate*glm::vec4(glm::vec3(0.f, 0.f, 0.f), 1.0));

			TreeWasAttacked(enemies[j], playerLocation); // check if anything is being attacked

			for (int i = 0; i < bulletQ.size(); i++)
			{
				tempBullet = bulletQ.front();
				if ((tempBullet->position.x > enemyLocation.x - 5.f && tempBullet->position.x < enemyLocation.x + 5.f)
					&& (tempBullet->position.y > enemyLocation.y - 5.f && tempBullet->position.y < enemyLocation.y + 5.f))
				{

					enemies[j]->setProperty(enemies[j]->getHealth() - tempBullet->getDamage(), enemies[j]->getAttack());
					tempBullet->position = glm::vec3(100.f, 100.f, 0.f);
					tempBullet->direction = glm::vec3(0.f, 1.f, 0.f);
					//bullets.erase(bullets.begin() + i);
				}

				bulletQ.pop();
				bulletQ.push(tempBullet);
				//if ((bullets[i]->position.x > enemyLocation.x - 5.f && bullets[i]->position.x < enemyLocation.x + 5.f)
				//	&& (bullets[i]->position.y > enemyLocation.y - 5.f && bullets[i]->position.y < enemyLocation.y + 5.f))
				//{
				//
				//	enemies[j]->setProperty(enemies[j]->getHealth() - bullets[i]->getDamage(), enemies[j]->getAttack());
				//	bullets[i]->position = glm::vec3(100.f, 100.f, 0.f);
				//	bullets[i]->direction = glm::vec3(0.f, 1.f, 0.f);
				//	bullets.erase(bullets.begin() + i);
				//}
			}
			if (enemies[j]->getHealth() <= 0)
			{
				enemies[j]->setBool(true);
				enemies[j]->position = glm::vec3(-100.f, -100.f, -2.f);
			}
		}
		if (tree_health < previousTreeHealth) { treeDamaged = true; } // check if tree took damage at all
		else { treeDamaged = false; }
		if (player_health < previousPlayerHealth) { playerDamaged = true; } // check if tree took damage at all
		else { playerDamaged = false; }
		if ((mt > 20.1f) && (mt < 20.15f)) {
			start1res = se.system->playSound(start1sound, 0, false, &dialoguechannel);
		}
		if ((mt > 40.1f) && (mt < 40.15f)) {
			enemy1result = se.system->playSound(enemydialogue1, 0, false, &dialoguechannel);
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
		for (int i = 0; i < bulletQ.size(); i++)
		{
			tempBullet = bulletQ.front();

			tempBullet->update(deltaTime);
			//bullets[i]->velocity = bullets[i]->direction * bullets[i]->speed;
			tempBullet->translate = glm::translate(glm::mat4(), tempBullet->position);
			tempBullet->transform = tempBullet->translate *tempBullet->rotate * glm::scale(glm::mat4(), glm::vec3(tempBullet->scale));

			bulletLocation += tempBullet->transform * glm::vec4(glm::vec3(0.f), 1.f);
			if ((tempBullet->position.x > 100.f || tempBullet->position.x < -100.f) || (tempBullet->position.y > 100.f || tempBullet->position.y < -100.f))
			{
				//std::cout << glm::to_string(bullets[i]->position) << std::endl;
				//bullets.erase(bullets.begin() + i);
				bulletLocation = glm::vec4(0.f);
			}

			bulletQ.pop();
			bulletQ.push(tempBullet);
		}

		//enemies movement
		for (int i = 0; i < enemies.size(); i++)
		{
			if (!enemies[i]->getBool()) {
				enemies[i]->update(deltaTime);
				if (i >=0 && i < (enemies.size() / 2) / 2)
					enemies[i]->animate(deltaTime);
			}
			// Check collision with player and tree
			if (glm::length(enemies[i]->position - playerLocation) > 5.f && glm::length(enemies[i]->position - tree.position) > 13.f)
			{
				enemies[i]->shouldMove = true;
			}

			else { enemies[i]->shouldMove = false; }

			if (enemies[i]->shouldMove)
			{
				enemies[i]->translate = glm::translate(glm::mat4(), enemies[i]->position);
			}

			/*enemies[i]->seek(0.5f * glm::vec3(0.f, 0.f, 9.f) + 0.5f * glm::vec3(playerLocation));*/
			if (glm::length(enemies[i]->position - tree.position) < 7.f)
			{
				//enemies[i]->seek(glm::vec3(0.f, 0.f, 0.f));
				enemies[i]->velocity = glm::normalize(tree.position - enemies[i]->position) * enemies[i]->speed;

				enemies[i]->velocity = glm::normalize(playerLocation - enemies[i]->position) * enemies[i]->speed;

				// New things
				glm::vec3 ePos = enemies[i]->position;
				glm::vec3 tPos = tree.position;
				glm::normalize(ePos);
				glm::normalize(tPos);
				//float cosa = glm::dot(ePos, tPos);
				//glm::clamp(cosa, -1.0f, 1.0f);
				glm::vec3 axis = glm::cross(ePos, tPos);
				//std::cout << axis.x << " " << axis.y << " " << axis.z << std::endl;
				//float angle = glm::degrees(glm::acos(cosa));
				
				glm::vec3 dirToEnemy;
				dirToEnemy.x = tree.position.x - enemies[i]->position.x;
				dirToEnemy.y = tree.position.y - enemies[i]->position.y;
				
				// Calculate enemy angle on y axis
				float rot = atan2(dirToEnemy.y, -dirToEnemy.x);

				enemies[i]->rotate = glm::rotate(glm::mat4(), rot, axis);
			}
			else
			{
				if (glm::length(enemies[i]->position - playerLocation) < 40.f)
				{
					//enemies[i]->seek(glm::vec3(playerLocation));
					enemies[i]->velocity = glm::normalize(playerLocation - enemies[i]->position) * enemies[i]->speed;

					// New things
					glm::vec3 from_vector = enemies[i]->position;
					glm::vec3 to_vector = playerLocation;
					glm::normalize(from_vector);
					glm::normalize(to_vector);
					//float cosa = glm::dot(from_vector, to_vector);
					//glm::clamp(cosa, -1.0f, 1.0f);
					glm::vec3 axis = glm::cross(from_vector, to_vector);
					//std::cout << "PlayerAxis: " << axis.x << " " << axis.y << " " << axis.z << std::endl;
					//float angle = glm::degrees(glm::acos(cosa));

					glm::vec3 dirToEnemy;
					dirToEnemy.x = playerLocation.x - enemies[i]->position.x;
					dirToEnemy.y = playerLocation.y - enemies[i]->position.y;

					// Calculate player angle on y axis
					float rot = atan2(dirToEnemy.y, -dirToEnemy.x);

					enemies[i]->rotate = glm::rotate(glm::mat4(), rot, axis);

					//enemies[i]->rotate = glm::rotate(glm::mat4(), enemies[i]->rotZ, glm::vec3(0.f, 0.f, 0.f));
				}

				else
				{
					//enemies[i]->seek(glm::vec3(0.f, 0.f, 0.f));
					enemies[i]->velocity = glm::normalize(glm::vec3(0.f, 0.f, 0.f) - enemies[i]->position) * enemies[i]->speed;

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

		ash.transform = ash.translate *ash.rotate * glm::scale(glm::mat4(), glm::vec3(ash.scale));
		barrel.transform = barrel.translate * barrel.rotate * glm::scale(glm::mat4(), glm::vec3(barrel.scale));
		bridge.transform = bridge.translate * bridge.rotate * glm::scale(glm::mat4(), glm::vec3(bridge.scale));
		brokentower.transform = brokentower.translate * brokentower.rotate * glm::scale(glm::mat4(), glm::vec3(brokentower.scale));
		cactus.transform = cactus.translate * cactus.rotate * glm::scale(glm::mat4(), glm::vec3(cactus.scale));
		fseast.transform = fseast.translate * fseast.rotate * glm::scale(glm::mat4(), glm::vec3(fseast.scale));
		fsnortheast.transform = fsnortheast.translate * fsnortheast.rotate * glm::scale(glm::mat4(), glm::vec3(fsnortheast.scale));
		fssoutheast.transform = fssoutheast.translate * fssoutheast.rotate * glm::scale(glm::mat4(), glm::vec3(fssoutheast.scale));
		fwood.transform = fwood.translate * fwood.rotate * glm::scale(glm::mat4(), glm::vec3(fwood.scale));
		gate.transform = gate.translate * gate.rotate * glm::scale(glm::mat4(), glm::vec3(gate.scale));
		hill.transform = hill.translate * hill.rotate * glm::scale(glm::mat4(), glm::vec3(hill.scale));
		hroof.transform = hroof.translate * hroof.rotate * glm::scale(glm::mat4(), glm::vec3(hroof.scale));
		hsand.transform = hsand.translate * hsand.rotate * glm::scale(glm::mat4(), glm::vec3(hsand.scale));
		hwalls.transform = hwalls.translate * hwalls.rotate * glm::scale(glm::mat4(), glm::vec3(hwalls.scale));
		hwindows.transform = hwindows.translate * hwindows.rotate * glm::scale(glm::mat4(), glm::vec3(hwindows.scale));
		hwood.transform = hwood.translate * hwood.rotate * glm::scale(glm::mat4(), glm::vec3(hwood.scale));
		hpath1.transform = hpath1.translate * hpath1.rotate * glm::scale(glm::mat4(), glm::vec3(hpath1.scale));
		hpath2.transform = hpath2.translate * hpath2.rotate * glm::scale(glm::mat4(), glm::vec3(hpath2.scale));
		lamp.transform = lamp.translate * lamp.rotate * glm::scale(glm::mat4(), glm::vec3(lamp.scale));
		pipe.transform = pipe.translate * pipe.rotate * glm::scale(glm::mat4(), glm::vec3(pipe.scale));
		plane.transform = plane.translate * plane.rotate * glm::scale(glm::mat4(), glm::vec3(plane.scale));
		planegrass.transform = planegrass.translate * planegrass.rotate * glm::scale(glm::mat4(), glm::vec3(planegrass.scale));
		planeroad.transform = planeroad.translate * planeroad.rotate * glm::scale(glm::mat4(), glm::vec3(planeroad.scale));
		rocks.transform = rocks.translate * rocks.rotate * glm::scale(glm::mat4(), glm::vec3(rocks.scale));
		torch.transform = torch.translate * torch.rotate * glm::scale(glm::mat4(), glm::vec3(torch.scale));
		water.transform = water.translate * water.rotate * glm::scale(glm::mat4(), glm::vec3(water.scale));
		watertower.transform = watertower.translate *watertower.rotate * glm::scale(glm::mat4(), glm::vec3(watertower.scale));
		wirepole.transform = wirepole.translate * wirepole.rotate * glm::scale(glm::mat4(), glm::vec3(wirepole.scale));
				
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
			cameraProjection = glm::perspective(90.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, near_plane, far_plane);
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
	drawHealth();
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); // position and size of viewport
	glEnable(GL_BLEND); // Allows Transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	cameraOrtho = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, -1000.0f, 1000.0f);
	//std::cout << t << std::endl;
	if (t >= 5) { otherUI["protect"]->shouldDraw = false; }

	for (auto itr = otherUI.begin(); itr != otherUI.end(); itr++)
	{
		if (itr->second->shouldDraw == true)
			itr->second->draw(noLight, glm::mat4(1.0f), cameraOrtho, pointLights, directionalLight);
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}
void Game::drawHealth()
{
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); // position and size of viewport
	glEnable(GL_BLEND); // Allows Transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	cameraOrtho = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, -1000.0f, 1000.0f);

	//treeUI["treeHP10"]->draw(noLight, glm::mat4(1.0f), cameraOrtho, pointLights, directionalLight);

	// update hearts based on player health and output num value for it
	//std::cout << "players health: " << player_health << std::endl;
	if (player_health <= player_start_health * 0.1) { switchUIToDraw(playerUI["playerHP1"], PLAYER); }
	else if (player_health <= player_start_health * 0.2) { switchUIToDraw(playerUI["playerHP2"], PLAYER); }
	else if (player_health <= player_start_health * 0.3) { switchUIToDraw(playerUI["playerHP3"], PLAYER); }
	else if (player_health <= player_start_health * 0.4) { switchUIToDraw(playerUI["playerHP4"], PLAYER); }
	else if (player_health <= player_start_health * 0.5) { switchUIToDraw(playerUI["playerHP5"], PLAYER); }
	else if (player_health <= player_start_health * 0.6) { switchUIToDraw(playerUI["playerHP6"], PLAYER); }
	else if (player_health <= player_start_health * 0.7) { switchUIToDraw(playerUI["playerHP7"], PLAYER); }
	else if (player_health <= player_start_health * 0.8) { switchUIToDraw(playerUI["playerHP8"], PLAYER); }
	else if (player_health <= player_start_health * 0.9) { switchUIToDraw(playerUI["playerHP9"], PLAYER); }
	else if (player_health <= player_start_health * 1) { switchUIToDraw(playerUI["playerHP10"], PLAYER); }

	// same thing with tree
	//std::cout << "tree health: " << tree_health << std::endl;
	if (tree_health <= tree_start_health * 0.1) { switchUIToDraw(treeUI["treeHP1"], TREE); }
	else if (tree_health <= tree_start_health * 0.2) { switchUIToDraw(treeUI["treeHP2"], TREE); }
	else if (tree_health <= tree_start_health * 0.3) { switchUIToDraw(treeUI["treeHP3"], TREE); }
	else if (tree_health <= tree_start_health * 0.4) { switchUIToDraw(treeUI["treeHP4"], TREE); }
	else if (tree_health <= tree_start_health * 0.5) { switchUIToDraw(treeUI["treeHP5"], TREE); }
	else if (tree_health <= tree_start_health * 0.6) { switchUIToDraw(treeUI["treeHP6"], TREE); }
	else if (tree_health <= tree_start_health * 0.7) { switchUIToDraw(treeUI["treeHP7"], TREE); }
	else if (tree_health <= tree_start_health * 0.8) { switchUIToDraw(treeUI["treeHP8"], TREE); }
	else if (tree_health <= tree_start_health * 0.9) { switchUIToDraw(treeUI["treeHP9"], TREE); }
	else if (tree_health <= tree_start_health * 1) { switchUIToDraw(treeUI["treeHP10"], TREE); }

	// draw ui that's set to true
	for (auto itr = playerUI.begin(); itr != playerUI.end(); itr++)
	{
		if (itr->second->shouldDraw && curPUIShouldDraw)
			itr->second->draw(noLight, glm::mat4(1.0f), cameraOrtho, pointLights, directionalLight);
	}
	for (auto itr = treeUI.begin(); itr != treeUI.end(); itr++)
	{
		if (itr->second->shouldDraw && curTUIShouldDraw)
			itr->second->draw(noLight, glm::mat4(1.0f), cameraOrtho, pointLights, directionalLight);
	}

	//std::cout << t << std::endl;
	if (t >= 5) { otherUI["protect"]->shouldDraw = false; }

	//for (auto itr = otherUI.begin(); itr != otherUI.end(); itr++)
	//{
	//	if (itr->second->shouldDraw == true)
	//		itr->second->draw(noLight, glm::mat4(1.0f), cameraOrtho, pointLights, directionalLight);
	//}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}
void Game::draw()
{
	internal::StartUI(WINDOW_WIDTH, WINDOW_HEIGHT);

	//glClearColor(0.5, 0.5, 0.5, 0);	
	glClearColor(.4f, .2f, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	sceneBuffer.bindFrameBufferForDrawing();
	sceneBuffer.clearFrameBuffer(glm::vec4(0));

	materials["default"]->bind();
	materials["default"]->sendUniform("u_lightPos", cameraTransform * directionalLight.positionOrDirection);
	drawScene();

	sceneBuffer.unbindFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

	//For shadows
	//cull front
	//render to depth map
	//bind fbo for drawing
	//clear, send uniform, and render scene
	//unbind fbo
	//reset cull to back

	//render scene with shadow mapping
	//set viewport, then clear
	//send uniforms
	//bind depth texture
	//render scene

	glCullFace(GL_FRONT);
	sceneBuffer.bindFrameBufferForDrawing();
	sceneBuffer.clearFrameBuffer(glm::vec4(0));
	drawScene();
	drawSceneWithShadows(*materials["depthMap"], false);
	/*depthMap.bind();
	depthMapFBO.bindDepthTextureForSampling(GL_TEXTURE0);*/
	//meshes["quad"]->draw();
	sceneBuffer.unbindFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
	glCullFace(GL_BACK);

	sceneBuffer.bindFrameBufferForDrawing();
	sceneBuffer.clearFrameBuffer(glm::vec4(0));
	//drawScene();
	drawSceneWithShadows(*materials["shadows"], true);
	//meshes["quad"]->draw();

	///*if (temp >= player.playerMesh.size())
	//	temp = 0;
	//meshes["player" + std::to_string(temp)]->draw();
	//temp++;*/
	//
	sceneBuffer.unbindFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
	sceneBuffer.unbindTexture(GL_TEXTURE0);
	sceneBuffer.unbindTexture(GL_TEXTURE1);

	brightPass(); // Implement this function!
	blurBrightPass(); // Implement this function!

	sceneBuffer.bindTextureForSampling(0, GL_TEXTURE1);
	blurBuffer.bindTextureForSampling(0, GL_TEXTURE0);


	FrameBufferObject::unbindFrameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
	FrameBufferObject::clearFrameBuffer(glm::vec4(0));
	materials["bloom"]->bind();
	materials["bloom"]->sendUniformMat4("u_mvp", glm::value_ptr(emptyMat), false);
	materials["bloom"]->sendUniformMat4("u_mv", glm::value_ptr(cameraTransform), false);
	
	meshes["quad"]->draw();
	sceneBuffer.unbindTexture(GL_TEXTURE1);
	blurBuffer.unbindTexture(GL_TEXTURE0);

	//drawFboAttachmentToBackBuffer(sceneBuffer, 0);
	//drawFboAttachmentToBackBuffer(buffer, 0);

	if (debug)
	{
		if (ImGui::CollapsingHeader("Lighting", 0))
		{
			if (ImGui::Button("Lighting"))
			{
				light = !light;
				if (light)
				{
					directionalLight.ambient  = originalD.ambient;
					directionalLight.diffuse  = originalD.diffuse;
					directionalLight.specular = originalD.specular;
					player.sendDirectionalLightUniforms(phong, cameraTransform, directionalLight);

					for (int i = 0; i < pointLights.size(); i++)
					{
						pointLights[i].ambient  = originalP1.ambient;
						pointLights[i].diffuse  = originalP1.diffuse;
						pointLights[i].specular = originalP1.specular;
					}
					player.sendPointLightUniforms(phong, cameraTransform, pointLights);
				}
				else
				{
					directionalLight.ambient  = glm::vec3(0);
					directionalLight.diffuse  = glm::vec3(0);
					directionalLight.specular = glm::vec3(0);
					player.sendDirectionalLightUniforms(phong, cameraTransform, directionalLight);

					for (int i = 0; i < pointLights.size(); i++)
					{
						pointLights[i].ambient  = glm::vec3(0);
						pointLights[i].diffuse  = glm::vec3(0);
						pointLights[i].specular = glm::vec3(0);
					}
					player.sendPointLightUniforms(phong, cameraTransform, pointLights);
				}
			}
		}
		if(ImGui::CollapsingHeader("Post-Processing", 0))
		{
			if (ImGui::CollapsingHeader("Bloom", 0))
			{
				ImGui::SliderFloat("Bloom Threshold", &bloomThreshold, 0.01f, 1.f, "%.2f", 1.f);
				//std::cout << bloomThreshold << std::endl;
				ImGui::SliderInt("Blur passes", &pass, 1, 1000);
			}
			if (ImGui::CollapsingHeader("Shadows", 0))
			{

			}
		}
		tree_health = tree_start_health;
		player_health = player_start_health;
	}
	internal::EndUI();
	glutSwapBuffers();

}

//Input callback funct
void Game::keyboardDown(unsigned char key, int mouseX, int mouseY) 
{
	glm::vec3 playerLocation = glm::vec3(player.translate*
		glm::vec4(glm::vec3(1.f, 1.f, 1.f), 1.0));

	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[(int)key] = true;
	io.AddInputCharacter((int)key); // this is what makes keyboard input work in imgui

									// This is what makes the backspace button work
	int keyModifier = glutGetModifiers();
	switch (keyModifier)
	{
	/*case GLUT_ACTIVE_SHIFT:
		io.KeyShift = true;
		break;

	case GLUT_ACTIVE_CTRL:
		io.KeyCtrl = true;
		break;

	case GLUT_ACTIVE_ALT:
		io.KeyAlt = true;
		break;*/
	}
	switch (key) 
	{
	case 27://esc
		exit(1);
		break;
	case'q':
		//exit(1);
		break;
	case't':
		std::cout << "Total elapsed time: " << updateTimer->getCurrentTime() / 1000.f<<std::endl;
	
		break;
	case'g':
		if (arrow.position == glm::vec3(-30.f, -1.f, 0.f))
		{
			state = GameStates::PLAYING;
			plane.scale = 2.f;

			cameraTransform = glm::lookAt(cameraEye,
				cameraCtr, glm::vec3(0.f, -1.f, 0.f));
			originalCameraTransform = cameraTransform;

			cameraProjection = glm::perspective(150.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, near_plane, far_plane);
			glViewport(0, 0, 20.f, 20.f);
		}
		selection = se.system->playSound(selectsound, 0, false, &menusounds);


		if (arrow.position == glm::vec3(-30.f, 25.f, 0.f))
			exit(1);
		break;
	case'l':
		state = GameStates::TUTORIAL;
		plane.scale = 2.f;

		cameraTransform = glm::lookAt(cameraEye,
			cameraCtr, glm::vec3(0.f, -1.f, 0.f));
		originalCameraTransform = cameraTransform;

		cameraProjection = glm::perspective(150.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.f);
		glViewport(0, 0, 20.f, 20.f);
		break;
	case 'w':
		if (!GameStates::PLAYING) {
			scroll = se.system->playSound(scrollsound, 0, false, &menusounds);
		}
		wKeyDown = true;
		isUp = true;
		break;
	case 'a':
		aKeyDown = true;
		isLeft = true;
		break;
	case 's':
		if (!GameStates::PLAYING) {
			scroll = se.system->playSound(scrollsound, 0, false, &menusounds);
		}
		sKeyDown = true;	
		isDown = true;
		break;
	case 'i':
		debug = !debug;
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

			cameraProjection = glm::perspective(90.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, near_plane, far_plane);
			glViewport(0, 0, 20.f, 20.f);
		}
		if (!pause)
		{
			state = GameStates::PLAYING;
			ash.scale = 2.f;
			barrel.scale = 2.f;
			bridge.scale = 2.f;
			brokentower.scale = 2.f;
			cactus.scale = 2.f;
			fseast.scale = 2.f;
			fsnortheast.scale = 2.f;
			fssoutheast.scale = 2.f;
			fwood.scale = 2.f;
			gate.scale = 2.f;
			hill.scale = 2.f;
			hroof.scale = 2.f;
			hsand.scale = 2.f;
			hwalls.scale = 2.f;
			hwindows.scale = 2.f;
			hwood.scale = 2.f;
			hpath1.scale = 2.f;
			hpath2.scale = 2.f;
			lamp.scale = 2.f;
			pipe.scale = 2.f;
			plane.scale = 2.f;
			planegrass.scale = 2.f;
			planeroad.scale = 2.f;
			rocks.scale = 2.f;
			torch.scale = 2.f;
			water.scale = 2.f;
			watertower.scale = 2.f;
			wirepole.scale = 2.f;
			cameraTransform = glm::lookAt(cameraEye,
				cameraCtr, glm::vec3(0.f, -1.f, 0.f));
			originalCameraTransform = cameraTransform;

			cameraProjection = glm::perspective(150.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, near_plane, far_plane);
		}

		break;
	default:
		break;
	}
}
void Game::keyboardUp(unsigned char key, int mouseX, int mouseY)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = false;

	int keyModifier = glutGetModifiers();
	io.KeyShift = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	switch (keyModifier)
	{
	/*case GLUT_ACTIVE_SHIFT:
		io.KeyShift = true;
		break;

	case GLUT_ACTIVE_CTRL:
		io.KeyCtrl = true;
		break;

	case GLUT_ACTIVE_ALT:
		io.KeyAlt = true;
		break;*/
	}
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
	ImGui::GetIO().MouseDown[0] = !state;

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
	ImGui::GetIO().MousePos = ImVec2((float)x, (float)y);
}
