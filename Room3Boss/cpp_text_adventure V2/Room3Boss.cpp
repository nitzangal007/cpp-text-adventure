#include "Room3Boss.h"
#include "Screens.h"
#include "ColorUtils.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <conio.h>
// we used chatGPT to help us design the state machine and overall structure of this class.
// ==========================================
// Constructor
// ==========================================

Room3Boss::Room3Boss()
    : state_(BossState::Inactive)
    , currentTaskIndex_(0)
    , taskValuesGenerated_(false)
    , countdownValue_(3)
    , bombsVisible_(true)
    , pendingScorePenalty_(0)
    , pendingLifePenalty_(0)
{
    // Seed RNG with current time
    rng_.seed(static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count()
    ));
    
    initSwitchPositions();
}

// ==========================================
// Switch Position Initialization
// ==========================================

void Room3Boss::initSwitchPositions()
{
    // User coordinates given as (Y, X), but Point constructor is (X, Y)
    // Left room switches (bits 7-4, left to right)
    switches_[0].position = Point(6, 16);   // BIT7: user said (16, 6)
    switches_[1].position = Point(14, 16);  // BIT6: user said (16, 14)
    switches_[2].position = Point(24, 16);  // BIT5: user said (16, 24)
    switches_[3].position = Point(32, 16);  // BIT4: user said (16, 32)
    
    // Right room switches (bits 3-0, left to right)
    switches_[4].position = Point(46, 16);  // BIT3: user said (16, 46)
    switches_[5].position = Point(55, 16);  // BIT2: user said (16, 55)
    switches_[6].position = Point(65, 16);  // BIT1: user said (16, 65)
    switches_[7].position = Point(73, 16);  // BIT0: user said (16, 73)
    
    // Initialize all switches to OFF
    for (int i = 0; i < 8; ++i) {
        switches_[i].isOn = false;
        switches_[i].wasPlayerOnLastFrame = false;
    }
}

// ==========================================
// Core Lifecycle
// ==========================================

void Room3Boss::init()
{
    state_ = BossState::PreBoss;
    currentTaskIndex_ = 0;
    taskValuesGenerated_ = false;
    taskData_ = TaskData{};
    countdownValue_ = 3;
    bombsVisible_ = true;
    pendingScorePenalty_ = 0;
    pendingLifePenalty_ = 0;
    
    // Reset switches
    for (int i = 0; i < 8; ++i) {
        switches_[i].isOn = false;
        switches_[i].wasPlayerOnLastFrame = false;
    }
    
    // Clear snapshot
    std::memset(boardSnapshot_, ' ', sizeof(boardSnapshot_));
    
    // Briefing hasn't been shown yet
    briefingShown_ = false;
}

void Room3Boss::reset()
{
    state_ = BossState::Inactive;
    currentTaskIndex_ = 0;
    taskValuesGenerated_ = false;
    taskData_ = TaskData{};
    pendingScorePenalty_ = 0;
    pendingLifePenalty_ = 0;
    
    for (int i = 0; i < 8; ++i) {
        switches_[i].isOn = false;
        switches_[i].wasPlayerOnLastFrame = false;
    }
}

// ==========================================
// State Machine
// ==========================================

void Room3Boss::transitionTo(BossState newState)
{
    state_ = newState;
}

// ==========================================
// Frame Update
// ==========================================

void Room3Boss::update(Screens& screens, Player& p1, Player& p2)
{
    if (state_ == BossState::Inactive) {
        return;
    }
    
    switch (state_) {
        case BossState::PreBoss: {
            // Check if player stepped on H tile (briefing trigger)
            // Only trigger if briefing hasn't been shown yet
            Point hTile(H_TILE_X, H_TILE_Y);
            if (!briefingShown_ && (p1.getPosition() == hTile || p2.getPosition() == hTile)) {
                // Show briefing (BLOCKING - draws once, waits for key)
                showBriefing();
                // Mark briefing as shown
                briefingShown_ = true;
                // Remove H tile so it's visually gone
                screens.setCharAt(hTile, EMPTY_SPACE);
                // After showBriefing() returns, we're back in PreBoss state
                return;
            }
            
            // Check if player stepped on start switch
            Point startSwitch(START_SWITCH_X, START_SWITCH_Y);
            if (p1.getPosition() == startSwitch || p2.getPosition() == startSwitch) {
                // Start countdown
                countdownStartTime_ = std::chrono::steady_clock::now();
                countdownValue_ = 3;
                
                // Close both entrances
                closeEntrances(screens);
                
                // Remove torches from players
                removeTorchesFromPlayers(p1, p2);
                
                // Teleport players to their designated start positions
                // Left player (P1) to (20, 15), Right player (P2) to (52, 15)
                p1.setPosition(Point(20, 15));
                p2.setPosition(Point(52, 15));
                
                transitionTo(BossState::Countdown);
                return;
            }
            break;
        }
        
        case BossState::Briefing:
            // This state is no longer used (showBriefing is blocking)
            break;
        
        case BossState::Countdown: {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                now - countdownStartTime_
            ).count();
            
            countdownValue_ = 3 - static_cast<int>(elapsed);
            
            if (countdownValue_ <= 0) {
                // Countdown finished, start first task
                currentTaskIndex_ = 0;
                taskValuesGenerated_ = false;
                startTask(screens, p1, p2);
                transitionTo(BossState::TaskRunning);
            }
            break;
        }
        
        case BossState::TaskRunning: {
            // Update switches
            updateSwitches(screens, p1, p2);
            
            // Update bomb blink if in last 10 seconds
            updateBombBlink();
            
            // Check for timeout
            if (getTimeRemaining() <= 0) {
                handleTaskFailure(screens, p1, p2);
            }
            
            // Check if both players reached exit (shouldn't happen during task, but safety)
            Point exit3(EXIT_3_X, EXIT_3_Y);
            if (p1.getPosition() == exit3 && p2.getPosition() == exit3) {
                // This shouldn't happen during task - exit is blocked
            }
            break;
        }
        
        case BossState::Victory: {
            // Check if both players reached exit '3'
            Point exit3(EXIT_3_X, EXIT_3_Y);
            if (p1.getPosition() == exit3 && p2.getPosition() == exit3) {
                transitionTo(BossState::Exit);
            }
            break;
        }
        
        case BossState::Exit:
        case BossState::TaskFail:
        case BossState::TaskSuccess:
            // Handled elsewhere or transitional states
            break;
            
        default:
            break;
    }
}

// ==========================================
// Input Handling
// ==========================================

bool Room3Boss::handleInput(char key, Screens& screens, Player& p1, Player& p2)
{
    if (state_ == BossState::Inactive) {
        return false;
    }
    
    // Handle briefing dismiss
    if (state_ == BossState::Briefing) {
        // Any key dismisses the briefing
        transitionTo(BossState::PreBoss);
        return true;
    }
    
    // Handle 'V' submission during task
    if (state_ == BossState::TaskRunning) {
        char upperKey = static_cast<char>(std::toupper(static_cast<unsigned char>(key)));
        if (upperKey == 'V') {
            uint8_t currentValue = readSwitchByte();
            if (currentValue == taskData_.targetValue) {
                handleTaskSuccess(screens);
            } else {
                handleTaskFailure(screens, p1, p2);
            }
            return true;
        }
    }
    
    return false;
}

// ==========================================
// 8-Bit Switch System
// ==========================================

void Room3Boss::updateSwitches(Screens& screens, const Player& p1, const Player& p2)
{
    for (int i = 0; i < 8; ++i) {
        bool playerOn = (p1.getPosition() == switches_[i].position ||
                         p2.getPosition() == switches_[i].position);
        
        // Edge detection: toggle only on step-on, not hold
        if (playerOn && !switches_[i].wasPlayerOnLastFrame) {
            switches_[i].isOn = !switches_[i].isOn;
            
            // Update board visual
            char switchChar = switches_[i].isOn ? SWITCH_ON : SWITCH_OFF;
            screens.setCharAt(switches_[i].position, switchChar);
        }
        
        switches_[i].wasPlayerOnLastFrame = playerOn;
    }
}

uint8_t Room3Boss::readSwitchByte() const
{
    uint8_t value = 0;
    
    // Left room: switches[0..3] → bits 7,6,5,4 (left-to-right)
    for (int i = 0; i < 4; ++i) {
        if (switches_[i].isOn) {
            value |= (1 << (7 - i));
        }
    }
    
    // Right room: switches[4..7] → bits 3,2,1,0 (left-to-right)
    for (int i = 0; i < 4; ++i) {
        if (switches_[4 + i].isOn) {
            value |= (1 << (3 - i));
        }
    }
    
    return value;
}

std::string Room3Boss::getBitsString() const
{
    uint8_t value = readSwitchByte();
    std::string bits;
    
    for (int i = 7; i >= 0; --i) {
        bits += ((value >> i) & 1) ? '1' : '0';
        if (i == 4) bits += ' ';  // Space between nibbles
    }
    
    return bits;
}

// ==========================================
// Board Snapshot / Restore
// ==========================================

void Room3Boss::takeSnapshot(const Screens& screens, const Player& p1, const Player& p2)
{
    // Copy board state
    for (int y = 0; y < Screen::MAX_Y; ++y) {
        for (int x = 0; x < Screen::MAX_X; ++x) {
            boardSnapshot_[y][x] = screens.getCharAtPublic(Point(x, y));
        }
    }
    
    // Save player positions
    player1Snapshot_ = p1.getPosition();
    player2Snapshot_ = p2.getPosition();
    
    // Save switch states
    for (int i = 0; i < 8; ++i) {
        switchStatesSnapshot_[i] = switches_[i].isOn;
    }
}

void Room3Boss::restoreSnapshot(Screens& screens, Player& p1, Player& p2)
{
    // Restore board state
    for (int y = 0; y < Screen::MAX_Y; ++y) {
        for (int x = 0; x < Screen::MAX_X; ++x) {
            screens.setCharAt(Point(x, y), boardSnapshot_[y][x]);
        }
    }
    
    // Restore player positions to FIXED spawn points (not snapshot positions)
    // Left player (P1) to (20, 15), Right player (P2) to (52, 15)
    p1.setPosition(Point(20, 15));
    p2.setPosition(Point(52, 15));
    
    // Restore switch states
    for (int i = 0; i < 8; ++i) {
        switches_[i].isOn = switchStatesSnapshot_[i];
        switches_[i].wasPlayerOnLastFrame = false;  // Reset edge detection
        
        // Sync visual
        char switchChar = switches_[i].isOn ? SWITCH_ON : SWITCH_OFF;
        screens.setCharAt(switches_[i].position, switchChar);
    }
}

// ==========================================
// Random Number Generator
// ==========================================

void Room3Boss::generateTaskValues()
{
    if (taskValuesGenerated_) {
        return;  // Don't regenerate on retry
    }
    
    switch (currentTaskIndex_) {
        case 0: {
            // Task 1: Write Value - random 0x00..0xFF
            taskData_.taskNumber = 1;
            taskData_.timeLimit = TASK1_TIME;
            taskData_.targetValue = static_cast<uint8_t>(rng_() % 256);
            taskData_.operandA = 0;
            taskData_.operandB = 0;
            
            std::ostringstream oss;
            oss << "Mission 1/3 (" << TASK1_TIME << "s): Set byte to 0x"
                << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
                << static_cast<int>(taskData_.targetValue) << ". Press V.";
            taskData_.displayText = oss.str();
            break;
        }
        
        case 1: {
            // Task 2: NOT operation - compute ~0xXY
            taskData_.taskNumber = 2;
            taskData_.timeLimit = TASK2_TIME;
            taskData_.operandA = static_cast<uint8_t>(rng_() % 256);
            taskData_.operandB = 0;
            taskData_.targetValue = ~taskData_.operandA;  // Bitwise NOT
            
            std::ostringstream oss;
            oss << "Mission 2/3 (" << TASK2_TIME << "s): Compute NOT 0x"
                << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
                << static_cast<int>(taskData_.operandA) << ". Press V.";
            taskData_.displayText = oss.str();
            break;
        }
        
        case 2: {
            // Task 3: Binary Addition - A + B (in binary format)
            // Ensure A + B <= 255
            taskData_.taskNumber = 3;
            taskData_.timeLimit = TASK3_TIME;
            taskData_.operandA = static_cast<uint8_t>(rng_() % 256);
            taskData_.operandB = static_cast<uint8_t>(rng_() % (256 - taskData_.operandA));
            taskData_.targetValue = taskData_.operandA + taskData_.operandB;
            
            // Helper lambda to convert uint8_t to binary string
            auto toBinary = [](uint8_t val) -> std::string {
                std::string bits;
                for (int i = 7; i >= 0; --i) {
                    bits += ((val >> i) & 1) ? '1' : '0';
                }
                return bits;
            };
            
            std::ostringstream oss;
            oss << "Mission 3/3 (" << TASK3_TIME << "s): " 
                << toBinary(taskData_.operandA) << " + " 
                << toBinary(taskData_.operandB) << " = ? Press V.";
            taskData_.displayText = oss.str();
            break;
        }
        
        default:
            break;
    }
    
    taskValuesGenerated_ = true;
}

// ==========================================
// Task Flow
// ==========================================

void Room3Boss::startTask(Screens& screens, Player& p1, Player& p2)
{
    // Generate task values (only if not already generated for retry)
    generateTaskValues();
    
    // Take snapshot for potential restore
    takeSnapshot(screens, p1, p2);
    
    // Start timer
    taskStartTime_ = std::chrono::steady_clock::now();
    blinkStartTime_ = taskStartTime_;
    bombsVisible_ = true;
}

void Room3Boss::handleTaskSuccess(Screens& screens)
{
    currentTaskIndex_++;
    taskValuesGenerated_ = false;  // Allow new values for next task
    
    if (currentTaskIndex_ >= 3) {
        // All 3 tasks complete - Victory!
        removeExitBlocker(screens);
        removeAllBombs(screens);
        transitionTo(BossState::Victory);
    } else {
        // Move to next task
        // Reset switches to OFF for clean start
        for (int i = 0; i < 8; ++i) {
            switches_[i].isOn = false;
            switches_[i].wasPlayerOnLastFrame = false;
            screens.setCharAt(switches_[i].position, SWITCH_OFF);
        }
        
        transitionTo(BossState::TaskRunning);
        startTask(screens, 
            *reinterpret_cast<Player*>(&player1Snapshot_),  // Placeholder - will be fixed in integration
            *reinterpret_cast<Player*>(&player2Snapshot_));
    }
}

void Room3Boss::handleTaskFailure(Screens& screens, Player& p1, Player& p2)
{
    // Set penalties
    pendingScorePenalty_ = SCORE_PENALTY;
    pendingLifePenalty_ = LIFE_PENALTY;
    
    // Restore to task start state
    restoreSnapshot(screens, p1, p2);
    
    // Restart same task (values remain the same)
    taskStartTime_ = std::chrono::steady_clock::now();
    blinkStartTime_ = taskStartTime_;
    bombsVisible_ = true;
    
    // Stay in TaskRunning with same task
    transitionTo(BossState::TaskRunning);
}

int Room3Boss::getTimeRemaining() const
{
    if (state_ != BossState::TaskRunning) {
        return 0;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        now - taskStartTime_
    ).count();
    
    int remaining = taskData_.timeLimit - static_cast<int>(elapsed);
    return (remaining > 0) ? remaining : 0;
}

// ==========================================
// Victory & Cleanup
// ==========================================

void Room3Boss::removeExitBlocker(Screens& screens)
{
    // Remove the M blocking exit '3'
    screens.setCharAt(Point(M_BLOCKER_X, M_BLOCKER_Y), EMPTY_SPACE);
}

void Room3Boss::removeAllBombs(Screens& screens)
{
    // Scan and remove all 'B' tiles
    for (int y = 0; y < Screen::MAX_Y; ++y) {
        for (int x = 0; x < Screen::MAX_X; ++x) {
            if (screens.getCharAtPublic(Point(x, y)) == AUTO_BOMB) {
                screens.setCharAt(Point(x, y), EMPTY_SPACE);
            }
        }
    }
}

void Room3Boss::closeEntrances(Screens& screens)
{
    // Place M at both entrances to lock players in
    screens.setCharAt(Point(LEFT_ENTRANCE_X, LEFT_ENTRANCE_Y), M_TRAP);
    screens.setCharAt(Point(RIGHT_ENTRANCE_X, RIGHT_ENTRANCE_Y), M_TRAP);
}

void Room3Boss::removeTorchesFromPlayers(Player& p1, Player& p2)
{
    p1.setHasTorch(false);
    p2.setHasTorch(false);
}

// ==========================================
// Bomb Blinking
// ==========================================

void Room3Boss::updateBombBlink()
{
    int remaining = getTimeRemaining();
    
    if (remaining > BOMB_BLINK_THRESHOLD) {
        bombsVisible_ = true;
        return;
    }
    
    // In last 10 seconds - blink every 0.5s
    auto now = std::chrono::steady_clock::now();
    auto sinceBlinkStart = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - blinkStartTime_
    ).count();
    
    // Toggle every BOMB_BLINK_INTERVAL_MS
    int cycles = static_cast<int>(sinceBlinkStart / BOMB_BLINK_INTERVAL_MS);
    bombsVisible_ = (cycles % 2 == 0);
}

bool Room3Boss::shouldBombsBlink() const
{
    return (state_ == BossState::TaskRunning && getTimeRemaining() <= BOMB_BLINK_THRESHOLD);
}

bool Room3Boss::areBombsCurrentlyVisible() const
{
    if (state_ == BossState::Victory || state_ == BossState::Exit) {
        return false;  // Bombs removed in victory
    }
    return bombsVisible_;
}

// ==========================================
// Penalty & Restart Control
// ==========================================

bool Room3Boss::isRestartDisabled() const
{
    // Disable 'R' during active boss fight
    return (state_ != BossState::Inactive && state_ != BossState::PreBoss);
}

// ==========================================
// Rendering - Overlays
// ==========================================

void Room3Boss::drawOverlay() const
{
    // Only draw countdown overlay here (briefing is handled by showBriefing())
    if (state_ == BossState::Countdown) {
        // Draw countdown overlay (centered on screen)
        gotoxy(35, 10);
        std::cout << "================================";
        gotoxy(35, 11);
        std::cout << "    THE GAME WILL START IN...   ";
        gotoxy(35, 12);
        std::cout << "              " << countdownValue_ << "                 ";
        gotoxy(35, 13);
        std::cout << "================================";
    }
}

void Room3Boss::showBriefing()
{
    // This is a BLOCKING method - displays once and waits for key
    
    // Clear screen once
    system("cls");
    
    std::cout << "\n\n";
    std::cout << "  ========================================================\n";
    std::cout << "              FINAL ROOM - JOHNNY'S BYTE-LOCK\n";
    std::cout << "  ========================================================\n\n";
    
    std::cout << "  Well well well... two heroes, one exit.\n";
    std::cout << "  I split you up because teamwork is hilarious.\n\n";
    
    std::cout << "  Each of you gets 4 switches. Together you build one\n";
    std::cout << "  8-bit number: 0xXY.\n\n";
    
    std::cout << "  I'll throw 3 timed challenges at you: 20s, 40s, 60s.\n\n";
    
    std::cout << "  Submit: press V.\n";
    std::cout << "  Wrong answer, time's up, or you hug a B?\n";
    std::cout << "  Congrats - you become confetti.\n";
    std::cout << "  (-2 lives, -500 score, try again.)\n\n";
    
    std::cout << "  Last 10 seconds of each challenge? The bombs blink.\n";
    std::cout << "  Your heartbeat won't.\n\n";
    
    std::cout << "  Beat all three and I'll remove the M blocking exit 3.\n";
    std::cout << "  Now stop reading and start sweating.\n\n";
    
    std::cout << "  --------------------------------------------------------\n";
    std::cout << "                  Press any key to continue...\n";
    std::cout << "  --------------------------------------------------------\n";
    
    // Wait for any key (BLOCKING)
    _getch();
    
    // Transition back to PreBoss
    state_ = BossState::PreBoss;
}

void Room3Boss::drawTaskBar(int legendY) const
{
    if (state_ != BossState::TaskRunning && state_ != BossState::Victory) {
        return;
    }
    
    if (state_ == BossState::Victory) {
        gotoxy(0, legendY);
        std::cout << "VICTORY! Both players reach exit '3' to complete!";
        return;
    }
    
    // Draw task info
    gotoxy(0, legendY);
    std::cout << taskData_.displayText;
    
    // Draw timer
    gotoxy(0, legendY + 1);
    int remaining = getTimeRemaining();
    std::cout << "Time: " << remaining << "s";
    
    // Draw bits
    std::cout << "    Bits: " << getBitsString();
    
    // Clear rest of line
    std::cout << "                    ";
}

// ==========================================
// Bomb Step Callback
// ==========================================

void Room3Boss::onBombStepped(Screens& screens, Player& p1, Player& p2)
{
    if (state_ == BossState::TaskRunning) {
        handleTaskFailure(screens, p1, p2);
    }
}
