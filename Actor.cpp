#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <algorithm>
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

//=======================================================================
//=============================Actor Definition==========================
//=======================================================================
Actor::Actor(StudentWorld* world, int startX, int startY, Direction startDir,
	bool visible, int imageID, double size, int depth) 
	: GraphObject(imageID,startX,startY,startDir,size,depth) 
{
	m_world=std::make_shared <StudentWorld*> (world);
	m_alive = 1;
	setVisible(visible);
}

// Is this actor alive?
bool Actor::isAlive() const
{
	return m_alive;
}

// Mark this actor as dead.
void Actor::setDead()
{
	m_alive = 0;
}

// Get this actor's world
StudentWorld* Actor::getWorld() const
{
	return *m_world.get();
}

// Can other actors pass through this actor?
bool Actor::canActorsPassThroughMe() const
{
	return true;
}

void Actor::doSomething()
{
	
}

bool Actor::annoyed(unsigned int amt)
{
	return false;
}

//=======================================================================
//=============================Human Definition======================
//=======================================================================
Human::Human(StudentWorld* world, int startX, int startY, Direction startDir,
	int imageID, unsigned int hitPoints) : Actor(world, startX, startY,startDir, true, imageID, 1.0, 0)
{
	m_hitPoint = hitPoints;
}

int Human::getHitPoints() const
{
	return m_hitPoint;
}

bool Human::annoyed(unsigned int amount)
{
	m_hitPoint -= amount;
	return true;
}

bool Human::canPickThingsUp() const
{
	return true;
}

// Human Actor move one step toward current direction
void Human::moveForward()
{
	switch (getDirection())
	{
	case right:
		if(getWorld()->canActorMoveTo(this, getX() + 1, getY()))
			moveTo(getX() + 1, getY());
		break;
	case left:
		if (getWorld()->canActorMoveTo(this, getX() - 1, getY()))
			moveTo(getX() - 1, getY());
		break;
	case up:
		if (getWorld()->canActorMoveTo(this, getX(), getY() + 1))
			moveTo(getX(), getY() + 1);
		break;
	case down:
		if (getWorld()->canActorMoveTo(this, getX(), getY() - 1))
			moveTo(getX(), getY() - 1);
		break;
	default:
		break;
	}
}
//=======================================================================
//=============================Tunnelman Definition======================
//=======================================================================
Tunnelman::Tunnelman(StudentWorld* world, int startX, int startY) 
	: Human(world, startX, startY, right, TID_PLAYER, 10)
{
	if (world == nullptr)
	{
		std::cerr << "***** must be in some world!" << std::endl;
		exit(1);
	}
	if (startX < 0 || startX > VIEW_WIDTH - 4 || startY < 0 || startY > VIEW_HEIGHT - 4)
	{
		std::cerr << "***** created with invalid coordinates (" << startX << ","
			<< startY << ")!" << std::endl;
		exit(1);
	}
}

bool Tunnelman::annoyed(unsigned int amount)
{
	return Human::annoyed(amount);
}

void Tunnelman::addGold()
{
	m_golds++;
}

void Tunnelman::doSomething()
{
	//Try to get user input(if any is available)  
	//If the user pressed the UP key and that square is open then 
	//Increase my y location by one 
	//If the user pressed the DOWN key and that square is open then 
	//Decrease my y location by one ...  
	//If the user pressed the space bar to fire and the Tunnelman has water, then 
	//Introduce a new Squirt object into the game facing the same direction as the player ... 
	if(isStunned())
		decStunnedTime();
	if (!isAlive() || isStunned())
		return;
	int ch;
	if (getWorld()->getKey(ch) == true)
	{
		// user hit a key this tick!
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			if (getDirection() != left)
				setDirection(left);
			else
			{
				
				if (!getWorld()->canActorMoveTo(this, getX() - 1, getY()))
					moveTo(getX(), getY());
				else
				{
					getWorld()->clearEarth(getX() - 1, getY());
					moveTo(getX() - 1, getY());
				}
			}
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() != right)
				setDirection(right);
			else
			{
				
				if (!getWorld()->canActorMoveTo(this, getX() + 1, getY()))
					moveTo(getX(), getY());
				else
				{
					getWorld()->clearEarth(getX() + 1, getY());
					moveTo(getX() + 1, getY());
				}
			}
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() != down)
				setDirection(down);
			else
			{
				
				if (!getWorld()->canActorMoveTo(this, getX(), getY() - 1))
					moveTo(getX(), getY());
				else
				{
					getWorld()->clearEarth(getX(), getY() - 1);
					moveTo(getX(), getY() - 1);
				}
			}
			break;
		case KEY_PRESS_UP:
			if (getDirection() != up)
				setDirection(up);
			else
			{
				if (!getWorld()->canActorMoveTo(this, getX(), getY() + 1))
					moveTo(getX(), getY());
				else
				{
					getWorld()->clearEarth(getX(), getY() + 1);
					moveTo(getX(), getY() + 1);
				}
			}
			break;
		case KEY_PRESS_SPACE:
			if (m_squirts > 0)
			{
				getWorld()->addActor(new Squirt(getWorld(), this->getX(), this->getY(), this->getDirection()));
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
				m_squirts--;
			}
			break;
		case 'z':
			if (m_sonars > 0)
			{
				getWorld()->playSound(SOUND_SONAR);
				getWorld()->revealNearbyObjects(this->getX(), this->getY(), 12);
				m_sonars--;
			}
			break;
		case 'Z':
			if (m_sonars > 0)
			{
				getWorld()->playSound(SOUND_SONAR);
				getWorld()->revealNearbyObjects(this->getX(), this->getY(), 12);
				m_sonars--;
			}
			break;
		default:
			break;
		}
	}
}
//=======================================================================
//=============================Protester Definition======================
//=======================================================================
Protester::Protester(StudentWorld* world, int startX, int startY, int imageID,
	unsigned int hitPoints) 
	: Human(world, startX, startY, left, imageID, hitPoints) {};

void Protester::addGold()
{

}

bool Protester::annoyed(unsigned int amount)
{
	return Human::annoyed(amount);
}

void Protester::setMustLeaveOilField()
{

}

void Protester::doSomething()
{
	if (!isAlive())
		return;
}

//=============================Regular Protester Definition======================
RegularProtester::RegularProtester(StudentWorld* world, int startX, int startY)
	: Protester(world, startX, startY, TID_PROTESTER, 5) 
{
	ticksToDoSomething = std::max(0, 3 - int(world->getLevel()) / 4);
};

void RegularProtester::doSomething()
{
	//If I am facing the Tunnelman and he is next to me, then 
	//Shout at the Tunnelman(to annoy him)  
	//Else if the Tunnelman is visible via direct line of sight, then 
	//Switch direction to face the Tunnelman  
	//Move one square in this direction  
	//Else if I’m about to run into an obstacle, then 
	//Pick a new direction Move one square in this direction 
	//Else  Move one square in my current direction 
	if (!isAlive())
		return;
	if (--currentTicks <= 0)
	{
		currentTicks = ticksToDoSomething;
		if (--ticksToChargePower <= 0)
			ticksToChargePower = 0;
		if (getHitPoints() <= 0)
		{
			if (getX() == 60 && getY() == 60)
				setDead();
			else
				setMustLeaveOilField();
		}
		else 
			if (getWorld()->isTunnelmanNearBy(this, 4) 
				&& getWorld()->isFacingTunnelman(this) && ticksToChargePower == 0)
			{
				getWorld()->findTunnelman(this, 50)->annoyed(2);
				getWorld()->playSound(SOUND_PROTESTER_YELL);
				ticksToChargePower = 15;
				return;
			}
			else
				if (getWorld()->lineOfSightToTunnelman(this)!=none)
				{
					setDirection(getWorld()->lineOfSightToTunnelman(this));
					moveForward();
					numSquaresToMoveInCurrentDirection = 10;
				}
				else
				{
					/*if (--numSquaresToMoveInCurrentDirection <= 0)
					{
						numSquaresToMoveInCurrentDirection = 10;
						do {
							GraphObject::Direction r = GraphObject::Direction (rand() % 4 + 1);
						}while()
						
						setDirection()
					}
					else*/
						moveForward();
				}
		
	}

}

void RegularProtester::addGold()
{

}


//=============================Hardcore Protester Definition======================
HardcoreProtester::HardcoreProtester(StudentWorld* world, int startX, int startY)
	: Protester(world, startX, startY, TID_HARD_CORE_PROTESTER, 20) 
{
	ticksToDoSomething = std::max(0, 3 - int(world->getLevel()) / 4);
};

void HardcoreProtester::doSomething()
{

}

void HardcoreProtester::addGold()
{

}

//=======================================================================
//=============================Earth Definition==========================
//=======================================================================
Earth::Earth(StudentWorld* world, int startX, int startY)
	: Actor(world, startX, startY, right, true, TID_EARTH, 0.25, 3)
{
	if (world == nullptr)
	{
		std::cerr << "***** Earth must be in some world!" << std::endl;
		exit(1);
	}
	if (startX < 0 || startX > VIEW_WIDTH - 4 || startY < 0 || startY > VIEW_HEIGHT - 4)
	{
		std::cerr << "***** Earth created with invalid coordinates (" << startX << ","
			<< startY << ")!" << std::endl;
		exit(1);
	}
	this->setVisible(true);
}

void Earth::doSomething()
{

}

//=======================================================================
//=============================Boulders Definition======================
//=======================================================================
Boulder::Boulder(StudentWorld* world, int startX, int startY)
	: Actor(world, startX, startY, down, true, TID_BOULDER, 1.0, 1)
{
	if (world == nullptr)
	{
		std::cerr << "***** must be in some world!" << std::endl;
		exit(1);
	}
	if (startX < 0 || startX > VIEW_WIDTH - 4 || startY < 0 || startY > VIEW_HEIGHT - 4)
	{
		std::cerr << "***** created with invalid coordinates (" << startX << ","
			<< startY << ")!" << std::endl;
		exit(1);
	}
	getWorld()->clearEarth(getX(), getY());
	m_currentState = stable;
	this->setVisible(true);
}

Boulder::~Boulder()
{
	/*delete this;*/
}

void Boulder::doSomething()
{
	if (!isAlive())
		return;
	switch (m_currentState)
	{
	case stable:
		if (getWorld()->canActorMoveTo(this, getX(), getY() - 1))
			m_currentState = waiting;
		break;
	case waiting:
		if (m_timeWaiting < 1)
		{
			m_currentState = falling;
			getWorld()->playSound(SOUND_FALLING_ROCK);
		}
		m_timeWaiting--;
		break;
	case falling:
		if (getWorld()->canActorMoveTo(this, getX(), getY() - 1))
		{
			moveTo(getX(), getY() - 1);
			getWorld()->annoyActors(this, 100, 3);
			getWorld()->annoyTunnelman(this, 100, 3);
		}
		else
		{
			setDead();
		}
		break;
	default:
		break;
	}
}

bool Boulder::canActorsPassThroughMe() const 
{
	return false;
}

//====================================================================
//=============================Squirt Definition======================
//=======================================================================
Squirt::Squirt(StudentWorld* world, int startX, int startY, Direction startDir)
	: Actor(world, startX, startY, startDir, true, TID_WATER_SPURT, 1.0, 1)
{
	if (world == nullptr)
	{
		std::cerr << "***** must be in some world!" << std::endl;
		exit(1);
	}
	if (startX < 0 || startX > VIEW_WIDTH - 4 || startY < 0 || startY > VIEW_HEIGHT - 4)
	{
		std::cerr << "***** created with invalid coordinates (" << startX << ","
			<< startY << ")!" << std::endl;
		exit(1);
	}
}

void Squirt::doSomething()
{
	if (!isAlive())
		return;
	if (m_travelDistance <= 0)
	{
		setDead();
		return;
	}
		
	switch (this->getDirection())
	{
	case GraphObject::right:
		if (getWorld()->canActorMoveTo(this, getX() + 1, getY()))
		{
			moveTo(getX() + 1, getY());
			m_travelDistance--;
			if (getWorld()->annoyActors(this, 2, 3) > 0)
			{
				setDead();
				return;
			}
		}
		else
		{
			setDead();
		}
		break;
	case GraphObject::left:
		if (getWorld()->canActorMoveTo(this, getX() - 1, getY()))
		{
			moveTo(getX() - 1, getY());
			m_travelDistance--;
			if (getWorld()->annoyActors(this, 2, 3) > 0)
			{
				setDead();
				return;
			}
		}
		else
		{
			setDead();
		}
		break;
	case GraphObject::up:
		if (getWorld()->canActorMoveTo(this, getX(), getY() + 1))
		{
			moveTo(getX(), getY() + 1);
			m_travelDistance--;
			if (getWorld()->annoyActors(this, 2, 3) > 0)
			{
				setDead();
				return;
			}
		}
		else
		{
			setDead();
		}
		break;
	case GraphObject::down:
		if (getWorld()->canActorMoveTo(this, getX(), getY() - 1))
		{
			moveTo(getX(), getY() - 1);
			m_travelDistance--;
			if (getWorld()->annoyActors(this, 2, 3) > 0)
			{
				setDead();
				return;
			}
		}
		else
		{
			setDead();
		}
		break;
	default:
		break;
	}
	
}


//=============================================================================
//=============================GoodiesObjects Definition======================
//=============================================================================
GoodiesObject::GoodiesObject(StudentWorld* world, int startX, int startY, int imageID,
	int soundToPlay, bool activateOnPlayer, bool activateOnProtester, bool initallyActive)
	: Actor(world, startX, startY, right, false, imageID, 1.0, 2)
{
	if (activateOnPlayer || activateOnProtester) // Oil and Gold
		setVisible(false);
	if (initallyActive)          // Sonar and Water
	{
		setVisible(true);
		setTime();
	}
		
}

void GoodiesObject::setTime()
{
	int b = (300 - 10 * getWorld()->getLevel());
	m_time = std::max( 100, b);
}

void GoodiesObject::decTime()
{
	if (--m_time <= 0)
		setDead();
}

int GoodiesObject::getTime()
{
	return m_time;
}

void GoodiesObject::doSomething()
{
}
//=============================Oil Barrel Definition======================
OilBarrel::OilBarrel(StudentWorld* world, int startX, int startY)
	: GoodiesObject(world, startX, startY, TID_BARREL, SOUND_FOUND_OIL, true, false, false){}

void OilBarrel::doSomething()
{
	if (getTime() != 0)
		decTime();
	if (!isAlive())
		return;
	//check if tunnelman nearby
	if (getWorld()->isTunnelmanNearBy(this, 4))
	{
		if (!isVisible())
			setVisible(true);
	}
	//check if tunnelman or Protester step on
	if (getWorld()->isTunnelmanNearBy(this, 0))
	{
		setDead();
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->decOilBarrel();
		getWorld()->increaseScore(1000);
	}
}

bool OilBarrel::needsToBePickedUpToFinishLevel() const
{
	return true;
}

//=============================GoldNugget Definition======================
GoldNugget::GoldNugget(StudentWorld* world, int startX, int startY, bool temporary)
	: GoodiesObject(world, startX, startY, TID_GOLD, SOUND_GOT_GOODIE, true, true, false) 
{
	if (temporary)
		setTime();
}

void GoldNugget::doSomething()
{
	if (getTime() != 0)
		decTime();
	if (!isAlive())
		return;
	//check if tunnelman nearby
	if (getWorld()->isTunnelmanNearBy(this, 4))
	{
		if (!isVisible())
			setVisible(true);
	}
	//check if tunnelman or Protester step on
	if (getWorld()->isTunnelmanNearBy(this, 0))
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->giveGold();
		getWorld()->increaseScore(10);
	}
}

//=============================Sonar Kit Definition======================
SonarKit::SonarKit(StudentWorld* world, int startX, int startY)
	: GoodiesObject(world, startX, startY, TID_SONAR, SOUND_GOT_GOODIE, false, false, true) 
{
	getWorld()->setSonarKitNum(1);
}
void SonarKit::doSomething()
{
	
	decTime();
	if (!isAlive())
	{
		getWorld()->setSonarKitNum(0);
		return;
	}
		
	//check if tunnelman steps on
	if (getWorld()->isTunnelmanNearBy(this, 0))
	{
		setDead();
		getWorld()->setSonarKitNum(0);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->giveSonar();
		getWorld()->increaseScore(75);
	}
}

//=============================Water Pool Definition======================
WaterPool::WaterPool(StudentWorld* world, int startX, int startY)
	: GoodiesObject(world, startX, startY, TID_WATER_POOL, SOUND_GOT_GOODIE, false, false, true) {}

void WaterPool::doSomething()
{
	if (!isAlive())
		return;
	decTime();
	//check if tunnelman steps on
	if (getWorld()->isTunnelmanNearBy(this, 0))
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->giveWater();
		getWorld()->increaseScore(100);
	}
}