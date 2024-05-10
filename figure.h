#ifndef FIGURE_H
#define FIGURE_H

#include <cstdlib>
#include <cmath>
#include "globals.h"

/* General */

int returnFigureAmount() {
    int count = 1;

    if(ifEnemy(pawn) && ifEnemyAlive(pawn)) count++;
    if(ifEnemy(rook) && ifEnemyAlive(rook)) count++;
    if(ifEnemy(queen) && ifEnemyAlive(queen)) count++;

    return count;
}

float simplifyNumber(float num) {
    if (num > 0.0f)      return  1.0f;
    else if (num < 0.0f) return -1.0f;
    else                 return  0.0f;
}

bool canMove(Figure figure) {
    if (moveSequence[figure.figureType] == WAIT) return true;
    else                                         return false;
}

void initMoveSequence() {
    moveSequence[player.figureType]                                           = WAIT;
    if (ifEnemy(pawn) && ifEnemyAlive(pawn))   moveSequence[pawn.figureType]  = WAIT;
    else                                       moveSequence[pawn.figureType]  = NONE;
    if (ifEnemy(rook) && ifEnemyAlive(rook))   moveSequence[rook.figureType]  = WAIT;
    else                                       moveSequence[rook.figureType]  = NONE;
    if (ifEnemy(queen) && ifEnemyAlive(queen)) moveSequence[queen.figureType] = WAIT;
    else                                       moveSequence[queen.figureType] = NONE;
}

void controlMoveSequence() {
    if (!canMove(player)) {
        if (moveSequence[pawn.figureType] == WAIT) {
            moveEnemy(pawn);
            moveSequence[pawn.figureType] = DONE;
        }
        else if (moveSequence[rook.figureType] == WAIT) {
            moveEnemy(rook);
            moveSequence[rook.figureType] = DONE;
        }
        else if (moveSequence[queen.figureType] == WAIT) {
            moveEnemy(queen);
            moveSequence[queen.figureType] = DONE;
        }
        else
            initMoveSequence();
    }
}

/* Player */

void movePlayer(int dx, int dy) {
    if (player.cell == QUICKTILE) {
        level.data[player.row*level.columns+player.column] = WALL;
        PlaySound(rand()%2 == 0 ? quicktileBreakSound0 : quicktileBreakSound1);
        player.shouldJump = true;
    }

    int nextPlayerColumn = static_cast<int>(player.column) + dx;
    int nextPlayerRow = static_cast<int>(player.row)       + dy;

    player.cell = level.data[nextPlayerRow * level.columns + nextPlayerColumn];

    if(ifEnemyInCell(nextPlayerRow, nextPlayerColumn)) {
        labelKillEnemy(nextPlayerRow, nextPlayerColumn);
        nextPlayerColumn += dx;
        nextPlayerRow += dy;
    }

    initMovement(player, player.row, player.column, nextPlayerRow, nextPlayerColumn);
    player.row = static_cast<size_t>(nextPlayerRow);
    player.column = static_cast<size_t>(nextPlayerColumn);
    level.moves--;

    if (level.moves % 5 == 0) {
        if      (level.moves > 15)  PlaySound(rand()%2 == 0 ? clockTickSound0 : clockTickSound1);
        else if (level.moves == 15) PlaySound(clockHighSound);
        else if (level.moves == 10) PlaySound(clockNormalSound);
    }

    if (level.moves <= 5 )
        PlaySound(clockLowSound);
}

void playerCompleteMovement() {
    player.shouldJump = false;

    /* Trigger ending? */
    if (levelIndex == 4 && isExitOpen) {
        player.position = static_cast<int>(player.row * level.columns + player.column);

        if ((level.data[player.position + 1] == EXIT ||
            level.data[player.position - 1] == EXIT ||
            level.data[player.position - level.columns] == EXIT ||
            level.data[player.position + level.columns] == EXIT) &&
            level.data[queen.row*level.columns+queen.column] != EXIT) {
            playAnimation = threat;
            initAnimation();
            return;
        }

        if (level.data[player.position] == EXIT) {
            playAnimation = monologue;
            initAnimation();
            return;
        }
    }

    gameState = GAME_STATE;

    player.cell = level.data[player.row * level.columns + player.column];

    if (ifEnemy(pawn)) {
        if (pawn.isToBeKilled) killEnemy(pawn);
    }

    if (ifEnemy(rook)) {
        if (rook.isToBeKilled) killEnemy(rook);
    }

    if (ifEnemy(queen)) {
        if (queen.isToBeKilled) killEnemy(queen);
    }

    switch (player.cell) {
        case WALL:
            PlaySound(figureFallSound);
            playAnimation = playerFalling;
            initAnimation();
            break;
        case KEY:
            PlaySound(pickupKeySound);
            player.keys++;
            level.data[player.row * level.columns + player.column] = FLOOR;
            if (player.keys == level.keys) {
                isExitOpen = true;
                PlaySound(exitOpenSound);
            }
            break;
        case MARBLE:
            PlaySound(marbleTopupSound);
            level.moves += (LEVELS[levelIndex].moves/2);
            level.moves = (level.moves > LEVELS[levelIndex].moves ?  LEVELS[levelIndex].moves : level.moves);
            level.data[player.row * level.columns + player.column] = FLOOR;
            break;
        case EXIT:
            if (isExitOpen) {
                playAnimation = levelEnding;
                initAnimation();
                PlaySound(puzzleCompleteSound);
                return;
            }
            break;
    }

    if (level.moves == 0) {
        playAnimation = playerOutOfMoves;
        PlaySound(levelBreakSound);
        initAnimation();
        return;
    }

    moveSequence[player.figureType] = DONE;
}

/* Enemies */

bool ifEnemy(Enemy enemy) {
    if (enemy.row != 0 && enemy.column != 0) return true;
    else return false;
}

bool ifEnemyAlive(Enemy enemy) {
    if (!enemy.isKilled) return true;
    else return false;
}

bool ifEnemyInCell(size_t row, size_t column) {
    if ((pawn.row == row && pawn.column == column && !pawn.isKilled) ||
        (rook.row == row && rook.column == column && !rook.isKilled) ||
        (queen.row == row && queen.column == column && !queen.isKilled)) return true;
    else return false;
}

void labelKillEnemy(size_t row, size_t column) {
    if (pawn.row == row && pawn.column == column) pawn.isToBeKilled = true;
    else if (rook.row == row && rook.column == column) rook.isToBeKilled = true;
    else if (queen.row == row && queen.column == column) queen.isToBeKilled = true;
}

void killEnemy(Enemy &enemy) {
    PlaySound(enemyBreakSound);
    enemy.isToBeKilled = false;
    enemy.isKilled = true;
    enemy.image = enemyBroken;
    moveSequence[enemy.figureType] = NONE;
}

bool enemyCanBeKilled(Enemy enemy, int dx, int dy) {
    player.position = static_cast<int>(player.row * level.columns + player.column);
    enemy.position = static_cast<int>((enemy.row + dx) * level.columns + (enemy.column + dy));

    if (enemy.position == player.position + 1 ||
        enemy.position == player.position - 1 ||
        enemy.position == player.position - level.columns ||
        enemy.position == player.position + level.columns) {
        if ((level.data[enemy.position + 1] == WALL && enemy.position - 1 == player.position) ||
            (level.data[enemy.position - 1] == WALL && enemy.position + 1 == player.position) ||
            (level.data[enemy.position + level.columns] == WALL && enemy.position - level.columns == player.position) ||
            (level.data[enemy.position - level.columns] == WALL && enemy.position + level.columns == player.position))
            return false;
        else return true;
    }
    else return false;
}

bool isViableSpot(Enemy enemy, int dx, int dy) {
    enemy.position = static_cast<int>((enemy.row + dx) * level.columns + (enemy.column + dy));
    if (enemy.isSuicidal) {
        if (level.data[enemy.position] != WALL) return true;
        else return false;
    }
    else {
        if (enemyCanBeKilled(enemy, dx, dy)) {
            return false;
        }
        else {
            if (level.data[enemy.position] != WALL) {
                return true;
            }
            else {
                return false;
            }
        }
    }
}

bool isWall(Enemy enemy, int dx, int dy) {
    enemy.position = static_cast<int>((enemy.row + dx) * level.columns + (enemy.column + dy));
    if (level.data[enemy.position] == WALL) return true;
    else                                    return false;
}

float potentialDistance (Enemy enemy, int dx, int dy) {
    player.location = {static_cast<float>(player.column), static_cast<float>(player.row)};
    enemy.location = {static_cast<float>(enemy.column+dy), static_cast<float>(enemy.row+dx)};
    enemy.distance = static_cast<float>(sqrt( pow(enemy.location.x - player.location.x, 2) + pow(enemy.location.y - player.location.y, 2) ));

    return enemy.distance;
}

void moveToCloser(Enemy enemy, float &min, int dx, int dy, Vector2 &move) {
    if (!isViableSpot(enemy, dx, dy)) return;
    else {
        float distance = potentialDistance(enemy, dx, dy);
        if (distance < min) {
            min = distance;
            move = {static_cast<float>(dx), static_cast<float>(dy)};
        }
    }
}

void moveToFurther(Enemy enemy, int dx, int dy, Vector2 &move) {
    if (!isViableSpot(enemy, dx, dy)) return;
    move = {static_cast<float>(dx), static_cast<float>(dy)};
}

void movePawn(Enemy &enemy, float min) {
    moveToCloser(enemy, min, 0, -1, enemy.move);
    moveToCloser(enemy, min, -1, 0, enemy.move);
    moveToCloser(enemy, min, 1, 0, enemy.move);
    moveToCloser(enemy, min, 0, 1, enemy.move);
}

void analyzeBranch(Enemy &enemy, int dx, int dy, float &min) {
    for (int i = 1; i < std::max(level.columns, level.rows); i++) {
        if (isWall(enemy, dx * i, dy * i)) break;
        moveToCloser(enemy, min, dx * i, dy * i, enemy.move);
    }
}

void moveRook(Enemy &enemy, float min) {
    analyzeBranch(enemy,  0,  -1, min);
    analyzeBranch(enemy, -1,   0,  min);
    analyzeBranch(enemy,  1,   0,  min);
    analyzeBranch(enemy,  0,   1,  min);
}

void moveQueen(Enemy &enemy, float min) {
    analyzeBranch(enemy,  0,   1,  min);
    analyzeBranch(enemy,  1,   1,  min);
    analyzeBranch(enemy,  1,   0,  min);
    analyzeBranch(enemy,  1,  -1,  min);
    analyzeBranch(enemy,  0,  -1,  min);
    analyzeBranch(enemy, -1,  -1,  min);
    analyzeBranch(enemy, -1,   0,  min);
    analyzeBranch(enemy, -1,   1,  min);
}

void labelKillFigure (Enemy &target, size_t row, size_t column) {
    if (ifEnemy(target) && ifEnemyAlive(target))
        if (row == target.row && column == target.column)
            target.isToBeKilled = true;
}

void checkIfKillFigure (Enemy &enemy, size_t row, size_t column) {
    if (row == player.row && column == player.column) {
        player.isToBeKilled = true;
    }

    switch(enemy.figureType) {
        case Figure::PAWN_TYPE:
            labelKillFigure(rook, row, column);
            labelKillFigure(queen, row, column);
            break;
        case Figure::ROOK_TYPE:
            labelKillFigure(pawn, row, column);
            labelKillFigure(queen, row, column);
            break;
        case Figure::QUEEN_TYPE:
            labelKillFigure(pawn, row, column);
            labelKillFigure(rook, row, column);
            break;
    }
}

void pathKillFigures (Enemy &enemy) {
    for (int i = 1; i <= static_cast<int>(std::max(fabsf(enemy.move.x), fabsf(enemy.move.y))); i++) {
        checkIfKillFigure(enemy, enemy.row + static_cast<size_t>(simplifyNumber(enemy.move.x))*i, enemy.column + static_cast<size_t>(simplifyNumber(enemy.move.y))*i);
    }
    if (player.isToBeKilled || pawn.isToBeKilled || rook.isToBeKilled || queen.isToBeKilled) {
        enemy.move.x += simplifyNumber(enemy.move.x);
        enemy.move.y += simplifyNumber(enemy.move.y);
    }
}

void moveEnemy(Enemy &enemy) {
    if (enemy.cell == QUICKTILE) {
        level.data[enemy.row*level.columns+enemy.column] = WALL;
        PlaySound(rand()%2 == 0 ? quicktileBreakSound0 : quicktileBreakSound1);
        enemy.shouldJump = true;
    }

    enemy.location = {static_cast<float>(enemy.column), static_cast<float>(enemy.row)};
    player.location = {static_cast<float>(player.column), static_cast<float>(player.row)};
    enemy.distance = static_cast<float>(sqrt( pow(enemy.location.x - player.location.x, 2) + pow(enemy.location.y - player.location.y, 2) ));
    float min = enemy.distance;
    enemy.move = {0, 0};

    if (enemy.figureType == Figure::PAWN_TYPE) {
        movePawn(enemy, min);
    }

    else if (enemy.figureType == Figure::ROOK_TYPE) {
        moveRook(enemy, min);
    }

    else if (enemy.figureType == Figure::QUEEN_TYPE) {
        moveQueen(enemy, min);
    }

    if (enemy.move.x == 0 && enemy.move.y == 0) {
        moveToFurther(enemy, -1,  0, enemy.move);
        moveToFurther(enemy,  1,  0, enemy.move);
        moveToFurther(enemy,  0,  1, enemy.move);
        moveToFurther(enemy,  0, -1, enemy.move);
    }

    if (enemy.move.x == 0 && enemy.move.y == 0) {
        enemy.isToBeKilled = true;
    }

    pathKillFigures(enemy);

    initMovement(enemy, enemy.row, enemy.column, enemy.row + static_cast<size_t>(enemy.move.x), enemy.column + static_cast<size_t>(enemy.move.y));
    enemy.row    = enemy.row    + static_cast<size_t>(enemy.move.x);
    enemy.column = enemy.column + static_cast<size_t>(enemy.move.y);
}

void enemyCompleteMovement(Enemy &enemy) {
    enemy.shouldJump = false;
    enemy.cell = level.data[enemy.row * level.columns + enemy.column];
    gameState = GAME_STATE;

    if (pawn.isToBeKilled) killEnemy(pawn);
    if (rook.isToBeKilled) killEnemy(rook);
    if (queen.isToBeKilled) killEnemy(rook);

    if (player.isToBeKilled) {
        player.isToBeKilled = false;
        player.isKilled = true;
        player.image = playerBroken;
        PlaySound(playerBreakSound);
        playAnimation = playerKilled;
        initAnimation();
    }

    if (enemy.cell == WALL) {
        switch (enemy.figureType) {
            case Figure::PAWN_TYPE:  playAnimation = pawnFalling;  break;
            case Figure::ROOK_TYPE:  playAnimation = rookFalling;  break;
            case Figure::QUEEN_TYPE: playAnimation = queenFalling; break;
        }
        PlaySound(figureFallSound);
        initAnimation();
    }
}

#endif //FIGURE_H
