Ofek Feinstein
206576407

Nitzan Gal
211722525

====================
GENERAL INSTRUCTIONS
====================
- Restart current room: Press 'R' (disabled during the Room 3 boss fight phases and losing 1 live when does it).
- Pause menu: Press ESC (resume with ESC, return to main menu with 'H').
- Recommended to play the game with colors!!

- Most exercise 2 new features are in screen3. if you want to skip screen 1 and 2 go to game.cpp/Game::initGame() and switch the comments for player1 and player2 starting positions and
setCurrentscreen from 1 to 3

====================
REQUIRED SUBMISSION FILES
====================
- readme.txt
- bonus.txt
- riddles.txt
- adv-world_01.screen, adv-world_02.screen, adv-world_03.screen, adv-world_04.screen
  (Screens are loaded in lexicographical order as required.)

====================
COLOR MODE (BONUS)
====================
- Colors can be toggled ON/OFF from the main menu by pressing 'C'.
- MONO mode behaves identically to the required specification.

====================
LEVEL 1 SOLUTION
====================
- Initial Setup: Both players must move obstacles to clear a path to the split point.
- The Split: One player operates switches to clear the path for the other player.
  - Step 1: Gatekeeper steps on the top switch (row 4) to open the first horizontal wall.
  - Step 2: Runner advances. Gatekeeper moves to the second switch (row 8).
  - Step 3: Coordinate movements. Final switch requires one player to hold it open.

====================
LEVEL 2 SOLUTION
====================
- The Binary Door (Door 7):
  - The door requires the decimal value 7 to open.
  - Switches correspond to binary bits: 8 - 4 - 2 - 1.
  - Activate 4, 2, and 1 (rightmost three). Leave 8 off. Code: 0111.
- The Timed Relay:
  - After Player 1 passes Door 7, hit the switch and sprint to the upper switch (row 4)
    to open the final wall for Player 2 (timed).
- Traps:
  - Avoid getting stuck in the obstacle maze at the bottom left.

====================
LEVEL 3 SOLUTION (BOSS STAGE)
====================
Phase 1 - Navigation and M-Doors:
- LEFT player navigates to activate a switch, opening the M-door for LEFT player.
- LEFT player then activates another switch to open the RIGHT player's M-door.
- Time your movement through deadly M tiles (cross when they are hidden).

Phase 2 - Boss Fight (8-Bit Binary Challenge):
1. Step on 'H' tile to start the boss briefing.
2. 3 sequential math tasks using an 8-bit system:
   - 4 bits controlled by LEFT player, 4 bits controlled by RIGHT player.
3. Time limits: 20/40/60 seconds per task.
4. Press 'V' to submit answers.
5. Penalties:
   - Wrong answer: -1 life, -100 score
   - Timeout: -1 life, -50 score
   - Stepping on 'B' bomb during task: -1 life

Phase 3 - Victory:
- After completing all tasks, M-traps are removed.
- Exit door becomes accessible; both players reach Door 3 to win.

Tips:
- No restart during the boss fight.
- Watch for bomb tiles and blinking M-traps.

====================
RIDDLES FILE FORMAT
====================
- File name: riddles.txt
- Format per line:
  SCREEN|X,Y|question|option1,option2,option3,option4|correctIndex
- Lines starting with '#' are comments.

====================
AI ASSISTANCE & METHODOLOGY
====================
We utilized AI tools to enhance workflow, brainstorm ideas, and improve efficiency while
maintaining full responsibility for the codebase and its logic.
- ChatGPT: Used for high-level planning, class design, and comment generation.
- GitHub Copilot: Assisted in implementation tasks inside Visual Studio 2022.
- Gemini Pro: Consulted for complex logic related to interactive mechanics.
