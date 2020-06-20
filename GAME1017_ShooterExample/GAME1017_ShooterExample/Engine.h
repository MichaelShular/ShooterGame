#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include "Sprites.h"
#include "StateManager.h"
#include "EventManager.h"
#include "FontManager.h"
#include "SoundManager.h"
#include "TextureManager.h"


class Engine
{
// I am avoiding in-class initialization.
private: // Private properties.
	bool m_bRunning; // Loop control flag.
	 // This restricts the player from firing again unless they release the Spacebar.
	const Uint8* m_iKeystates; // Keyboard state container.
	Uint32 m_start, m_end, m_delta, m_fps; // Fixed timestep variables.
	SDL_Window* m_pWindow; // This represents the SDL window.
	SDL_Renderer* m_pRenderer; // This represents the buffer to draw to.
	bool m_pPause, m_dead;


private: // Private methods.
	bool Init(const char* title, int xpos, int ypos, int width, int height, int flags);
	void Wake();
	void Sleep();
	
	void Update();
	void Render();
	void Clean();
public: // Public methods.
	Engine();
	~Engine();
	void HandleEvents();
	void CheckCollision();
	int Run();
	static Engine& Instance();
	SDL_Renderer* GetRenderer();
	bool& Running();
	bool& Pause();
	bool& Dead();
	
};