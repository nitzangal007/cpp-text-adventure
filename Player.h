#pragma once

#include "Point.h"

class Player {
public:
    enum class Id {
        First,
        Second
    };
    enum Carriers {
        KEY = 'K',
        BOMB = '@',
        TORCH = '!'
    };
private:
    static constexpr int NUM_KEYS = 5;
    Id   id;          // which player (1 or 2)
    Point pos;        // current position on the board
    char symbol;      // how the player is drawn on screen
	char heldItem = ' '; // item currently held by the player
	char keys[NUM_KEYS]; // keys collected by the player
    

    // inventory / state flags (you'll decide what you really need)
    bool hasKey = false;
    bool hasTorch = false;
    bool hasBomb = false;

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
    Id getId() {
		return id;
    }
    const Point& getPosition() {
		return pos;
    }
    char getSymbol() {
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
    bool hasKeyItem(){
		return hasKey;
	}
    void giveKey() {
		hasKey = true;
		heldItem = 'K';
    }
    void removeKey() {
		hasKey = false;
		heldItem = ' ';
    }

    bool hasTorchItem() {
		return hasTorch;
    }
    void giveTorch() {
		hasTorch = true;
        heldItem = '!';
    }
    void removeTorch() {
		hasTorch = false;
		heldItem = ' ';
    }

    bool hasBombItem() {
		return hasBomb;
    }
    void giveBomb() {
		hasBomb = true;
		heldItem = '@';
    }
    void removeBomb() {
		hasBomb = false;
		heldItem = ' ';
    }
};
