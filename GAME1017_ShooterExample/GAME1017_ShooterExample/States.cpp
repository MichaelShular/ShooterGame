#include "States.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "FontManager.h"
#include "StateManager.h" // Make sure this is NOT in "States.h" or circular reference.
#include "TextureManager.h"
#include "Engine.h"
#include "Utilities.h"
#include <iostream>
#include <ctime>
using namespace std;
#define WIDTH 1280
#define HEIGHT 720
#define FPS 60
#define BGSCROLL 2 // Could these scroll/speed values be handled in the class? Yes. Consider it!
#define PSPEED 6

// Begin State. CTRL+M+H and CTRL+M+U to turn on/off collapsed code.
void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}
void State::Resume() {}
// End State.

// Begin PlayState.
PlayState::PlayState() :m_iESpawn(0), m_iESpawnMax(60), m_pivot({ 0,0 }) {
	m_bEBNull = false;
	m_bENull = false;
	m_bPBNull = false;
	m_bCanShoot = true;
	srand((unsigned)time(NULL));
}

void PlayState::Enter() 
{
	for (int f = 0; f < 5; f++)
	{
		bgArray[f*3] = { {0,0,1280,720}, {0, 0, 1280, 720} };
		bgArray[f*3+1] = { {0,0,1280,720}, {1280, 0, 1280, 720} };
		bgArray[f*3+2] = { {0,0,1280,720}, {-1280, 0, 1280, 720} };
	}
	m_pauseBtn = new PauseButton({ 0,0,480,140 }, { 0, 0, 240,70 });
	m_resumeBtn = new ResumeButton({ 0,0,480,140 }, { 520, 230, 240,70 });
	m_quitBtn = new QuitButton({ 0,0,480,140 }, { 520, 300, 240, 70 });

	m_player = new Player( { 129, 171, 16, 22 },{ 10, 50, 32, 44} );
	SOMA::Load("Aud/enemy.wav", "enemy", SOUND_SFX);
	SOMA::Load("Aud/explode.wav", "explode", SOUND_SFX);
	SOMA::Load("Aud/laser.wav", "laser", SOUND_SFX);
	m_playerStartX = m_player->GetSrcP()->x;
	double adfv = 223;
	int aaro = 42;
	int arfg = 24; 
}

void PlayState::Update()
{
	if (m_pauseBtn->Update() == 1)
		return;
	if (Engine::Instance().Pause() == true)
	{
		if (m_resumeBtn->Update() == 1)
			return;
		if (m_quitBtn->Update() == 1)
			return;
	}
	if (EVMA::KeyHeld(SDL_SCANCODE_X))
	{
		Engine::Instance().Pause() = true;
	}
	// Scroll the backgrounds. Check if they need to snap back.
	if (Engine::Instance().Pause() == false)
	{
		for (int f = 0; f < 5; f++)
		{
			bgArray[f * 3].GetDstP()->x -= m_srollBG[f];
			bgArray[f * 3 + 1].GetDstP()->x -= m_srollBG[f];
			bgArray[f * 3 + 2].GetDstP()->x -= m_srollBG[f];
		}
		for (int f = 0; f < 5; f++)
		{
			if (bgArray[f * 3 + 2].GetDstP()->x <= -1280)
			{
				bgArray[f * 3 + 2].GetDstP()->x = 1280;

			}
			if (bgArray[f * 3 + 1].GetDstP()->x <= -1280)
			{
				bgArray[f * 3 + 1].GetDstP()->x = 1280;
			}
			if (bgArray[f * 3].GetDstP()->x <= -1280)
			{
				bgArray[f * 3].GetDstP()->x = 1280;
			}
		}
		// Player animation/movement.
		m_player->Animate(m_playerStartX); // Oh! We're telling the player to animate itself. This is good! Hint hint.
		if (EVMA::KeyHeld(SDL_SCANCODE_A) && m_player->GetDstP()->x > m_player->GetDstP()->h)
			m_player->GetDstP()->x -= PSPEED;
		else if (EVMA::KeyHeld(SDL_SCANCODE_D) && m_player->GetDstP()->x < WIDTH / 2)
			m_player->GetDstP()->x += PSPEED;
		if (EVMA::KeyHeld(SDL_SCANCODE_W) && m_player->GetDstP()->y > 0)
			m_player->GetDstP()->y -= PSPEED;
		else if (EVMA::KeyHeld(SDL_SCANCODE_S) && m_player->GetDstP()->y < HEIGHT - m_player->GetDstP()->w)
			m_player->GetDstP()->y += PSPEED;
	


		if (EVMA::KeyHeld(SDL_SCANCODE_SPACE) && m_bCanShoot)
		{
			m_bCanShoot = false;
			m_vPBullets.push_back(new Bullet({ 0,0,128,128 }, { m_player->GetDstP()->x + 16,m_player->GetDstP()->y + 10, 50, 50 }, 30));
			//Mix_PlayChannel(-1, m_vSounds[1], 0);

		}
		if (EVMA::KeyReleased(SDL_SCANCODE_SPACE))
		{
			m_bCanShoot = true;
		}

		// Enemy animation/movement.
		for (int i = 0; i < (int)m_vEnemies.size(); i++)
		{
			m_vEnemies[i]->Update(); // Oh, again! We're telling the enemies to update themselves. Good good!
			if (m_vEnemies[i]->GetDstP()->x < -56)
			{
				delete m_vEnemies[i];
				m_vEnemies[i] = nullptr;
				m_bENull = true;
			}
		}
		if (m_bENull) CleanVector<Enemy*>(m_vEnemies, m_bENull); // Better to have a logic check (if) than a function call all the time!
		// Update enemy spawns.
		if (m_iESpawn++ == m_iESpawnMax)
		{
			m_vEnemies.push_back(new Enemy({ 0,100,40,56 }, { WIDTH,56 + rand() % (HEIGHT - 114),40,56 }, &m_vEBullets,
				30 + rand() % 91)); // Randomizing enemy bullet spawn to every 30-120 frames.
			m_iESpawn = 0;
		}
		// Update the bullets. Player's first.

		for (int i = 0; i < (int)m_vPBullets.size(); i++)
		{
			m_vPBullets[i]->Update();
			if (m_vPBullets[i]->GetDstP()->x > WIDTH)
			{
				delete m_vPBullets[i];
				m_vPBullets[i] = nullptr;
				m_bPBNull = true;
			}
		}

		if (m_bPBNull) CleanVector<Bullet*>(m_vPBullets, m_bPBNull);
		// Now enemy bullets. Is Update() getting a little long?
		for (int i = 0; i < (int)m_vEBullets.size(); i++)
		{
			m_vEBullets[i]->Update();
			if (m_vEBullets[i]->GetDstP()->x < -10)
			{
				delete m_vEBullets[i];
				m_vEBullets[i] = nullptr;
				m_bEBNull = true;
			}
		}
		if (m_bEBNull) CleanVector<Bullet*>(m_vEBullets, m_bEBNull);
		CheckCollision();
	}
	

}

void PlayState::CheckCollision()
{
	// Player vs. Enemy.
	SDL_Rect p = { m_player->GetDstP()->x - 20, m_player->GetDstP()->y , 32, 44 };
	for (int i = 0; i < (int)m_vEnemies.size(); i++)
	{
		SDL_Rect e = { m_vEnemies[i]->GetDstP()->x, m_vEnemies[i]->GetDstP()->y - 40, 56, 40 };
		if (SDL_HasIntersection(&p, &e))
		{
			// Game over!
			cout << "Player goes boom!" << endl;
			SOMA::PlaySound("explode");
			break;
		}
	}
	// Player bullets vs. Enemies.
	for (int i = 0; i < (int)m_vPBullets.size(); i++)
	{
		SDL_Rect b = { m_vPBullets[i]->GetDstP()->x - 25, m_vPBullets[i]->GetDstP()->y, 50, 50 };
		for (int j = 0; j < (int)m_vEnemies.size(); j++)
		{
			if (m_vEnemies[j] == nullptr) continue;
			SDL_Rect e = { m_vEnemies[j]->GetDstP()->x, m_vEnemies[j]->GetDstP()->y - 40, 56, 40 };
			if (SDL_HasIntersection(&b, &e))
			{
				SOMA::PlaySound("explode");
				delete m_vEnemies[j];
				m_vEnemies[j] = nullptr;
				delete m_vPBullets[i];
				m_vPBullets[i] = nullptr;
				m_bENull = true;
				m_bPBNull = true;
				break;
			}
		}
	}
	if (m_bENull) CleanVector<Enemy*>(m_vEnemies, m_bENull);
	if (m_bPBNull) CleanVector<Bullet*>(m_vPBullets, m_bPBNull);
	// Enemy bullets vs. player.
	for (int i = 0; i < (int)m_vEBullets.size(); i++)
	{
		if (SDL_HasIntersection(&p, m_vEBullets[i]->GetDstP()))
		{
			// Game over!
			cout << "Player goes boom!" << endl;
			SOMA::PlaySound("explode");
			delete m_vEBullets[i];
			m_vEBullets[i] = nullptr;
			CleanVector<Bullet*>(m_vEBullets, m_bEBNull);
			break;
		}
	}
	
}

void PlayState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer()); // Clear the screen with the draw color.
	// Render stuff. Background first.
	for (int i = 0; i < 3; i++)
	{
		SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("titleBG"), bgArray[i].GetSrcP(), bgArray[i].GetDstP());
	}
	for (int i = 0; i < 3; i++)
	{
		SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("montainFar"), bgArray[i+3].GetSrcP(), bgArray[i+3].GetDstP());
	}
	for (int i = 0; i < 3; i++)
	{
		SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("montainClose"), bgArray[i + 6].GetSrcP(), bgArray[i + 6].GetDstP());
	}
	for (int i = 0; i < 3; i++)
	{
		SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("treeFar"), bgArray[i + 9].GetSrcP(), bgArray[i + 9].GetDstP());
	}
	for (int i = 0; i < 3; i++)
	{
		SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("treeClose"), bgArray[i + 12].GetSrcP(), bgArray[i + 12].GetDstP());
	}
	//Pause Btn TODO: Change sprite
	if (Engine::Instance().Pause() == false)
	{
		SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("quit"),
			m_pauseBtn->GetSrcP(), m_pauseBtn->GetDstP());
	}



		// Player.
	SDL_RenderCopyEx(Engine::Instance().GetRenderer(), TEMA::GetTexture("0x72"), m_player->GetSrcP(), m_player->GetDstP(), 0, &m_pivot, SDL_FLIP_NONE);
	/*SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 255, 128);
	SDL_RenderFillRect(m_pRenderer, m_player->GetDstP());*/
	// Player bullets.	

	for (int i = 0; i < (int)m_vPBullets.size(); i++)
	{
		SDL_RenderCopyEx(Engine::Instance().GetRenderer(), TEMA::GetTexture("EnergyBall"), m_vPBullets[i]->GetSrcP(), m_vPBullets[i]->GetDstP(), 0, &m_pivot, SDL_FLIP_NONE);
		/*SDL_SetRenderDrawColor(m_pRenderer, 255, 255, 0, 128);
		SDL_RenderFillRect(m_pRenderer, m_vPBullets[i]->GetDstP());*/
	}
	// Enemies.
	for (int i = 0; i < (int)m_vEnemies.size(); i++)
	{
		SDL_RenderCopyEx(Engine::Instance().GetRenderer(), TEMA::GetTexture("sprites"), m_vEnemies[i]->GetSrcP(), m_vEnemies[i]->GetDstP(), -90, &m_pivot, SDL_FLIP_NONE);
		/*SDL_SetRenderDrawColor(m_pRenderer, 255, 0, 0, 128);
		SDL_RenderFillRect(m_pRenderer, m_vEnemies[i]->GetDstP());*/
	}
	// Enemy bullets.
	for (int i = 0; i < (int)m_vEBullets.size(); i++)
		SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("sprites"), m_vEBullets[i]->GetSrcP(), m_vEBullets[i]->GetDstP());
	// Resume and quit Btn
	if (Engine::Instance().Pause() == true)
	{
		SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("quit"),
			m_quitBtn->GetSrcP(), m_quitBtn->GetDstP());
		SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("play"),
			m_resumeBtn->GetSrcP(), m_resumeBtn->GetDstP());
	}
	SDL_RenderPresent(Engine::Instance().GetRenderer());
	if (dynamic_cast<PlayState*>(STMA::GetStates().back()))
		State::Render();
}


void PlayState::Exit()
{
	delete m_player;
	m_player = nullptr;

}

void PlayState::Resume()
{

}

TitleState::TitleState() {


}

void TitleState::Enter()
{
	
	m_playBtn = new PlayButton({ 0,0,480,140 }, { 400, 768 / 2,240,70 });
	m_quitBtn = new QuitButton({ 0,0,480,140 }, { 640, 768 / 2,240,70 });

}

void TitleState::Update()
{

	if (m_playBtn->Update() == 1)
		return;
	if (m_quitBtn->Update() == 1)
		return;
}

void TitleState::Render()
{
	SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("titleBG"), 0, 0);
	SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("play"), 
		m_playBtn->GetSrcP(), m_playBtn->GetDstP());
	SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("quit"),
		m_quitBtn->GetSrcP(), m_quitBtn->GetDstP());
	State::Render();

	//SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 128, 0, 255, 255);
	//SDL_RenderClear(Engine::Instance().GetRenderer());

	State::Render();
}

void TitleState::Exit()
{
	std::cout << "Exiting TitleState..." << std::endl;
}