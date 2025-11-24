#include "Player.h"


void Player::handleKeyPress(char key_pressed) {
    size_t index = 0;
    for (char k : keys) {
        if (std::tolower(k) == std::tolower(key_pressed)) {
			setDirection((Direction)index);
            return;
        }
        ++index;
    }
}
