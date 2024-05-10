#ifndef UI_UX_H
#define UI_UX_H

#include "globals.h"
#include <cmath>

void controlMenu(Menu &menu) {
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_D) || GetMouseWheelMove() == -1) {
        menu.entrySelected++;
        PlaySound(scrollSound);
    }
    else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_A) || GetMouseWheelMove() == 1) {
        menu.entrySelected--;
        PlaySound(scrollSound);
    }
    if (menu.entrySelected+1 > menu.entries.size()) menu.entrySelected = 0;
    if (menu.entrySelected+1 <= 0) menu.entrySelected = menu.entries.size()-1;
}

void toMainMenu() {
    gameState = MENU_STATE;
    gameFrame = 0;
}

void fromMainMenu() {
    levelIndex = startFromLevel;
    playAnimation = fromMenu;
    PlaySound(menuExitSound);
    initAnimation();
}

bool isAffirmativeButtonPressed() {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_E) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return true;
    else return false;
}

bool isNegativeButtonPressed() {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_Q)) return true;
    else return false;
}

void controlMusic() {
    switch (gameState) {
        case INTRO_STATE:
            if (imageIteration < 4) {
                PlayMusicStream(introTheme);
                UpdateMusicStream(introTheme);
            }
            else
                StopMusicStream(introTheme);
            break;

        case MENU_STATE: case CHOOSE_LEVEL_STATE: case TUTORIAL_STATE:
            PlayMusicStream(mainTheme);
            UpdateMusicStream(mainTheme);
            break;
        case GAME_STATE:
            StopMusicStream(mainTheme);
            break;
        case ANIMATION_STATE:
            if ((playAnimation == monologue || playAnimation == threat) && endingMonologue.textIndex < endingMonologue.entries.size()-1) {
                PlayMusicStream(endingTheme);
                UpdateMusicStream(endingTheme);
            }
            break;
    }
}

void controlEsc() {
    switch (gameState) {
        case MENU_STATE: SetExitKey(KEY_ESCAPE); break;
        default:         SetExitKey(0);          break;
    }
}

void controlMenus() {
    switch (gameState) {
        case MENU_STATE:         controlMenu(mainMenu);        break;
        case CHOOSE_LEVEL_STATE: controlMenu(chooseLevelMenu); break;
        case PAUSED_STATE:       controlMenu(pauseMenu);       break;
        case GAME_OVER_STATE:    controlMenu(gameOverMenu);    break;
        case VICTORY_STATE:      controlMenu(victoryMenu);     break;
    }
}

void interpretSelection(Menu &menu) {
    switch (menu.type) {
        case Menu::MAIN_MENU_TYPE:
            if(isAffirmativeButtonPressed()) {
                switch (mainMenu.entrySelected) {
                    case 0:
                        startFromLevel = 0;
                        fromMainMenu();
                        break;

                    case 1: gameState = CHOOSE_LEVEL_STATE; break;
                    case 2: gameState = TUTORIAL_STATE;     break;
                    case 3: CloseWindow();                  break;
                }
                PlaySound(clickSound);
            }
            break;

        case Menu::CHOOSE_LEVEL_TYPE:
            if (isNegativeButtonPressed()) {
                gameState = MENU_STATE;
                PlaySound(backOutSound);
            }
            if (isAffirmativeButtonPressed()) {
                startFromLevel = chooseLevelMenu.entrySelected;
                fromMainMenu();

                mainMenu.entrySelected = chooseLevelMenu.entrySelected = 0;
                PlaySound(clickSound);
            }
            break;

        case Menu::PAUSE_TYPE:
            if (isNegativeButtonPressed()) {
                PlaySound(clickSound);
                if (exitFromAnimation) gameState = ANIMATION_STATE;
                else                   gameState = GAME_STATE;
            }

            if (isAffirmativeButtonPressed())
                switch (pauseMenu.entrySelected) {
                    case 0:
                        if (exitFromAnimation) gameState = ANIMATION_STATE;
                        else                   gameState = GAME_STATE;
                        break;

                    case 1:
                        switch (playAnimation)
                            case playerFalling: case playerKilled: case playerOutOfMoves:
                                player.lives--;

                        loadNextLevel(0);
                        PlaySound(clickSound);
                        break;
                    case 2:
                        toMainMenu();
                        pauseMenu.entrySelected = 0;
                        player.lives = player.livesMax;
                        PlaySound(backOutSound);
                        break;
                    case 3:
                        CloseWindow();
                        break;
                }
            break;
        case Menu::GAME_OVER_TYPE:
            if (isAffirmativeButtonPressed()) {
                switch (gameOverMenu.entrySelected) {
                    case 0:
                        levelIndex = startFromLevel;
                        loadNextLevel(0);
                        PlaySound(clickSound);
                        break;
                    case 1:
                        toMainMenu();
                        gameOverMenu.entrySelected = 0;
                        PlaySound(backOutSound);
                        break;
                    case 2:
                        CloseWindow();
                        break;
                }
            }
            break;
        case Menu::VICTORY_TYPE:
            if (isNegativeButtonPressed()) toMainMenu();

            if (isAffirmativeButtonPressed()) {
                switch (victoryMenu.entrySelected) {
                    case 0:
                        toMainMenu();
                        break;
                    case 1:
                        CloseWindow();
                        break;
                }
            }
            break;
    }
}

void drawGUI() {
    /* display the amount of moves */
    moveCountCritical.line = moveCountNormal.line = std::to_string(level.moves);
    drawText( level.moves > 5 ? moveCountNormal : moveCountCritical, moveCountNormal.size*screenScale, {moveCountNormal.size*screenScale, screenHeight-moveCountNormal.size*screenScale});

    Vector2 clockSize = {clockWidth * screenScale, clockWidth * screenScale * 5};
    Vector2 clockPosition = {clockWidth*0.1f, screenHeight - clockSize.y};
    Vector2 marbleSize = {clockSize.x, clockSize.y / static_cast<float>(LEVELS[levelIndex].moves)};
    for (int i = 1; i <= LEVELS[levelIndex].moves; i++) {
        if (i <= level.moves) {
            unsigned char gb = level.moves > 5 ? 255 : 100;
            DrawRectangle(static_cast<int>(clockPosition.x+(clockSize.x/2.667f)),
                          static_cast<int>((clockPosition.y+clockSize.y) - (static_cast<float>(i) * marbleSize.y) + 1),
                          static_cast<int>(marbleSize.x* 0.275f),
                          static_cast<int>(marbleSize.y* 1.25f),
                          {255, gb, gb, 255});
        }
    }
    drawImage((playAnimation == finalScene && gameFrame - beginningFrame >= 30 ? movesClockBrokenImage : movesClockImage), clockPosition.x, clockPosition.y, clockSize.x, clockSize.y);

    /* draw hearts */
    const float barHeight = GAME_HEART_SIZE * screenScale;
    Vector2 barSize = {barHeight * 3, barHeight};
    Vector2 barPosition = {(screenWidth - barSize.x) * 0.5f, screenHeight - 1.2f * barSize.y};
    switch (player.lives) {
        case 3: drawImage(healthBarFullImage,    barPosition.x, barPosition.y, barSize.x, barSize.y); break;
        case 2: drawImage(healthBarChippedImage, barPosition.x, barPosition.y, barSize.x, barSize.y); break;
        case 1: drawImage(healthBarDamagedImage, barPosition.x, barPosition.y, barSize.x, barSize.y); break;
    }

    /* draw keys */
    const float keyHeight = GAME_KEY_SIZE * screenScale;
    Vector2 keyPosition = {screenWidth-keyHeight, screenHeight};
    for (int i = 1; i <= level.keys; i++) {
        drawImage(i <= player.keys ? keyImage : keyOutlineImage, keyPosition.x, keyPosition.y - keyHeight * static_cast<float>(i) * 1.1f, keyHeight);
    }
}

void drawIntro() {
    int i = static_cast<int>(gameFrame - beginningFrame) + 1;

    if (imageIteration != 1 && imageIteration != 3) gameFrame++;
    else {
        if (i < 90 || i > 90) gameFrame++;
        else {
            drawText(introHint);
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) gameFrame++;
        }
    }

    if (i >= 180) {
        imageIteration++;
        beginningFrame = gameFrame;

        switch (imageIteration) {
            case 4: PlaySound(enemyBreakSound); break;
            case 5: PlaySound(exitOpenSound);   break;
        }
    }

    if (imageIteration > 5) toMainMenu();

    float size = 0.8f * std::min(screenWidth, screenHeight);
    Vector2 imagePosition = {(screenWidth - size) * 0.5f, (screenHeight - size) * 0.5f};
    Texture2D image;

    switch (imageIteration) {
        case 0: image = story0; break;
        case 1: image = story1; break;
        case 2: image = story2; break;
        case 3: image = story3; break;
        case 4: image = story4; break;
        case 5: image = story5; break;
    }
    drawImage(image, imagePosition.x, imagePosition.y, size);

    fade(i,   0,  60, static_cast<unsigned char>( 256.0f - (static_cast<float>(animationDuration) / 30.0f * static_cast<float>(i))));
    fade(i, 120, 180, static_cast<unsigned char>( 256.0f - (static_cast<float>(animationDuration) / 30.0f * static_cast<float>( 181 - i))));
}

void drawMainMenu() {
    const float titleSize = MENU_TITLE_SIZE * screenScale;
    const float figureSize = titleSize * 0.2f;
    const float amplitude = 15.0f * screenScale;
    float titleOffsetY = amplitude*sinf(static_cast<float>(gameFrame%70)*(PI/35.0f));
    float figureOffsetY = amplitude*2.0f*sinf(static_cast<float>(gameFrame%70)*(PI/35.0f));

    Vector2 titlePosition = {(screenWidth-titleSize)*0.5f, 0};
    Vector2 figurePosition = {titlePosition.x+(titleSize-figureSize)*0.5f, titlePosition.y+(titleSize-figureSize)*0.2f};
    drawImage(menuTitle, titlePosition.x, titlePosition.y + titleOffsetY, titleSize);
    drawImage(playerImage, figurePosition.x, figurePosition.y + figureOffsetY - 2.0f*amplitude, figureSize);

    drawText(authorship);
}

void drawChooseLevelMenu() {
    drawMenu(chooseLevelMenu);
}

void drawTutorial() {
    float size = 0.8f*std::min(screenWidth, screenHeight);
    Vector2 imagePosition = {(screenWidth-size)*0.5f, (screenHeight-size)*0.5f};
    drawSprite(tutorialSprite, imagePosition.x, imagePosition.y, size, size);
}

void drawPauseMenu() {
    drawMenu(pauseMenu);
}

void drawGameOverMenu() {
    drawText(gameOverTitle);
    drawMenu(gameOverMenu);
}

void drawVictoryMenu() {
    drawText(victoryTitle);
    drawMenu(victoryMenu);
}

#endif //UI_UX_H
