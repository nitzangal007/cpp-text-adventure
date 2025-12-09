אופק פיינשטיין
206576407

ניצן גל
211722525

AI Assistance & Methodology:
We utilized AI tools to enhance our workflow, brainstorm ideas, and improve efficiency while maintaining full responsibility for the codebase and its logic. This process provided valuable experience in integrating advanced development tools.
- ChatGPT: Used for high-level planning, designing the class skeleton (Game, Screen), and generating code comments.
- GitHub Copilot: Assisted in the implementation of functions and standard coding tasks within Visual Studio 2022.
- Gemini Pro 3: Consulted for complex logic implementation, specifically for interactive elements like Bombs, Switches, and Obstacles.

General:
- Press 'R' at any time to restart the current level if you get stuck.

Level 1 Solution:
- Initial Setup: Both players must skillfully move the obstacles at the start to clear a path to the split point. Cooperation is key.
- The Split: One player acts as the "Gatekeeper" on the left, operating switches to clear the path for the other player.
    - Step 1: The Gatekeeper steps on the top switch (row 4) to open the first horizontal wall.
    - Step 2: The Runner advances. The Gatekeeper moves to the second switch (row 8) to control vertical gates.
    - Step 3: Coordinate movements to pass through each section. The final switch (bottom right) requires one player to hold it open for the other to reach the exit.

Level 2 Solution:
- The Binary Door (Door 7):
    - The door requires the decimal value 7 to open.
    - The switches at the bottom correspond to binary bits: 8 - 4 - 2 - 1.
    - Solution: Activate switches for 4, 2, and 1 (Rightmost three). Leave the 8 (Leftmost) switch alone. Code: 0111.
- The Timed Relay:
    - After Player 1 passes Door 7, they must hit the switch and immediately sprint to the upper switch (row 4) to open the final wall for Player 2.
    - Warning: This is timed! Speed is crucial.
- Traps:
    - Avoid getting stuck in the obstacle maze at the bottom left. Plan your pushes!
