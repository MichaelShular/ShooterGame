#include "Engine.h"
#include <iostream>
#include <ctime>


#define WIDTH 1024
#define HEIGHT 768
#define FPS 60
#define BGSCROLL 2 // Could these scroll/speed values be handled in the class? Yes. Consider it!
#define PSPEED 6
using namespace std;

Engine::Engine() 
{	// Again showing a mix of either initializers or in-body initialization. Initializers happen first.
	m_bRunning = false; // Setting all to false.
	cout << "Engine class constructed!" << endl; 
}
Engine::~Engine(){}

bool Engine::Init(const char* title, int xpos, int ypos, int width, int height, int flags)
{
	cout << "Initializing game." << endl;
	// Attempt to initialize SDL.
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		// Create the window.
		m_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (m_pWindow != nullptr) // Window init success.
		{
			m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);
			if (m_pRenderer != nullptr) // Renderer init success.
			{
				EVMA::Init();
				SOMA::Init();
				TEMA::Init();
			
			}
			else return false; // Renderer init fail.
		}
		else return false; // Window init fail.
	}
	else return false; // SDL init fail.
	m_fps = (Uint32)round((1 / (double)FPS) * 1000); // Sets FPS in milliseconds and rounds.
	TEMA::RegisterTexture("Img/startButton.png", "play");
	TEMA::RegisterTexture("Img/quitButtons.png", "quit");
	TEMA::RegisterTexture("Img/background.png", "background");
	TEMA::RegisterTexture("Img/sprites.png", "sprites");

	STMA::ChangeState(new PlayState);
	m_bRunning = true; // Everything is okay, start the engine.
	cout << "Success!" << endl;
	SOMA::AllocateChannels(16);
	return true;
}

void Engine::Wake()
{
	m_start = SDL_GetTicks();
}

void Engine::Sleep()
{
	m_end = SDL_GetTicks();
	m_delta = m_end - m_start;
	if (m_delta < m_fps) // Engine has to sleep.
		SDL_Delay(m_fps - m_delta);
}

void Engine::HandleEvents()
{
	EVMA::HandleEvents();
}

void Engine::CheckCollision()
{
	
}

/* Update is SUPER way too long on purpose! Part of the Assignment 1, if you use
   this program as a start project is to chop up Update and figure out where each
   part of the code is supposed to go. A practice in OOP is to have objects handle
   their own behaviour and this is a big hint for you. */
void Engine::Update()
{
	STMA::Update();
}

/* In the render I commented out some lines that rendered the original destination rectangles
   for the sprites so I can show you the after-effect of rotating via SDL_RenderCopyEX(). In 
   order to do collision, I manually created new rectangles for collision in the CheckCollision method. */
void Engine::Render()
{
	SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
	SDL_RenderClear(m_pRenderer);
	STMA::Render();
	SDL_RenderPresent(m_pRenderer);
}

void Engine::Clean()
{
	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	EVMA::Quit();
	FOMA::Quit();
	SOMA::Quit();
	STMA::Quit();
	TEMA::Quit();
	IMG_Quit();
	SDL_Quit();
}

int Engine::Run()
{
	
	if (m_bRunning) // What does this do and what can it prevent?
		return -1;
	if (Init("GAME1017 Shooter Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0) == false)
		return 1;
	while (m_bRunning)
	{
		Wake();
		HandleEvents();
		Update();
		Render();
		if (m_bRunning)
			Sleep();
	}
	Clean();
	return 0;
}

Engine& Engine::Instance()
{
	static Engine instance; // C++11 will prevent this line from running more than once. Magic statics.
	return instance;
}

SDL_Renderer* Engine::GetRenderer()
{
	return m_pRenderer;
}

bool& Engine::Running()
{
	return m_bRunning;
}


