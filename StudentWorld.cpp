#include "StudentWorld.h"
#include <string>
#include <iostream>
#include <sstream> 
#include <iomanip>
#include <utility>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir)
{
}

StudentWorld:: ~StudentWorld()
{
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	for (int i = 0; i <= 59; i++)
	{
		for (int k = 0; k <= 59; k++)
		{
			if (earth[i][k]!=nullptr)
			{
				delete earth[i][k];
				earth[i][k] = nullptr;
			}
		}
	}
	auto it = actor.begin();
	while (it != actor.end())
	{
		if ((*it) != nullptr)
		{
			delete *it;
		}
		it = actor.erase(it);
	}

}

void StudentWorld::addActor(Actor* a)
{
	actor.push_back(a);
}

void StudentWorld::clearEarth(int x, int y)
{
	for (int i = x; i < x + 4; i++)
	{
		for (int k = y; k < y + 4; k++)
		{
			if (i <= 59 && i >= 0 && k <= 59 && k >= 0 && earth[i][k] != nullptr)
			{
				delete earth[i][k];
				earth[i][k] = nullptr;
				this->playSound(SOUND_DIG);
			}
		}
	}
	
}

int StudentWorld::init()
{

	B = std::min(int(getLevel() / 2 + 2), 9);

	//number of barrels left
	m_numBarrels = std::min(2 + int(getLevel()), 21);

	G = std::max(5 - int(getLevel()) / 2, 2);

	//There is a 1 in C chance that a new Water Pool or Sonar Kit Goodie will be added to the oil field during any particular tick
	C = int(getLevel()) * 25 + 300;

	// A new Protester (Regular or Hardcore) may only be added to the oil field after at least T ticks 
	T = std::max(25, int(200 - getLevel()));

	// P of Protesters that should be on the oil field
	P = std::min(15, 2 + int(getLevel()*1.5));

	//the Protester being a Hard Core Protester (vs. a Regular Protester) must be determined with this formula
	probabilityOfHardcore = std::min(90, int(getLevel()) * 10 + 30);

	//Create Tunnelman
	player = new Tunnelman(this, 30, 60);
	
	//Create Earths
	for (int i = 0; i <= 59; i++)
	{
		for (int k = 0; k <= 59; k++)
		{
			earth[i][k] = new Earth(this, i, k);
			if (i >= 30 && i <= 33 && k >= 4 && k <= 59)
			{
				delete earth[i][k];
				earth[i][k] = nullptr;
			}
		}
	}

	// Create Boulders
	srand(time(NULL));
	for (int i = 0; i < B; i++)
	{
		int x = rand() % 56;
		int y = rand() % 56;
		bool occupied = 1;
		Tunnelman* a = new Tunnelman(this, 30, 60);
		while (occupied)
		{
			if (x >= 26 && x <= 33 && y >= 0 && y <= 59){}
			else if (canActorMoveTo(a, x, y))
			{
				occupied = 0;
				break;
			}
			x = rand() % 56;
			y = rand() % 56;
		}
		for (int i = 0; i < 4; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				if (earth[x + i][y + k] != nullptr)
				{
					delete earth[x + i][y + k];
					earth[x + i][y + k] = nullptr;
				}
			}
		}
		actor.push_back(new Boulder(this, x, y));
		delete a;
		a = nullptr;
	}

	// Create Gold Nuggets
	for(int i = 0; i < G; i++)
	{
		int x = rand() % 56;
		int y = rand() % 56;
		bool occupied = 1;
		Tunnelman* a = new Tunnelman(this, 30, 60);
		while (occupied)
		{
			if (x >= 26 && x <= 33 && y >= 0 && y <= 59) {}
			else if (canActorMoveTo(a, x, y))
			{
				occupied = 0;
				break;
			}
			x = rand() % 56;
			y = rand() % 56;
		}
		actor.push_back(new GoldNugget(this, x, y, false));
		delete a;
		a = nullptr;
	}

	// Create Barrels of Oil
	for (int i = 0; i < m_numBarrels; i++)
	{
		int x = rand() % 56;
		int y = rand() % 56;
		bool occupied = 1;
		Tunnelman* a = new Tunnelman(this, 30, 60);
		while (occupied)
		{
			if (x >= 26 && x <= 33 && y >= 0 && y <= 59) {}
			else if (canActorMoveTo(a, x, y))
			{
				occupied = 0;
				break;
			}
			x = rand() % 56;
			y = rand() % 56;
		}
		actor.push_back(new OilBarrel(this, x, y));
		delete a;
		a = nullptr;
	}

	actor.push_back(new RegularProtester(this, 56, 60));
	/*actor.push_back(new SonarKit(this, 0, 60));*/
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	//// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
	//// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	//decLives();
	//return GWSTATUS_PLAYER_DIED;

	// Update the Game Status Line
	updateDisplayText(); // update the score/lives/level text at screen top

	// The term "Actors" refers to all Protesters, the player, Goodies,
	// Boulders, Barrels of oil, Holes, Squirts, the Exit, etc.
	if (rand() % (C) < 1) //Create Sonar or Water 1/C chance
	{
		if (rand() % 100 < 20) //Create Sonar 20% chance
		{
			if(numSonarKitOnField == 0)
				actor.push_back(new SonarKit(this, 0, 60));
		}
		else //Create Water Pool 80% chance
		{
			int x = rand() % 56;
			int y = rand() % 56;
			bool occupied = 1;
			WaterPool* a = new WaterPool(this, 60, 60);
			while (occupied)
			{
				if (canActorMoveTo(a, x, y))
				{
					occupied = 0;
					break;
				}
			x = rand() % 56;
			y = rand() % 56;
			}
			actor.push_back(new WaterPool(this, x, y));
			delete a;
			a = nullptr;
		}
	}

	// Give each Actor a chance to do something
	for (int i = 0 ;i< actor.size();i++)
	{
		if (actor[i] != nullptr)
		{
			// ask each actor to do something (e.g. move)
			actor[i]->doSomething();
			if (!player->isAlive() || player->getHitPoints() <= 0)
			{
				decLives();
				playSound(SOUND_PLAYER_GIVE_UP);
				return GWSTATUS_PLAYER_DIED;
			}
				
			if (false) //theplayerCompletedTheCurrentLevel() == true
			{
				return GWSTATUS_FINISHED_LEVEL;
			}
		}
	}
		// Remove newly-dead actors after each tick
	removeDeadGameObjects(); // delete dead game objects
	// return the proper result
	if (!player->isAlive())
		return GWSTATUS_PLAYER_DIED;
	else
		player->doSomething();
	// If the player has collected all of the Barrels on the level, then
	// return the result that the player finished the level
	if (m_numBarrels <= 0) //theplayerCompletedTheCurrentLevel() == true
	{
		playFinishedLevelSound();
		return GWSTATUS_FINISHED_LEVEL;
	}

	// the player hasn't completed the current level and hasn't died
	// let them continue playing the current level
	return GWSTATUS_CONTINUE_GAME;
}

bool StudentWorld::canActorMoveTo(Actor* a, int x, int y) const
{
	//if (a == nullptr)
	//	return 0;
	if (x > 56 || x < 0 || y > 60 || y < 0)
		return false;
	
	{
		for (int i = 0; i < 4; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				if (x + i >= 0 && x + i <= 59 && y + k >= 0 && y + k <= 59 
					&& earth[x + i][y + k] != nullptr && !a->canDigThroughEarth())
				{
					return false;
				}
					
			}
				
		}
	}
	
	for (auto it = actor.begin(); it != actor.end(); ++it)
	{
		if ((*it) == nullptr || (*it) == a || (*it)->canActorsPassThroughMe())
			break;
		int xActor = (*it)->getX(), yActor = (*it)->getY();
		std::vector<pair<int, int>> location_theA;
		std::vector<pair<int, int>> location_actors;
		std::pair<int, int> location;
		for (int i = 0; i < 4; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				location.first = x + i;
				location.second = y + k;
				location_theA.push_back(location);
				location.first = xActor + i;
				location.second = yActor + k;
				location_actors.push_back(location);
			}
		}
		for (auto it = location_theA.begin(); it != location_theA.end(); ++it)
		{
			for (auto it2 = location_actors.begin(); it2 != location_actors.end(); ++it2)
			{
				if ((it->first == it2->first && it->second == it2->second))
					return false;
			}
		}
	}
	return true;
}

int StudentWorld::annoyActors(Actor* annoyer, int points, int radius)
{
	int numAnnoyed = 0;
	int x = annoyer->getX(), y = annoyer->getY();
	std::vector<pair<int, int>> location_annoyer;
	std::pair<int, int> location;
	switch (annoyer->getDirection())
	{
	case GraphObject::right:
		for (int i = 0; i < radius; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				location.first = x + i;
				location.second = y + k;
				location_annoyer.push_back(location);
			}
		}
		break;
	case GraphObject::left:
		for (int i = radius; i > 0; i--)
		{
			for (int k = 0; k < 4; k++)
			{
				location.first = x + i;
				location.second = y + k;
				location_annoyer.push_back(location);
			}
		}
		break;
	case GraphObject::up:
		for (int i = 0; i < 4; i++)
		{
			for (int k = 0; k < radius; k++)
			{
				location.first = x + i;
				location.second = y + k;
				location_annoyer.push_back(location);
			}
		}
		break;
	case GraphObject::down:
		for (int i = 0; i < 4; i++)
		{
			for (int k = radius; k > 0; k--)
			{
				location.first = x + i;
				location.second = y + k;
				location_annoyer.push_back(location);
			}
		}
		break;
	default:
		break;
	}

	for (auto it = actor.begin(); it != actor.end(); ++it)
	{
		if ((*it) == nullptr || (*it) == annoyer)
			break;
		int xActor = (*it)->getX(), yActor = (*it)->getY();
		std::vector<pair<int, int>> location_actor;
		for (int i = 0; i < 4; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				location.first = xActor + i;
				location.second = yActor + k;
				location_actor.push_back(location);
			}
		}
		
		for (auto it1 = location_annoyer.begin(); it1 != location_annoyer.end(); ++it1)
		{
			for (auto it2 = location_actor.begin(); it2 != location_actor.end(); ++it2)
			{
				if (it1->first == it2->first && it1->second == it2->second/* && !(*it)->getStunned()*/)
				{
					(*it)->annoyed(points);
					/*(*it)->setStunned(true);*/
					numAnnoyed++;
				}
			}
		}
	}
	return numAnnoyed;
}

void StudentWorld::annoyTunnelman(Actor* annoyer, int points, int radius)
{
	int x = annoyer->getX(), y = annoyer->getY();
	std::vector<pair<int, int>> location_annoyer;
	std::pair<int, int> location;
	switch (annoyer->getDirection())
	{
	case GraphObject::right:
		for (int i = 0; i < radius; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				location.first = x + i;
				location.second = y + k;
				location_annoyer.push_back(location);
			}
		}
		break;
	case GraphObject::left:
		for (int i = radius; i > 0; i--)
		{
			for (int k = 0; k < 4; k++)
			{
				location.first = x + i;
				location.second = y + k;
				location_annoyer.push_back(location);
			}
		}
		break;
	case GraphObject::up:
		for (int i = 0; i < 4; i++)
		{
			for (int k = 0; k < radius; k++)
			{
				location.first = x + i;
				location.second = y + k;
				location_annoyer.push_back(location);
			}
		}
		break;
	case GraphObject::down:
		for (int i = 0; i < 4; i++)
		{
			for (int k = radius; k > 0; k--)
			{
				location.first = x + i;
				location.second = y + k;
				location_annoyer.push_back(location);
			}
		}
		break;
	default:
		break;
	}

	int xActor = (player)->getX(), yActor = (player)->getY();
	std::vector<pair<int, int>> location_player;
	for (int i = 0; i < 4; i++)
	{
		for (int k = 0; k < 4; k++)
		{
			location.first = xActor + i;
			location.second = yActor + k;
			location_player.push_back(location);
		}
	}
	for (auto it1 = location_annoyer.begin(); it1 != location_annoyer.end(); ++it1)
	{
		for (auto it2 = location_player.begin(); it2 != location_player.end(); ++it2)
		{
			if (it1->first == it2->first && it1->second == it2->second)
			{
				if (!player->isStunned())
				{
					if (player->getHitPoints() < points)
					{
						(player)->annoyed(player->getHitPoints());
					}
					else
					{
						(player)->annoyed(points);
						int timeStunned = points / 20;
						player->setStunned(points);
					}
				}
			}
		}
	}
}

void StudentWorld::removeDeadGameObjects()
{
	auto it = actor.begin();
	while (it != actor.end())
	{
		if ((*it) != nullptr && !(*it)->isAlive())
		{
			delete *it;
			it = actor.erase(it);
		}
		else
			it++;
	}
}

void StudentWorld::playFinishedLevelSound()
{
	this->playSound(SOUND_FINISHED_LEVEL);
}

void StudentWorld::cleanUp()
{
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	for (int i = 0; i <= 59; i++)
	{
		for (int k = 0; k <= 59; k++)
		{
			if (earth[i][k] != nullptr)
			{
				delete earth[i][k];
				earth[i][k] = nullptr;
			}
		}
	}
	auto it = actor.begin();
	while(it != actor.end())
	{
		if ((*it) != nullptr)
		{
			delete *it;
		}
		it = actor.erase(it);
	}
}

void StudentWorld::updateDisplayText()
{
	int level = this->getLevel();
	int lives = this->getLives();
	int health = this->player->getHitPoints();
	int squirts = this->player->getWater();
	int gold = this->player->getGold(); //getPlayerGoldCount();
	int barrelsLeft = m_numBarrels; //getNumberOfBarrelsRemainingToBePickedUp();
	int sonar = this->player->getSonar(); //getPlayerSonarChargeCount(); 
	int score = this->getScore();

	// Next, create a string from your statistics, of the form:
	// Lvl: 52 Lives: 3 Hlth: 80% Wtr: 20 Gld: 3 Oil Left: 2 Sonar: 1 Scr: 321000
	ostringstream oss;
	oss.setf(ios::fixed);  
	oss.precision(0);
	
	oss << std::setw(5) << "Lvl: " << std::setw(2) << level ;
	oss << std::setw(8) << "Lives: " << std::setw(1) << lives ;
	oss << std::setw(7) << "Hlth: " << std::setw(3) << health*10 <<"%";
	oss << std::setw(6) << "Wtr: " << std::setw(2) << squirts ;
	oss << std::setw(6) << "Gld: " << std::setw(2) << gold ;
	oss << std::setw(11) << "Oil Left: " << std::setw(2) << barrelsLeft ;
	oss << std::setw(8) << "Sonar: " << std::setw(2) << sonar ;
	oss << std::setw(6) << "Scr: ";
	oss.fill('0');
	oss << std::setw(6) << score;
	string s = oss.str();//someFunctionYouUseToFormatThingsNicely(level, lives, health, squirts, gold, barrelsLeft, sonar, score);
	// Finally, update the display text at the top of the screen with your
	setGameStatText(s);
	// calls our provided GameWorld::setGameStatText
}

Actor* StudentWorld::scanPickerUpper(Actor* a, int radius) const
{
	return player;
}

bool StudentWorld::isTunnelmanNearBy(Actor* a, int radius) const
{
	if (a == nullptr)
		return 0;
	int x = a->getX(), y = a->getY();
	std::vector<pair<int, int>> location_a;
	std::pair<int, int> location;
	for (int i = 0 - radius; i < 4 + radius; i++)
	{
		for (int k = 0 - radius; k < 4 + radius; k++)
		{
			if (x + i >= 0 && x + i <= 59 && y + k >= 0 && y + k <= 63)
			{
				location.first = x + i;
				location.second = y + k;
				location_a.push_back(location);
			}
		}
	}

	int xActor = (player)->getX(), yActor = (player)->getY();
	std::vector<pair<int, int>> location_player;
	for (int i = 0; i < 4; i++)
	{
		for (int k = 0; k < 4; k++)
		{
			location.first = xActor + i;
			location.second = yActor + k;
			location_player.push_back(location);
		}
	}
	for (auto it1 = location_a.begin(); it1 != location_a.end(); ++it1)
	{
		for (auto it2 = location_player.begin(); it2 != location_player.end(); ++it2)
		{
			if (it1->first == it2->first && it1->second == it2->second)
			{
				return true;
			}
		}
	}
	return false;
}

void StudentWorld::revealNearbyObjects(int x, int y, int radius)
{
	std::vector<pair<int, int>> location_a;
	std::pair<int, int> location;
	for (int i = 0 - radius; i < 4 + radius; i++)
	{
		for (int k = 0 - radius; k < 4 + radius; k++)
		{
			if (x + i >= 0 && x + i <= 59 && y + k >= 0 && y + k <= 63)
			{
				location.first = x + i;
				location.second = y + k;
				location_a.push_back(location);
			}
		}
	}

	for (auto it = actor.begin(); it != actor.end(); ++it)
	{
		if ((*it) == nullptr)
			break;
		int xActor = (*it)->getX(), yActor = (*it)->getY();
		std::vector<pair<int, int>> location_actor;
		for (int i = 0; i < 4; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				location.first = xActor + i;
				location.second = yActor + k;
				location_actor.push_back(location);
			}
		}

		for (auto it1 = location_a.begin(); it1 != location_a.end(); ++it1)
		{
			for (auto it2 = location_actor.begin(); it2 != location_actor.end(); ++it2)
			{
				if (it1->first == it2->first && it1->second == it2->second)
				{
					if (!(*it)->isVisible())
						(*it)->setVisible(true);
				}
			}
		}
	}
}

// Is the Actor a facing the Tunnelman?
bool StudentWorld::isFacingTunnelman(Actor* a) const
{
	int x = a->getX(), y = a->getY();
	std::vector<pair<int, int>> location_a;
	std::pair<int, int> location;
	for (int i = 0; i < 4; i++)
	{
		for (int k = 0; k < 4; k++)
		{
			if (x + i >= 0 && x + i <= 59 && y + k >= 0 && y + k <= 63)
			{
				location.first = x + i;
				location.second = y + k;
				location_a.push_back(location);
			}
		}
	}
	int xPlayer = (player)->getX(), yPlayer = (player)->getY();
	std::vector<pair<int, int>> location_player;
	for (int i = 0; i < 4; i++)
	{
		for (int k = 0; k < 4; k++)
		{
			location.first = xPlayer + i;
			location.second = yPlayer + k;
			location_player.push_back(location);
		}
	}
	for (auto it1 = location_a.begin(); it1 != location_a.end(); ++it1)
	{
		for (auto it2 = location_player.begin(); it2 != location_player.end(); ++it2)
		{
			if (it1->first == it2->first)
			{
				if (y < yPlayer) //direction is up
				{
					if (a->getDirection() == GraphObject::up)
						return true;
					return false;
				}
				else  //direction is down
				{
					if (a->getDirection() == GraphObject::down)
						return true;
					return false;
				}
			}
			if (it1->second == it2->second)
			{
				if (x < xPlayer) //direction is right
				{
					if (a->getDirection() == GraphObject::right)
						return true;
					return false;
				}
				else  //direction is left
				{
					if (a->getDirection() == GraphObject::left)
						return true;
					return false;
				}
			}
		}
	}
	return false;
}

// return the direction of the line of sight to the Tunnelman
GraphObject::Direction StudentWorld::lineOfSightToTunnelman(Actor* a) const
{
	int x = a->getX(), y = a->getY();
	std::vector<pair<int, int>> location_a;
	std::pair<int, int> location;
	for (int i = 0 ; i < 4; i++)
	{
		for (int k = 0; k < 4; k++)
		{
			if (x + i >= 0 && x + i <= 59 && y + k >= 0 && y + k <= 63)
			{
				location.first = x + i;
				location.second = y + k;
				location_a.push_back(location);
			}
		}
	}
	int xPlayer = (player)->getX(), yPlayer = (player)->getY();
	std::vector<pair<int, int>> location_player;
	for (int i = 0; i < 4; i++)
	{
		for (int k = 0; k < 4; k++)
		{
			location.first = xPlayer + i;
			location.second = yPlayer + k;
			location_player.push_back(location);
		}
	}
	for (auto it1 = location_a.begin(); it1 != location_a.end(); ++it1)
	{
		for (auto it2 = location_player.begin(); it2 != location_player.end(); ++it2)
		{
			if (it1->first == it2->first)
			{
				int distance;
				if (y < yPlayer) //direction is up
				{
					distance = yPlayer - y;
					for (int i = 1; i < distance; i++)
					{
						if(!canActorMoveTo(a, x, y + i))
							return GraphObject::none;
					}
					return GraphObject::up;
				}
				else  //direction is down
				{
					distance = y - yPlayer;
					for (int i = 1; i < distance; i++)
					{
						if (!canActorMoveTo(a, x, y - i))
							return GraphObject::none;
					}
					return GraphObject::down;
				}
			}
			if (it1->second == it2->second)
			{
				int distance;
				if (x < xPlayer) //direction is right
				{
					distance = xPlayer - x;
					for (int i = 1; i < distance; i++)
					{
						if (!canActorMoveTo(a, x + i, y))
							return GraphObject::none;
					}
					return GraphObject::right;
				}
				else  //direction is left
				{
					distance = x - xPlayer;
					for (int i = 1; i < distance; i++)
					{
						if (!canActorMoveTo(a, x - i, y))
							return GraphObject::none;
					}
					return GraphObject::left;
				}
			}
		}
	}
	return GraphObject::none;
}

//return Tunnelman pointer within the radius
Actor* StudentWorld::findTunnelman(Actor* a, int radius) const
{
	if (a == nullptr)
		return nullptr;
	int x = a->getX(), y = a->getY();
	std::vector<pair<int, int>> location_a;
	std::pair<int, int> location;
	for (int i = 0 - radius; i < 4 + radius; i++)
	{
		for (int k = 0 - radius; k < 4 + radius; k++)
		{
			if (x + i >= 0 && x + i <= 59 && y + k >= 0 && y + k <= 63)
			{
				location.first = x + i;
				location.second = y + k;
				location_a.push_back(location);
			}
		}
	}

	int xActor = (player)->getX(), yActor = (player)->getY();
	std::vector<pair<int, int>> location_player;
	for (int i = 0; i < 4; i++)
	{
		for (int k = 0; k < 4; k++)
		{
			location.first = xActor + i;
			location.second = yActor + k;
			location_player.push_back(location);
		}
	}
	for (auto it1 = location_a.begin(); it1 != location_a.end(); ++it1)
	{
		for (auto it2 = location_player.begin(); it2 != location_player.end(); ++it2)
		{
			if (it1->first == it2->first && it1->second == it2->second)
			{
				return player;
			}
		}
	}
	return nullptr;
}