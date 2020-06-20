#include <algorithm> // For min/max.
#include <cmath>	 // For cos/sin.
#include "Sprites.h"
#include "Engine.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "StateManager.h"
using namespace std;

Sprite::Sprite(){}
Sprite::Sprite(SDL_Rect s, SDL_Rect d) :m_rSrc(s), m_rDst(d) {}
SDL_Rect* Sprite::GetSrcP() { return &m_rSrc; }
SDL_Rect* Sprite::GetDstP() { return &m_rDst; }

double Sprite::GetAngle() { return m_angle; }

AnimatedSprite::AnimatedSprite(int a, int fm, int sm, SDL_Rect s, SDL_Rect d) :
	Sprite(s, d), m_iFrameMax(fm), m_iSpriteMax(sm)
{
	// Added this here (and not in header line) just because. Note: initializers ARE resolved before body statements.
	m_angle = a;
	m_iSprite = m_iFrame = 0; // Chaining assignments to the same value.
}

void AnimatedSprite::Animate(int startPosition,int betweenSprite)
{
	m_iFrame++;
	if (m_iFrame == m_iFrameMax)
	{
		m_iFrame = 0;
		m_iSprite++;
		if (m_iSprite == m_iSpriteMax)
		{
			m_iSprite = 0;
		}
	}
	
		m_rSrc.x = startPosition + ((m_rSrc.w + betweenSprite) * m_iSprite);
	
}

Player::Player(SDL_Rect s, SDL_Rect d) : AnimatedSprite(90, 16, 8, s, d) {}

Bullet::Bullet(SDL_Rect s, SDL_Rect d, int spd) :
	Sprite(s, d), m_speed(spd), m_active(true) {}

void Bullet::Update()
{
	
	m_rDst.x += m_speed;
}



Enemy::Enemy(SDL_Rect s, SDL_Rect d, vector<Bullet*>* bVec, int fr) : AnimatedSprite(-90, 4, 4, s, d),
m_pBulletVec(bVec),  m_bulletTimer(0), m_timerMax(fr) {}

void Enemy::Update()
{
	Animate(21,10);
	m_rDst.x -= 3;
	if (m_bulletTimer++ == m_timerMax)
	{
		m_bulletTimer = 0;
		m_pBulletVec->push_back(new Bullet({ 160,100,14,14 }, { m_rDst.x,m_rDst.y + 22,14,14 }, -7));
		
		SOMA::PlaySound("enemy");
	}
}

Button::Button(SDL_Rect s, SDL_Rect d)
	: Sprite(s, d), m_state(STATE_UP) {}


bool Button::MouseCollision()
{
	const int mx = EVMA::GetMousePos().x;
	const int my = EVMA::GetMousePos().y;
	return (mx < (m_rDst.x + m_rDst.w) && mx > m_rDst.x &&
		my < (m_rDst.y + m_rDst.h) && my > m_rDst.y);
}

int Button::Update()
{
	bool col = MouseCollision();
	switch (m_state)
	{
	case STATE_UP:
		if (col)
			m_state = STATE_OVER;
		break;
	case STATE_OVER:
		if (!col)
			m_state = STATE_UP;
		else if (col && EVMA::MousePressed(1))
			m_state = STATE_DOWN;
		break;
	case STATE_DOWN:
		if (EVMA::MouseReleased(1))
		{
			if (col)
			{
				m_state = STATE_OVER;
				
				Execute();
				return 1;
			}
			else
				m_state = STATE_UP;
		}
		break;
	}
	m_rSrc.x = m_rSrc.w * (int)m_state;
	return 0;
	
}

PlayButton::PlayButton(SDL_Rect src, SDL_Rect dst) :Button(src, dst) {}

void PlayButton::Execute()
{
	//SOMA::SetSoundVolume(60);
	SOMA::PlaySound("laser");
	STMA::ChangeState(new PlayState);
}

QuitButton::QuitButton(SDL_Rect src, SDL_Rect dst) :Button(src, dst) {}

void QuitButton::Execute()
{
	SOMA::PlaySound("laser");
	Engine::Instance().Running() = false;

}

PauseButton::PauseButton(SDL_Rect src, SDL_Rect dst) :Button(src, dst) {}

void PauseButton::Execute()
{
	SOMA::PlaySound("laser");
	Engine::Instance().Pause() = true;
}

ResumeButton::ResumeButton(SDL_Rect src, SDL_Rect dst) :Button(src, dst) {}

void ResumeButton::Execute()
{
	SOMA::PlaySound("laser");
	Engine::Instance().Pause() = false;
}

void MenuButton::Execute()
{
	SOMA::PlaySound("laser");
	STMA::ChangeState(new TitleState);
}

MenuButton::MenuButton(SDL_Rect src, SDL_Rect dst) :Button(src, dst) {}

void VolInButton::Execute()
{
	SOMA::PlaySound("laser");
	SOMA::SetAllVolume(Engine::Instance().setvol(2));
}

VolInButton::VolInButton(SDL_Rect src, SDL_Rect dst) :Button(src, dst) {}

void VolDeButton::Execute()
{
	SOMA::PlaySound("laser");
	SOMA::SetAllVolume(Engine::Instance().setvol(-2));
}

VolDeButton::VolDeButton(SDL_Rect src, SDL_Rect dst) : Button(src, dst) {}
