#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
//#include "Level.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_crystals(0), m_bonus(1000), finishedLevel(false)
{}

StudentWorld::~StudentWorld() //TODO
{ cleanUp(); }

void StudentWorld:: setDisplayText()
{
    int score = getScore();
    int level = getLevel();
    unsigned int bonus = m_bonus;
    int livesLeft = getLives();
    double health = (player->getHealth()/20.00) * 100;
    int ammo = player->getAmmo();
    ostringstream oss;
    oss.fill('0');
    oss << "Score: " << setw(7) << score << "  ";
    oss.fill('0');
    oss << "Level: " << setw(2) << level << "  ";
    oss.fill(' ');
    oss << "Lives: " << setw(2) << livesLeft << "  ";
    oss << "Health: " << setw(3) << health << "%  ";
    oss << "Ammo: " << setw(3) << ammo << "  ";
    oss << "Bonus: " << setw(4) << bonus;
    string s = oss.str(); 
    setGameStatText(s);

}
int StudentWorld::init()
{
    int level = getLevel();
    m_bonus = 1000;
    string lvl = to_string(level);
    if (level < 10)
    {
        lvl = "0" + lvl;
    }
    string curLevel = "level" + lvl + ".txt";
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(curLevel);
    if (result == Level::load_fail_file_not_found || result == Level:: load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR; // something bad happened!
    // otherwise the load was successful and you can access the
    // contents of the level – here’s an example
    for (double x = 0; x < 15; x++)
        for (double y = 0; y < 15; y++)
        {
            Level::MazeEntry item = lev.getContentsOf(x, y);
            switch(item)
            {
                case Level::empty:
                    break;
                case Level::exit:
                    m_actors.push_back(new Exit(x, y, this));
                    break;
                case Level::player:
                    player = new Player(x, y, this);
                    m_actors.insert(m_actors.begin(), player);
                    break;
                case Level::horiz_ragebot:
                    m_actors.insert(m_actors.begin(), new Ragebot(x, y, 0, this));
                    break;
                case Level::vert_ragebot:
                    m_actors.insert(m_actors.begin(), new Ragebot(x, y, 270, this));
                    break;
                case Level::thiefbot_factory:
                    m_actors.push_back(new Factory(x, y, this)); 
                    break;
                case Level::mean_thiefbot_factory:
                    m_actors.push_back(new MeanFactory(x, y, this));
                    break;
                case Level::wall:
                    m_actors.push_back(new Wall(x, y, this));
                    break;
                case Level::marble:
                    m_actors.push_back(new Marble(x, y, this));
                    break;
                case Level::pit:
                    m_actors.push_back(new Pit(x, y, this));
                    break;
                case Level::crystal:
                    m_actors.push_back(new Crystal(x, y, this));
                    m_crystals++;
                    break;
                case Level::restore_health:
                    m_actors.push_back(new restoreHealth(x, y, this));
                    break;
                case Level::extra_life:
                    m_actors.push_back(new XtraLife(x, y, this));
                    break;
                case Level::ammo:
                    m_actors.push_back(new Ammo(x, y, this));
                    break;
            }
        }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //set display text
    setDisplayText();
    
    //call do something for every actor
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i]->isAlive())
            m_actors[i]->doSomething();
    }
    
    //if player died, perform tasks
    if(!player->isAlive())
    {
        
        playSound(SOUND_PLAYER_DIE);
        decLives();
        if (getLives() == 0)
            return GWSTATUS_PLAYER_DIED;
        return GWSTATUS_PLAYER_DIED;
    }
    
    if (finishedLevel == true)
    {
        finishedLevel = false;
        playSound(SOUND_FINISHED_LEVEL);
        increaseScore(2000);
        increaseScore(m_bonus);
        return GWSTATUS_FINISHED_LEVEL;
    }

    //remove dead actors
    vector<Actor*>::iterator p = m_actors.begin();
    while (p != m_actors.end())
    {
        if ((*p)->isAlive() == false) {
            delete *p;
            p = m_actors.erase(p); }
        else
            p++;
    }
    
   // decrement bonus
    if (m_bonus > 0)
        m_bonus--;
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    vector<Actor*>::iterator p = m_actors.begin();
    while (p != m_actors.end())
    {
        delete *p;
        p = m_actors.erase(p);
    }
}

//check if path to player is empty from a x,y for shooting robots
bool StudentWorld::pathEmpty(double x, double y, int dir)
{
    int px = player->getX();
    int py = player->getY();
    Actor* actorOn;
    
    if (x == px)
    {
        if (dir == Actor::up && y < py) //UP
        {
            for (int j = y+1; j < py; j++)
            {
                actorOn = actorHere(nullptr, px, j);
                if (actorOn != nullptr)
                    if (actorOn->isAttackable())
                        return false;
            }
            return true;
        }
        else if (dir == Actor::down && x == px && y > py) //DOWN
        {
            for (int j = y-1; j > py; j--)
            {
                actorOn = actorHere(nullptr, px, j);
                if (actorOn != nullptr)
                    if (actorOn->isAttackable())
                        return false;
            }
            return true;
        }
    }
    else if (y == py)
    {
        if (dir == Actor::left && x > px) //LEFT
        {
            for (int i = x-1; i > px; i--)
            {
                actorOn = actorHere(nullptr, i, py);
                if (actorOn != nullptr)
                    if (actorOn->isAttackable())
                        return false;
            }
           return true;
        }
        
        else if (dir == Actor::right && x < px) //RIGHT
        {
            for (int i = x+1; i < px; i++)
            {
                actorOn = actorHere(nullptr, i, py);
                if (actorOn != nullptr)
                    if (actorOn->isAttackable())
                    return false;
            }
            return true;
        }
        return false;
    }
    return false;
}

//return a pointer to actor on the same square
Actor* StudentWorld:: actorHere(Actor* curr, double x, double y) const
{
    for (int i = 0; i < m_actors.size(); i++)
    {
    if (m_actors[i]->getX() == x && m_actors[i]->getY() == y && m_actors[i] != curr)
        return m_actors[i];
    }
    return nullptr;
}

//attempt to move actor to the x,y passed in
void StudentWorld:: moveActor(Actor *curr, double x, double y, int dir) const
{
    //check if it is trying to move out of bounds
    if (x < 0 || y < 0 || x >= VIEW_WIDTH || y >= VIEW_HEIGHT)
        return;
    
    //see if square is empty
    Actor* squareActor = actorHere(nullptr, x, y);
    
    //not empty
    if (squareActor != nullptr)
    {
        //peas always move
        if (curr->isTransparent())
            curr->moveTo(x,y);
        
        else if (squareActor->isWalkable() && (curr == player || curr->isBot())) {
            curr->moveTo(x,y); }
        //try to push
        else if (squareActor->getPushed(dir, curr))
            curr->moveTo(x, y);
        //cannot push
        return;
    }
    
    //is empty
    curr->moveTo(x, y);
}

//perform factory census
bool StudentWorld::countThieves(Actor* factory, double x, double y)
{
    Actor* actorOn = actorHere(factory, x, y);
    if (actorOn != nullptr && actorOn->steals())
        return false;
    int thieves = 0;
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i]->steals() && abs(m_actors[i]->getX() - x) <= 3 && abs(m_actors[i]->getY() - y ) <= 3 )
            thieves++;
    }
    if (thieves < 3)
    {
        int random = randInt(1, 50);
        if (random == 25)
            return true;
        

    }
    return false;
}

//return if the player is also on x,y
bool StudentWorld:: colocationWithPlayer(double x, double y)
{
    return (player->getX() == x && player->getY() == y );
}


void StudentWorld :: ammoGoodie()
{
    player->addAmmo();
}

void StudentWorld :: restoreHealthGoodie()
{
    player->setHealth(20);
}
