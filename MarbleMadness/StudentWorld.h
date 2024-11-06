#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
class Actor;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    //sets display
    void setDisplayText();
    
    // Try to move actor to x, y
    void moveActor(Actor* curr, double x, double y, int dir) const;
    
    // Returns a pointer to actor on the square
    Actor* actorHere(Actor* curr, double x, double y) const;
    
   //  If a pea were at x,y moving in direction dx,dy, could it hit the player without encountering any obstructions?
    bool pathEmpty(double x, double y, int dir);
    
    // Is the player on the same square as an Actor?
    bool colocationWithPlayer(double x, double y);
    
    //Count number of thiefbots for factory census
    bool countThieves(Actor* factory, double x, double y);
    
    // Increase the amount of ammunition the player has
    void ammoGoodie();
  
    // Restore player's health to the full amount.
    void restoreHealthGoodie();
    
    // Add an actor to the world (used for peas and thiefbots)
    void addActor(Actor* actor) {m_actors.insert(m_actors.begin()++, actor);}
    
    // Decrement crystal count by one
    void getCrystal() {m_crystals--;}
    
    // Have you collected all the crystals on this level?
    bool collectedAllCrystals() {return (m_crystals == 0);}
    
    // Indicate that the player has finished the level.
    void reachedExit() {finishedLevel = true;}
    

private:
    std::vector<Actor*> m_actors;
    Actor*  player;
    int m_crystals;
    int m_bonus; 
    bool finishedLevel;

};

#endif // STUDENTWORLD_H_
