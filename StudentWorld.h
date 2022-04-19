#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <algorithm>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
//class Actor;
//class Tunnelman;
//class Earth;
class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);
	virtual ~StudentWorld();

	void addActor(Actor* a);
	void clearEarth(int x, int y);
	virtual int init();
	virtual int move();
	virtual void cleanUp();

	bool canActorMoveTo(Actor* a, int x, int y) const;
	int annoyActors(Actor* annoyer, int points, int radius);
	void annoyTunnelman(Actor* annoyer, int points, int radius);
	Actor* scanPickerUpper(Actor* a, int radius) const;
	bool isTunnelmanNearBy(Actor* a, int radius) const;
	void giveSonar() { player->addSonar(); };
	void giveWater() { player->addWater(); };
	void giveGold() { player->addGold(); };
	void decOilBarrel() { m_numBarrels--; };
	void setSonarKitNum(int num) { numSonarKitOnField = num; };
	void revealNearbyObjects(int x, int y, int radius);
	void stunSomeone(Human * victim, int stunTime) { victim->setStunned(stunTime); };

	// Is the Actor a facing the Tunnelman?
	bool isFacingTunnelman(Actor* a) const;

	// return the direction of the line of sight to the Tunnelman
	GraphObject::Direction lineOfSightToTunnelman(Actor* a) const;

	//return Tunnelman pointer within the radius
	Actor* findTunnelman(Actor* a, int radius) const;

	void removeDeadGameObjects();
	void playFinishedLevelSound();
	void updateDisplayText();
	
private:
	std::vector <Actor*> actor;
	Tunnelman* player;
	Earth* earth[60][60] = {nullptr};

	//G Gold Nuggets in each level
	int G;

	//L Barrels of oil in each level
	int m_numBarrels;

	//B Boulders in each level
	int B; 

	// A new Protester (Regular or Hardcore) may only be added to the oil field after at least T ticks 
	int T;

	// P of Protesters that should be on the oil field
	int P;

	//the Protester being a Hard Core Protester (vs. a Regular Protester) must be determined with this formula
	int probabilityOfHardcore;

	//There is a 1 in C chance that a new Water Pool or Sonar Kit Goodie will be added to the oil field during any particular tick
	int C;

	int numSonarKitOnField = 0;
	
};

#endif // STUDENTWORLD_H_
