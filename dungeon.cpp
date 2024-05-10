/*
 *
 * Files:
 *      dungeon.cpp  -  file with the main loop
 *      globals.h    -  declaration of all functions, classes, and other data types with thorough comments
 *      levels.h     -  handles level interpretation
 *      figure.h     -  controls the player and the enemy figures
 *      drawing.h    -  commonly used functions for drawing
 *      animation.h  -  handles everything related to animation
 *      ui_ux.h      -  GUI, menus, and screens are handled by this file
 *      assets.h     -  fonts, images, and sounds
 *
 */

#include "raylib.h"

// Forward Declarations
#include "globals.h"

// Gameplay
#include "levels.h"
#include "figure.h"

// Visuals
#include "drawing.h"
#include "animation.h"
#include "ui_ux.h"
#include "assets.h"

void updateGame() {
    controlMusic();
    controlEsc();
    controlMenus();

    switch(gameState) {
        case INTRO_STATE:                                                     // avoid accidental skips
            if((isAffirmativeButtonPressed() || isNegativeButtonPressed()) && !IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) toMainMenu();
            break;

        case MENU_STATE:
            interpretSelection(mainMenu);
            break;

        case CHOOSE_LEVEL_STATE:
            interpretSelection(chooseLevelMenu);
            break;

        case TUTORIAL_STATE:
            if (isAffirmativeButtonPressed() || isNegativeButtonPressed()) {
                gameState = MENU_STATE;
                PlaySound(backOutSound);
            }
            break;

        case GAME_STATE:
            if (canMove(player)) {
                if      (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)   ) { movePlayer( 0, -1); }
                else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) ) { movePlayer(-1,  0); }
                else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN) ) { movePlayer( 0,  1); }
                else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) { movePlayer( 1,  0); }
            }
            if (isNegativeButtonPressed()) {
                PlaySound(backOutSound);
                gameState = PAUSED_STATE;
                exitFromAnimation = false;
            }
            controlMoveSequence();
            break;

        case PAUSED_STATE:
            interpretSelection(pauseMenu);
            break;

        case ANIMATION_STATE:
            if (isNegativeButtonPressed()) {
                PlaySound(backOutSound);
                gameState = PAUSED_STATE;
                exitFromAnimation = true;
            }
            break;

        case GAME_OVER_STATE:
            interpretSelection(gameOverMenu);
            break;

        case VICTORY_STATE:
            interpretSelection(victoryMenu);
            break;
    }
}

void drawGame() {
    ClearBackground(BLACK);

    switch(gameState) {
        case INTRO_STATE:
            drawIntro();
            break;

        case MENU_STATE:
            gameFrame++;
            drawMainMenu();
            drawMenu(mainMenu);
            fade(static_cast<int>(gameFrame), 0, 60, -static_cast<float>(animationDuration) / 30.0f, 256);
            break;

        case CHOOSE_LEVEL_STATE:
            gameFrame++;
            drawMainMenu();
            drawChooseLevelMenu();
            break;

        case TUTORIAL_STATE:
            gameFrame++;
            drawTutorial();
            break;

        case GAME_STATE:
            gameFrame++;
            /* Draw the board */
            drawLevel();
            /* Draw the figures */
            if (ifEnemy(pawn)) drawFigure(pawn);
            if (ifEnemy(rook)) drawFigure(rook);
            if (ifEnemy(queen)) drawFigure(queen);
            drawFigure(player);
            /* Draw the GUI */
            drawGUI();
            break;

        case ANIMATION_STATE:
            gameFrame++;
            drawAnimation();
            break;

        case PAUSED_STATE:
            drawPauseMenu();
            break;

        case VICTORY_STATE:
            drawVictoryMenu();
            break;

        case GAME_OVER_STATE:
            drawGameOverMenu();
            break;
    }
}

int main() {                       // Since we are dealing with pixel arts, antialiasing is actually detrimental
    SetConfigFlags(FLAG_VSYNC_HINT /*| FLAG_MSAA_4X_HINT*/);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(1), "Dungeon");
    SetTargetFPS(60);
    ToggleFullscreen();
    HideCursor();

    InitAudioDevice();
    initGraphics();

    loadFonts();
    loadImages();
    loadSounds();

    while (!WindowShouldClose()) {
        BeginDrawing();

        updateGame();
        drawGame();

        EndDrawing();
    }

    unloadFonts();
    unloadImages();
    unloadSounds();

    delete[] loadLevel;
    CloseAudioDevice();
    CloseWindow();

    return 0;
}