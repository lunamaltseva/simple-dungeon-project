#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "raylib.h"

#include "globals.h"

#include <vector>
#include <string>
#include <cmath>

void drawText(const Text &text, float size, Vector2 position) {
    DrawTextEx(menuFont, text.line.c_str(), position, size, 4, text.color);
}

void drawText(const Text &text) {
    const char *TEXT         = text.line.c_str();
    const float TEXT_SIZE    = text.size * screenScale;
    const float TEXT_X_SHIFT = text.xShift * screenScale;
    const float TEXT_Y_SHIFT = text.yShift * screenScale;

    Vector2 textSize = MeasureTextEx(menuFont, TEXT, TEXT_SIZE, 4);
    Vector2 textPosition = {(screenWidth - textSize.x) * 0.5f + TEXT_X_SHIFT, (screenHeight - textSize.y) * 0.5f + TEXT_Y_SHIFT };

    DrawTextEx(menuFont, TEXT, textPosition, TEXT_SIZE, 4, text.color);
}

void drawAnimatedText(AnimatedText &text) {
    int i = static_cast<int>(static_cast<float>(gameFrame - beginningFrame));
    if (i > text.frametime) {
        beginningFrame = gameFrame;
        text.textIndex++;
    }

    if (text.textIndex >= text.entries.size()) return;

    Text entry = {
            text.entries[text.textIndex],
            text.size, text.xShift, text.yShift,
            text.color
    };

    drawText(entry);
}

void drawIconsAboveText(const Text &text, size_t checkLevelIndex) {
    const char *TEXT         = text.line.c_str();
    const float TEXT_SIZE    = text.size * screenScale;
    const float TEXT_X_SHIFT = text.xShift * screenScale;
    const float TEXT_Y_SHIFT = text.yShift * screenScale;

    Vector2 textSize = MeasureTextEx(menuFont, TEXT, TEXT_SIZE, 4);
    Vector2 textPosition = {(screenWidth - textSize.x) * 0.5f + TEXT_X_SHIFT, (screenHeight - textSize.y) * 0.5f + TEXT_Y_SHIFT };

    size_t numOfElements = 0;
    listLevelElements(gameElements, gameElementsCount, checkLevelIndex);
    for (int i = 0; i < gameElementsCount; i++) if (gameElements[i]) numOfElements++;

    const float imageSize = textSize.x/gameElementsCount;
    Vector2 imagePosition = {textPosition.x + 0.5f*(textSize.x-imageSize*numOfElements), textPosition.y - textSize.y};

    for (int i = 0, j = 0; i < gameElementsCount; i++) {
        if (gameElements[i]) {
            if (i == keyElement)          drawSprite(keySprite,    imagePosition.x+imageSize*j, imagePosition.y, imageSize);
            if (i == marbleElement)       drawSprite(marbleSprite, imagePosition.x+imageSize*j, imagePosition.y, imageSize);
            if (i == quicktileElement)    drawImage(quickTileImage,    imagePosition.x+imageSize*j, imagePosition.y, imageSize);
            if (i == pawnElement)         drawImage(pawnImage,    imagePosition.x+imageSize*j, imagePosition.y, imageSize);
            if (i == rookElement)         drawImage(rookImage,    imagePosition.x+imageSize*j, imagePosition.y, imageSize);
            if (i == queenElement)        drawImage(queenImage,   imagePosition.x+imageSize*j, imagePosition.y, imageSize);
            j++;
        }
    }
}

void drawMenu(const Menu &menu) {
    for (int i = 0; i < menu.entries.size(); i++) {
        Text entry = {
                menu.entries[i],
                (menu.entrySelected == i ? menu.size*1.2f : menu.size), menu.xShift + i*menu.dX, menu.yShift + i*menu.dY,
                (menu.entrySelected == i ? menu.selected : menu.unselected)
        };
        drawText(entry);
        if (menu.type == Menu::CHOOSE_LEVEL_TYPE) drawIconsAboveText(entry, i);
    }
}

void drawImage(Texture2D image, float x, float y, float size) {
    drawImage(image, x, y, size, size);
}

void drawImage(Texture2D image, float x, float y, float width, float height) {
    Rectangle source = { 0.0f, 0.0f, static_cast<float>(image.width), static_cast<float>(image.height) };
    Rectangle destination = { x, y, width, height };
    DrawTexturePro(image, source, destination, { 0.0f, 0.0f }, 0.0f, WHITE);
}

void drawSprite(sprite &sprite, float x, float y, float size) {
    drawSprite(sprite, x, y, size, size);
}

void drawSprite(sprite &sprite, float x, float y, float width, float height) {
    drawImage(sprite.frames[sprite.frameIndex], x, y, width, height);

    if (sprite.prevGameFrame == gameFrame) {
        return;
    }
    if (sprite.framesSkipped < sprite.framesToSkip) {
        ++sprite.framesSkipped;
    } else {
        sprite.framesSkipped = 0;

        ++sprite.frameIndex;
        if (sprite.frameIndex >= sprite.frameCount) {
            sprite.frameIndex = sprite.loop ? 0 : sprite.frameCount - 1;
        }
    }
    sprite.prevGameFrame = gameFrame;
}

void drawLevelHalo(int i) {
    float expansion = 1.0f + returnFigureAmount()*i*0.0075f;
    for (size_t row = 0; row < level.rows; row++) {
        for (size_t column = 0; column < level.columns; column++) {
            float x = static_cast<float>(column) * cellSize + shiftToCenterCellByX - (cellSize*expansion)*0.5f + cellSize * 0.5f;
            float y = static_cast<float>(row) * cellSize + shiftToCenterCellByY - (cellSize*expansion)*0.5f + cellSize * 0.5f;

            char cell = level.data[row*level.columns + column];
            if (cell == WALL) {
                    if ( (row > 0 && row < level.rows) && (column > 0 && column < level.columns) ) {
                        char cell_above = level.data[(row - 1) * level.columns + column];
                        if (cell_above != WALL)
                            drawImage(boardHaloImage, x, y-cellSize*0.625f, cellSize*expansion, cellSize*expansion);
                    }
            }
            else
                drawImage(boardHaloImage, x, y, cellSize*expansion);
        }
    }
    fade(i, 200.0f/(60/returnFigureAmount()), 55);
}

void fade(unsigned char transparency) {
    DrawRectangle(0, 0, static_cast<int>(screenWidth), static_cast<int>(screenHeight), {0, 0, 0, transparency});
}

void fade(int i, float step) {
    fade(static_cast<unsigned char>(step*i));
}

void fade(int i, float step, float baseline) {
    fade(static_cast<unsigned char>(baseline + step*static_cast<float>(i)));
}

void fade(int i, int from, int until, unsigned char transparency) {
    if (i >= from && i <= until) fade(transparency);
}

void fade(int i, int from, int until, float step, float baseline) {
    if (i >= from && i <= until) fade(static_cast<unsigned char>(baseline + step*static_cast<float>(i)));
}

void drawEnemies() {
    if (ifEnemy(pawn)  && !pawn.isFalling)  drawFigure(pawn);
    if (ifEnemy(rook)  && !rook.isFalling)  drawFigure(rook);
    if (ifEnemy(queen) && !queen.isFalling) drawFigure(queen);
}

void drawFigures() {
    drawEnemies();
    drawFigure(player);
}

void deriveGraphicsMetricsFromLoadedLevel() {
    screenWidth  = static_cast<float>(GetRenderWidth());
    screenHeight = static_cast<float>(GetRenderHeight());

    cellSize = std::min( screenWidth  / static_cast<float>(level.columns), screenHeight / static_cast<float>(level.rows) ) * CELL_SCALE;
    screenScale = static_cast<float>( std::min( screenWidth, screenHeight ) / 7.5 / screenScale_DIVISOR );

    float level_width  = static_cast<float>(level.columns) * cellSize;
    float level_height = static_cast<float>(level.rows)    * cellSize;

    shiftToCenterCellByX = (screenWidth - level_width)   * 0.5f;
    shiftToCenterCellByY = (screenHeight - level_height) * 0.3f;
}

Texture2D floorTexture(size_t row, size_t column) {
    return ((row*level.columns+column) % 2 == 1 ? boardWhiteImage : boardBlackImage);
}

Texture2D wallTexture(size_t row, size_t column) {
    if ( (row > 0 && row < level.rows) && (column > 0 && column < level.columns) ) {
        char cell_above = level.data[(row - 1) * level.columns + column];
        if (cell_above != WALL && cell_above != QUICKTILE)
            return ((row*level.columns+column) % 2 == 0 ? wallDepthWhiteImage : wallDepthBlackImage);
        else if (cell_above == QUICKTILE)
            return wallDepthQuickTileImage;
    }
    return wallNoneImage;
}

void drawLevel() {
    for (size_t row = 0; row < level.rows; row++) {
        for (size_t column = 0; column < level.columns; column++) {
            float x = static_cast<float>(column) * cellSize + shiftToCenterCellByX;
            float y = static_cast<float>(row) * cellSize + shiftToCenterCellByY;

            char cell = level.data[row*level.columns + column];
            switch (cell) {
                case PAWN:
                case ROOK:
                case QUEEN:
                case FLOOR:
                    drawImage(floorTexture(row, column), x, y, cellSize);
                    break;
                case QUICKTILE:
                    drawImage(boardQuickTileImage, x, y, cellSize);
                    break;
                case KEY:
                    drawImage(floorTexture(row, column), x, y, cellSize);
                    drawSprite(keySprite, x, y-(cellSize*0.3125f), cellSize);
                    break;
                case ENTRANCE:
                    drawImage(floorTexture(row, column), x, y, cellSize);
                    drawImage(entranceImage, x, y, cellSize);
                    break;
                case MARBLE:
                    drawImage(floorTexture(row, column), x, y, cellSize);
                    drawSprite(marbleSprite, x, y-(cellSize*0.3125f), cellSize);
                    break;
                case WALL:
                    drawImage(wallTexture(row, column), x, y, cellSize);
                    break;
                case EXIT:
                    drawImage(floorTexture(row, column), x, y, cellSize);
                    drawImage((isExitOpen ? exitImage : exitClosedImage), x, y, cellSize);
                    break;
                default:
                    break;
            }
        }
    }
    if (playAnimation == playerOutOfMoves) {
        drawImage(fractureImage, 0, shiftToCenterCellByY, screenWidth, screenHeight);
    }
}

void drawFigure(Figure figure) {
    float x = static_cast<float>(figure.column) * cellSize + shiftToCenterCellByX;
    float y = static_cast<float>(figure.row) * cellSize + shiftToCenterCellByY;

    drawImage(figure.image, x, y-(cellSize*0.3125f), cellSize);
}

#endif // GRAPHICS_H
