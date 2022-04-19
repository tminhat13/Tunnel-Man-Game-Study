#ifndef ACTOR_H_
#define ACTOR_H_
#include <memory>
#include <algorithm>
#include "GraphObject.h"
class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject 
{
public:    
	Actor(StudentWorld* world, int startX, int startY, Direction startDir,
		bool visible, int imageID, double size, int depth);
	virtual void doSomething();

	bool isAlive() const;

	void setDead();

	StudentWorld* getWorld() const;

	virtual bool canActorsPassThroughMe() const;

	virtual bool annoyed(unsigned int amt);
	
	//// Can this actor dig through Earth?
	virtual bool canDigThroughEarth() const { return false; };

	// Can this actor pick items up?
	virtual bool canPickThingsUp() const { return false; };

	//// Does this actor hunt Tunnelman?
	//virtual bool huntsTunnelman() const;

	// Can this actor need to be picked up to finish the level?
	virtual bool needsToBePickedUpToFinishLevel() const { return false; };

	//// Move this actor to x,y if possible, and return true; otherwise,
	//// return false without moving.
	//bool moveToIfPossible(int x, int y);

private:
	std::shared_ptr <StudentWorld*> m_world;
	bool m_alive;
	
};

class Human : public Actor
{
public:
	Human(StudentWorld* world, int startX, int startY, Direction startDir,
		int imageID, unsigned int hitPoints);

	// Pick up a gold nugget.
	virtual void addGold() = 0;

	// How many hit points does this actor have left?
	int getHitPoints() const;
	virtual bool annoyed(unsigned int amount);
	virtual bool canPickThingsUp() const;
	void setStunned(int time) { m_timeStunned = time; };
	bool isStunned() { return m_timeStunned > 0; };
	void decStunnedTime() { if(m_timeStunned > 0) m_timeStunned--; };

	// Human Actor move one step toward current direction
	void moveForward();
	
private:
	int m_hitPoint;
	int m_timeStunned = 0;
};

class Tunnelman : public Human
{
public:
	Tunnelman(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
	virtual bool annoyed(unsigned int amount);
	virtual void addGold();
	virtual bool canDigThroughEarth() const { return true; };
	void addSonar() { m_sonars++; };
	void addWater() { m_squirts += 5; };
	int getGold() const { return m_golds; };
	int getSonar() const { return m_sonars; };
	int getWater() const { return m_squirts; };

private:
	
	int m_squirts = 5;
	int m_sonars = 1;
	int m_golds = 0;
};


class Protester : public Human
{
public:
	Protester(StudentWorld* world, int startX, int startY, int imageID,
		unsigned int hitPoints);
	virtual void doSomething();
	virtual void addGold();

	virtual bool annoyed(unsigned int amount);
	void setMustLeaveOilField();
	/*void setTicksToNextMove();*/
};

class RegularProtester : public Protester
{
public:
	RegularProtester(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
	virtual void addGold();
private:
	int ticksToDoSomething;
	int currentTicks = 0;
	int ticksToChargePower = 0;
	int numSquaresToMoveInCurrentDirection = 10;
};

class HardcoreProtester : public Protester
{
public:
	HardcoreProtester(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
	virtual void addGold();
private:
	int ticksToDoSomething;
	int currentSticks = 1;
};

class Earth : public Actor
{
public:
	Earth(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
};

class Boulder : public Actor
{
public:
	Boulder(StudentWorld* world, int startX, int startY);
	~Boulder();
	virtual void doSomething();
	virtual bool canActorsPassThroughMe() const;
	
private:
	enum state { stable, waiting, falling };
	state m_currentState;
	int m_timeWaiting = 30;
};

class Squirt : public Actor
{
public:
	Squirt(StudentWorld* world, int startX, int startY, Direction startDir);
	virtual void doSomething();
private:
	int m_travelDistance = 4;
};



class GoodiesObject : public Actor
{
public:
	GoodiesObject(StudentWorld* world, int startX, int startY, int imageID,
		int soundToPlay, bool activateOnPlayer, bool activateOnProtester, bool initallyActive);
	virtual void doSomething();

	void setTime();
	void decTime();
	int getTime();

private:
	int m_time = 0;
	bool m_activateOnPlayer;
	bool m_activateOnProtester;
	bool m_initallyActive;
};

class OilBarrel : public GoodiesObject
{
public:
	OilBarrel(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
	virtual bool needsToBePickedUpToFinishLevel() const;
};

class GoldNugget : public GoodiesObject
{
public:
	GoldNugget(StudentWorld* world, int startX, int startY, bool temporary);
	virtual void doSomething();
};

class SonarKit : public GoodiesObject
{
public:
	SonarKit(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
};

class WaterPool : public GoodiesObject
{
public:
	WaterPool(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
};


#endif // ACTOR_H_
