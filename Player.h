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
	char heldItem = ' '; // currently held item symbol (if any)

    

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
        return heldItem == 'K';
	}
    void collectKey() {
		heldItem = 'K';
    }
   

    bool hasTorch() const {
        return heldItem == '!';
    }
    void collectTorch() {
        heldItem = '!';
    }
  

    bool hasBomb() const {
        return heldItem == '@';
    }
    void collectBomb() {
        heldItem = '@';
    }
    

	char getHeldItem() const {
		return heldItem;
	}
    void removeHeldItem() {
        heldItem = ' ';
    }

    void reset(const Point& startPos);
};
