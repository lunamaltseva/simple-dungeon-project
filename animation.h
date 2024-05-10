#ifndef ANIMATION_H
#define ANIMATION_H

#include <cmath>
#include "raylib.h"
#include "globals.h"

void initMovement(Figure figure, size_t rowFrom, size_t columnFrom, size_t rowTo, size_t columnTo) {
    positionFrom = {static_cast<float>(columnFrom) * cellSize + shiftToCenterCellByX,
                    static_cast<float>(rowFrom) * cellSize + shiftToCenterCellByY};
    positionTo   = {static_cast<float>(columnTo) * cellSize + shiftToCenterCellByX,
                    static_cast<float>(rowTo) * cellSize + shiftToCenterCellByY};
    path = {positionTo.x-positionFrom.x, positionTo.y-positionFrom.y};
    if      (figure.figureType == Figure::PLAYER_TYPE) playAnimation = playerMoving;
    else if (figure.figureType == Figure::PAWN_TYPE)   playAnimation = pawnMoving;
    else if (figure.figureType == Figure::ROOK_TYPE)   playAnimation = rookMoving;
    else if (figure.figureType == Figure::QUEEN_TYPE)  playAnimation = queenMoving;
    initAnimation();
}

void initAnimation() {
    framesPerEntity = 60/returnFigureAmount(); //using GetFPS() here breaks the movement
    beginningFrame = gameFrame;
    gameState = ANIMATION_STATE;
}

void drawOtherFigures(Figure figure) {
    switch (figure.figureType) {
        case Figure::PLAYER_TYPE:
            if (ifEnemy(pawn)) drawFigure(pawn);
            if (ifEnemy(rook)) drawFigure(rook);
            if (ifEnemy(queen)) drawFigure(queen);
            break;
        case Figure::PAWN_TYPE:
            drawFigure(player);
            if (ifEnemy(rook)) drawFigure(rook);
            if (ifEnemy(queen)) drawFigure(queen);
            break;
        case Figure::ROOK_TYPE:
            drawFigure(player);
            if (ifEnemy(pawn)) drawFigure(pawn);
            if (ifEnemy(queen)) drawFigure(queen);
            break;
        case Figure::QUEEN_TYPE:
            drawFigure(player);
            if (ifEnemy(pawn)) drawFigure(pawn);
            if (ifEnemy(rook)) drawFigure(rook);
            break;
    }
}

void animateMovement(Figure &figure, int i) {
    /* Draw Elements On Screen */
    drawGUI();
    drawLevel();
    drawOtherFigures(figure);

    drawImage(figure.image,
              positionFrom.x + 0.5f*(path.x * (1.0f-cosf( static_cast<float>(i)/static_cast<float>(framesPerEntity)*PI ))),
              positionFrom.y + 0.5f*(path.y * (1.0f-cosf( static_cast<float>(i)/static_cast<float>(framesPerEntity)*PI ))) - cellSize * 0.3125f - (figure.shouldJump ? ((1.0f-cosf(static_cast<float>(i)/static_cast<float>(framesPerEntity)*2.0f*PI))*0.5f)*cellSize*0.4f : 0.0f),
              cellSize);
}

void animateFalling(Figure &figure, int i) {
    drawImage(figure.image,
              static_cast<float>(figure.column) * cellSize + shiftToCenterCellByX,
              static_cast<float>(figure.row)    * cellSize + shiftToCenterCellByY - cellSize * 0.3125f + powf(static_cast<float>(i)/60.0f, 2.0f)*cellSize*9.8f,
              cellSize);

    /* Draw Elements On Screen */
    drawLevel();
    drawOtherFigures(figure);
    drawGUI();
}

void endAnimation(int i) {
    // Fade out, then black out
    if (i < 90) {
        fade(i, static_cast<float>(animationDuration)/45.0f);
    } else DrawRectangle(0, 0, static_cast<int>(screenWidth), static_cast<int>(screenHeight), {0, 0, 0, 255});

    // Restart the Level
    if (i > animationDuration) {
        restartLevel();
    }
}

void endAnimation(Enemy &enemy, int i) {
    // Purge the enemy
    if (i > animationDuration) {
        killEnemy(enemy);
        enemy.row = enemy.column = 0;
        moveSequence[enemy.figureType] = NONE;
        gameState = GAME_STATE;
    }
}

void drawAnimation() {
    int i = static_cast<int>(static_cast<float>(gameFrame - beginningFrame));

    switch (playAnimation) {
        case fromMenu:
        {
            const float titleSize = MENU_TITLE_SIZE * screenScale;
            const float figureSize = titleSize * 0.2f;
            const float amplitude = 15.0f * screenScale;
            float figureOffsetY = amplitude*2.0f*sinf(static_cast<float>(gameFrame%70)*(PI/35.0f));

            Vector2 titlePosition = {(screenWidth-titleSize)*0.5f, 0};
            Vector2 figurePosition = {titlePosition.x+(titleSize-figureSize)*0.5f, titlePosition.y+(titleSize-figureSize)*0.2f};

            drawImage(playerImage, figurePosition.x, figurePosition.y + figureOffsetY - amplitude*2.0f + pow(i/60.0f, 2)*cellSize*9.8f, figureSize);
        }

            if (i < 50) fade(i, static_cast<float>(animationDuration)/25.0f);
            else        DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 255} );

            if (i > 60) loadNextLevel(0);

            break;

        case playerMoving:
            if (level.moves <= 5) drawLevelHalo(i);
            animateMovement(player, i);
            if (i >= framesPerEntity - 1) playerCompleteMovement();
            break;

        case pawnMoving:
            animateMovement(pawn, i);
            if (i >= framesPerEntity - 1) enemyCompleteMovement(pawn);
            break;

        case rookMoving:
            animateMovement(rook, i);
            if (i >= framesPerEntity - 1) enemyCompleteMovement(rook);
            break;

        case queenMoving:
            animateMovement(queen, i);
            if (i >= framesPerEntity - 1) enemyCompleteMovement(queen);
            break;

        case playerOutOfMoves:
            shiftToCenterCellByY += powf(static_cast<float>(i)/60.0f, 2.0f)*cellSize*0.3f;

            drawLevel();
            drawFigures();
            drawGUI();

            endAnimation(i);
            break;

        case playerFalling:
            animateFalling(player, i);
            endAnimation(i);
            break;

        case pawnFalling:
            animateFalling(pawn, i);
            endAnimation(pawn, i);
            if (player.isKilled) endAnimation(i);
            break;

        case rookFalling:
            animateFalling(rook, i);
            endAnimation(rook, i);
            if (player.isKilled) endAnimation(i);
            break;

        case queenFalling:
            animateFalling(queen, i);
            endAnimation(queen, i);
            if (player.isKilled) endAnimation(i);
            break;

        case playerKilled:
            drawLevel();
            drawGUI();
            drawFigures();

            endAnimation(i);
            break;

        case levelStarting:
            // No flashing lights
            i = i == 0 ? 1 : i;

            drawLevel();
            drawGUI();
            drawEnemies();

            fade(i, 0, 120, -(static_cast<float>(animationDuration)/60.0f), 256);

            drawImage(player.image,
                      static_cast<float>(player.column) * cellSize + shiftToCenterCellByX,
                      (static_cast<float>(player.row) * cellSize + shiftToCenterCellByY - cellSize * 0.3125f) - static_cast<float>(animationDuration-i)*(screenHeight*0.025f),
                      cellSize);

            if (i >= animationDuration) completeLevelDrawing();
            break;

        case levelEnding:
            i = i == 0 ? 1 : i;

            drawLevel();
            drawGUI();
            drawEnemies();

            if (i <= 32)
                drawImage(player.image,
                          static_cast<float>(player.column) * cellSize + shiftToCenterCellByX,
                          (static_cast<float>(player.row) * cellSize + shiftToCenterCellByY - cellSize * 0.3125f) - static_cast<float>((sinf(static_cast<float>(i)/60.0f*2.0f*PI)*0.5f)*cellSize),
                          cellSize);

            fade(i, 0, 120, (static_cast<float>(animationDuration)/60.0f), 0);

            if (i >= animationDuration) loadNextLevel(1);
            break;

        case gameOver:
            drawAnimatedText(gameOverTitle);
            if (gameOverTitle.textIndex >= gameOverTitle.entries.size()) {
                gameOverTitle.textIndex = 0;
                gameState = GAME_OVER_STATE;
            }
            break;

        case threat:
            drawImage(queenImage, (screenWidth - std::min(screenHeight, screenWidth) * 0.1f)*0.5f, (screenHeight - std::min(screenHeight, screenWidth) * 0.1f)*0.5f, std::min(screenHeight, screenWidth) * 0.1f);

            if (i == 5) PlaySound(queenSpeakingSound);
            drawAnimatedText(endingThreat);
            if (endingThreat.textIndex >= endingThreat.entries.size()) {
                endingThreat.textIndex = 0;
                gameState = GAME_STATE;
            }
            break;

        case monologue:
            drawImage(queenImage, (screenWidth - std::min(screenHeight, screenWidth) * 0.1f)*0.5f, (screenHeight - std::min(screenHeight, screenWidth) * 0.1f)*0.5f, std::min(screenHeight, screenWidth) * 0.1f);

            if (i == 5) PlaySound(queenSpeakingSound);
            drawAnimatedText(endingMonologue);
            if (endingMonologue.textIndex >= endingMonologue.entries.size()) {
                positionFrom = {static_cast<float>(queen.column) * cellSize + shiftToCenterCellByX,
                                static_cast<float>(queen.row) * cellSize + shiftToCenterCellByY};
                positionTo   = {-cellSize, screenHeight};
                path = {positionTo.x-positionFrom.x, positionTo.y-positionFrom.y};
                endingMonologue.textIndex = 0;
                playAnimation = finalScene;
                initAnimation();
            }
            break;

        case finalScene:
            if (i <= 30) animateMovement(queen, i);
            else {
                drawLevel();
                drawFigure(player);
                drawGUI();
            }
            if (i >= 60) drawImage(fractureImage, 0, 0, screenWidth, screenHeight);
            if (i >= 120) drawImage(fractureMoreImage, 0, 0, screenWidth, screenHeight);
            if (i >= 150) DrawRectangle(0, 0, static_cast<int>(screenWidth), static_cast<int>(screenHeight), {0, 0, 0, 255});

            switch(i) {
                case 30:
                    PlaySound(clockShatterSound);
                    PlaySound(clockHissSound);
                    PlaySound(levelDismantleSound);
                    break;
                case 60:
                    PlaySound(enemyBreakSound);
                    PlaySound(playerBreakSound);
                    PlaySound(levelBreakSound);
                    break;
                case 120:
                    PlaySound(enemyBreakSound);
                    PlaySound(playerBreakSound);
                    break;
                case 150:
                    PlaySound(playerDeathSound);
                    break;
            }

            if (i >= 450) gameState = VICTORY_STATE;

            break;
    }
}

#endif //ANIMATION_H
