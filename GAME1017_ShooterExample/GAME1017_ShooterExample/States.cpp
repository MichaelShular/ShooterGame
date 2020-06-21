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
	m_bPBNull =  m_loseState = false;
	m_bCanShoot = true;
	m_Hp = 8;
	srand((unsigned)time(NULL));
}

void PlayState::Enter()
{
	for (int f = 0; f < 5; f++)
	{
		bgArray[f * 3] = { {0,0,1280,720}, {0, 0, 1280, 720} };
		bgArray[f * 3 + 1] = { {0,0,1280,720}, {1280, 0, 1280, 720} };
		bgArray[f * 3 + 2] = { {0,0,1280,720}, {-1280, 0, 1280, 720} };
	}

	Hp[0] = { { 289, 258, 13, 12 }, { 1000 , 725, 65, 60 } };
	Hp[1] = { { 289, 258, 13, 12 }, { 1070 , 725, 65, 60 } };
	Hp[2] = { { 289, 258, 13, 12 }, { 1140 , 725, 65, 60 } };
	Hp[3] = { { 289, 258, 13, 12 }, { 1210 , 725, 65, 60 } };

	m_scoreString = "SCORE : ";
	m_pauseBtn = new PauseButton({ 0,0,67,16 }, { 10, 736, 198,48 });
	m_resumeBtn = new ResumeButton({ 0,0,67,16 }, { 541, 288 ,198,48 });
	m_volIn = new VolInButton({ 0, 21, 11, 11 }, { 607, 732, 55, 55 });
	m_volDe = new VolDeButton({ 0, 16, 11, 5 }, { 673, 745, 55, 28 });
	m_quitBtn = new QuitButton({ 0,0,67,16 }, { 541, 384 ,198,48 });
	m_player = new Player( { 129, 171, 16, 22 },{ 10, 360, 32, 44} );
	FOMA::SetSize("Ttf/Munro.ttf", "Font1", 150);
	gameOver = new Label("Font1", WIDTH / 2 - 400, HEIGHT / 2 - 200, "YOU ARE DEAD");
	pressEnter = new Label("Font2", WIDTH / 2 - 250, HEIGHT / 2, "Press Enter");
	FOMA::SetSize("Ttf/Munro.ttf", "Font1", 40);
	pause = new Label("Font1", 40, 737, "P A U S E", { 0, 0, 0, 0 });
	resume = new Label("Font1", 558, 289, "R E S U M E", { 0, 0, 0, 0 });
	quit = new Label("Font1", 575, 385, "Q  U  I  T", { 0, 0, 0, 0 });
	Score = new Label("Font1", 0, 20, m_scoreString, { 0, 0, 0, 0 });
	FinalScore = new Label("Font2", WIDTH / 2 - 250, HEIGHT / 2 + 200, "boi");
	SOMA::SetMusicVolume(Engine::Instance().getvol());
	SOMA::PlayMusic("PBGM");
	SOMA::SetSoundVolume(Engine::Instance().getvol());
	m_playerStartX = m_player->GetSrcP()->x;
}

void PlayState::Update()
{
	if (m_pauseBtn->Update() == 1)
		return;
	if (m_volIn->Update() == 1)
		return;
	if (m_volDe->Update() == 1)
		return;
	if (Engine::Instance().Pause() == true)
	{
		if (m_resumeBtn->Update() == 1)
			return;
		if (m_quitBtn->Update() == 1)
			return;
	}
	
	int xpos = 289;
	if (m_Hp % 2 == 0)
		xpos = 321;
	if (m_Hp % 2 == 1)
		xpos = 305;
	
	if(m_Hp == 6 || m_Hp == 7)
		Hp[0].GetSrcP()->x = xpos;
	else if (m_Hp == 5 || m_Hp == 4)
		Hp[1].GetSrcP()->x = xpos;
	else if (m_Hp == 3 || m_Hp == 2)
		Hp[2].GetSrcP()->x = xpos;
	else if (m_Hp == 1)
		Hp[3].GetSrcP()->x = xpos;
	else if (m_Hp == 0)
	{
		Hp[3].GetSrcP()->x = 321;
		m_player->GetSrcP()->x = 371;
		m_player->GetSrcP()->y = 80;
		m_player->GetSrcP()->w = 10;
		m_player->GetSrcP()->h = 18;
		Engine::Instance().Dead() = true;
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
		if (m_Hp > 0)
			m_player->Animate(m_playerStartX); // Oh! We're telling the player to animate itself. This is good! Hint hint.
		
		if (m_Hp < 1)
			m_player->Animate(371, 6);
		if (EVMA::KeyHeld(SDL_SCANCODE_A) && m_player->GetDstP()->x > m_player->GetDstP()->h)
			m_player->GetDstP()->x -= PSPEED;
		else if (EVMA::KeyHeld(SDL_SCANCODE_D) && m_player->GetDstP()->x < WIDTH / 2)
			m_player->GetDstP()->x += PSPEED;
		if (EVMA::KeyHeld(SDL_SCANCODE_W) && m_player->GetDstP()->y > 0)
			m_player->GetDstP()->y -= PSPEED;
		else if (EVMA::KeyHeld(SDL_SCANCODE_S) && m_player->GetDstP()->y < HEIGHT - m_player->GetDstP()->w)
			m_player->GetDstP()->y += PSPEED;
		if (Engine::Instance().Dead() == true)
		{
			FinalScore = new Label("Font2", WIDTH / 2 - 250, HEIGHT / 2 + 200, m_newScoreString);
			SOMA::SetSoundVolume(0);
			if (EVMA::KeyHeld(SDL_SCANCODE_KP_ENTER) || EVMA::KeyHeld(SDL_SCANCODE_RETURN))
			{		
				m_loseState = true;
			}
		}
		//Volume Control
		if (EVMA::KeyHeld(SDL_SCANCODE_MINUS))
		{
			SOMA::SetAllVolume(Engine::Instance().setvol(-1));
			std::cout << Engine::Instance().getvol() << endl;
		}
		else if (EVMA::KeyHeld(SDL_SCANCODE_EQUALS))
		{
			SOMA::SetAllVolume(Engine::Instance().setvol(1));
			std::cout << Engine::Instance().getvol() << endl;
		}

		if (EVMA::KeyHeld(SDL_SCANCODE_SPACE) && m_bCanShoot)
		{
			m_bCanShoot = false;
			m_vPBullets.push_back(new Bullet({ 0,0,128,128 }, { m_player->GetDstP()->x + 16,m_player->GetDstP()->y + 10, 50, 50 }, 30));
			SOMA::PlaySound("laser");

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
			m_vEnemies.push_back(new Enemy({ 21,366,22,33 }, { WIDTH,56 + rand() % (HEIGHT - 114),44,66 }, &m_vEBullets,
				50 + rand() % 91)); // Randomizing enemy bullet spawn to every 30-120 frames.
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
		if (Engine::Instance().Dead() == false)
			CheckCollision();
	}	
	//Score update
	int x = Engine::Instance().getSco();
	m_numScore = std::to_string(x);
	m_newScoreString = m_scoreString + m_numScore;
	Score->SetText(m_scoreString);
	Score = new Label("Font1", 0, 20, m_newScoreString, { 0, 0, 0, 0 });
	if (m_loseState)
	{
		STMA::ChangeState(new LoseState);
	}
}

void PlayState::CheckCollision()
{
	// Player vs. Enemy.
	SDL_Rect p = { m_player->GetDstP()->x - 20, m_player->GetDstP()->y , 32, 44 };
	for (int i = 0; i < (int)m_vEnemies.size(); i++)
	{
		SDL_Rect e = { m_vEnemies[i]->GetDstP()->x, m_vEnemies[i]->GetDstP()->y +5, 44,60 };
		if (SDL_HasIntersection(&p, &e))
		{
			delete m_vEnemies[i];
			m_vEnemies[i] = nullptr;
			// Game over!
			cout << "Player goes boom!" << endl;
			SOMA::PlaySound("explode");
			m_bENull = true;
			--m_Hp;
			if (Engine::Instance().getSco() > 15)
				Engine::Instance().setSco(-20);
			break;
		}
	}
	if (m_bENull) CleanVector<Enemy*>(m_vEnemies, m_bENull);
	// Player bullets vs. Enemies.
	for (int i = 0; i < (int)m_vPBullets.size(); i++)
	{
		SDL_Rect b = { m_vPBullets[i]->GetDstP()->x - 25, m_vPBullets[i]->GetDstP()->y, 50, 50 };
		for (int j = 0; j < (int)m_vEnemies.size(); j++)
		{
			if (m_vEnemies[j] == nullptr) continue;
			SDL_Rect e = { m_vEnemies[j]->GetDstP()->x, m_vEnemies[j]->GetDstP()->y +5, 44, 60 };
			if (SDL_HasIntersection(&b, &e))
			{
				SOMA::PlaySound("explode");
				delete m_vEnemies[j];
				m_vEnemies[j] = nullptr;
				delete m_vPBullets[i];
				m_vPBullets[i] = nullptr;
				m_bENull = true;
				m_bPBNull = true;
				Engine::Instance().setSco(10);
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
			--m_Hp;
			if (Engine::Instance().getSco() > 0)
				Engine::Instance().setSco(-5);
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
	//for volume
	SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"),
		m_volIn->GetSrcP(), m_volIn->GetDstP());
	SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"),
		m_volDe->GetSrcP(), m_volDe->GetDstP());
	//Pause Btn TODO: Change sprite
	SDL_RenderCopyEx(Engine::Instance().GetRenderer(), TEMA::GetTexture("0x72"), m_player->GetSrcP(), m_player->GetDstP(), 0, &m_pivot, SDL_FLIP_NONE);
	if (Engine::Instance().Dead() == false)
	{
		if (Engine::Instance().Pause() == false)
		{
			
			SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"),
				m_pauseBtn->GetSrcP(), m_pauseBtn->GetDstP());
			pause->Render();
		}
		for (int i = 0; i < 4; ++i)
		{
			SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("0x72"), Hp[i].GetSrcP(), Hp[i].GetDstP());
		}
		Score->Render();

		// Player.
		
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
			SDL_RenderCopyEx(Engine::Instance().GetRenderer(), TEMA::GetTexture("0x72"), m_vEnemies[i]->GetSrcP(), m_vEnemies[i]->GetDstP(), 0, &m_pivot, SDL_FLIP_HORIZONTAL);
			/*SDL_SetRenderDrawColor(m_pRenderer, 255, 0, 0, 128);
			SDL_RenderFillRect(m_pRenderer, m_vEnemies[i]->GetDstP());*/
		}
		// Enemy bullets.
		for (int i = 0; i < (int)m_vEBullets.size(); i++)
			SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("sprites"), m_vEBullets[i]->GetSrcP(), m_vEBullets[i]->GetDstP());
		// Resume and quit Btn
		if (Engine::Instance().Pause() == true)
		{
			
			SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"),
				m_quitBtn->GetSrcP(), m_quitBtn->GetDstP());
			SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"),
				m_resumeBtn->GetSrcP(), m_resumeBtn->GetDstP());
			quit->Render();
			resume->Render();
		}
	}
	if (Engine::Instance().Dead() == true)
	{
		gameOver->Render();
		pressEnter->Render();
		FinalScore->Render();
	}
	SDL_RenderPresent(Engine::Instance().GetRenderer());
	if (dynamic_cast<PlayState*>(STMA::GetStates().back()))
		State::Render();
}


void PlayState::Exit()
{
	SOMA::StopMusic();
	delete m_player;
	m_player = nullptr;

}

void PlayState::Resume()
{

}

TitleState::TitleState() {}

void TitleState::Enter()
{
	for (int f = 0; f < 5; f++)
	{
		bgArray[f * 3] = { {0,0,1280,720}, {0, 0, 1280, 720} };
		bgArray[f * 3 + 1] = { {0,0,1280,720}, {1280, 0, 1280, 720} };
		bgArray[f * 3 + 2] = { {0,0,1280,720}, {-1280, 0, 1280, 720} };
	}
	m_playBtn = new PlayButton({ 0,0,67,16 }, { 541, 288 ,198,48});
	m_quitBtn = new QuitButton({ 0,0,67,16 }, { 541, 384 ,198,48 });
	FOMA::SetSize("Ttf/Munro.ttf", "Font1", 40);
	play = new Label("Font1", 571, 289, "P  L  A  Y", { 0, 0, 0, 0 });
	quit = new Label("Font1", 573, 385, "Q  U  I  T", { 0, 0, 0, 0 });
	m_volIn = new VolInButton({0, 21, 11, 11}, { 607, 732, 55, 55 });
	m_volDe = new VolDeButton({0, 16, 11, 5}, { 673, 745, 55, 28 });
	SOMA::SetMusicVolume(20);
	SOMA::PlayMusic("TBGM");
}

void TitleState::Update()
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
	if (m_playBtn->Update() == 1)
		return;
	if (m_quitBtn->Update() == 1)
		return;
	if (m_volIn->Update() == 1)
		return;
	if (m_volDe->Update() == 1)
		return;
	if (EVMA::KeyHeld(SDL_SCANCODE_MINUS))
	{
		SOMA::SetAllVolume(Engine::Instance().setvol(-1));
		std::cout << Engine::Instance().getvol() << endl;
	}
	else if (EVMA::KeyHeld(SDL_SCANCODE_EQUALS))
	{
		SOMA::SetAllVolume(Engine::Instance().setvol(1));
		std::cout << Engine::Instance().getvol() << endl;
	}
}

void TitleState::Render()
{
	for (int i = 0; i < 3; i++)
	{
		SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("titleBG"), bgArray[i].GetSrcP(), bgArray[i].GetDstP());
	}
	for (int i = 0; i < 3; i++)
	{
		SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("montainFar"), bgArray[i + 3].GetSrcP(), bgArray[i + 3].GetDstP());
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
	
	SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"), 
		m_playBtn->GetSrcP(), m_playBtn->GetDstP());
	SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"),
		m_quitBtn->GetSrcP(), m_quitBtn->GetDstP());
	SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"),
		m_volIn->GetSrcP(), m_volIn->GetDstP());
	SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"),
		m_volDe->GetSrcP(), m_volDe->GetDstP());

	play->Render();
	quit->Render();

	//SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 128, 0, 255, 255);
	//SDL_RenderClear(Engine::Instance().GetRenderer());

	State::Render();
}

void TitleState::Exit()
{
	SOMA::StopMusic();
	std::cout << "Exiting TitleState..." << std::endl;
}

LoseState::LoseState() {}

void LoseState::Update()
{
	if (m_MenuBtn->Update() == 1)
		return;
}

void LoseState::Render()
{
	SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("titleBG"), NULL, NULL);
	SDL_RenderCopy(Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"),
		m_MenuBtn->GetSrcP(), m_MenuBtn->GetDstP());
	menu->Render();
	State::Render();
}

void LoseState::Enter()
{
	
	m_MenuBtn = new MenuButton({ 0,0,67,16 }, { 541, 336, 198,48 });
	menu = new Label("Font1", 568, 336, "M  E  N  U", { 0, 0, 0, 0 });
}

void LoseState::Exit()
{
}
