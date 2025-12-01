#pragma once

#include "Point.h"

class Player {
public:
    enum class Id {
        First,
        Second
    };
private:
    static constexpr int NUM_KEYS = 5;
    Id   id;          // which player (1 or 2)
    Point pos;        // current position on the board
    char symbol;      // how the player is drawn on screen
	char keys[NUM_KEYS]; // control keys for this player
    

    // inventory / state flags (you'll decide what you really need)
    bool hasKeyFlag = false;
    bool hasTorchFlag = false;
    bool hasBombFlag = false;

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

    // setters
    void setPosition(const Point& p)
	{
		pos = p;
	}

    // keyboard input handling
    void handleKeyPress(char key_pressed);


    // inventory interface (you can refine this later)
    bool hasKey() const{
		return hasKeyFlag;
	}
    void collectKey() {
		hasKeyFlag = true;
    }
    void removeKey() {
		hasKeyFlag = false;
    }

    bool hasTorch() const {
		return hasTorchFlag;
    }
    void collectTorch() {
		hasTorchFlag = true;
    }
    void removeTorch() {
        hasTorchFlag = false;   
    }

    bool hasBomb() const {
		return hasBombFlag;
    }
    void collectBomb() {
		hasBombFlag = true;   
    }
    void removeBomb() {
		hasBombFlag = false;    
    }



    void reset(const Point& startPos);
};
