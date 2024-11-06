#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor: public GraphObject
{
public:
    Actor(int imageID, double startX, double startY, int dir, bool life, int hp, StudentWorld* world) : GraphObject(imageID, startX, startY, dir), m_alive(life), m_hp(hp), m_world(world) { setVisible(life); }
    
    // Action to perform each tick
    virtual void doSomething() = 0;
    
    // return hit points the actor has left
    int getHealth() const {return m_hp;}

    // Set this actor's hit points to new amount.
    void setHealth(int newHealth) {m_hp = newHealth;}
    
    // is the actor alive?
    bool isAlive() const { return m_alive; }
    
    // mark actor as dead
    void die() { m_alive = false; }
    
    // get this actor's world
    StudentWorld* getWorld() const { return m_world; }
    
    //produces coordinate to move to depending on current coordinates and direction
    void newCoords(double &x, double &y, int dir);
    
    //reverses direction for ragebots
    void switchDir(int& dir);
    
    // only for player, called by StudentWorld when on Ammo Goodie
   virtual void addAmmo() {}
    
    // implemented for Actors that can take damage
    virtual void takeDamage() {}
    
    // for actors that shoot (only player and bots)
    virtual void shoot();
    
    // return the amount of player's ammo for display text
    virtual int getAmmo() {return -1;}
    
    // allows marbles to get pushed
    virtual bool getPushed(int dir, Actor* pusher) {return false;}
   
    // Can an agent occupy the same square as this actor?
    virtual bool isWalkable() const {return false;}
    
    //Can this actor be swallowed by a pit?
    virtual bool isSwallowable() const {return false;}
    
    // Can a marble occupy the same square as this actor?
    virtual bool allowsMarble() const {return false;}
    
    // is the actor the player?
    virtual bool isPlayer() const {return false;}
    
    // is the actor some type of robot?
    virtual bool isBot() const {return true;}
    
    //can peas pass over item ?
    virtual bool isTransparent() const {return false; }
    
    //can be picked up by thiefbots ?
    virtual bool isStealable() const {return false;}
    
    // Does this actor count when a factory counts items near it?
    virtual bool steals() const {return false;}
    
    // Does this actor stop peas?
    virtual bool isAttackable() const {return false;}
    
private:
    bool m_alive;
    int m_hp;
    StudentWorld* m_world;
};

class Player: public Actor
{
public:
    Player(double startX, double startY, StudentWorld* world) : Actor(IID_PLAYER, startX, startY, right, true, 20, world), m_peas(20) {}
    virtual void takeDamage();
    virtual void doSomething();
    virtual void shoot();
    
    virtual void addAmmo() {m_peas+= 20;}
    virtual int getAmmo() {return m_peas;}
    virtual bool isPlayer() const {return true;}
    virtual bool isAttackable() const {return true;}

    

    
private:
    int m_peas;
};


class Marble: public Actor
{
public:
    
    Marble(double startX, double startY, StudentWorld* world) : Actor(IID_MARBLE, startX, startY, none, true, 10, world) {}
    virtual void takeDamage(); 
    virtual bool getPushed(int dir, Actor* pusher);
    virtual void doSomething() {}
    virtual bool isSwallowable() const {return true;}
    virtual bool isAttackable() const {return true;}
};


class Wall: public Actor
{
public:
    Wall(double startX, double startY, StudentWorld* world) : Actor(IID_WALL, startX, startY, 0, true, 100, world) {}
    virtual void doSomething() {}
    virtual bool isAttackable() const {return true;}
};


class Exit: public Actor
{
public:
    Exit(double startX, double startY, StudentWorld* world) : Actor(IID_EXIT, startX, startY, none, true, 100, world), status(false) { setVisible(false);}
    virtual void doSomething();
    virtual bool isWalkable() const {
        return true;}
private:
    //keeps track of if exit is already visible or not
    bool status;

};


class Pit: public Actor
{
public:
    Pit(double startX, double startY, StudentWorld* world) : Actor (IID_PIT, startX, startY, 0, true, 100, world)
    {}
    virtual bool getPushed(int dir, Actor* pusher);
    virtual void doSomething();
    virtual bool allowsMarble() const {return true; }

};

class Pea: public Actor
{
public:
    Pea(double startX, double startY, int dir, StudentWorld* world) : Actor(IID_PEA, startX, startY, dir, true, 100, world)
    {}
    virtual void doSomething();
    virtual bool isWalkable() {return true;}
    virtual bool isTransparent() const {return true;}


};

class Goodie: public Actor
{
public:
    Goodie(int imageID, double startX, double startY,  StudentWorld* world, int bonusPts) : Actor(imageID, startX, startY, none, true, 100, world), m_bonus(bonusPts)    {}
    virtual void doTask() = 0;
    virtual void doSomething();
    virtual bool isStealable() const {return true;}
    virtual bool isWalkable() const {return true;}
private:
    int m_bonus;
};
 
class restoreHealth: public Goodie
{
public:
    restoreHealth(double startX, double startY,  StudentWorld* world) : Goodie(IID_RESTORE_HEALTH, startX, startY, world, 500) {}
    virtual void doTask();
};

class Ammo: public Goodie
{
public:
    Ammo(double startX, double startY, StudentWorld* world): Goodie(IID_AMMO, startX, startY, world, 100) {}
    virtual void doTask();
};



class XtraLife: public Goodie
{
public:
    XtraLife(double startX, double startY,  StudentWorld* world) : Goodie(IID_EXTRA_LIFE, startX, startY, world, 1000) {}
    virtual void doTask();
};



class Crystal: public Goodie
{
public:
    Crystal(double startX, double startY, StudentWorld* world) : Goodie(IID_CRYSTAL, startX, startY, world, 50) {}
    virtual bool isStealable() const {return false;}
    virtual void doTask();
};


class Robot: public Actor
{
public:
    Robot(int imageID, double startX, double startY, int dir, StudentWorld* world, int hp);
    virtual void takeDamage();
    virtual void doSomething();
    virtual void doTask() = 0;
    int getTotalTicks() {return m_ticks;}
    int getActionTicks() {return actionTicks;}
    virtual bool isBot() const {return true;}
    virtual bool isAttackable() const {return true;}
    void shoot();

    
private:
    int m_ticks; //TODO possibly move this to studentWorld 
    int actionTicks;
   // void shoot();
};


class Ragebot : public Robot
{
public:
    Ragebot(double startX, double startY, int dir, StudentWorld* world) : Robot(IID_RAGEBOT, startX, startY, dir, world, 10)
    {}
    virtual void doTask();
   // virtual void doSomething();
};

class Thiefbot : public Robot
{
public:
    Thiefbot(double startX, double startY, StudentWorld* world, int bonus = 10, int imageID = IID_THIEFBOT) : Robot(imageID, startX, startY, right, world, 5), m_dist(0), m_maxDist(randInt(1, 6)), m_goodie(nullptr) {}
    virtual void doTask();
    virtual void takeDamage();
    virtual bool steals() const {return true;}
    
private:
    int randDir(); 
    int m_dist;
    int m_maxDist;
    Actor*  m_goodie;
    int m_bonus;
};


class MeanThiefbot : public Thiefbot
{
public:
    MeanThiefbot(double startX, double startY, StudentWorld* world) : Thiefbot(startX, startY, world, 20, IID_MEAN_THIEFBOT) {setHealth(8);}
    virtual void doTask();
    //virtual void takeDamage(); 
};

class Factory: public Actor
{
public:
    Factory(double startX, double startY, StudentWorld* world) : Actor(IID_ROBOT_FACTORY, startX, startY, 0, true, 100, world) {}
    virtual void doSomething();
    virtual bool isAttackable() const {return true;}

};

class MeanFactory : public Factory
{
public:
    MeanFactory(double startX, double startY, StudentWorld* world) : Factory(startX, startY, world) {}
    virtual void doSomething();
};



#endif // ACTOR_H_
