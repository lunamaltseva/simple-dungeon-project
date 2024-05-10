#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"
#include "globals.h"

#include <string>
#include <cassert>

void loadFonts() {
    menuFont = LoadFontEx(nullptr, 128, NULL, 0); // I like the default font
}

void unloadFonts() {
    UnloadFont(menuFont);
}

void loadImages() {
    // Story
    story0                 = LoadTexture("data/images/story/story0.png");
    story1                 = LoadTexture("data/images/story/story1.png");
    story2                 = LoadTexture("data/images/story/story2.png");
    story3                 = LoadTexture("data/images/story/story3.png");
    story4                 = LoadTexture("data/images/story/story4.png");
    story5                 = LoadTexture("data/images/story/story5.png");

    // Menus
    menuTitle              = LoadTexture("data/images/title.png");
    tutorialSprite         = loadSprite("data/images/tutorial/tutorial", ".png", 4);

    // Level
    boardWhiteImage             = LoadTexture("data/images/board1.png");
    boardBlackImage             = LoadTexture("data/images/board2.png");
    boardQuickTileImage         = LoadTexture("data/images/board_quick.png");
    wallDepthWhiteImage         = LoadTexture("data/images/wall_depth1.png");
    wallDepthBlackImage         = LoadTexture("data/images/wall_depth2.png");
    wallDepthQuickTileImage     = LoadTexture("data/images/wall_depth_quick.png");
    wallNoneImage               = LoadTexture("data/images/wall_none.png");

    boardHaloImage              = LoadTexture("data/images/board_halo.png");

    entranceImage          = LoadTexture("data/images/entrance.png");
    exitClosedImage        = LoadTexture("data/images/exit_closed.png");
    exitImage              = LoadTexture("data/images/exit.png");

    // Figures
    playerImage            = LoadTexture("data/images/player.png");
    pawnImage              = LoadTexture("data/images/enemy_pawn.png");
    rookImage              = LoadTexture("data/images/enemy_rook.png");
    queenImage             = LoadTexture("data/images/enemy_queen.png");

    playerBroken           = LoadTexture("data/images/player_broken.png");
    enemyBroken            = LoadTexture("data/images/enemy_broken.png");

    // UI
    movesClockImage             = LoadTexture("data/images/moves_clock.png");
    movesClockBrokenImage       = LoadTexture("data/images/moves_clock_broken.png");

    healthBarFullImage     = LoadTexture("data/images/health_bar_full.png");
    healthBarChippedImage  = LoadTexture("data/images/health_bar_chipped.png");
    healthBarDamagedImage  = LoadTexture("data/images/health_bar_damaged.png");

    fractureImage          = LoadTexture("data/images/fracture.png");
    fractureMoreImage      = LoadTexture("data/images/fracture_more.png");

    keyImage               = LoadTexture("data/images/key.png");
    keySprite              = loadSprite("data/images/key/key", ".png", 4);
    keyOutlineImage        = LoadTexture("data/images/key_outline.png");

    marbleSprite           = loadSprite("data/images/marble/marble", ".png", 4);

    quickTileImage         = LoadTexture("data/images/quicktile.png");

}

void unloadImages() {
    UnloadTexture(story0);
    UnloadTexture(story1);
    UnloadTexture(story2);
    UnloadTexture(story3);
    UnloadTexture(story4);
    UnloadTexture(story5);
    UnloadTexture(menuTitle);
    UnloadTexture(wallNoneImage);
    UnloadTexture(wallDepthWhiteImage);
    UnloadTexture(wallDepthBlackImage);
    UnloadTexture(boardWhiteImage);
    UnloadTexture(boardBlackImage);
    UnloadTexture(playerImage);
    UnloadTexture(pawnImage);
    UnloadTexture(enemyBroken);
    UnloadTexture(playerBroken);
    UnloadTexture(entranceImage);
    UnloadTexture(exitClosedImage);
    UnloadTexture(movesClockImage);
    UnloadTexture(healthBarFullImage);
    UnloadTexture(healthBarChippedImage);
    UnloadTexture(healthBarDamagedImage);
    UnloadTexture(exitImage);
    UnloadTexture(fractureImage);
    UnloadTexture(keyImage);
    UnloadTexture(keyOutlineImage);
    unloadSprite(keySprite);
}

sprite loadSprite(
    const std::string &file_name_prefix,
    const std::string &file_name_suffix,
    size_t frameCount,
    bool loop,
    size_t framesToSkip
) {
    assert(frameCount < 100);

    sprite result = {
        frameCount, framesToSkip, 0, 0, loop, 0, new Texture2D[frameCount]
    };

    for (size_t i = 0; i < frameCount; ++i) {
        std::string file_name;
        if (frameCount < 10) {
            file_name = file_name_prefix;
            file_name += std::to_string(i);
            file_name += file_name_suffix;
        } else {
            file_name = file_name_prefix;
            file_name += i < 10 ? ("0" + std::to_string(i)) : std::to_string(i);
            file_name += file_name_suffix;
        }
        result.frames[i] = LoadTexture(file_name.c_str());
    }

    return result;
}

void unloadSprite(sprite &sprite) {
    assert(sprite.frames != nullptr);

    for (size_t i = 0; i < sprite.frameCount; ++i) {
        UnloadTexture(sprite.frames[i]);
    }
    delete[] sprite.frames;
    sprite.frames = nullptr;
}

void loadSounds() {
    // UI
    menuExitSound       = LoadSound("data/sounds/menu_exit.ogg");
    clickSound          = LoadSound("data/sounds/click.ogg");
    scrollSound         = LoadSound("data/sounds/scroll.ogg");
    backOutSound        = LoadSound("data/sounds/backout.ogg");

    // Clock
    clockTickSound0     = LoadSound("data/sounds/clock_tick0.ogg");
    clockTickSound1     = LoadSound("data/sounds/clock_tick1.ogg");
    clockHighSound      = LoadSound("data/sounds/clock_high.ogg");
    clockNormalSound    = LoadSound("data/sounds/clock_normal.ogg");
    clockLowSound       = LoadSound("data/sounds/clock_low.ogg");

    // Figures
    playerLandedSound   = LoadSound("data/sounds/player_landed.ogg");
    playerBreakSound    = LoadSound("data/sounds/player_break.ogg");
    figureFallSound     = LoadSound("data/sounds/figure_fall.ogg");
    enemyBreakSound     = LoadSound("data/sounds/enemy_break.ogg");
    playerDeathSound    = LoadSound("data/sounds/lose.ogg");
    queenSpeakingSound  = LoadSound("data/sounds/queen_speaking.ogg");

    // Level & Elements
    puzzleCompleteSound = LoadSound("data/sounds/puzzle_complete.ogg");
    exitOpenSound       = LoadSound("data/sounds/exit_open.ogg");
    pickupKeySound      = LoadSound("data/sounds/key_pickup.ogg");
    marbleTopupSound    = LoadSound("data/sounds/marble_topup.ogg");
    quicktileBreakSound0= LoadSound("data/sounds/quicktile_break0.ogg");
    quicktileBreakSound1= LoadSound("data/sounds/quicktile_break1.ogg");
    levelBreakSound     = LoadSound("data/sounds/level_break.ogg");

    // Ending
    clockShatterSound   = LoadSound("data/sounds/clock_shatter.ogg");
    clockHissSound      = LoadSound("data/sounds/clock_hiss.ogg");
    levelDismantleSound = LoadSound("data/sounds/level_dismantle.ogg");

    // Music
    mainTheme           = LoadMusicStream("data/music/main_loop.ogg");
    introTheme          = LoadMusicStream("data/music/intro_loop.ogg");
    endingTheme         = LoadMusicStream("data/music/ending_loop.ogg");

    SetMusicVolume(introTheme, 0.5f);
    SetMusicVolume(mainTheme, 0.5f);
    SetMusicVolume(endingTheme, 0.5f);
}

void unloadSounds() {
    UnloadSound(menuExitSound);
    UnloadSound(clickSound);
    UnloadSound(scrollSound);
    UnloadSound(backOutSound);
    UnloadSound(clockTickSound0);
    UnloadSound(clockTickSound1);
    UnloadSound(clockHighSound);
    UnloadSound(clockNormalSound);
    UnloadSound(clockLowSound);
    UnloadSound(playerLandedSound);
    UnloadSound(playerBreakSound);
    UnloadSound(figureFallSound);
    UnloadSound(enemyBreakSound);
    UnloadSound(playerDeathSound);
    UnloadSound(puzzleCompleteSound);
    UnloadSound(exitOpenSound);
    UnloadSound(pickupKeySound);
    UnloadSound(marbleTopupSound);
    UnloadSound(quicktileBreakSound0);
    UnloadSound(quicktileBreakSound1);
    UnloadSound(levelBreakSound);
    UnloadSound(clockShatterSound);
    UnloadSound(clockHissSound);
    UnloadSound(levelDismantleSound);

    UnloadMusicStream(mainTheme);
    UnloadMusicStream(introTheme);
    UnloadMusicStream(endingTheme);
}

#endif // ASSETS_H
