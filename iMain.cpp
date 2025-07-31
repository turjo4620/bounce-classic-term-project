#include "iGraphics.h"
#include<stdio.h>
#include<math.h>
#include<stdbool.h>
#include "iSound.h"
#include <mmsystem.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>



#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 563
#define BUTTON_WIDTH 300
#define BUTTON_HEIGHT 100
#define SPLASH_START_BUTTON_WIDTH 180
#define SPLASH_START_BUTTON_HEIGHT 60
#define SPLASH_START_BUTTON_X (SCREEN_WIDTH / 2 - SPLASH_START_BUTTON_WIDTH / 2)
#define SPLASH_START_BUTTON_Y 30
#define MENU_BUTTON_WIDTH 250
#define MENU_BUTTON_HEIGHT 70
#define PLAY_BUTTON_X (SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2)
#define PLAY_BUTTON_Y 410
#define HELP_BUTTON_X (SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2)
#define HELP_BUTTON_Y 350
#define EXIT_BUTTON_X (SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2)
#define EXIT_BUTTON_Y 110
#define LEADERBOARD_BUTTON_X (SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2)
#define LEADERBOARD_BUTTON_Y 290
#define MENU_BUTTON_X (SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2)
#define SETTINGS_BUTTON_Y 230
#define ABOUT_US_BUTTON_Y 170
#define RESET_LEADERBOARD_BUTTON_WIDTH 200
#define RESET_LEADERBOARD_BUTTON_HEIGHT 50
#define RESET_LEADERBOARD_BUTTON_X (SCREEN_WIDTH - RESET_LEADERBOARD_BUTTON_WIDTH - 20)
#define RESET_LEADERBOARD_BUTTON_Y 20
#define PAUSE_BUTTON_X (SCREEN_WIDTH - 80)
#define PAUSE_BUTTON_Y (SCREEN_HEIGHT - 80)
#define PAUSE_BUTTON_SIZE 60
#define SPECIAL_BLOCK_RED 14
#define SPECIAL_BLOCK_GREEN 15
#define SPECIAL_BLOCK_BLUE 16
#define SPECIAL_BLOCK_YELLOW 17
#define RING1 4
#define RING2 8
#define BUTTON 5
#define GATE 6
#define FINISH_POINT 7
#define EMPTY 0
#define BRICK 1
#define SPIKE1 2
#define SPIKE2 20
#define THING 27
#define MOVING_ARROW 21
#define SPINNER 3
#define WATER_TILE 40
#define TILE_SIZE 150
#define CHARACTER_TILE 9
#define ROWS 60
#define COLS 320
#define TOTAL_FRAMES 18
#define ID_BRICK 1
#define MAX_LEVEL_TILES 100000
#define BASE_TILE_WIDTH 80.0f
#define BASE_TILE_HEIGHT 60.0f
#define LEVEL_START_X 0.0f
#define LEVEL_START_Y 0.0f
#define MAX_SPEED_X 8.0f
#define MAX_ARROWS 50
#define RED_FINISH_POINT 18
#define FADED_RING1 22
#define FADED_RING2 23

Image bgPlayingImage;
const int ROTATION_DELAY =5;
int rotationCounter = 0;
Image bgMenuImage;
int musicChannel = -1;

enum GameState {
    STATE_ADVENTURE_SPLASH,
    STATE_TITLE,
    STATE_LEVEL_SELECT,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_HELP_ADVENTURE,
    STATE_LEVEL_COMPLETED,
    STATE_QUIT_CONFIRM,
    STATE_GAME_OVER,
    STATE_ENTER_NAME,
    STATE_LEADERBOARD,
    STATE_GAME_SUMMARY,
    STATE_SETTINGS,
    STATE_ABOUT_US,
    STATE_BALL_BLASTING
};
int gameState = STATE_ADVENTURE_SPLASH;
int ballLives = 2;
bool isMusicEnabled = true;
bool isSoundEffectsEnabled = true;

struct PlayerScore {
    char name[50];
    int score;
    bool operator<(const PlayerScore& other) const {
        return score > other.score;
    }
};
std::vector<PlayerScore> leaderboard_level1;
std::vector<PlayerScore> leaderboard_level2;
std::vector<PlayerScore> leaderboard_level3;
char playerNameInput[50];
int playerNameIndex = 0;
char currentPlayerName[50];
int currentScore = 0;
bool gameEndedForScoreEntry = false;

float blastX, blastY;      // To store where the blast happens
int blastTimerCounter = 0; // To time the 2-second delay
float tileHeight = 60;
float tileWidth = 80;
float ballX;
float ballY;
int ballWidth = 70, ballHeight =72;
float gravity = 0.2;
float vy = 0;
float speedX = 0;
bool onGround = false;
int platformX = 0, platformY = 50;
int platformwidth = 800, platformHeight = 20;
float accelerationX = 4;
float frictionX = 0.95;
float jumpStrength = 15.0;
int cameraX = 0;
int cameraY = 0;
float startX = 0;
float startY = 0;
float groundY = 50;
int numActualTiles;
char ballImageNames[TOTAL_FRAMES][30];
int rotationFrame = 0;
int NUM_FLOOR_TILES ;
float lastScoringPositionX = 0.0f;
int mouse_x = 0;
int mouse_y = 0;
int leaderboardScrollOffsets[3] = {0, 0, 0};
int currentLevel = 1;
bool levelCompleted = false;

struct Tile {
    int id;
    float x, y;
    float width, height;
    bool isSolid;
    bool hitByBall;
};
struct Tile levelTiles[MAX_LEVEL_TILES];
int ringsCollected = 0;
int totalRingsInLevel = 0;

struct MovingArrow {
    float tileIndex;
    float baseY;
    float moveRange;
    float speed;
    bool movingUp;
};
MovingArrow movingArrows[MAX_ARROWS];
int numMovingArrows = 0;


void drawAdventureSplashScreen() {
    iShowImage(0, 0, "images/front.png");
    if (mouse_x >= SPLASH_START_BUTTON_X && mouse_x <= SPLASH_START_BUTTON_X + SPLASH_START_BUTTON_WIDTH &&
        mouse_y >= SPLASH_START_BUTTON_Y && mouse_y <= SPLASH_START_BUTTON_Y + SPLASH_START_BUTTON_HEIGHT) {
        iSetColor(50, 200, 50);
    } else {
        iSetColor(0, 150, 0);
    }
    iFilledRectangle(SPLASH_START_BUTTON_X, SPLASH_START_BUTTON_Y, SPLASH_START_BUTTON_WIDTH, SPLASH_START_BUTTON_HEIGHT);
    iSetColor(255, 255, 255);
    iRectangle(SPLASH_START_BUTTON_X, SPLASH_START_BUTTON_Y, SPLASH_START_BUTTON_WIDTH, SPLASH_START_BUTTON_HEIGHT);
    iTextBold(SPLASH_START_BUTTON_X + 60, SPLASH_START_BUTTON_Y + 25, "START", GLUT_BITMAP_TIMES_ROMAN_24);
}

void countTotalRings() {
    totalRingsInLevel = 0;
    for (int i = 0; i < numActualTiles; i++) {
        if (levelTiles[i].id == RING1 || levelTiles[i].id == RING2) {
            totalRingsInLevel++;
        }
    }
}

// Replace your existing drawGameOverScreen() function with this

void drawGameOverScreen() {
    // Draw the background image
    // Add a semi-transparent dark overlay for the "pause" effect


    // --- Dialog Box ---
    int dialogWidth = 400;
    int dialogHeight = 250;
    int dialogX = SCREEN_WIDTH / 2 - dialogWidth / 2;
    int dialogY = SCREEN_HEIGHT / 2 - dialogHeight / 2;

    // iSetColor(50, 50, 70); // Dark blue-gray for the dialog
    // iFilledRectangle(dialogX, dialogY, dialogWidth, dialogHeight);
    // iSetColor(200, 200, 220); // Light border
    // iRectangle(dialogX, dialogY, dialogWidth, dialogHeight);
    iShowImage(dialogX + 50, dialogY - 50, "images/gameover.png");
    // --- Text inside Dialog ---
    // iSetColor(255, 60, 60); // Red "Game Over" text
    // iText(dialogX + 130, dialogY + 200, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);

    // char finalScoreText[50];
    // sprintf(finalScoreText, "Your Score: %d", currentScore);
    // iSetColor(255, 255, 0); // Yellow score text
    // iText(dialogX + 125, dialogY + 160, finalScoreText, GLUT_BITMAP_HELVETICA_18);

    // // --- Buttons inside Dialog ---
    // const int brown_r = 139, brown_g = 69, brown_b = 19;
    // const int light_brown_r = 160, light_brown_g = 82, light_brown_b = 45;
    // float button_w = 170; // Smaller buttons for the dialog
    // float button_h = 50;
    
    // // Restart Button Position
    // float restart_btn_x = dialogX + 25;
    // float restart_btn_y = dialogY + 50;

    // // Main Menu Button Position
    // float mainMenu_btn_x = dialogX + dialogWidth - button_w - 25;
    // float mainMenu_btn_y = dialogY + 50;


    // // --- Restart Button Drawing ---
    // if (mouse_x >= restart_btn_x && mouse_x <= restart_btn_x + button_w &&
    //     mouse_y >= restart_btn_y && mouse_y <= restart_btn_y + button_h) {
    //     iSetColor(light_brown_r, light_brown_g, light_brown_b);
    // } else {
    //     iSetColor(brown_r, brown_g, brown_b);
    // }
    // iFilledRectangle(restart_btn_x, restart_btn_y, button_w, button_h);
    // iSetColor(255, 255, 255);
    // iRectangle(restart_btn_x, restart_btn_y, button_w, button_h);
    // iRectangle(restart_btn_x + 1, restart_btn_y + 1, button_w - 2, button_h - 2);
    // iText(restart_btn_x + 45, restart_btn_y + 18, "RESTART", GLUT_BITMAP_TIMES_ROMAN_24);

    // // --- Main Menu Button Drawing ---
    // if (mouse_x >= mainMenu_btn_x && mouse_x <= mainMenu_btn_x + button_w &&
    //     mouse_y >= mainMenu_btn_y && mouse_y <= mainMenu_btn_y + button_h) {
    //     iSetColor(light_brown_r, light_brown_g, light_brown_b);
    // } else {
    //     iSetColor(brown_r, brown_g, brown_b);
    // }
    // iFilledRectangle(mainMenu_btn_x, mainMenu_btn_y, button_w, button_h);
    // iSetColor(255, 255, 255);
    // iRectangle(mainMenu_btn_x, mainMenu_btn_y, button_w, button_h);
    // iRectangle(mainMenu_btn_x + 1, mainMenu_btn_y + 1, button_w - 2, button_h - 2);
    // iText(mainMenu_btn_x + 30, mainMenu_btn_y + 18, "MAIN MENU", GLUT_BITMAP_TIMES_ROMAN_24);
}
void resetBallPosition() {
    if (currentLevel == 1) {
        ballX = 100.0f;
        ballY = BASE_TILE_HEIGHT + 1;
    }
    else if (currentLevel == 2) {
        float respawnX = ballX - (10 * BASE_TILE_WIDTH);
        float levelStartX = 7 * BASE_TILE_WIDTH;
        if (respawnX < levelStartX) {
            ballX = levelStartX;
        } else {
            ballX = respawnX;
        }
        ballY = BASE_TILE_HEIGHT + 1;
    }
   else if (currentLevel == 3) {
      if ( ballY >= 39* BASE_TILE_HEIGHT && ballX >= 16* BASE_TILE_WIDTH && ballX < 54* BASE_TILE_WIDTH)
      {
        { 
            float levelStartX = 54 * BASE_TILE_WIDTH;
            ballX = levelStartX;
            ballY = BASE_TILE_HEIGHT * 45;
        }
      }

else if ( ballY >= 29* BASE_TILE_HEIGHT && ballX <= 27* BASE_TILE_WIDTH && ballX >= 4* BASE_TILE_WIDTH)
 {
    { 
        float levelStartX = 13 * BASE_TILE_WIDTH;
        ballX = levelStartX;
        ballY = BASE_TILE_HEIGHT * 45;
    }
       
 }
else  {
        float levelStartX = 35 * BASE_TILE_WIDTH; 
        ballX = levelStartX;
        ballY = BASE_TILE_HEIGHT * 26;

     }

}
    speedX = 0;
    vy = 0;
    lastScoringPositionX = ballX;
}

void drawLevelCompletedScreen() {
    iShowImage(0, 0, "images/levelcomplete.png");

    // Display score
    char finalScoreText[50];
    sprintf(finalScoreText, "Your Score: %d", currentScore);
    iSetColor(255, 255, 0);
    iText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 250, finalScoreText, GLUT_BITMAP_TIMES_ROMAN_24);

    // --- Button Styling & Positioning ---
    const int brown_r = 139, brown_g = 69, brown_b = 19;
    const int light_brown_r = 160, light_brown_g = 82, light_brown_b = 45;
    float button_w = MENU_BUTTON_WIDTH;
    float button_h = 50;
    float button_x = SCREEN_WIDTH / 2 + 100;

    // Y positions are now lower
    float nextLevelY = SCREEN_HEIGHT / 2 - 30;
    float restartY = SCREEN_HEIGHT / 2 - 110;
    float mainMenuY = SCREEN_HEIGHT / 2 - 190;

    // --- Next Level Button (Only if not the last level) ---
    if (currentLevel < 3) {
        if (mouse_x >= button_x && mouse_x <= button_x + button_w &&
            mouse_y >= nextLevelY && mouse_y <= nextLevelY + button_h) {
            iSetColor(light_brown_r, light_brown_g, light_brown_b);
        } else {
            iSetColor(brown_r, brown_g, brown_b);
        }
        iFilledRectangle(button_x, nextLevelY, button_w, button_h);
        iSetColor(255, 255, 255);
        iRectangle(button_x, nextLevelY, button_w, button_h);
        iRectangle(button_x + 1, nextLevelY + 1, button_w - 2, button_h - 2);
        iText(button_x + 70, nextLevelY + 18, "NEXT LEVEL", GLUT_BITMAP_TIMES_ROMAN_24);
    }

    // --- Restart Button ---
    if (mouse_x >= button_x && mouse_x <= button_x + button_w &&
        mouse_y >= restartY && mouse_y <= restartY + button_h) {
        iSetColor(light_brown_r, light_brown_g, light_brown_b);
    } else {
        iSetColor(brown_r, brown_g, brown_b);
    }
    iFilledRectangle(button_x, restartY, button_w, button_h);
    iSetColor(255, 255, 255);
    iRectangle(button_x, restartY, button_w, button_h);
    iRectangle(button_x + 1, restartY + 1, button_w - 2, button_h - 2);
    iText(button_x + 80, restartY + 18, "RESTART", GLUT_BITMAP_TIMES_ROMAN_24);

    // --- Main Menu Button ---
    if (mouse_x >= button_x && mouse_x <= button_x + button_w &&
        mouse_y >= mainMenuY && mouse_y <= mainMenuY + button_h) {
        iSetColor(light_brown_r, light_brown_g, light_brown_b);
    } else {
        iSetColor(brown_r, brown_g, brown_b);
    }
    iFilledRectangle(button_x, mainMenuY, button_w, button_h);
    iSetColor(255, 255, 255);
    iRectangle(button_x, mainMenuY, button_w, button_h);
    iRectangle(button_x + 1, mainMenuY + 1, button_w - 2, button_h - 2);
    iText(button_x + 65, mainMenuY + 18, "MAIN MENU", GLUT_BITMAP_TIMES_ROMAN_24);
}
void addMovingArrow(float startRow, float startcol, float baseRow, float range, float speed, bool startMovingUp) {
    levelTiles[numActualTiles].id = MOVING_ARROW;
    levelTiles[numActualTiles].x = startRow * BASE_TILE_WIDTH;
    levelTiles[numActualTiles].y = startcol * BASE_TILE_HEIGHT;
    levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
    levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
    levelTiles[numActualTiles].isSolid = false;
    int idx = numActualTiles;
    numActualTiles++;
    movingArrows[numMovingArrows].tileIndex = idx;
    movingArrows[numMovingArrows].baseY = baseRow * BASE_TILE_HEIGHT;
    movingArrows[numMovingArrows].moveRange = range;
    movingArrows[numMovingArrows].speed = speed;
    movingArrows[numMovingArrows].movingUp = startMovingUp;
    numMovingArrows++;
}

void addBrickBlock(float startCol, float endCol, float startRow,float endRow) {
    for (int i = startCol; i <= endCol; i++) {
        for (int j = startRow; j <= endRow; j++) {
            if (numActualTiles < MAX_LEVEL_TILES) {
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
        }
    }
}

void addRing(float col,float row ,int ringType) {
    if (numActualTiles < MAX_LEVEL_TILES) {
        levelTiles[numActualTiles].id = ringType;
        levelTiles[numActualTiles].x = col * BASE_TILE_WIDTH;
        levelTiles[numActualTiles].y = row * BASE_TILE_HEIGHT;
        levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
        levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
        levelTiles[numActualTiles].isSolid = false;
        numActualTiles++;
    }
}

void addSpike(float col, float row, int spikeType) {
    if (numActualTiles < MAX_LEVEL_TILES) {
        levelTiles[numActualTiles].id = spikeType;
        levelTiles[numActualTiles].x = col * BASE_TILE_WIDTH;
        levelTiles[numActualTiles].y = row * BASE_TILE_HEIGHT;
        levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
        levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
        levelTiles[numActualTiles].isSolid = false;
        numActualTiles++;
    }
}

void initializeLevel1() {
    ballX = 100.0f;
    ballY = BASE_TILE_HEIGHT + 1;
    speedX = 0;
    vy = 0;
    NUM_FLOOR_TILES = 112;
    numActualTiles = 0;
    ringsCollected = 0;
    addBrickBlock(0, NUM_FLOOR_TILES - 1, 0, 0);
    addBrickBlock(0, NUM_FLOOR_TILES - 1, 9, 9);
    addBrickBlock(0, 0, 0, 9);

       for (int i = 111; i <=111; i++) {
        for (int j = 1; j <=1; j++)
            
            {
                levelTiles[numActualTiles].id = RED_FINISH_POINT;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = 2 * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                 levelTiles[numActualTiles].hitByBall = false;
                numActualTiles++;
            }
    }
    addBrickBlock(111,111,3,9);
    addBrickBlock(4, 9, 4, 9);
    addBrickBlock(13, 14, 0, 5);
    addBrickBlock(19, 21, 3, 9);
    for (int i = 24; i <= 27; i++) {
        for (int j = 0; j <=4; j++)
            if(j==4 || (j<=3 && (i==24 || i==25)))
            {
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    addBrickBlock(30, 33, 3, 4);
    addBrickBlock(31, 32, 2, 2);
    for (int i = 36; i <= 39; i++) {
        for (int j = 0; j <=4; j++)
            if(j==4 || (j<=3 && (i==38 || i==39)))
            {
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    for (int i = 42; i <= 44; i++) {
        for (int j = 4; j <9; j++)
            if(j==8 || (j<=7 && (i==42 || i==43)))
            {
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    for (int i = 64; i <= 66; i++) {
        for (int j = 4; j <9; j++)
            if(j==9-1 || (j<=9-2 && (i==65 || i==66)))
            {
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    addBrickBlock(69, 70, 0, 5);
    addBrickBlock(73, 74, 4, 9);
    addBrickBlock(77, 78, 0, 5);
    addBrickBlock(81, 82, 4, 9);
    addBrickBlock(85, 86, 0, 5);
    addBrickBlock(91, 92, 0, 2);
    addBrickBlock(93, 93, 0, 1);
    addBrickBlock(94, 95, 0, 3);
    addBrickBlock(98, 99, 4, 9);
    addBrickBlock(102 ,103, 0, 3);
    addBrickBlock(104, 104, 0, 1);
    addBrickBlock(105, 106, 0, 2);
    addRing(7,1,RING1);
    addRing(31, 5 ,RING1);
    addRing(73, 1 ,RING1);
    addRing(81, 1 ,RING1);
    addRing(96.5, 3 ,RING2);
    addRing(100.5, 3 ,RING2);
    addSpike(16,1,SPIKE1);
    addSpike(28,1,SPIKE1);
    addSpike(34,1,SPIKE1);
    addSpike(49,1,SPIKE1);
    addSpike(52,1,SPIKE1);
    addSpike(56,1,SPIKE1);
    addSpike(59,1,SPIKE1);
    addSpike(93,2,SPIKE1);
    addSpike(104,2,SPIKE1);
    countTotalRings();
}

void initializeLevel2(){
    numActualTiles = 0;
    ballX = 7* BASE_TILE_WIDTH;
    ballY = 7*BASE_TILE_HEIGHT ;
    speedX = 0;
    vy = 0;
    NUM_FLOOR_TILES = 134;
    addBrickBlock(0, NUM_FLOOR_TILES - 1, 0, 0);
    addBrickBlock(0, 1, 0, ROWS - 1);
    float rightEdgeX = (NUM_FLOOR_TILES - 4) * BASE_TILE_WIDTH;
   for (int i = 129; i <=129; i++) {
        for (int j = 20; j <=20; j++)
            
            {
                levelTiles[numActualTiles].id = RED_FINISH_POINT;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = 2 * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                 levelTiles[numActualTiles].hitByBall = false;
                numActualTiles++;
            }
    }

   for (int i = 129; i <= 130; i++) {
        for (int j = 1; j <=29; j++)
            
            {if(i ==129 && (j == 20 || j== 21)) continue;
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }

    for (int i = 0; i < NUM_FLOOR_TILES - 7; i++) {
        levelTiles[numActualTiles].id = ID_BRICK;
        levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
        levelTiles[numActualTiles].y = 540;
        levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
        levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
        levelTiles[numActualTiles].isSolid = true;
        numActualTiles++;
    }
    addBrickBlock(12, 13, 0, 5);
    for (int i = 14; i <= 19; i++) {
        for (int j = 4; j <= 5; j++) {
            if ((i == 14 && j== 4) || (i == 15 && j== 4) || (i == 16 && j== 4) || (i == 17 && j== 4))  continue;
           {
                if (numActualTiles < MAX_LEVEL_TILES) {
                    levelTiles[numActualTiles].id = ID_BRICK;
                    levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                    levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                    levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                    levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                    levelTiles[numActualTiles].isSolid = true;
                    numActualTiles++;
                }
            }
        }
    }
    addBrickBlock(22, 23, 4,8);
    addBrickBlock(26, 27, 1, 5);
    addBrickBlock(30, 31, 6, 9);
    addBrickBlock(37, 41, 1, 5);
    addBrickBlock(61,62, 4, 8);
    const int pyramidHeight = 6;
    const int centerCol = 72;
    for (int row = 0; row < pyramidHeight; row++) {
        int tilesInRow = 2 * row + 1;
        int startCol = centerCol - row;
        for (int col = 0; col < tilesInRow; col++) {
            if (numActualTiles < MAX_LEVEL_TILES) {
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + (startCol + col) * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + (pyramidHeight - row - 1) * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
        }
    }
    addBrickBlock(85, 86, 4,  11);
    addBrickBlock(96, 97, 6, 9);
    addBrickBlock(96, 97, 1, 3);
    addBrickBlock(107,108, 1, 5);
    addBrickBlock(111, 112, 4, 8);
    addBrickBlock(115, 118, 4, 4);
    addBrickBlock(117, 118, 1, 5);
    addBrickBlock(122, 123, 5, 8);
    addBrickBlock(126, 128, 5, 5);
    for (int step = 0; step < 6; step++) {
        int start_i = 125 - step;
        int start_j = 9 + step;
        for (int i = start_i; i >= start_i - 1; i--) {
            for (int j = start_j; j <= start_j + 1; j++) {
                if (numActualTiles < MAX_LEVEL_TILES) {
                    levelTiles[numActualTiles].id = ID_BRICK;
                    levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                    levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                    levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                    levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                    levelTiles[numActualTiles].isSolid = true;
                    numActualTiles++;
                }
            }
        }
    }
    for (int step = 0; step < 6; step++) {
        int start_i = 110 + step;
        int start_j = 9 + step;
        for (int i = start_i; i <= start_i + 1; i++) {
            for (int j = start_j; j <= start_j + 1; j++) {
                if (numActualTiles < MAX_LEVEL_TILES) {
                    levelTiles[numActualTiles].id = ID_BRICK;
                    levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                    levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                    levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                    levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                    levelTiles[numActualTiles].isSolid = true;
                    numActualTiles++;
                }
            }
        }
    }
    addBrickBlock(103, 104, 10, 30);
    addBrickBlock(109, 128, 19, 19);
    addBrickBlock(105, 107, 15, 15);
    addBrickBlock(105, 120, 23, 23);
    addBrickBlock(105, 128, 28, 29);
    addRing(18,6,RING1);
    addRing(37,6,RING1);
    addRing(61,1,RING1);
    addRing(85,1,RING1);
    addRing(96,4,RING1);
    addRing(107,6,RING1);
    addRing(119,24,RING1);
    addRing(127.5,9,RING2);
    addSpike(10,1,SPIKE1);
    addSpike(127,1,SPIKE1);
    addSpike(117,10,SPIKE1);
    addMovingArrow(3.5, 2, 4, 180, 1.2, false);
    addMovingArrow(2, 2, 4, 180, 1.2, false);
    addMovingArrow(46, 2, 4, 180, 1.2, false);
    addMovingArrow(51, 2, 4, 180, 1.2, false);
    addMovingArrow(56, 2, 4, 180, 1.2, false);
    addMovingArrow(88, 2, 4, 180, 1.2, false);
}

void initializelevel3(){
    ballX = 54*BASE_TILE_WIDTH;
    ballY = BASE_TILE_HEIGHT*44;
    speedX = 0;
    vy = 0;
    NUM_FLOOR_TILES = 200;
    numActualTiles = 0;
    ringsCollected = 0;
   for (int i = 61; i <= 61; i++) {
        for (int j = 40; j <=40; j++)
            
            {
                levelTiles[numActualTiles].id = RED_FINISH_POINT;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = 2 * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                 levelTiles[numActualTiles].hitByBall = false;
                numActualTiles++;
            }
    }

    for (int i = 0; i < 68; i++) {
        if(i==33 || i==34 || i==35 || i==36) continue;
        if (numActualTiles < MAX_LEVEL_TILES) {
            levelTiles[numActualTiles].id = ID_BRICK;
            levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
            levelTiles[numActualTiles].y = BASE_TILE_HEIGHT*29;
            levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
            levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].isSolid = true;
            numActualTiles++;
        }
    }
    addBrickBlock(0, 1,20, 69);
    addBrickBlock(0, 132, 47, 47);
    addBrickBlock(6, 7,35, 35);
    addBrickBlock(2, 4, 39, 39);
    for (int i = 10; i <= 12; i++) {
        for (int j = 29; j <=33; j++)
            
            {if(i==12 && j== 33) continue;
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    for (int i = 22; i <= 25; i++) {
        for (int j = 29; j <=33; j++)
            { if((i ==23  && j ==33) || (i ==22  && j ==33)) continue;
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    addBrickBlock(10, 11, 39, 46);
    for (int i = 12; i <= 15; i++) {
        for (int j = 39; j <=42; j++)
            if((i==12 && j<=42) ||(i==13 && j<=41 )|| (i==14  && j<=40) || (i==15 && j==39))
            {
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    for (int i = 18; i <= 23; i++) {
        for (int j = 39; j <=43; j++)
            if(((i==22 || i==23)||((i==21 && j<=42) ||(i==20 && j<=41 )|| (i==19  && j<=40) || (i==18 && j==39))))
            {
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    for (int i = 20; i < 133; i++) {
        if( i==92 || i== 93) continue;
        if (numActualTiles < MAX_LEVEL_TILES) {
            levelTiles[numActualTiles].id = ID_BRICK;
            levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
            levelTiles[numActualTiles].y = BASE_TILE_HEIGHT*39;
            levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
            levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].isSolid = true;
            numActualTiles++;
        }
    }
    for (int i = 44; i <= 46; i++) {
        for (int j = 39; j <=41; j++)
            if((i==45 || i==46) || (i==44 && j==40))
            {
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    for (int i = 44; i <= 46; i++) {
        for (int j = 45; j <=47; j++)
            if((i==45 || i==46) || (i==44 && j==46))
            {
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    for (int i = 58; i <= 61; i++)
    {
        for (int j = 40; j <=47; j++)
           
            { if((i==58 && j>=41) || (i==59 && j>=42) ||(i == 61 && (j == 40 || j == 41) ))  continue;
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }

    for (int i = 68; i <= 70; i++) {
        for (int j = 39; j <=41; j++)
            if((i==68 || i==69) || (i==70 && j==40))
            {
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    for (int i = 68; i <= 70; i++) {
        for (int j =45; j <=47; j++)
            if((i==68 || i==69) || (i==70 && j==46))
            {
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    for (int i = 75; i <= 79; i++) {
        for (int j = 39; j <=41; j++)
            {  if((i==75 && j==41) || (i==79 && j==41)) continue;
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    for (int i = 75; i <= 79; i++) {
        for (int j = 45; j <=47; j++)
            {  if((i==75 && j==45) || (i==79 && j==45)) continue;
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    for (int i = 87; i <= 90; i++) {
        for (int j = 39; j <=41; j++)
            {  if((i==87 && j==41) || (i==90 && j==41)) continue;
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    for (int i = 95; i <= 110; i++) {
        for (int j = 39; j <=41; j++)
            {  if((i==95 && j==41) ) continue;
                levelTiles[numActualTiles].id = ID_BRICK;
                levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
                levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
                levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
                levelTiles[numActualTiles].isSolid = true;
                numActualTiles++;
            }
    }
    addBrickBlock(131, 132, 40, 47);
    addBrickBlock(111,112, 44, 44);
    addBrickBlock(113, 114, 42, 42);
    addBrickBlock(115, 116, 44, 44);
    addBrickBlock(118, 119, 41, 41);
    addBrickBlock(121, 122, 40, 40);
    addBrickBlock(122, 123, 43, 43);
    for (int i = 126; i <=128; i++)
    {
        for (int j = 41; j <=42; j++)
        {
            if((i==126 && j==42)|| (i== 128 && j==41)) continue;
            levelTiles[numActualTiles].id = ID_BRICK;
            levelTiles[numActualTiles].x = LEVEL_START_X + i* BASE_TILE_WIDTH;
            levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
            levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].isSolid = true;
            numActualTiles++;
        }
    }
    for (int i = 31; i <= 32; i++) {
        for (int j = 29; j <=38; j++)
        {
            if(j == 32 || j==33 ) continue;
            levelTiles[numActualTiles].id = ID_BRICK;
            levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
            levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
            levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].isSolid = true;
            numActualTiles++;
        }
    }
    for (int i = 22; i <= 46; i++) {
         if((i==22 || i==23) ||(i==45 || i==46))
        for (int j = 21; j <=30; j++)
        {
            levelTiles[numActualTiles].id = ID_BRICK;
            levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
            levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
            levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].isSolid = true;
            numActualTiles++;
        }
    }
    addBrickBlock(22, 36, 25, 25);
    addBrickBlock(22, 46, 21, 21);
    for (int i = 37; i <= 38; i++) {
        for (int j = 29; j <=38; j++)
        {
            if(j == 32 || j==33  ) continue;
            levelTiles[numActualTiles].id = ID_BRICK;
            levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
            levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
            levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].isSolid = true;
            numActualTiles++;
        }
    }
    for (int i = 44; i <= 47; i++) {
        for (int j = 29; j <=32; j++)
        {
            if((i==46 && j== 32) || (i == 47 && (j==31 || j==32))) continue;
            levelTiles[numActualTiles].id = ID_BRICK;
            levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
            levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
            levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].isSolid = true;
            numActualTiles++;
        }
    }
    for (int i = 62; i <= 67; i++) {
        for (int j = 34; j <=38; j++)
        {
            if((i==62 && (j== 34 || j==35 ||j==36 || j==37)) ||(i==63 && (j== 34 || j==35 ||j==36)) || (i ==64 && (j==34 || j==35)) ||(i==65 && j== 34) ) continue;
            levelTiles[numActualTiles].id = ID_BRICK;
            levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
            levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
            levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].isSolid = true;
            numActualTiles++;
        }
    }
    for (int i = 66; i <= 68; i++) {
        for (int j =14 ; j <=29; j++)
        {
            if(i==68 && j== 29) continue;
            levelTiles[numActualTiles].id = ID_BRICK;
            levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
            levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
            levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
            levelTiles[numActualTiles].isSolid = true;
            numActualTiles++;
        }
    }
    addBrickBlock(68, 132,  14, 14);
    addBrickBlock(74, 91, 17, 39);
    addBrickBlock(94, 111, 17, 39);
    addBrickBlock(112, 132, 22, 22);
    addBrickBlock(132, 133, 14, 22);
    addBrickBlock(123, 124, 15, 15);
    addRing(45,42,RING1);
    addRing(31,32,RING1);
    addRing(37,32,RING1);
    addRing(74,15,RING1);
    addRing(128,43,RING1);
    addRing(4,40,RING1); 
    addRing(68,42,RING1);
    addRing(77,42,RING1);
    addRing(16.5,39,RING2);
    addSpike(39,30,SPIKE1);
    addSpike(43,30,SPIKE1);
    addSpike(123,16,SPIKE1);
    addSpike(123,40,SPIKE1);
    addSpike(124,40,SPIKE1);
    addSpike(129,40,SPIKE1);
    addSpike(6,33.4,SPIKE2);
    addSpike(4,45.4,SPIKE2);
    addSpike(2,45.4,SPIKE2);
    addSpike(53,37.4,SPIKE2);
    addSpike(58,37.4,SPIKE2);
    addSpike(70,37.4,SPIKE2);
    levelTiles[numActualTiles].id = THING;
    levelTiles[numActualTiles].x = LEVEL_START_X + 24 * BASE_TILE_WIDTH;
    levelTiles[numActualTiles].y = LEVEL_START_Y + 26 * BASE_TILE_HEIGHT;
    levelTiles[numActualTiles].width = BASE_TILE_WIDTH;
    levelTiles[numActualTiles].height = BASE_TILE_HEIGHT;
    levelTiles[numActualTiles].isSolid = true;
    numActualTiles++;
    addMovingArrow(41, 43, 42.5, 145, 1.2, false);
    addMovingArrow(39.5,43, 42.5, 145, 1.2, false);
    addMovingArrow(38,43, 42.5, 145, 1.2, false);
    addMovingArrow(36.5,43, 42.5, 145, 1.2, false);
    addMovingArrow(35, 43, 42.5, 145, 1.2, false);
    addMovingArrow(33.5, 43, 42.5, 145, 1.2, false);
    addMovingArrow(26, 33, 33.5, 205, 1.2, false);
    addMovingArrow(117, 17, 17.5, 145, 1.2, false);

// ----water drawing----

for (int i = 75; i <= 75; i++) {         
    for (int j = 15; j <= 15; j++) 
    {    
        levelTiles[numActualTiles].id = WATER_TILE;
        levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
        levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
        levelTiles[numActualTiles].width = BASE_TILE_WIDTH* 35;
        levelTiles[numActualTiles].height = BASE_TILE_HEIGHT * 2;
        levelTiles[numActualTiles].isSolid = false;
        numActualTiles++;
    }
}

for (int i = 92; i <= 92; i++) {        
    for (int j = 15; j <= 15; j++) 
    {    
        levelTiles[numActualTiles].id = WATER_TILE;
        levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
        levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
        levelTiles[numActualTiles].width = BASE_TILE_WIDTH * 2;
        levelTiles[numActualTiles].height = BASE_TILE_HEIGHT * 25;
        levelTiles[numActualTiles].isSolid = false;
        numActualTiles++;
    }
}

for (int i = 13; i <= 13; i++) {         
    for (int j = 30; j <= 30; j++) 
    {    
        levelTiles[numActualTiles].id = WATER_TILE;
        levelTiles[numActualTiles].x = LEVEL_START_X + i * BASE_TILE_WIDTH;
        levelTiles[numActualTiles].y = LEVEL_START_Y + j * BASE_TILE_HEIGHT;
        levelTiles[numActualTiles].width = BASE_TILE_WIDTH* 9;
        levelTiles[numActualTiles].height = BASE_TILE_HEIGHT * 3;
        levelTiles[numActualTiles].isSolid = false;
        numActualTiles++;
    }
}

}

void drawGameUI() {
    iSetColor(255, 255, 255);
    char ringText[50];
    sprintf(ringText, "Rings: %d / %d", ringsCollected, totalRingsInLevel);
    iText(20, SCREEN_HEIGHT - 40, ringText, GLUT_BITMAP_TIMES_ROMAN_24);
    char livesText[50];
    sprintf(livesText, "Lives: %d", ballLives);
    iText(20, SCREEN_HEIGHT - 70, livesText, GLUT_BITMAP_TIMES_ROMAN_24);
    char scoreText[50];
    sprintf(scoreText, "Score: %d", currentScore);
    iText(SCREEN_WIDTH - 200, SCREEN_HEIGHT - 40, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);
}

void loadLevel(int level) {
    // Set lives based on the current level
    if (level == 1) {
        ballLives = 2;
    } else if (level == 2) {
        ballLives = 3;
    } else if (level == 3) {
        ballLives = 4;
    }

    ringsCollected = 0;
    currentScore = 0;
    lastScoringPositionX = (level == 1) ? 100.0f : ((level == 2) ? 7 * BASE_TILE_WIDTH : 54 * BASE_TILE_WIDTH);
    if (level == 1) {
        initializeLevel1();
    } else if (level == 2) {
        initializeLevel2();
    } else if (level == 3) {
        initializelevel3();
    }
    countTotalRings();
}

void updateAllMovingArrows() {
    for (int i = 0; i < numMovingArrows; i++) {
        int idx = movingArrows[i].tileIndex;
        float& y = levelTiles[idx].y;
        if (movingArrows[i].movingUp) {
            y += movingArrows[i].speed;
            if (y >= movingArrows[i].baseY + movingArrows[i].moveRange) {
                movingArrows[i].movingUp = false;
            }
        } else {
            y -= movingArrows[i].speed;
            if (y <= movingArrows[i].baseY - movingArrows[i].moveRange) {
                movingArrows[i].movingUp = true;
            }
        }
    }
}

void drawSpinner(int x, int y) {
    int size = 60;
    iSetColor(0, 0, 255);
    iFilledRectangle(x + size/4, y + size/4, size/2, size/2);
    iSetColor(255, 255, 255);
    iLine(x, y, x + size, y + size);
    iLine(x + size, y, x, y + size);
}

void loadBallImageNames() {
    for (int i = 0; i < TOTAL_FRAMES; i++) {
        sprintf(ballImageNames[i], "images/ball_%d.png", i);
    }
}

void loadLeaderboard() {
    leaderboard_level1.clear();
    leaderboard_level2.clear();
    leaderboard_level3.clear();
    std::ifstream file("leaderboard.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string levelStr, nameStr, scoreStr;
            std::getline(ss, levelStr, ',');
            std::getline(ss, nameStr, ',');
            std::getline(ss, scoreStr, ',');
            if (!levelStr.empty() && !nameStr.empty() && !scoreStr.empty()) {
                int level = std::stoi(levelStr);
                PlayerScore ps;
                strncpy(ps.name, nameStr.c_str(), sizeof(ps.name) - 1);
                ps.name[sizeof(ps.name) - 1] = '\0';
                ps.score = std::stoi(scoreStr);
                if (level == 1) leaderboard_level1.push_back(ps);
                else if (level == 2) leaderboard_level2.push_back(ps);
                else if (level == 3) leaderboard_level3.push_back(ps);
            }
        }
        file.close();
        std::sort(leaderboard_level1.begin(), leaderboard_level1.end());
        std::sort(leaderboard_level2.begin(), leaderboard_level2.end());
        std::sort(leaderboard_level3.begin(), leaderboard_level3.end());
    }
}

void saveLeaderboard() {
    std::ofstream file("leaderboard.txt");
    if (file.is_open()) {
        for (const auto& score : leaderboard_level1) file << "1," << score.name << "," << score.score << "\n";
        for (const auto& score : leaderboard_level2) file << "2," << score.name << "," << score.score << "\n";
        for (const auto& score : leaderboard_level3) file << "3," << score.name << "," << score.score << "\n";
        file.close();
    }
}

void addScoreToLeaderboard(int level, const char* name, int score) {
    std::vector<PlayerScore>* currentLeaderboard = nullptr;
    if (level == 1) currentLeaderboard = &leaderboard_level1;
    else if (level == 2) currentLeaderboard = &leaderboard_level2;
    else if (level == 3) currentLeaderboard = &leaderboard_level3;
    else return;
    bool playerExists = false;
    for (auto& entry : *currentLeaderboard) {
        if (strcmp(entry.name, name) == 0) {
            playerExists = true;
            if (score > entry.score) entry.score = score;
            break;
        }
    }
    if (!playerExists) {
        PlayerScore newScore;
        strncpy(newScore.name, name, sizeof(newScore.name) - 1);
        newScore.name[sizeof(newScore.name) - 1] = '\0';
        newScore.score = score;
        currentLeaderboard->push_back(newScore);
    }
    std::sort(currentLeaderboard->begin(), currentLeaderboard->end());
    saveLeaderboard();
}

void drawEnterNameScreen() {
    iShowImage(0, 0, "images/landingpage.png");
    int dialogWidth = 400;
    int dialogHeight = 250;
    int dialogX = SCREEN_WIDTH / 2 - dialogWidth / 2;
    int dialogY = SCREEN_HEIGHT / 2 - dialogHeight / 2;
    iSetColor(50, 50, 70);
    iFilledRectangle(dialogX, dialogY, dialogWidth, dialogHeight);
    iSetColor(200, 200, 220);
    iRectangle(dialogX, dialogY, dialogWidth, dialogHeight);
    iSetColor(255, 255, 255);
    iText(dialogX + 60, dialogY + 200, "ENTER YOUR NICKNAME", GLUT_BITMAP_TIMES_ROMAN_24);
    float fieldWidth = 300;
    float fieldHeight = 40;
    float fieldX = dialogX + (dialogWidth - fieldWidth) / 2;
    float fieldY = dialogY + 110;
    iSetColor(255, 255, 255);
    iFilledRectangle(fieldX, fieldY, fieldWidth, fieldHeight);
    if (mouse_x >= fieldX && mouse_x <= fieldX + fieldWidth &&
        mouse_y >= fieldY && mouse_y <= fieldY + fieldHeight) {
        iSetColor(0, 150, 255);
    } else {
        iSetColor(100, 100, 100);
    }
    iRectangle(fieldX, fieldY, fieldWidth, fieldHeight);
    iSetColor(0, 0, 0);
    iText(fieldX + 10, fieldY + 10, playerNameInput, GLUT_BITMAP_HELVETICA_18);
    float buttonWidth = 150;
    float buttonHeight = 50;
    float buttonX = dialogX + (dialogWidth - buttonWidth) / 2;
    float buttonY = dialogY + 40;
    if (mouse_x >= buttonX && mouse_x <= buttonX + buttonWidth &&
        mouse_y >= buttonY && mouse_y <= buttonY + buttonHeight) {
        iSetColor(50, 220, 100);
    } else {
        iSetColor(0, 180, 50);
    }
    iFilledRectangle(buttonX, buttonY, buttonWidth, buttonHeight);
    iSetColor(255, 255, 255);
    iText(buttonX + 35, buttonY + 15, "SUBMIT", GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawLeaderboardScreen() {
    iShowLoadedImage2(0, 0, &bgMenuImage, SCREEN_WIDTH, SCREEN_HEIGHT);
    iSetColor(255, 255, 255);
    iText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 60, "LEADERBOARD", GLUT_BITMAP_TIMES_ROMAN_24);

    int headerY = SCREEN_HEIGHT - 120;
    int listStartY = headerY - 30;
    int col1_X = SCREEN_WIDTH * 0.1;
    int col2_X = SCREEN_WIDTH * 0.4;
    int col3_X = SCREEN_WIDTH * 0.7;

    iSetColor(255, 255, 0);
    iText(col1_X, headerY, "--- LEVEL 1 ---", GLUT_BITMAP_HELVETICA_18);
    iText(col2_X, headerY, "--- LEVEL 2 ---", GLUT_BITMAP_HELVETICA_18);
    iText(col3_X, headerY, "--- LEVEL 3 ---", GLUT_BITMAP_HELVETICA_18);

    // Leaderboard drawing logic remains the same...
    float name_X_offset = 35;
    float score_X_offset = 200;
    char rankStr[10];
    char nameStr[16];
    char scoreStr[20];
    iSetColor(255, 255, 255);

    for (size_t i = 0; i < leaderboard_level1.size(); ++i) {
        float currentY = (listStartY + leaderboardScrollOffsets[0]) - (i * 25);
        if (currentY < headerY && currentY > 120) {
            strncpy(nameStr, leaderboard_level1[i].name, 15); nameStr[15] = '\0';
            sprintf(rankStr, "%2d.", (int)i + 1); sprintf(scoreStr, "%d", leaderboard_level1[i].score);
            iText(col1_X, currentY, rankStr, GLUT_BITMAP_HELVETICA_18);
            iText(col1_X + name_X_offset, currentY, nameStr, GLUT_BITMAP_HELVETICA_18);
            iText(col1_X + score_X_offset, currentY, scoreStr, GLUT_BITMAP_HELVETICA_18);
        }
    }
    for (size_t i = 0; i < leaderboard_level2.size(); ++i) {
        float currentY = (listStartY + leaderboardScrollOffsets[1]) - (i * 25);
        if (currentY < headerY && currentY > 120) {
            strncpy(nameStr, leaderboard_level2[i].name, 15); nameStr[15] = '\0';
            sprintf(rankStr, "%2d.", (int)i + 1); sprintf(scoreStr, "%d", leaderboard_level2[i].score);
            iText(col2_X, currentY, rankStr, GLUT_BITMAP_HELVETICA_18);
            iText(col2_X + name_X_offset, currentY, nameStr, GLUT_BITMAP_HELVETICA_18);
            iText(col2_X + score_X_offset, currentY, scoreStr, GLUT_BITMAP_HELVETICA_18);
        }
    }
    for (size_t i = 0; i < leaderboard_level3.size(); ++i) {
        float currentY = (listStartY + leaderboardScrollOffsets[2]) - (i * 25);
        if (currentY < headerY && currentY > 120) {
            strncpy(nameStr, leaderboard_level3[i].name, 15); nameStr[15] = '\0';
            sprintf(rankStr, "%2d.", (int)i + 1); sprintf(scoreStr, "%d", leaderboard_level3[i].score);
            iText(col3_X, currentY, rankStr, GLUT_BITMAP_HELVETICA_18);
            iText(col3_X + name_X_offset, currentY, nameStr, GLUT_BITMAP_HELVETICA_18);
            iText(col3_X + score_X_offset, currentY, scoreStr, GLUT_BITMAP_HELVETICA_18);
        }
    }

    // --- Styled & Aligned Buttons ---
    const int brown_r = 139, brown_g = 69, brown_b = 19;
    const int light_brown_r = 160, light_brown_g = 82, light_brown_b = 45;
    float button_w = MENU_BUTTON_WIDTH;
    float button_h = 50; // General height for all buttons on this screen
    float button_x = SCREEN_WIDTH / 2 - button_w / 2;

    // --- Reset All Data Button (Now centered) ---
    float reset_button_y = 110;
    if (mouse_x >= button_x && mouse_x <= button_x + button_w &&
        mouse_y >= reset_button_y && mouse_y <= reset_button_y + button_h) {
        iSetColor(light_brown_r, light_brown_g, light_brown_b);
    } else {
        iSetColor(brown_r, brown_g, brown_b);
    }
    iFilledRectangle(button_x, reset_button_y, button_w, button_h);
    iSetColor(255, 255, 255);
    iRectangle(button_x, reset_button_y, button_w, button_h);
    iRectangle(button_x + 1, reset_button_y + 1, button_w - 2, button_h - 2);
    iText(button_x + 30, reset_button_y + 15, "RESET ALL DATA", GLUT_BITMAP_TIMES_ROMAN_24);

    // --- Back to Menu Button ---
    float back_button_y = 50;
    if (mouse_x >= button_x && mouse_x <= button_x + button_w &&
        mouse_y >= back_button_y && mouse_y <= back_button_y + button_h) { // Uses button_h
        iSetColor(light_brown_r, light_brown_g, light_brown_b);
    } else {
        iSetColor(brown_r, brown_g, brown_b);
    }
    iFilledRectangle(button_x, back_button_y, button_w, button_h); // Uses button_h
    iSetColor(255, 255, 255);
    iRectangle(button_x, back_button_y, button_w, button_h); // Uses button_h
    iRectangle(button_x + 1, back_button_y + 1, button_w - 2, button_h - 2); // Uses button_h
    iText(button_x + 35, back_button_y + 18, "BACK TO MENU", GLUT_BITMAP_TIMES_ROMAN_24);
}
void drawGameSummaryScreen() {
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    iShowImage(0, 0, "images/landingpage.png");
    iSetColor(255, 255, 255);
    iText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 100, "GAME SUMMARY", GLUT_BITMAP_TIMES_ROMAN_24);
    char summaryText[100];
    sprintf(summaryText, "%s - Your score is: %d", playerNameInput, currentScore);
    iText(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 30, summaryText, GLUT_BITMAP_HELVETICA_18);
    iSetColor(180, 180, 0);
    iFilledRectangle(SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, SCREEN_HEIGHT / 2 - 50, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
    iSetColor(255, 255, 255);
    iText(SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2 - 25, "LEADERBOARD", GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(100, 100, 100);
    iFilledRectangle(SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, SCREEN_HEIGHT / 2 - 150, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
    iSetColor(255, 255, 255);
    iText(SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2 - 125, "MAIN MENU", GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawTitleScreen() {
    iShowImage(0, 0, "images/landingpage.png");

    // Define brown colors
    const int brown_r = 139, brown_g = 69, brown_b = 19;
    const int light_brown_r = 160, light_brown_g = 82, light_brown_b = 45;

    // --- PLAY Button ---
    if (mouse_x >= MENU_BUTTON_X && mouse_x <= MENU_BUTTON_X + MENU_BUTTON_WIDTH &&
        mouse_y >= PLAY_BUTTON_Y && mouse_y <= PLAY_BUTTON_Y + 50) {
        iSetColor(light_brown_r, light_brown_g, light_brown_b); // Lighter brown for hover
    } else {
        iSetColor(brown_r, brown_g, brown_b); // Brown for normal state
    }
    iFilledRectangle(MENU_BUTTON_X, PLAY_BUTTON_Y, MENU_BUTTON_WIDTH, 50);
    // Set color to white for borders and text
    iSetColor(255, 255, 255);
    iRectangle(MENU_BUTTON_X, PLAY_BUTTON_Y, MENU_BUTTON_WIDTH, 50); // Outer border
    iRectangle(MENU_BUTTON_X + 1, PLAY_BUTTON_Y + 1, MENU_BUTTON_WIDTH - 2, 50 - 2); // Inner border
    iText(MENU_BUTTON_X + 95, PLAY_BUTTON_Y + 18, "PLAY", GLUT_BITMAP_TIMES_ROMAN_24);

    // --- HELP Button ---
    if (mouse_x >= MENU_BUTTON_X && mouse_x <= MENU_BUTTON_X + MENU_BUTTON_WIDTH &&
        mouse_y >= HELP_BUTTON_Y && mouse_y <= HELP_BUTTON_Y + 50) {
        iSetColor(light_brown_r, light_brown_g, light_brown_b);
    } else {
        iSetColor(brown_r, brown_g, brown_b);
    }
    iFilledRectangle(MENU_BUTTON_X, HELP_BUTTON_Y, MENU_BUTTON_WIDTH, 50);
    iSetColor(255, 255, 255);
    iRectangle(MENU_BUTTON_X, HELP_BUTTON_Y, MENU_BUTTON_WIDTH, 50);
    iRectangle(MENU_BUTTON_X + 1, HELP_BUTTON_Y + 1, MENU_BUTTON_WIDTH - 2, 50 - 2);
    iText(MENU_BUTTON_X + 95, HELP_BUTTON_Y + 18, "HELP", GLUT_BITMAP_TIMES_ROMAN_24);

    // --- LEADERBOARD Button ---
    if (mouse_x >= MENU_BUTTON_X && mouse_x <= MENU_BUTTON_X + MENU_BUTTON_WIDTH &&
        mouse_y >= LEADERBOARD_BUTTON_Y && mouse_y <= LEADERBOARD_BUTTON_Y + 50) {
        iSetColor(light_brown_r, light_brown_g, light_brown_b);
    } else {
        iSetColor(brown_r, brown_g, brown_b);
    }
    iFilledRectangle(MENU_BUTTON_X, LEADERBOARD_BUTTON_Y, MENU_BUTTON_WIDTH, 50);
    iSetColor(255, 255, 255);
    iRectangle(MENU_BUTTON_X, LEADERBOARD_BUTTON_Y, MENU_BUTTON_WIDTH, 50);
    iRectangle(MENU_BUTTON_X + 1, LEADERBOARD_BUTTON_Y + 1, MENU_BUTTON_WIDTH - 2, 50 - 2);
    iText(MENU_BUTTON_X + 40, LEADERBOARD_BUTTON_Y + 18, "LEADERBOARD", GLUT_BITMAP_TIMES_ROMAN_24);

    // --- SETTINGS Button ---
    if (mouse_x >= MENU_BUTTON_X && mouse_x <= MENU_BUTTON_X + MENU_BUTTON_WIDTH &&
        mouse_y >= SETTINGS_BUTTON_Y && mouse_y <= SETTINGS_BUTTON_Y + 50) {
        iSetColor(light_brown_r, light_brown_g, light_brown_b);
    } else {
        iSetColor(brown_r, brown_g, brown_b);
    }
    iFilledRectangle(MENU_BUTTON_X, SETTINGS_BUTTON_Y, MENU_BUTTON_WIDTH, 50);
    iSetColor(255, 255, 255);
    iRectangle(MENU_BUTTON_X, SETTINGS_BUTTON_Y, MENU_BUTTON_WIDTH, 50);
    iRectangle(MENU_BUTTON_X + 1, SETTINGS_BUTTON_Y + 1, MENU_BUTTON_WIDTH - 2, 50 - 2);
    iText(MENU_BUTTON_X + 75, SETTINGS_BUTTON_Y + 18, "SETTINGS", GLUT_BITMAP_TIMES_ROMAN_24);

    // --- ABOUT US Button ---
    if (mouse_x >= MENU_BUTTON_X && mouse_x <= MENU_BUTTON_X + MENU_BUTTON_WIDTH &&
        mouse_y >= ABOUT_US_BUTTON_Y && mouse_y <= ABOUT_US_BUTTON_Y + 50) {
        iSetColor(light_brown_r, light_brown_g, light_brown_b);
    } else {
        iSetColor(brown_r, brown_g, brown_b);
    }
    iFilledRectangle(MENU_BUTTON_X, ABOUT_US_BUTTON_Y, MENU_BUTTON_WIDTH, 50);
    iSetColor(255, 255, 255);
    iRectangle(MENU_BUTTON_X, ABOUT_US_BUTTON_Y, MENU_BUTTON_WIDTH, 50);
    iRectangle(MENU_BUTTON_X + 1, ABOUT_US_BUTTON_Y + 1, MENU_BUTTON_WIDTH - 2, 50 - 2);
    iText(MENU_BUTTON_X + 75, ABOUT_US_BUTTON_Y + 18, "ABOUT US", GLUT_BITMAP_TIMES_ROMAN_24);

    // --- EXIT Button ---
    if (mouse_x >= MENU_BUTTON_X && mouse_x <= MENU_BUTTON_X + MENU_BUTTON_WIDTH &&
        mouse_y >= EXIT_BUTTON_Y && mouse_y <= EXIT_BUTTON_Y + 50) {
        iSetColor(light_brown_r, light_brown_g, light_brown_b);
    } else {
        iSetColor(brown_r, brown_g, brown_b);
    }
    iFilledRectangle(MENU_BUTTON_X, EXIT_BUTTON_Y, MENU_BUTTON_WIDTH, 50);
    iSetColor(255, 255, 255);
    iRectangle(MENU_BUTTON_X, EXIT_BUTTON_Y, MENU_BUTTON_WIDTH, 50);
    iRectangle(MENU_BUTTON_X + 1, EXIT_BUTTON_Y + 1, MENU_BUTTON_WIDTH - 2, 50 - 2);
    iText(MENU_BUTTON_X + 95, EXIT_BUTTON_Y + 18, "EXIT", GLUT_BITMAP_TIMES_ROMAN_24);
}


void drawAboutUsScreen() {
    iShowImage(0, 0, "images/landingpage.png");
    iSetColor(0, 0, 0);

    // Title
    iText(SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT - 70, "ABOUT US", GLUT_BITMAP_TIMES_ROMAN_24);

    // Main Text
    iText(100, SCREEN_HEIGHT - 140, "We are two curious and passionate students from the Department of CSE, BUET (Batch 2024),", GLUT_BITMAP_HELVETICA_18);
    iText(100, SCREEN_HEIGHT - 165, "currently in Level 1-Term 1. Bounce Classic is our first collaborative project in the", GLUT_BITMAP_HELVETICA_18);
    iText(100, SCREEN_HEIGHT - 190, "field of computer programming.", GLUT_BITMAP_HELVETICA_18);
    iText(100, SCREEN_HEIGHT - 240, "This game was born out of countless hours of trial, error, and learning. From coding basic", GLUT_BITMAP_HELVETICA_18);
    iText(100, SCREEN_HEIGHT - 265, "mechanics to designing visual interactions, we explored the fun side of logic and creativity.", GLUT_BITMAP_HELVETICA_18);
    iText(100, SCREEN_HEIGHT - 290, "Every bounce in this game carries a piece of our dedication.", GLUT_BITMAP_HELVETICA_18);
    iText(100, SCREEN_HEIGHT - 340, "We hope you had a great time playing it-because we definitely had a great time building it!", GLUT_BITMAP_HELVETICA_18);

    // Developers
    iSetColor(255, 255, 0);
    iText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 400, "DEVELOPERS:", GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(0, 0, 0);
    iText(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT - 435, "Turjo Sarkar Prince - 2405142", GLUT_BITMAP_HELVETICA_18);
    iText(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT - 460, "Md Azadur Rahman Sayem - 2405139", GLUT_BITMAP_HELVETICA_18);


    // --- Back to Menu Button (Styled) ---
    // Define colors and dimensions
    const int brown_r = 139, brown_g = 69, brown_b = 19;
    const int light_brown_r = 160, light_brown_g = 82, light_brown_b = 45;
    float button_x = SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2;
    float button_y = 30; // Y-position in the lower part of the screen
    float button_w = MENU_BUTTON_WIDTH;
    float button_h = 50; // Standard height to match other buttons

    if (mouse_x >= button_x && mouse_x <= button_x + button_w &&
        mouse_y >= button_y && mouse_y <= button_y + button_h) {
        iSetColor(light_brown_r, light_brown_g, light_brown_b);
    } else {
        iSetColor(brown_r, brown_g, brown_b);
    }
    iFilledRectangle(button_x, button_y, button_w, button_h);


    iSetColor(255, 255, 255);


    iRectangle(button_x, button_y, button_w, button_h);
    iRectangle(button_x + 1, button_y + 1, button_w - 2, button_h - 2);


    iText(SCREEN_WIDTH / 2 - 90, button_y + 18, "BACK TO MENU", GLUT_BITMAP_TIMES_ROMAN_24);
}
void drawSettingsScreen() {
    drawTitleScreen();
    int dialogWidth = 400;
    int dialogHeight = 250;
    int dialogX = SCREEN_WIDTH / 2 - dialogWidth / 2;
    int dialogY = SCREEN_HEIGHT / 2 - dialogHeight / 2;
    iSetColor(50, 50, 70);
    iFilledRectangle(dialogX, dialogY, dialogWidth, dialogHeight);
    iSetColor(200, 200, 220);
    iRectangle(dialogX, dialogY, dialogWidth, dialogHeight);
    iSetColor(255, 255, 255);
    iText(dialogX + 150, dialogY + 200, "SETTINGS", GLUT_BITMAP_TIMES_ROMAN_24);
    int musicButtonX = dialogX + 50;
    int musicButtonY = dialogY + 120;
    iSetColor(0, 150, 200);
    iFilledRectangle(musicButtonX, musicButtonY, 300, 50);
    iSetColor(255, 255, 255);
    const char* musicText = isMusicEnabled ? "MUSIC: ON" : "MUSIC: OFF";
    iText(musicButtonX + 100, musicButtonY + 18, musicText, GLUT_BITMAP_HELVETICA_18);
    int soundButtonX = dialogX + 50;
    int soundButtonY = dialogY + 50;
    iSetColor(0, 150, 200);
    iFilledRectangle(soundButtonX, soundButtonY, 300, 50);
    iSetColor(255, 255, 255);
    const char* soundText = isSoundEffectsEnabled ? "SOUNDS: ON" : "SOUNDS: OFF";
    iText(soundButtonX + 95, soundButtonY + 18, soundText, GLUT_BITMAP_HELVETICA_18);
}

void drawHelpScreen() {
    iShowImage(0, 0, "images/landingpage.png");
    iSetColor(0, 0, 0);
    iText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 100, "HELP / INSTRUCTIONS", GLUT_BITMAP_TIMES_ROMAN_24);
    iText(50, SCREEN_HEIGHT - 200, "- Use 'A' to move left, 'D' to move right.", GLUT_BITMAP_HELVETICA_18);
    iText(50, SCREEN_HEIGHT - 230, "- Press 'W' or 'SPACE' to jump.", GLUT_BITMAP_HELVETICA_18);
    iText(50, SCREEN_HEIGHT - 260, "- Collect all rings to complete a level.", GLUT_BITMAP_HELVETICA_18);
    iText(50, SCREEN_HEIGHT - 290, "- Avoid spikes and other hazards.", GLUT_BITMAP_HELVETICA_18);
    iText(50, SCREEN_HEIGHT - 320, "- Press 'P' to pause/unpause the game.", GLUT_BITMAP_HELVETICA_18);
    iText(50, SCREEN_HEIGHT - 350, "- Press 'M' to return to Main Menu during gameplay.", GLUT_BITMAP_HELVETICA_18);
    iText(50, SCREEN_HEIGHT - 380, "- Press 'ESC' to exit the game.", GLUT_BITMAP_HELVETICA_18);
    
    // --- Styled Back to Menu Button ---
    const int brown_r = 139, brown_g = 69, brown_b = 19;
    const int light_brown_r = 160, light_brown_g = 82, light_brown_b = 45;
    float button_x = SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2;
    float button_y = 50;
    float button_h = 50;

    if (mouse_x >= button_x && mouse_x <= button_x + MENU_BUTTON_WIDTH &&
        mouse_y >= button_y && mouse_y <= button_y + button_h) {
        iSetColor(light_brown_r, light_brown_g, light_brown_b);
    } else {
        iSetColor(brown_r, brown_g, brown_b);
    }
    iFilledRectangle(button_x, button_y, MENU_BUTTON_WIDTH, button_h);
    iSetColor(255, 255, 255);
    iRectangle(button_x, button_y, MENU_BUTTON_WIDTH, button_h);
    iRectangle(button_x + 1, button_y + 1, MENU_BUTTON_WIDTH - 2, button_h - 2);
    iText(button_x + 40, button_y + 18, "BACK TO MENU", GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawLevelSelect() {
    iShowImage(0, 0, "images/landingpage.png");
    iSetColor(255, 255, 255);
    iShowImage(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 170, "images/level.png");
    // iText(SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT - 100, "SELECT A LEVEL", GLUT_BITMAP_TIMES_ROMAN_24);
    
    float button_x = SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2;
    float button_height = 50;
    float level1_y = SCREEN_HEIGHT / 2 + 80;
    float level2_y = SCREEN_HEIGHT / 2;
    float level3_y = SCREEN_HEIGHT / 2 - 80;
    float back_y = 50;
    const int brown_r = 139, brown_g = 69, brown_b = 19;
    const int light_brown_r = 160, light_brown_g = 82, light_brown_b = 45;
    // --- Level 1 Button ---
    if (mouse_x >= button_x && mouse_x <= button_x + MENU_BUTTON_WIDTH && mouse_y >= level1_y && mouse_y <= level1_y + button_height) {
        iSetColor(light_brown_r, light_brown_g, light_brown_b);
    } else {
        iSetColor(brown_r, brown_g, brown_b);
    }
    iFilledRectangle(button_x, level1_y, MENU_BUTTON_WIDTH, button_height);
    iSetColor(255, 255, 255);
    iRectangle(button_x, level1_y, MENU_BUTTON_WIDTH, button_height); // Added border
    iText(button_x + 85, level1_y + 18, "LEVEL 1", GLUT_BITMAP_TIMES_ROMAN_24);

    // --- Level 2 Button ---
    if (mouse_x >= button_x && mouse_x <= button_x + MENU_BUTTON_WIDTH && mouse_y >= level2_y && mouse_y <= level2_y + button_height) {
        iSetColor(light_brown_r, light_brown_g, light_brown_b);
    } else {
        iSetColor(brown_r, brown_g, brown_b);
    }
    iFilledRectangle(button_x, level2_y, MENU_BUTTON_WIDTH, button_height);
    iSetColor(255, 255, 255);
    iRectangle(button_x, level2_y, MENU_BUTTON_WIDTH, button_height); // Added border
    iText(button_x + 85, level2_y + 18, "LEVEL 2", GLUT_BITMAP_TIMES_ROMAN_24);

    // --- Level 3 Button ---
    if (mouse_x >= button_x && mouse_x <= button_x + MENU_BUTTON_WIDTH && mouse_y >= level3_y && mouse_y <= level3_y + button_height) {
        iSetColor(light_brown_r, light_brown_g, light_brown_b);
    } else {
        iSetColor(brown_r, brown_g, brown_b);
    }
    iFilledRectangle(button_x, level3_y, MENU_BUTTON_WIDTH, button_height);
    iSetColor(255, 255, 255);
    iRectangle(button_x, level3_y, MENU_BUTTON_WIDTH, button_height); // Added border
    iText(button_x + 85, level3_y + 18, "LEVEL 3", GLUT_BITMAP_TIMES_ROMAN_24);

    // --- Back to Menu Button ---
    if (mouse_x >= button_x && mouse_x <= button_x + MENU_BUTTON_WIDTH && mouse_y >= back_y && mouse_y <= back_y + button_height) {
        iSetColor(light_brown_r, light_brown_g, light_brown_b);
    } else {
        iSetColor(brown_r, brown_g, brown_b);
    }
    iFilledRectangle(button_x, back_y, MENU_BUTTON_WIDTH, button_height);
    iSetColor(255, 255, 255);
    iRectangle(button_x, back_y, MENU_BUTTON_WIDTH, button_height); // Added border
    iText(button_x + 40, back_y + 18, "BACK TO MENU", GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawPauseMenu() {
    // This menu is drawn as an overlay on top of the paused game world.
    

    // --- Button Styling & Positioning ---
    float button_w = MENU_BUTTON_WIDTH;
    float button_h = 50;
    float button_x = SCREEN_WIDTH / 2 - button_w / 2;
    float resume_y = SCREEN_HEIGHT / 2 + 70;
    float restart_y = SCREEN_HEIGHT / 2;
    float mainMenu_y = SCREEN_HEIGHT / 2 - 70;

    // --- Resume Button (Green) ---
    if (mouse_x >= button_x && mouse_x <= button_x + button_w && mouse_y >= resume_y && mouse_y <= resume_y + button_h) {
        iSetColor(50, 220, 100);
    } else {
        iSetColor(0, 180, 50);
    }
    iFilledRectangle(button_x, resume_y, button_w, button_h);
    iSetColor(255, 255, 255);
    iRectangle(button_x, resume_y, button_w, button_h);
    iRectangle(button_x + 1, resume_y + 1, button_w - 2, button_h - 2);
    iText(button_x + 85, resume_y + 18, "RESUME", GLUT_BITMAP_TIMES_ROMAN_24);

    // --- Restart Button (Blue) ---
    if (mouse_x >= button_x && mouse_x <= button_x + button_w && mouse_y >= restart_y && mouse_y <= restart_y + button_h) {
        iSetColor(50, 150, 220);
    } else {
        iSetColor(0, 100, 180);
    }
    iFilledRectangle(button_x, restart_y, button_w, button_h);
    iSetColor(255, 255, 255);
    iRectangle(button_x, restart_y, button_w, button_h);
    iRectangle(button_x + 1, restart_y + 1, button_w - 2, button_h - 2);
    iText(button_x + 80, restart_y + 18, "RESTART", GLUT_BITMAP_TIMES_ROMAN_24);

    // --- Main Menu Button (Red) ---
    if (mouse_x >= button_x && mouse_x <= button_x + button_w && mouse_y >= mainMenu_y && mouse_y <= mainMenu_y + button_h) {
        iSetColor(220, 100, 50);
    } else {
        iSetColor(180, 50, 0);
    }
    iFilledRectangle(button_x, mainMenu_y, button_w, button_h);
    iSetColor(255, 255, 255);
    iRectangle(button_x, mainMenu_y, button_w, button_h);
    iRectangle(button_x + 1, mainMenu_y + 1, button_w - 2, button_h - 2);
    iText(button_x + 65, mainMenu_y + 18, "MAIN MENU", GLUT_BITMAP_TIMES_ROMAN_24);

    iShowImage(SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT - 210,  "images/pause.png");
}

void drawQuitConfirmDialog()
{
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    iShowImage(0, 0, "images/landingpage.png");
    int dialogWidth = 600;
    int dialogHeight = 250;
    int dialogX = (SCREEN_WIDTH / 2) - (dialogWidth / 2);
    int dialogY = (SCREEN_HEIGHT / 2) - (dialogHeight / 2);
    iSetColor(220, 220, 220);
    iFilledRectangle(dialogX, dialogY, dialogWidth, dialogHeight);
    iSetColor(0, 0, 0);
    iRectangle(dialogX, dialogY, dialogWidth, dialogHeight);
    char* questionText = "Are you sure to quit?";
    int textWidth = 0;
    textWidth = strlen(questionText) * 13;
    iSetColor(0, 0, 0);
    iText(dialogX + (dialogWidth / 2) - (textWidth / 2), dialogY + dialogHeight - 100, questionText, GLUT_BITMAP_TIMES_ROMAN_24);
    int buttonWidth = 150;
    int buttonHeight = 60;
    int buttonPadding = 50;
    int yesButtonX = dialogX + (dialogWidth / 2) - buttonWidth - (buttonPadding / 2);
    int yesButtonY = dialogY + 40;
    iSetColor(220, 60, 60);
    iFilledRectangle(yesButtonX, yesButtonY, buttonWidth, buttonHeight);
    iSetColor(255, 255, 255);
    iText(yesButtonX + (buttonWidth / 2) - 15, yesButtonY + (buttonHeight / 2) - 8, "YES", GLUT_BITMAP_HELVETICA_18);
    int noButtonX = dialogX + (dialogWidth / 2) + (buttonPadding / 2);
    int noButtonY = dialogY + 40;
    iSetColor(60, 180, 60);
    iFilledRectangle(noButtonX, noButtonY, buttonWidth, buttonHeight);
    iSetColor(255, 255, 255);
    iText(noButtonX + (buttonWidth / 2) - 15, noButtonY + (buttonHeight / 2) - 8, "NO", GLUT_BITMAP_HELVETICA_18);
}

void iDraw() {
    iClear();
switch (gameState) {
    case STATE_ADVENTURE_SPLASH:
        drawAdventureSplashScreen();
        break;
    case STATE_TITLE:
        drawTitleScreen();
        break;
    case STATE_HELP_ADVENTURE:
        drawHelpScreen();
        break;
    case STATE_LEVEL_SELECT:
        drawLevelSelect();
        break;
    case STATE_QUIT_CONFIRM:
        drawQuitConfirmDialog();
        break;
    case STATE_LEVEL_COMPLETED:
        drawLevelCompletedScreen();
        break;
    case STATE_ENTER_NAME:
        drawEnterNameScreen();
        break;
    case STATE_LEADERBOARD:
        drawLeaderboardScreen();
        break;
    case STATE_GAME_SUMMARY:
        drawGameSummaryScreen();
        break;
    case STATE_ABOUT_US:
        drawAboutUsScreen();
        break;    
    case STATE_SETTINGS:
        drawSettingsScreen();
        break;

    // This combined case handles drawing for all in-game states
    case STATE_PLAYING:
    case STATE_PAUSED:
    case STATE_GAME_OVER:
    case STATE_BALL_BLASTING:
        iSetColor(85, 206, 255);
        iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        iShowLoadedImage2(0, 0, &bgPlayingImage, SCREEN_WIDTH, SCREEN_HEIGHT);

        // This loop draws all the level tiles
        for (int i = 0; i < numActualTiles; i++) {
            struct Tile currentTile = levelTiles[i];
            float drawX = currentTile.x - cameraX;
            float drawY = currentTile.y - cameraY;
            if (drawX + currentTile.width < 0 || drawX > SCREEN_WIDTH) continue;

            if (currentTile.id == ID_BRICK) {
                iSetColor(150, 75, 0); iFilledRectangle(drawX, drawY, currentTile.width, currentTile.height);
                iSetColor(0, 0, 0); iRectangle(drawX, drawY, currentTile.width, currentTile.height);
            } else if (currentTile.id == RED_FINISH_POINT) {
                iSetColor(255, 0, 0); iFilledRectangle(drawX, drawY, currentTile.width, currentTile.height);
            } else if (currentTile.id == SPIKE1) {
                iShowImage(drawX, drawY, "images/spike_1.png");
            } else if (currentTile.id == SPIKE2) {
                iShowImage(drawX, drawY, "images/spike_2.png");
            } else if (currentTile.id == RING1) {
                iShowImage(drawX, drawY, "images/ring1.png");
            } else if (currentTile.id == RING2) {
                iShowImage(drawX, drawY, "images/ring2.png");
            } else if (currentTile.id == FADED_RING1) {
                iShowImage(drawX, drawY, "images/faded_ring1.png");
            } else if (currentTile.id == FADED_RING2) {
                iShowImage(drawX, drawY, "images/faded_ring2.png");
            } else if (currentTile.id == THING) {
                iShowImage(drawX, drawY, "images/thing.png");
            } else if (levelTiles[i].id == MOVING_ARROW) {
                iShowImage(levelTiles[i].x - cameraX, levelTiles[i].y - cameraY, "images/arrow.png");
            } else if (currentTile.id == WATER_TILE) {
                iSetColor(0, 128, 255); iFilledRectangle(drawX, drawY, currentTile.width, currentTile.height);
            }
        }

        // Now, draw the correct elements for each specific state
        if (gameState == STATE_PLAYING) {
            iShowImage(ballX - cameraX, ballY - cameraY, ballImageNames[rotationFrame]);
            // iSetColor(0, 0, 0); iFilledRectangle(PAUSE_BUTTON_X, PAUSE_BUTTON_Y, PAUSE_BUTTON_SIZE, PAUSE_BUTTON_SIZE);
            iSetColor(255, 255, 255); iFilledRectangle(PAUSE_BUTTON_X + 15, PAUSE_BUTTON_Y + 15, 10, 30);
            iFilledRectangle(PAUSE_BUTTON_X + 35, PAUSE_BUTTON_Y + 15, 10, 30);
        } else if (gameState == STATE_BALL_BLASTING) {
            iShowImage(blastX - cameraX, blastY - cameraY, "images/blast.png"); 
        } else if (gameState == STATE_PAUSED) {
            iShowImage(ballX - cameraX, ballY - cameraY, ballImageNames[rotationFrame]);
            drawPauseMenu();
        } else if (gameState == STATE_GAME_OVER) {
            iShowImage(ballX - cameraX, ballY - cameraY, ballImageNames[rotationFrame]);
            drawGameOverScreen();
        }

        drawGameUI(); // Draw UI for all these states
        break;
}
}

void iMouseMove(int mx, int my)
{
    mouse_x = mx;
    mouse_y = my;
}

void iMouseDrag(int mx, int my)
{
}

void iMouse(int button, int state, int mx, int my) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (gameState == STATE_ADVENTURE_SPLASH &&
            mx >= SPLASH_START_BUTTON_X && mx <= SPLASH_START_BUTTON_X + SPLASH_START_BUTTON_WIDTH &&
            my >= SPLASH_START_BUTTON_Y && my <= SPLASH_START_BUTTON_Y + SPLASH_START_BUTTON_HEIGHT) {
            if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
            gameState = STATE_TITLE;
            if (isMusicEnabled && musicChannel == -1) {
                musicChannel = iPlaySound("music/game_music.wav", true, 80);
            }
        }
        else if (gameState == STATE_TITLE) {
            if (mx >= MENU_BUTTON_X && mx <= MENU_BUTTON_X + MENU_BUTTON_WIDTH && my >= PLAY_BUTTON_Y && my <= PLAY_BUTTON_Y + 50) {
                if(isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
                gameState = STATE_ENTER_NAME;
                playerNameInput[0] = '\0';
                playerNameIndex = 0;
            }
            else if (mx >= MENU_BUTTON_X && mx <= MENU_BUTTON_X + MENU_BUTTON_WIDTH && my >= HELP_BUTTON_Y && my <= HELP_BUTTON_Y + 50) {
                if(isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
                gameState = STATE_HELP_ADVENTURE;
            }
            else if (mx >= MENU_BUTTON_X && mx <= MENU_BUTTON_X + MENU_BUTTON_WIDTH && my >= LEADERBOARD_BUTTON_Y && my <= LEADERBOARD_BUTTON_Y + 50) {
                if(isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
                loadLeaderboard();
                gameState = STATE_LEADERBOARD;
            }
            else if (mx >= MENU_BUTTON_X && mx <= MENU_BUTTON_X + MENU_BUTTON_WIDTH && my >= SETTINGS_BUTTON_Y && my <= SETTINGS_BUTTON_Y + 50) {
                if(isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
                gameState = STATE_SETTINGS;
            }
            else if (mx >= MENU_BUTTON_X && mx <= MENU_BUTTON_X + MENU_BUTTON_WIDTH && my >= ABOUT_US_BUTTON_Y && my <= ABOUT_US_BUTTON_Y + 50) {
                if(isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
                gameState = STATE_ABOUT_US;
            }
            else if (mx >= MENU_BUTTON_X && mx <= MENU_BUTTON_X + MENU_BUTTON_WIDTH && my >= SETTINGS_BUTTON_Y && my <= SETTINGS_BUTTON_Y + 50) {
                if(isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
                gameState = STATE_SETTINGS;
            }
            else if (mx >= MENU_BUTTON_X && mx <= MENU_BUTTON_X + MENU_BUTTON_WIDTH && my >= EXIT_BUTTON_Y && my <= EXIT_BUTTON_Y + 50) {
                if(isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
                gameState = STATE_QUIT_CONFIRM;
            }
        }
        else if (gameState == STATE_SETTINGS) {
			int dialogWidth = 400;
			int dialogHeight = 250;
			int dialogX = SCREEN_WIDTH / 2 - dialogWidth / 2;
			int dialogY = SCREEN_HEIGHT / 2 - dialogHeight / 2;
			if (mx < dialogX || mx > dialogX + dialogWidth || my < dialogY || my > dialogY + dialogHeight) {
				gameState = STATE_TITLE;
			}
			else {
				int musicButtonX = dialogX + 50;
				int musicButtonY = dialogY + 120;
				int soundButtonX = dialogX + 50;
				int soundButtonY = dialogY + 50;
				if (mx >= musicButtonX && mx <= musicButtonX + 300 && my >= musicButtonY && my <= musicButtonY + 50) {
					isMusicEnabled = !isMusicEnabled;
					if (isMusicEnabled) {
						iResumeSound(musicChannel);
					} else {
						iPauseSound(musicChannel);
					}
				}
				else if (mx >= soundButtonX && mx <= soundButtonX + 300 && my >= soundButtonY && my <= soundButtonY + 50) {
					isSoundEffectsEnabled = !isSoundEffectsEnabled;
				}
			}
		}
		else if (gameState == STATE_LEVEL_SELECT) {
			float button_x = SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2;
			float button_height = 50;
			float level1_y = SCREEN_HEIGHT / 2 + 80;
			float level2_y = SCREEN_HEIGHT / 2;
			float level3_y = SCREEN_HEIGHT / 2 - 80;
			float back_y = 50;
			if (mx >= button_x && mx <= button_x + MENU_BUTTON_WIDTH && my >= level1_y && my <= level1_y + button_height) {
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
				currentLevel = 1;
				loadLevel(currentLevel);
				gameState = STATE_PLAYING;
				if (isMusicEnabled) iResumeSound(musicChannel);
			}
			else if (mx >= button_x && mx <= button_x + MENU_BUTTON_WIDTH && my >= level2_y && my <= level2_y + button_height) {
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
				currentLevel = 2;
				loadLevel(currentLevel);
				gameState = STATE_PLAYING;
				if (isMusicEnabled) iResumeSound(musicChannel);
			}
			else if (mx >= button_x && mx <= button_x + MENU_BUTTON_WIDTH && my >= level3_y && my <= level3_y + button_height) {
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
				currentLevel = 3;
				loadLevel(currentLevel);
				gameState = STATE_PLAYING;
				if (isMusicEnabled) iResumeSound(musicChannel);
			}
			else if (mx >= button_x && mx <= button_x + MENU_BUTTON_WIDTH && my >= back_y && my <= back_y + button_height) {
				gameState = STATE_TITLE;
				if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
			}
		}
        else if (gameState == STATE_PLAYING) {
            if (mx >= PAUSE_BUTTON_X && mx <= PAUSE_BUTTON_X + PAUSE_BUTTON_SIZE &&
                my >= PAUSE_BUTTON_Y && my <= PAUSE_BUTTON_Y + PAUSE_BUTTON_SIZE) {
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
                gameState = STATE_PAUSED;
            }
        }
        else if (gameState == STATE_PAUSED) {
            float button_w = MENU_BUTTON_WIDTH;
            float button_h = 50;
            float button_x = SCREEN_WIDTH / 2 - button_w / 2;
            float resume_y = SCREEN_HEIGHT / 2 + 70;
            float restart_y = SCREEN_HEIGHT / 2;
            float mainMenu_y = SCREEN_HEIGHT / 2 - 70;

            if (mx >= button_x && mx <= button_x + button_w && my >= resume_y && my <= resume_y + button_h) {
                gameState = STATE_PLAYING;
                if (isMusicEnabled) iResumeSound(musicChannel);
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
            }
            else if (mx >= button_x && mx <= button_x + button_w && my >= restart_y && my <= restart_y + button_h) {
                loadLevel(currentLevel);
                gameState = STATE_PLAYING;
                if (isMusicEnabled) iResumeSound(musicChannel);
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
            }
            else if (mx >= button_x && mx <= button_x + button_w && my >= mainMenu_y && my <= mainMenu_y + button_h) {
                gameState = STATE_TITLE;
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
            }
        }
        else if (gameState == STATE_HELP_ADVENTURE) {
        if (mx >= SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2 && mx <= SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2 + MENU_BUTTON_WIDTH &&
        my >= 50 && my <= 50 + 50) { // Height is now 50
        if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
        gameState = STATE_TITLE;
            }
        }
        else if (gameState == STATE_ABOUT_US) {
            float button_x = SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2;
            float button_y = 30; // Correct Y-position from draw function
            float button_w = MENU_BUTTON_WIDTH;
            float button_h = 50; 

            if (mx >= button_x && mx <= button_x + button_w &&
                my >= button_y && my <= button_y + button_h) {
                gameState = STATE_TITLE;
                if(isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
            }
        }
        else if (gameState == STATE_QUIT_CONFIRM) {
            int dialogWidth = 600, dialogHeight = 250;
            int dialogX = (SCREEN_WIDTH / 2) - (dialogWidth / 2);
            int dialogY = (SCREEN_HEIGHT / 2) - (dialogHeight / 2);
            int buttonWidth = 150, buttonHeight = 60, buttonPadding = 50;
            int yesButtonX = dialogX + (dialogWidth / 2) - buttonWidth - (buttonPadding / 2);
            int yesButtonY = dialogY + 40;
            int noButtonX = dialogX + (dialogWidth / 2) + (buttonPadding / 2);
            int noButtonY = dialogY + 40;
            if (mx >= yesButtonX && mx <= yesButtonX + buttonWidth && my >= yesButtonY && my <= yesButtonY + buttonHeight) {
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
                exit(0);
            }
            else if (mx >= noButtonX && mx <= noButtonX + buttonWidth && my >= noButtonY && my <= noButtonY + buttonHeight) {
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
                gameState = STATE_TITLE;
            }
        }
        else if (gameState == STATE_GAME_OVER) {
            float button_w = MENU_BUTTON_WIDTH;
            float button_h = 50;
            float button_x = SCREEN_WIDTH / 2 - button_w / 2;
            float restart_btn_y = 150;
            float mainMenu_btn_y = 80;

            if (mx >= 490 && mx <= 643 &&
                     my >= 180 && my <=250) {
                loadLevel(currentLevel);
                gameState = STATE_PLAYING;
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
            }
            else if (mx >= 383 && mx <= 461 &&
                my >= 180 && my <= 250) {
                gameState = STATE_TITLE;
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
            }
        }
        else if (gameState == STATE_LEVEL_COMPLETED) {
            float button_w = MENU_BUTTON_WIDTH;
            float button_h = 50;
            float buttonX = SCREEN_WIDTH / 2 + 100;
            
            float nextLevelY = SCREEN_HEIGHT / 2 - 30;
            float restartY = SCREEN_HEIGHT / 2 - 110;
            float mainMenuY = SCREEN_HEIGHT / 2 - 190;

            // Check for Next Level button click (Only if not the last level)
            if (currentLevel < 3 && mx >= buttonX && mx <= buttonX + button_w && my >= nextLevelY && my <= nextLevelY + button_h) {
                currentLevel++;
                loadLevel(currentLevel);
                gameState = STATE_PLAYING;
                if(isMusicEnabled) iResumeSound(musicChannel);
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
            }
            // Check for Restart button click
            else if (mx >= buttonX && mx <= buttonX + button_w && my >= restartY && my <= restartY + button_h) {
                loadLevel(currentLevel);
                gameState = STATE_PLAYING;
                if(isMusicEnabled) iResumeSound(musicChannel);
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
            }
            // Check for Main Menu button click
            else if (mx >= buttonX && mx <= buttonX + button_w && my >= mainMenuY && my <= mainMenuY + button_h) {
                gameState = STATE_TITLE;
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
            }
        }
		else if (gameState == STATE_ENTER_NAME) {
			int dialogWidth = 400;
			int dialogX = SCREEN_WIDTH / 2 - dialogWidth / 2;
			int dialogY = SCREEN_HEIGHT / 2 - 125;
			float buttonWidth = 150;
			float buttonHeight = 50;
			float buttonX = dialogX + (dialogWidth - buttonWidth) / 2;
			float buttonY = dialogY + 40;
			if (mx >= buttonX && mx <= buttonX + buttonWidth &&
				my >= buttonY && my <= buttonY + buttonHeight) {
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);    
				if (playerNameIndex > 0) {
					strcpy(currentPlayerName, playerNameInput);
				} else {
					strcpy(currentPlayerName, "Guest");
				}
				gameState = STATE_LEVEL_SELECT;
			}
		}
        else if (gameState == STATE_LEADERBOARD) {
            float button_w = MENU_BUTTON_WIDTH;
            float button_h = 50;
            float button_x = SCREEN_WIDTH / 2 - button_w / 2;
            float reset_button_y = 110;
            float back_button_y = 50;

            if (mx >= button_x && mx <= button_x + button_w &&
                my >= back_button_y && my <= back_button_y + button_h) {
                gameState = STATE_TITLE;
                leaderboardScrollOffsets[0] = 0;
                leaderboardScrollOffsets[1] = 0;
                leaderboardScrollOffsets[2] = 0;
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
            }
            else if (mx >= button_x && mx <= button_x + button_w &&
                     my >= reset_button_y && my <= reset_button_y + button_h) {
                leaderboard_level1.clear();
                leaderboard_level2.clear();
                leaderboard_level3.clear();
                saveLeaderboard();
                leaderboardScrollOffsets[0] = 0;
                leaderboardScrollOffsets[1] = 0;
                leaderboardScrollOffsets[2] = 0;
                if (isSoundEffectsEnabled) iPlaySound("music/click.wav", false);
            }
        }
    }
}

void iMouseWheel(int dir, int mx, int my) {
    if (gameState == STATE_LEADERBOARD) {
        int scrollSpeed = 25;
        int columnIndex = -1;
        if (mx < SCREEN_WIDTH / 3) {
            columnIndex = 0;
        } else if (mx < (SCREEN_WIDTH * 2) / 3) {
            columnIndex = 1;
        } else {
            columnIndex = 2;
        }
        if (dir == 1) {
            leaderboardScrollOffsets[columnIndex] += scrollSpeed;
        } else if (dir == -1) {
            leaderboardScrollOffsets[columnIndex] -= scrollSpeed;
        }
        if (leaderboardScrollOffsets[columnIndex] > 0) {
            leaderboardScrollOffsets[columnIndex] = 0;
        }
        size_t maxEntries = 0;
        if (columnIndex == 0) maxEntries = leaderboard_level1.size();
        else if (columnIndex == 1) maxEntries = leaderboard_level2.size();
        else if (columnIndex == 2) maxEntries = leaderboard_level3.size();
        if (maxEntries > 0) {
            int contentHeight = (maxEntries + 2) * 25;
            int viewableHeight = SCREEN_HEIGHT - 200;
            int maxScrollDown = contentHeight - viewableHeight;
            if (maxScrollDown < 0) {
                maxScrollDown = 0;
            }
            if (leaderboardScrollOffsets[columnIndex] < -maxScrollDown) {
                leaderboardScrollOffsets[columnIndex] = -maxScrollDown;
            }
        } else {
            leaderboardScrollOffsets[columnIndex] = 0;
        }
    }
}

void iKeyboard(unsigned char key, int state) {
    if (gameState == STATE_ENTER_NAME && state == 0) {
		if (key == '\b') {
			if (playerNameIndex > 0) {
				playerNameIndex--;
				playerNameInput[playerNameIndex] = '\0';
			}
		}
		else if (key == '\r') {
			if (playerNameIndex > 0) {
				strcpy(currentPlayerName, playerNameInput);
			} else {
				strcpy(currentPlayerName, "Guest");
			}
			playerNameInput[0] = '\0';
			playerNameIndex = 0;
			gameState = STATE_LEVEL_SELECT;
		}
		else if (playerNameIndex < sizeof(playerNameInput) - 1) {
			if (isalnum(key) || key == ' ') {
				playerNameInput[playerNameIndex++] = key;
				playerNameInput[playerNameIndex] = '\0';
			}
		}
	}
    if (state == 0) {
        if (key == 'p') {
            if (gameState == STATE_PLAYING) gameState = STATE_PAUSED;
            else if (gameState == STATE_PAUSED) gameState = STATE_PLAYING;
        }
        if (key == 'm') {
            if (gameState == STATE_PLAYING || gameState == STATE_PAUSED ||
                gameState == STATE_HELP_ADVENTURE || gameState == STATE_LEADERBOARD ||
                gameState == STATE_GAME_SUMMARY) {
                gameState = STATE_TITLE;
                ballLives = 2;
                ringsCollected = 0;
                ballX = 100.0f;
                ballY = BASE_TILE_HEIGHT + 1;
                speedX = 0;
                vy = 0;
                onGround = true;
                cameraX = 0;
                cameraY = 0;
            }
        }
        if (key == 27) {
            exit(0);
        }
    }
}

void updateBall() {
    if (!onGround) {
        vy -= gravity;
    }

    bool inWater = false;
    for (int i = 0; i < numActualTiles; i++) {
        if (levelTiles[i].id == WATER_TILE) {
            if (ballX + ballWidth > levelTiles[i].x && ballX < levelTiles[i].x + levelTiles[i].width &&
                ballY + ballHeight > levelTiles[i].y && ballY < levelTiles[i].y + levelTiles[i].height) {
                inWater = true;
                break;
            }
        }
    }

    if (inWater) {
        vy += 0.6f;
        vy *= 0.95f;
        speedX *= 0.9f;
    }

    if (isKeyPressed('a')) {
        speedX -= accelerationX;
        rotationCounter++;
        if (rotationCounter >= ROTATION_DELAY) {
            rotationFrame = (rotationFrame - 1 + TOTAL_FRAMES) % TOTAL_FRAMES;
            rotationCounter = 0;
        }
    } else if (isKeyPressed('d')) {
        speedX += accelerationX;
        rotationCounter++;
        if (rotationCounter >= ROTATION_DELAY) {
            rotationFrame = (rotationFrame + 1) % TOTAL_FRAMES;
            rotationCounter = 0;
        }
    } else {
        speedX = 0;
    }

    if ((isKeyPressed('w') || isKeyPressed(' ')) && onGround) {
        vy = jumpStrength;
        onGround = false;
        rotationFrame = (rotationFrame + 3) % TOTAL_FRAMES;
    }

    if (speedX > MAX_SPEED_X) speedX = MAX_SPEED_X;
    if (speedX < -MAX_SPEED_X) speedX = -MAX_SPEED_X;

    // --- Horizontal Collision ---
    ballX += speedX;
    for (int i = 0; i < numActualTiles; ++i) {
        struct Tile tile = levelTiles[i];
        if (!tile.isSolid) continue;
        if (ballX + ballWidth > tile.x && ballX < tile.x + tile.width && ballY + ballHeight > tile.y && ballY < tile.y + tile.height) {
            if (tile.id == RED_FINISH_POINT) {
                int requiredRings = (int)ceil(totalRingsInLevel * 0.70);
                if (ringsCollected >= requiredRings) {
                    addScoreToLeaderboard(currentLevel, currentPlayerName, currentScore);
                    gameState = STATE_LEVEL_COMPLETED;
                    if (isSoundEffectsEnabled) iPlaySound("music/level_complete.wav", false);
                    speedX = 0; vy = 0; return;
                }
            }
            if (speedX > 0) ballX = tile.x - ballWidth;
            else if (speedX < 0) ballX = tile.x + tile.width;
            speedX = 0;
        }
    }

    float score_interval = 20 * BASE_TILE_WIDTH;
    if (ballX > lastScoringPositionX + score_interval) {
        currentScore += 50;
        lastScoringPositionX = ballX;
    }

    // --- Vertical Collision ---
    onGround = false;
    ballY += vy;
    for (int i = 0; i < numActualTiles; ++i) {
        struct Tile tile = levelTiles[i];
        if (!tile.isSolid) continue;
        if (ballX + ballWidth > tile.x && ballX < tile.x + tile.width && ballY + ballHeight > tile.y && ballY < tile.y + tile.height) {
            if (tile.id == RED_FINISH_POINT) {
                int requiredRings = (int)ceil(totalRingsInLevel * 0.70);
                if (ringsCollected >= requiredRings) {
                    addScoreToLeaderboard(currentLevel, currentPlayerName, currentScore);
                    gameState = STATE_LEVEL_COMPLETED;
                    if (isSoundEffectsEnabled) iPlaySound("music/level_complete.wav", false);
                    speedX = 0; vy = 0; return;
                }
            }
            if (vy > 0) {
                ballY = tile.y - ballHeight;
                vy = -vy * 0.1; // Bounce off a ceiling
            } else {
                ballY = tile.y + tile.height;
                onGround = true;
                vy = -vy * 0.3; // Bounce off the floor
            }
        }
    }

    // --- Item & Hazard Collision ---
    for (int i = 0; i < numActualTiles; i++) {
        if (levelTiles[i].id == RING1 || levelTiles[i].id == RING2 || levelTiles[i].id == SPIKE1 || levelTiles[i].id == SPIKE2 || levelTiles[i].id == MOVING_ARROW) {
            if (ballX + ballWidth > levelTiles[i].x && ballX < levelTiles[i].x + levelTiles[i].width &&
                ballY + ballHeight > levelTiles[i].y && ballY < levelTiles[i].y + levelTiles[i].height) {
                
                if (levelTiles[i].id == RING1) {
                    levelTiles[i].id = FADED_RING1;
                    ringsCollected++;
                    currentScore += 100;
                    if (isSoundEffectsEnabled){ iPlaySound("music/ring_collect.wav", false); }
                }
                else if (levelTiles[i].id == RING2) {
                    levelTiles[i].id = FADED_RING2;
                    ringsCollected++;
                    currentScore += 100;
                    if (isSoundEffectsEnabled){ iPlaySound("music/ring_collect.wav", false); }
                }
                else if (levelTiles[i].id == SPIKE1 || levelTiles[i].id == SPIKE2 || levelTiles[i].id == MOVING_ARROW) {
                    if (isSoundEffectsEnabled){ iPlaySound("music/spikehit.wav", false); }
                    ballLives--;
                    blastX = ballX;
                    blastY = ballY;
                    blastTimerCounter = 0;
                    gameState = STATE_BALL_BLASTING;
                    return;
                }
            }
        }
    }

    // --- Boundary and Death Plane Checks ---
    float levelActualWidth = NUM_FLOOR_TILES * BASE_TILE_WIDTH;
    if (ballX < 0) { ballX = 0; if (speedX < 0) speedX = 0; }
    if (ballX + ballWidth > levelActualWidth) { ballX = levelActualWidth - ballWidth; if (speedX > 0) speedX = 0; }
    if (ballY < -100) {
        ballLives--;
        if (isSoundEffectsEnabled) iPlaySound("music/hurt.wav", false);
        if (ballLives > 0) { resetBallPosition(); }
        else {
            addScoreToLeaderboard(currentLevel, currentPlayerName, currentScore);
            gameState = STATE_GAME_OVER;
            iStopSound(musicChannel);
            if (isSoundEffectsEnabled) iPlaySound("music/game_over.wav", false);
        }
    }

    // --- Camera Control ---
    int worldWidthForCamera = (int)levelActualWidth;
    int maxCameraX = worldWidthForCamera - SCREEN_WIDTH;
    if (maxCameraX < 0) maxCameraX = 0;
    cameraX = (int)(ballX + ballWidth / 2.0f) - SCREEN_WIDTH / 3;
    if (cameraX < 0) cameraX = 0;
    if (cameraX > maxCameraX) cameraX = maxCameraX;

    const int LEVEL_HEIGHT_PIXELS = ROWS * BASE_TILE_HEIGHT;
    int maxCameraY = LEVEL_HEIGHT_PIXELS - SCREEN_HEIGHT;
    if (maxCameraY < 0) maxCameraY = 0;
    cameraY = (int)(ballY + ballHeight / 2.0f) - SCREEN_HEIGHT / 2;
    if (cameraY < 0) cameraY = 0;
    if (cameraY > maxCameraY) cameraY = maxCameraY;
}
void gameLoop() {
    // This function will now handle ALL game updates
    
    // 1. Update game elements like moving arrows
    if (gameState == STATE_PLAYING || gameState == STATE_BALL_BLASTING) {
        updateAllMovingArrows();
    }

    // 2. Update the ball and main game logic
    if (gameState == STATE_PLAYING) {
        updateBall();
    } 
    // 3. Handle the blast animation timer
    else if (gameState == STATE_BALL_BLASTING) {
        blastTimerCounter++;
        // 100 ticks * 10ms timer interval = 1000ms = 1 second
        if (blastTimerCounter >= 100) { 
            if (ballLives > 0) {
                resetBallPosition();
                gameState = STATE_PLAYING;
            } else {
                addScoreToLeaderboard(currentLevel, currentPlayerName, currentScore);
                gameState = STATE_GAME_OVER;
                if (isSoundEffectsEnabled) iPlaySound("music/game_over.wav", false);
            }
        }
    }
}

void iSpecialKeyboard(unsigned char key, int state)
{
}

// void updateGameElements() {
//     if (gameState == STATE_PLAYING || gameState == STATE_BALL_BLASTING) {
//         updateAllMovingArrows();
//     }
// }
int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    iInitializeSound();
    playerNameInput[0] = '\0';
    playerNameIndex = 0;
    currentPlayerName[0] = '\0';
    iLoadImage(&bgPlayingImage, "images/landingpage.png");
    iLoadImage(&bgMenuImage, "images/landingpage.png");
    loadBallImageNames();
    loadLeaderboard();
    iSetTimer(10, gameLoop);
    iOpenWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Adventure Ball");
    return 0;
}
