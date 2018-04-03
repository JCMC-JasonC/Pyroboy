#include<iostream>
#include<GL/glew.h>
#include<GL/glut.h>
#include<GL/freeglut.h>
#include"Game.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1020
#define FRAMES_PER_SECOND 60

const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND;

Game* game;

void DisplayCallbackFunction(void)
{
	if (game->state == GameStates::MAIN_MENU)
	{
		game->uiDraw();
	}
	else if (game->state == GameStates::STARTUP)
	{
		game->startDraw();
		game->initializeGame();
	}
	else if (game->state == GameStates::PAUSE)
	{
		game->pauseMenu();
	}
	else if (game->state == GameStates::LOSE)
	{
		game->gameOverMenu();
	}
	else
	{
		game->draw();

	}
}

void KeyboardCallbackFunction(unsigned char key, int x, int y)
{
	game->keyboardDown(key, x, y);
}
void KeyboardUpCallbackFunction(unsigned char key, int x, int y)
{
	game->keyboardUp(key, x, y);
}

void TimerCallbackFunction(int value)
{
	if (game->state == GameStates::MAIN_MENU)
	{
		game->uiUpdate();
	}
	else if (game->state == GameStates::STARTUP)
	{
		game->startUpdate();
	}
	else if (game->state == GameStates::PAUSE)
	{
		game->pauseUpdate();
	}
	else if (game->state == GameStates::LOSE)
	{
		game->gameOverUpdate();
	}
	else if (game->state == GameStates::TUTORIAL)
	{
		game->gameOverUpdate();
	}
	else
	{
		game->update();

	}

	glutPostRedisplay();
	glutTimerFunc(FRAME_DELAY, TimerCallbackFunction, 0);
}

void MouseClickCallbackFunction(int button, int state, int x, int y)
{
	game->mouseClicked(button, state, x,y);
}

void MouseMotionCallbackFunction(int x, int y)
{
	game->mouseMoved(x, y);
}

void SpecialInputCallbackFunctionDown(int key, int x, int y)
{
	game->SpecialInputCallbackFunctionDown(key, x, y);
}
void SpecialInputCallbackFunctionUp(int key, int x, int y)
{
	game->SpecialInputCallbackFunctionUp(key, x, y);
}
void updateGame()
{
	glutPostRedisplay();
}
int main(int argc, char **argv) 
{
	//states =GameStates::MAIN_MENU;

	glutInit(&argc, argv);
	glutInitContextVersion(4, 2);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Game");
	//glutFullScreen();

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW could not be initialized" <<std::endl;
		system("pause");
		return 0;
	}

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	//setup callback functions
	glutDisplayFunc(DisplayCallbackFunction);
	glutKeyboardFunc(KeyboardCallbackFunction);
	glutKeyboardUpFunc(KeyboardUpCallbackFunction);
	glutMouseFunc(MouseClickCallbackFunction);
	glutMotionFunc(MouseMotionCallbackFunction);
	glutTimerFunc(1, TimerCallbackFunction, 0);
	glutSpecialFunc(SpecialInputCallbackFunctionDown);
	glutSpecialUpFunc(SpecialInputCallbackFunctionUp);
	glutIdleFunc(updateGame);
	
	internal::InitImGUI();
	game = new Game();
	
	game->startUp();
	//game->initializeGame();
	if (game->state == GameStates::MAIN_MENU)
	{
		game->mainMenu();
	}
	if (game->state == GameStates::PLAYING)
	{
	
	}
	glutMainLoop();
		
	return 0;
}