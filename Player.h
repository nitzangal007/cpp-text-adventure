#pragma once

#include "Point.h"
#include "GameConstants.h"
#include "Direction.h"
#include "Spring.h"

enum class SpringMode { None, Compressing, Launching };
class Player {
public:
    enum class Id {
        First,
        Second
    };
    

    struct SpringState {
        SpringMode mode = SpringMode::None;
        int springId = -1;
        int compressedCount = 0;
        Direction launchDir = Direction::STAY;
        int launchSpeed = 0;
        int ticksLeft = 0;
        int inheritedMomentum = 0;  // Force carried from previous spring (chaining)
    };
private:
    static constexpr int NUM_KEYS = 5;
    Id   id;          // which player (1 or 2)
    Point pos;        // current position on the board
    char symbol;      // how the player is drawn on screen
    char keys[NUM_KEYS]; // control keys for this player
    char heldItem = Tiles::EMPTY_SPACE; // currently held item symbol (if any)
    SpringState springState; // current spring state



public:
    // constructor
    Player(Id id, const Point& startPos, const char(&the_keys)[NUM_KEYS + 1], char symbol) : id(id), pos(startPos), symbol(symbol) {
        memcpy(keys, the_keys, NUM_KEYS * sizeof(keys[0]));
    }

    // movement
    void move();

    void stop()
    {
        pos.setDirection(Direction::STAY);
    }

    void setDirection(Direction dir) {
        pos.setDirection(dir);
    }



    // drawing
    void draw() {
        pos.draw();
    }

    // getters
    Id getId() const {
        return id;
    }
    const Point& getPosition() const {
        return pos;
    }
    char getSymbol() const {
        return symbol;
    }
    const SpringState& getSpringState() const { return springState; }
    SpringState& getSpringState() { return springState; }

    // setters
    void setPosition(const Point& p)
    {
        pos.setPosition(p.getX(), p.getY());  // Only change x,y, keep symbol
    }

    // keyboard input handling
    void handleKeyPress(char key_pressed);

    //Spring logics
    // Enters the spring: sets mode to Compressing and stores inherited momentum
    void handleSpringEntry(int springId, int inheritedForce = 0);
    // Increments compression count as player moves deeper
    void incrementCompression() {
		springState.compressedCount++;
    }
    // Activates launch using parameters calculated by the Spring class
    void launch(const SpringLaunchParams& params, Direction dir);
    // Updates flight timer and returns true if flight finished
    bool tickFlight();
    // Resets state to None
    void resetSpringState();
    // Copies momentum from another player (Collision logic)
    void absorbMomentum(const SpringState& otherState);

    // Returns push force based on spring state:
    // - If launching in launchDir: returns launchSpeed (boosted force)
    // - Otherwise: returns 1 (normal walking force)
    int computePushForce(Direction moveDir) const;


    //* inventory interface *

    bool hasKey() const{
        return heldItem == Tiles::KEY;
	}
    void collectKey() {
		heldItem = Tiles::KEY;
    }
   
    
    bool hasTorch() const {
        return heldItem == Tiles::TORCH;
    }
    void collectTorch() {
        heldItem = Tiles::TORCH;
    }
  

    bool hasBomb() const {
        return heldItem == Tiles::BOMB;
    }
    void collectBomb() {
        heldItem = Tiles::BOMB;
    }
    

	char getHeldItem() const {
		return heldItem;
	}
    void removeHeldItem() {
        heldItem = Tiles::EMPTY_SPACE;
    }

    void reset(const Point& startPos);
};
