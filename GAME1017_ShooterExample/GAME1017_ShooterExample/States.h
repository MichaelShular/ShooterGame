#pragma once
#ifndef _STATES_H_
#define _STATES_H_
#include <iostream>
#include <SDL.h>
#include "Sprites.h"



class State // This is the abstract base class for all specific states.
{
protected:
	State() {}
public:
	virtual void Update() = 0; // Having at least one 'pure virtual' method like this, makes a class abtract.
	virtual void Render();     // Meaning we cannot create objects of the class.
	virtual void Enter() = 0;  // Virtual keyword means we can override in derived class.
	virtual void Exit() = 0;
	virtual void Resume();
};

class PlayState : public State
{
private:
	Sprite bgArray[2];
	Player* m_player;
	SDL_Point m_pivot;
	int m_iESpawn, // The enemy spawn frame timer properties.
		m_iESpawnMax;
	bool m_bENull, m_bPBNull, m_bEBNull, m_bCanShoot;
	std::vector<Enemy*> m_vEnemies;
	std::vector<Bullet*> m_vPBullets;
	std::vector<Bullet*> m_vEBullets;
	


public:
	PlayState();
	void Update();
	void CheckCollision();
	void Render();
	void Enter();
	void Exit();
	void Resume();
};


#endif