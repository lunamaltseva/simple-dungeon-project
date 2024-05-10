#ifndef LEVELS_H
#define LEVELS_H

#include "globals.h"

// Make the menu not break
void initGraphics() {
    createLevelInstance();
    deriveGraphicsMetricsFromLoadedLevel();
}

// Pre-generation
void initLevel() {
    // Return to default values
    player.image = playerImage;
    player.isToBeKilled = false;
    player.isKilled = false;
    player.keys = 0;

    // Avoid quicktile bug
    player.cell = ' ';
    pawn.cell = ' ';
    rook.cell = ' ';
    queen.cell = ' ';

    // For control move sequence
    player.figureType = Figure::PLAYER_TYPE;
    pawn.figureType = Figure::PAWN_TYPE;
    rook.figureType = Figure::ROOK_TYPE;
    queen.figureType = Figure::QUEEN_TYPE;

    // Purge from level
    pawn.row = pawn.column = 0;
    rook.row = rook.column = 0;
    queen.row = queen.column = 0;

    isExitOpen = false;
}

void placePlayer() {
    for (size_t row = 0; row < level.rows; row++) {
        for (size_t column = 0; column < level.columns; column++) {
            char cell = level.data[row*level.columns + column];
            if (cell == ENTRANCE) {
                player.row = row;
                player.column = column;
            }
        }
    }
}

bool scanForEnemies() {
    for (size_t row = 0; row < level.rows; row++) {
        for (size_t column = 0; column < level.columns; column++) {
            char cell = level.data[row*level.columns + column];
            if (cell == PAWN || cell == ROOK || cell == QUEEN) {
                return true;
            }
        }
    }
    return false;
}

void placeEnemy() {
    if (!scanForEnemies()) {
        return;
    }
    else {
        for (size_t row = 0; row < level.rows; row++) {
            for (size_t column = 0; column < level.columns; column++) {
                char cell = level.data[row*level.columns + column];

                if (cell == PAWN) {
                    pawn.row = row;
                    pawn.column = column;
                    pawn.isKilled = false;
                    pawn.image = pawnImage;
                    continue;
                }

                if (cell == ROOK) {
                    rook.row = row;
                    rook.column = column;
                    rook.isKilled = false;
                    rook.image = rookImage;
                    continue;
                }

                if (cell == QUEEN) {
                    queen.row = row;
                    queen.column = column;
                    queen.isKilled = false;
                    queen.image = queenImage;
                    continue;
                }
            }
        }
    }
}

void loadLevelConfig() {
    switch(levelIndex + 1) {
        case 2:
        case 4:
            pawn.isSuicidal = true;
            break;
    }
}


// Generation
void createLevelInstance() {
    delete[] loadLevel;
    loadLevel = new char[LEVELS[levelIndex].rows*LEVELS[levelIndex].columns];
    for(int i = 0; i < LEVELS[levelIndex].rows*LEVELS[levelIndex].columns; i++)
        loadLevel[i] = LEVELS[levelIndex].data[i];

    level = { LEVELS[levelIndex].rows, LEVELS[levelIndex].columns, LEVELS[levelIndex].moves, LEVELS[levelIndex].keys, loadLevel };
}

void loadNextLevel(int jump) {
    levelIndex+=jump;
    initLevel();

    // Is game over?
    if (player.lives == 0) {
        PlaySound(playerDeathSound);
        playAnimation = gameOver;
        initAnimation();

        levelIndex = 0;
        player.lives = player.livesMax;

        return;
    }

    createLevelInstance();

    placePlayer();
    placeEnemy();
    initMoveSequence();

    loadLevelConfig();

    deriveGraphicsMetricsFromLoadedLevel();

    playAnimation = levelStarting;
    initAnimation();
}

void completeLevelDrawing() {
    PlaySound(playerLandedSound);
    gameState = GAME_STATE;
}

void listLevelElements(bool array[], size_t size, size_t checkLevelIndex) {
    for (int i = 0; i < size; i++)
        array[i] = false;

    for (size_t row = 0; row < LEVELS[checkLevelIndex].rows; row++) {
        for (size_t column = 0; column < LEVELS[checkLevelIndex].columns; column++) {
            char cell = LEVELS[checkLevelIndex].data[row*LEVELS[checkLevelIndex].columns + column];
            if      (cell == KEY)       array[keyElement]       = true;
            else if (cell == MARBLE)    array[marbleElement]    = true;
            else if (cell == QUICKTILE) array[quicktileElement] = true;
            else if (cell == PAWN)      array[pawnElement]      = true;
            else if (cell == ROOK)      array[rookElement]      = true;
            else if (cell == QUEEN)     array[queenElement]     = true;
        }
    }
}

void restartLevel() {
    player.lives--;
    loadNextLevel(0);
}

#endif // LEVELS_H
