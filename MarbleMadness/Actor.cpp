#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//Actor Functions
//determine new x,y to move to based on x,y entered and direction
void Actor::newCoords(double &x, double &y, int dir)
{
    switch (dir)
    {
        case up:
            y++;
            break;
        case down:
            y--;
            break;
        case right:
            x++;
            break;
        case left:
            x--;
            break;
    }
}

//reverse direction for ragebots
void Actor::switchDir(int& dir)
{
    switch(dir)
    {
        case up:
            dir = down;
            break;
        case down:
            dir = up;
            break;
        case left:
            dir = right;
            break;
        case right:
            dir = left;
            break;
    }
    setDirection(dir);
}
 
// called by player and shooting robots
void Actor::shoot()
{
    //add a pea one square in front of actor and play sound
    double x = getX();
    double y = getY();
    int dir = getDirection();
    newCoords(x, y, dir);
    Actor* pea = new Pea(x,y, dir,getWorld());
    getWorld()->addActor(pea);
}

void Player::takeDamage()
{
    setHealth(getHealth()-2);
    if (getHealth() > 0)
        getWorld()->playSound(SOUND_PLAYER_IMPACT);
    if (getHealth() <= 0)
        die(); 
}

void Player::shoot()
{
//if still have ammo
if (m_peas > 0)
    {
        Actor::shoot();
        getWorld()->playSound(SOUND_PLAYER_FIRE);
        m_peas--;
    }
                
}

void Player::doSomething()
{
    if (!isAlive())
        return;
    int ch;
    if (getWorld()->getKey(ch))
    {
        switch(ch)
        {
            case KEY_PRESS_ESCAPE:
                die();
                return;
            case KEY_PRESS_LEFT:
                ch = left;
                break;
            case KEY_PRESS_RIGHT:
                ch = right;
                break;
            case KEY_PRESS_UP:
                ch = up;
                break;
            case KEY_PRESS_DOWN:
                ch = down;
                break;
            case KEY_PRESS_SPACE:
                shoot();
                return;
            default:
                return;
        }
        setDirection(ch);
                
        double x = getX();
        double y = getY();
        
        newCoords(x, y, ch);
   
        getWorld()->moveActor(this, x, y, ch);
        }
    }


bool Marble:: getPushed(int dir, Actor* pusher)
{
    if (!pusher->isPlayer())
        return false;
    
    double x = getX();
    double y = getY();
    newCoords(x, y, dir);
    
    Actor* actorOn = getWorld()->actorHere(this, x, y);
    //square not occupied or marble can move over object
    if (actorOn == nullptr || actorOn->allowsMarble())
    {
        getWorld()->moveActor(this, x, y, dir);
        return true;
    }
    //square occupied
    return false;
    
}

void Marble::takeDamage()
{
    setHealth(getHealth()-2);

    if (getHealth() <= 0)
        die();
}

void Exit::doSomething()
{
    //if not visible and all crystals have been collected, make visible
    if (getWorld()->collectedAllCrystals() && status == false)
    {
        setVisible(true);
        getWorld()->playSound(SOUND_REVEAL_EXIT);
        status = true;
    }
        
    //if player on exit
    if (isVisible() && getWorld()->colocationWithPlayer(getX(), getY()))
        getWorld()->reachedExit();
}


void Pit::doSomething()
{
    if (!isAlive())
        return;
    
    //if actor on pit is a marble, kill pit and marble
    Actor* actorOn = getWorld()-> actorHere(this, getX(), getY());
    if (actorOn != nullptr && actorOn->isSwallowable())
    {
        die();
        actorOn->die();
    }
}

bool Pit::getPushed(int dir, Actor* pusher)
{
    //peas and marbles can pass over pit
    if (pusher->isTransparent() || pusher->isSwallowable())
        return true;
    else
        return false;
}

void Pea::doSomething()
{
    if (!isAlive())
        return;
    
    double x = getX();
    double y = getY();
    
    Actor* actorOn = getWorld()->actorHere(this, x, y);
   
    //actor can be attacked then attack
     if (actorOn != nullptr && actorOn->isAttackable()) {
        actorOn->takeDamage();
        die();
        return; }
    
    //move and attack again
    newCoords(x, y, getDirection());
    
    //check if actor can be attacked
    actorOn = getWorld()->actorHere(this, x, y);
    if (actorOn != nullptr && actorOn->isAttackable())
    {
        actorOn->takeDamage();
        die();
        return;
    }
    //move
    getWorld()->moveActor(this, x, y, getDirection());
}


//Goodie Functions
void Goodie::doSomething()
{
    if (!isAlive())
        return;
    //if player is on goodie,
    if (getWorld()->colocationWithPlayer(getX(), getY()))
    {
        getWorld()->increaseScore(m_bonus);
        die();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        doTask();
    }
}

void Ammo::doTask(){
    getWorld()->ammoGoodie(); }


void restoreHealth::doTask(){
    getWorld()->restoreHealthGoodie(); }


void XtraLife::doTask(){
    getWorld()->incLives(); }


void Crystal::doTask() {
    getWorld()->getCrystal(); }

Robot:: Robot(int imageID, double startX, double startY, int dir, StudentWorld* world, int hp) : Actor(imageID, startX, startY, dir, true, hp, world), m_ticks(0)
{
    //determine how often robot does something
    actionTicks = (28 - getWorld()->getLevel()) / 4;
    if (actionTicks < 3)
        actionTicks = 3;
}

void Robot::takeDamage()
{
    setHealth(getHealth()-2);
    //if robot is alive play damage sound
    if (getHealth() > 0)
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    
    //if robot dead, add points, set dead, play sound
    if (getHealth() <= 0) {
        die();
        getWorld()->increaseScore(100);
        getWorld()->playSound(SOUND_ROBOT_DIE); }
}

void Robot::shoot() {
    Actor::shoot();
    getWorld()->playSound(SOUND_ENEMY_FIRE);
}

void Robot::doSomething()
{
    if (!isAlive())
        return;
    m_ticks++;
    
    //if not resting, then perform action
   if (m_ticks % actionTicks == 0)
       doTask();
}

void Ragebot::doTask()
{
    //if path to player is empty, shoot at player
    if (getWorld()->pathEmpty(getX(), getY(), getDirection()))
    {
        shoot();
        return;
    }
    
    else //attempt to move
    {
        double x = getX();
        double y = getY();
        int dir = getDirection();
        newCoords(x, y, dir);
        Actor* actorOn = getWorld()->actorHere(this, x, y);
        if (actorOn != nullptr && !(actorOn->isWalkable()))
            switchDir(dir);
        getWorld()->moveActor(this, x, y, dir);
    }
}
 
//choose a random direction
int Thiefbot::randDir()
{
    int random = randInt(1, 4);
    switch(random)
    {
        case 1:
            return up;
            break;
        case 2:
            return down;
            break;
        case 3:
            return left;
            break;
        case 4:
            return right;
            break;
    }
    return up;
}

void Thiefbot::doTask()
{
    double x = getX();
    double y = getY();
    Actor* actorOn = getWorld()->actorHere(this, x, y);

    //if actor on square is a goodie and robot does not already have goodie, one in ten chance it will get picked up
    if (actorOn != nullptr && actorOn->isStealable() && m_goodie == nullptr && randInt(1, 10) == 3)
    {
        actorOn->setVisible(false);
        m_goodie = actorOn;
        getWorld()->playSound(SOUND_ROBOT_MUNCH);
        return;
    }
    
    //else if has not yet reached distance, try to move
    else if (m_dist < m_maxDist)
    {
        newCoords(x, y, getDirection());
        actorOn = getWorld()->actorHere(this, x, y);
        if (actorOn == nullptr || actorOn->isWalkable())
        {
            getWorld()->moveActor(this, x, y, getDirection());
            m_dist++;
            return;
        }
    }
    
    //if either already moved maxDist or object in way
    m_maxDist = randInt(1, 6);
    m_dist = 0;
    x = getX();
    y = getY();
    int dir = randDir();
    
    newCoords(x, y, dir); //generate new square to move to
    actorOn = getWorld()->actorHere(this, x, y); //see if projected square is occupied
    if (actorOn == nullptr || actorOn->isWalkable())
    {
        //if not occupied, move there
        setDirection(dir);
        getWorld()->moveActor(this, x, y, dir);
        m_dist++;
        return;
    }
    else //if occupied, try a different direction
    {
        for (int i = 0; i <= 270; i += 90)
        {
            if (i == dir)
                continue;
            x = getX();
            y = getY();
            newCoords(x, y, i);
            actorOn = getWorld()->actorHere(this, x, y);
            if (actorOn == nullptr || (actorOn->isWalkable()))
            {
                setDirection(i);
                getWorld()->moveActor(this, x, y, i);
                m_dist = 1;
                return;
            }
        }
    }
    setDirection(dir);
}

//decrease 2 hp if hit, if dead then drop goodie
void Thiefbot::takeDamage()
{
    setHealth(getHealth()-2);
    if (getHealth() > 0)
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    if (getHealth() <= 0) {
        if (m_goodie != nullptr)
        {
            m_goodie->moveTo(getX(), getY());
            m_goodie->setVisible(true);
        }
        die();
        
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(m_bonus);
    }
}

//if path to player is empty, shoot; if not, perform regular thiefbot tasks
void MeanThiefbot::doTask()
{
    if (getWorld()->pathEmpty(getX(), getY(), getDirection()))
    {
        shoot();
        return;
    }
    Thiefbot::doTask();
}

//if census count succeeds, produce a ragebot
void Factory::doSomething()
{
    if (getWorld()->countThieves(this, getX(), getY()))
    {
        getWorld()->addActor(new Thiefbot(getX(), getY(), getWorld()));
        getWorld()->playSound(SOUND_ROBOT_BORN);
    }
}

//if census count succeeds, produce a mean ragebot
void MeanFactory::doSomething()
{
    
    if (getWorld()->countThieves(this, getX(), getY()))
    {
        getWorld()->addActor(new MeanThiefbot(getX(), getY(), getWorld()));
        getWorld()->playSound(SOUND_ROBOT_BORN);
    } 
}
