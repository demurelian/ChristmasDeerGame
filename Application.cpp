#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
using namespace std;


const unsigned short    frameW = 341,
                        frameH = 76,
                        frameX = 227,
                        frameY = 167,
                        dY = 5,
                        tubeW = 137,
                        tubeH = 921,
                        tubeHalfY = tubeH * 3 / 8, 
                        groundOffset = 23,
                        groundW = 1366 + groundOffset,
                        groundH = 77,
                        groundSpeed = 3,
                        deerW = 80,
                        deerH = 80,
                        freeSpaceY = 15,
                        tubeSpeed = 10,
                        tubeBonusSpeed = 20,
                        deerOffsetX = 5,
                        deerStartX = 50,
                        textCoords = 25,
                        fontSize = 100,
                        CTBSw = 818,
                        CTBSh = 101,
                        CTBSx = 212,
                        CTBSy = 50,
                        WTTCDGw = 1092,
                        WTTCDGh = 50,
                        WTTCDGx = 137,
                        WTTCDGy = 50,
                        ScreenW = 1366,
                        ScreenH = 768;


const char  *defGr_p            =           "textures/ground.png",
            *bonusGr_p          =           "textures/groundBonus.png",
            *ttf_p              =           "ttf/sus.ttf",
            *bonusbg_p          =           "textures/2newbg.png",
            *bonuspipe_p        =           "textures/newBonusPipe.png",
            *gamebg_p = "textures/1newbg.jpg",
            *defpipe_p = "textures/1newPipe.png",
            *healthpipe_p = "textures/2newPipe.png",
            *bonusStartpipe_p = "textures/3newPipe.png",
            *deer_p = "textures/deerMale.png",
            *CTBS_p = "textures/menu/textCTBS.png",
            *mAvariaBlue_p = "textures/menu/textMusicAvariaBlue.png",
            *mAvariaWhite_p = "textures/menu/textMusicAvariaWhite.png",
            *mBlestyashieBlue_p = "textures/menu/textMusicBlestyashieBlue.png",
            *mBlestyashieWhite_p = "textures/menu/textMusicBlestyashieWhite.png",
            *mDigiBlue_p = "textures/menu/textMusicDigiBlue.png",
            *mDigiWhite_p = "textures/menu/textMusicDigiWhite.png",
            *mElkaBlue_p = "textures/menu/textMusicElkaBlue.png",
            *mElkaWhite_p = "textures/menu/textMusicElkaWhite.png",
            *mVerkaBlue_p = "textures/menu/textMusicVerkaBlue.png",
            *mVerkaWhite_p = "textures/menu/textMusicVerkaWhite.png",
            *menubg_p = "textures/anime.jpg",
            *frameBlue_p = "textures/menu/buttonFrameBlue.png",
            *frameWhite_p = "textures/menu/buttonFrameWhite.png",
            *textWTTCDG_p = "textures/menu/textWTTCDG.png",
            *textPlayBlue_p = "textures/menu/textPlayBlue.png",
            *textPlayWhite_p = "textures/menu/textPlayWhite.png",
            *textSettingsBlue_p = "textures/menu/textSettingsBlue.png",
            *textSettingsWhite_p = "textures/menu/textSettingsWhite.png",
            *textQuitBlue_p = "textures/menu/textQuitBlue.png",
            *textQuitWhite_p = "textures/menu/textQuitWhite.png",
            *avariamp3_p = "audio/avaria.mp3",
            *blestyashiemp3_p = "audio/blestyashie.mp3",
            *digimp3_p = "audio/digi.mp3",
            *elkamp3_p = "audio/elka.mp3",
            *verkamp3_p = "audio/verka.mp3";



enum ground {defGr, bonusGr};


enum tubeType {def, health, bonus};


enum menuType {start, inGame, death};


enum frame {FrameBlue, FrameWhite};


enum menuExitCode {quitApp, gameStart};


struct text
{
    SDL_Rect rect;
    SDL_Color color;
    SDL_Texture *texture;
    short fontSize;
};


struct obj
{
    SDL_Texture *texture;
    SDL_Rect rect;
};

struct animated_obj
{
    SDL_Texture *texture;
    SDL_Rect srcrect;
    SDL_Rect dstrect;
};


SDL_Texture* renderText(const string &text, SDL_Color color, SDL_Renderer *renderer, short fontSize)
{
    TTF_Font *font = TTF_OpenFont(ttf_p, fontSize);
    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
    return texture;
}


tubeType generator()
{
    int random_number = rand() % 100;
    if (random_number < 10/*%*/)
    {
        return health;
    } 
    else
    {    
        if (random_number < 20)
        {
            return bonus;
        }
        else
        {
            return def;
        }
    }
}


void bonusLvl(unsigned int &sessionHighscore, unsigned int move, unsigned int deerX, unsigned int deerAnimationSize, unsigned int deerTextureW, bool& gameOver, bool& gamehappening, bool& mainMenu, const Uint8 *keyboardState, animated_obj deer, unsigned int& score, SDL_Renderer* ren, obj *ground, text scoreText)
{
    SDL_Texture* bg = IMG_LoadTexture(ren, bonusbg_p);
    
    obj tube1;
    tube1.texture = IMG_LoadTexture(ren, bonuspipe_p);
    tube1.rect.x = ScreenW;
    tube1.rect.y = -rand() % groundH * 3;
    tube1.rect.w = tubeW;
    tube1.rect.h = tubeH;

    obj tube2;
    tube2.texture = IMG_LoadTexture(ren, bonuspipe_p);
    tube2.rect.x = ScreenW;
    tube2.rect.y = tube1.rect.y;
    tube2.rect.w = tubeW;
    tube2.rect.h = tubeH;

    unsigned int    bonusLives = 1,
                    animationTimer = SDL_GetTicks();

    bool    bonus = true,
            pipe2generated = false,
            pipe1_crossed_centre = false,
            pipe2_crossed_centre = false,
            pipe3_crossed_centre = false,
            pipe1_crossed = false,
            pipe2_crossed = false,
            pipe3_crossed = false,
            pipe1_failed = false,
            pipe2_failed = false,
            pipe3_failed = false,
                _pipe2generated = false;
    SDL_Event bonusE;

    while(bonus)
    {
        SDL_RenderClear(ren);
        //event handler
        while (SDL_PollEvent(&bonusE))
        {
            if (bonusE.type == SDL_KEYDOWN)
            {
                if(keyboardState[SDL_SCANCODE_ESCAPE])
                {
                    gameOver = true;
                    gamehappening = false;
                    mainMenu = true;
                    bonus = false;
                }
                if(keyboardState[SDL_SCANCODE_SPACE])
                {
                    move = 15;
                }
            }
        }
        //ground physics
        ground[bonusGr].rect.x -= groundSpeed;
        if (ground[bonusGr].rect.x <= -groundOffset)
        {
            ground[bonusGr].rect.x = 0;
        }
        //deer animation
        if (SDL_GetTicks() - animationTimer > 200)
        {
            animationTimer = SDL_GetTicks();
            deer.srcrect.x += deerAnimationSize;
            if (deer.srcrect.x >= deerTextureW)
            {
                deer.srcrect.x = 0;
            }
        }
        //deer acceleration and physics
        move--;
        deer.dstrect.y -= move;
        //collision with the sky or ground
        if (deer.dstrect.y >= ScreenH - deer.dstrect.h - groundH)// - deerAnimationSize)
        {
            deer.dstrect.y = ScreenH - deer.dstrect.h - groundH;// - deerAnimationSize;
            move = 0;
        }
        if (deer.dstrect.y <= 0 - freeSpaceY)
        {
            deer.dstrect.y = 0 - freeSpaceY;
            move = 0;
        }
        //tube1 centre cross check
        if (tube1.rect.x <= (ScreenW - tube1.rect.w) / 2 && !pipe1_crossed_centre && !_pipe2generated)
        {
            if (!pipe2generated)
                pipe2generated = true;
            pipe1_crossed_centre = true;
        }
        //
        if (tube1.rect.x <= (deerX - (tube1.rect.w -deer.dstrect.w)) && !pipe1_crossed)
        {
            score += 2;
            if (score > sessionHighscore)
            {
                sessionHighscore = score;
            }
            pipe1_crossed = true;
        } 
        //tubes physics
        tube1.rect.x -= tubeBonusSpeed;
        if (!pipe1_failed && tube1.rect.x < deerX + deer.dstrect.w  && tube1.rect.x > deerX - tube1.rect.w && (deer.dstrect.y <= tube1.rect.y + tubeHalfY - freeSpaceY || deer.dstrect.y >= tube1.rect.y + tube1.rect.h - tubeHalfY - deer.dstrect.h))
        {
            bonusLives--;
            if (bonusLives == 0)
            {
                bonus = false;
            }
            pipe1_failed = true;
        }

        if (tube1.rect.x <= -tube1.rect.w)
        {
            tube1.rect.x = ScreenW;
            tube1.rect.y = -rand() % groundH * 3;

            pipe1_crossed_centre = false;
            pipe1_crossed = false;
            pipe1_failed = false;
        }
        if (tube2.rect.x <= (ScreenW - tube2.rect.w) / 2 && !pipe2_crossed_centre)
        {
            pipe2_crossed_centre = true;
        }
        if (tube2.rect.x <= (deerX - (tube2.rect.w - deer.dstrect.w)) && !pipe2_crossed)
        {
            score += 2;
            if (score > sessionHighscore)
            {
                sessionHighscore = score;
            }
            pipe2_crossed = true;
        } 
        if (pipe2generated)
        {
            tube2.rect.x = ScreenW;
            tube2.rect.y = -rand() % groundH * 3;
            pipe2generated = false;
            _pipe2generated = true;
        }
        tube2.rect.x -= tubeBonusSpeed;
        if (!pipe2_failed && tube2.rect.x < deerX + deer.dstrect.w  && tube2.rect.x > deerX - tube2.rect.w && (deer.dstrect.y <= tube2.rect.y + tubeHalfY - freeSpaceY || deer.dstrect.y >= tube2.rect.y + tube2.rect.h - tubeHalfY - deer.dstrect.h))
        {
            bonusLives--;
            if (bonusLives == 0)
            {
                bonus = false;
            }
            pipe2_failed = true;
        }
        if (tube2.rect.x <= -tube2.rect.w)
        {
            tube2.rect.x = ScreenW;
            tube2.rect.y = -rand() % groundH * 3;

            pipe2_crossed_centre = false;
            pipe2_crossed = false;
            pipe2_failed = false;
        }
        scoreText.texture = renderText(to_string(score), scoreText.color, ren, scoreText.fontSize);
        SDL_QueryTexture(scoreText.texture, nullptr, nullptr, &scoreText.rect.w, &scoreText.rect.h);
        scoreText.rect.x = (ScreenW - scoreText.rect.w) / 2;

        SDL_RenderCopy(ren, bg, nullptr, nullptr);
        SDL_RenderCopy(ren, tube1.texture, nullptr, &tube1.rect);
        SDL_RenderCopy(ren, tube2.texture, nullptr, &tube2.rect);
        SDL_RenderCopy(ren, deer.texture, &deer.srcrect, &deer.dstrect);
        SDL_RenderCopy(ren, ground[bonusGr].texture, nullptr, &ground[bonusGr].rect);
        SDL_RenderCopy(ren, scoreText.texture, nullptr, &scoreText.rect);
        SDL_RenderPresent(ren);
    }
    SDL_DestroyTexture(bg);
    SDL_DestroyTexture(tube1.texture);
    SDL_DestroyTexture(tube2.texture);
}


void gameLoop(unsigned int &sessionHighscore, SDL_Renderer *ren, const Uint8 *keyboardState, bool& mainMenu)
{
    SDL_Texture *gameBg = IMG_LoadTexture(ren, gamebg_p);

    const unsigned short    tubeNumber = 3;

    SDL_Texture *tubeTexture[tubeNumber];
    tubeTexture[def] = IMG_LoadTexture(ren, defpipe_p);
    tubeTexture[health] = IMG_LoadTexture(ren, healthpipe_p);
    tubeTexture[bonus] = IMG_LoadTexture(ren, bonusStartpipe_p);

    obj ground[2];
    ground[defGr].texture = IMG_LoadTexture(ren, defGr_p);
    ground[defGr].rect.x = 0;
    ground[defGr].rect.y = ScreenH - groundH;
    ground[defGr].rect.w = groundW;
    ground[defGr].rect.h = groundH;
    ground[bonusGr].texture = IMG_LoadTexture(ren, bonusGr_p);
    ground[bonusGr].rect.x = 0;
    ground[bonusGr].rect.y = ScreenH - groundH;
    ground[bonusGr].rect.w = groundW;
    ground[bonusGr].rect.h = groundH;

    obj tube1;
    short tube1type = generator();
    tube1.texture = tubeTexture[tube1type];
    tube1.rect.x = ScreenW;
    tube1.rect.y = -rand() % groundH * 3;
    tube1.rect.w = tubeW;
    tube1.rect.h = tubeH;

    obj tube2;
    short tube2type = generator();
    tube2.texture = tubeTexture[tube1type];
    tube2.rect.x = ScreenW;
    tube2.rect.y = tube1.rect.y;
    tube2.rect.w = tubeW;
    tube2.rect.h = tubeH;

    obj tube3;
    short tube3type = generator();
    tube3.texture = tubeTexture[tube3type];
    tube3.rect.x = ScreenW;
    tube3.rect.y = -rand() % groundH * 3;
    tube3.rect.w = tubeW;
    tube3.rect.h = tubeH;

    int deerTextureW, deerTextureH;


    animated_obj deer;
    deer.texture = IMG_LoadTexture(ren, deer_p);
    SDL_QueryTexture(deer.texture, nullptr, nullptr, &deerTextureW, &deerTextureH);
    short deerAnimationSize = deerTextureW / 5;
    deer.dstrect.x = deerStartX;
    deer.dstrect.y = ScreenH - deerH - groundH;
    deer.dstrect.w = deerW;
    deer.dstrect.h = deerH;
    deer.srcrect.x = 0;
    deer.srcrect.y = 0;
    deer.srcrect.w = deerAnimationSize;
    deer.srcrect.h = deerAnimationSize;

    animated_obj santa;
    

    bool    gameOver = true,
            gamehappening = true,
            start = false,
            jumpOnStartHappend = false,
            pipe2generated = false,
            pipe1_crossed_centre = false,
            pipe2_crossed_centre = false,
            pipe3_crossed_centre = false,
            pipe1_crossed = false,
            pipe2_crossed = false,
            pipe3_crossed = false,
            pipe1_failed = false,
            pipe2_failed = false,
            pipe3_failed = false,
                _pipe2generated = false,
            bonushappend = false;

    short   move = 0,
            deerX,
            lives = 1;

    unsigned int    timer = SDL_GetTicks(),
                    moveTimer = SDL_GetTicks(),
                    animationTimer = SDL_GetTicks(),
                    score = 0;

    text scoreText;
    scoreText.rect.y = textCoords;
    scoreText.color = {0, 255, 255, 25};
    scoreText.fontSize = fontSize;

    text livesText;
    livesText.rect.x = textCoords;
    livesText.rect.y = textCoords;
    livesText.color = {255, 0, 0, 255};
    livesText.fontSize = fontSize;

    text SH;
    SH.rect.y = textCoords;
    SH.color = {0, 255, 0, 255};
    SH.fontSize = fontSize;

    SDL_Event game;
    while (gamehappening)
    {
        SDL_RenderClear(ren);
        //event handler
        while (SDL_PollEvent(&game))
        {
            if (game.type == SDL_KEYDOWN)
            {
                if(keyboardState[SDL_SCANCODE_ESCAPE])
                {
                    return;
                    //gamehappening = false;
                    //mainMenu = true;
                }
                if(keyboardState[SDL_SCANCODE_SPACE])
                {
                    start = true;
                    deer.srcrect.x = 0;
                    timer = SDL_GetTicks();
                    deer.srcrect.y += deerTextureW / 5;
                    if(deer.srcrect.y == deerTextureW)
                    {
                        deer.srcrect.y = 0;
                    }
                }
                if (game.type == SDL_QUIT)
                {
                    gamehappening = false;
                    mainMenu = false;
                }
            }
        }
        //deer start animation
        if (SDL_GetTicks() - timer > 500 + rand() % 1500)
        {
            timer = SDL_GetTicks();
            deer.srcrect.x = (rand() % 5)*deerAnimationSize;
            if (deer.srcrect.x >= deerTextureW)
            {
                deer.srcrect.x = 0;
            }
        }
        while (start)
        {
            SDL_RenderClear(ren);
            //event handler
            while (SDL_PollEvent(&game))
            {
                if (game.type == SDL_KEYDOWN)
                {
                    if(keyboardState[SDL_SCANCODE_ESCAPE])
                    {
                        return;/*
                        gamehappening = false;
                        start = false;
                        gamehappening = false;
                        start = false;*/
                    }
                    if(keyboardState[SDL_SCANCODE_SPACE])
                    {
                        deer.srcrect.y = 2 * deerAnimationSize;
                        move = 20;
                        jumpOnStartHappend = true;
                        mainMenu = true;
                    }
                }
                if (game.type == SDL_QUIT)
                {
                    start = false;
                    gamehappening = false;
                }
            }
            //ground physics
            ground[defGr].rect.x -= groundSpeed;
            if (ground[defGr].rect.x <= -groundOffset)
            {
                ground[defGr].rect.x = 0;
            }
            //stop jump animation and start fly-run animation
            
            if (SDL_GetTicks() - animationTimer > 200)
            {
                animationTimer = SDL_GetTicks();
                deer.srcrect.x += deerAnimationSize;
                if (deer.srcrect.x >= deerTextureW)
                {
                    deer.srcrect.x = 0;
                }
            }
            
            //deer acceleration and physics
                move--;
        
            deer.dstrect.y -= move;
            deer.dstrect.x += deerOffsetX;
            deer.srcrect.y = 2 * deerAnimationSize;//change deer animation line (in sprite sheet) to the 2nd
            //collision with sky or ground
            if (deer.dstrect.y >= ScreenH - deer.dstrect.h - groundH)// - deerAnimationSize)
            {
                deer.dstrect.y = ScreenH - deer.dstrect.h - groundH;// - deerAnimationSize;
                move = 0;
            }
            if (deer.dstrect.y <= 0 - freeSpaceY)// - deerAnimationSize)
            {
                deer.dstrect.y = 0 - freeSpaceY;// - deerAnimationSize;
                move = 0;
            }
            //start timer
            if(SDL_GetTicks() - timer > 500 + rand() % 500)
            {
                timer = SDL_GetTicks();
                start = false;
                gameOver = false;
                deerX = deer.dstrect.x;
            }
            SDL_RenderCopy(ren, gameBg, nullptr, nullptr);
            SDL_RenderCopy(ren, deer.texture, &deer.srcrect, &deer.dstrect);
            SDL_RenderCopy(ren, ground[defGr].texture, nullptr, &ground[defGr].rect);
            SDL_RenderPresent(ren);
        }
        
        while (!gameOver)
        {
            SDL_RenderClear(ren);
            if(bonushappend)
            {
                SDL_Delay(200);
                bonushappend = false;
            }
            //event handler
            while (SDL_PollEvent(&game))
            {
                if (game.type == SDL_KEYDOWN)
                {
                    if(keyboardState[SDL_SCANCODE_ESCAPE])
                    {
                        gameOver = true;
                        gamehappening = false;
                        mainMenu = true;
                    }
                    if(keyboardState[SDL_SCANCODE_SPACE])
                    {
                        move = 15;
                    }
                }
            }
            ground[defGr].rect.x -= groundSpeed;
            if (ground[defGr].rect.x <= -groundOffset)
            {
                ground[defGr].rect.x = 0;
            }

            //scoreText.rect.w = 25*numberOfDigits(score);
            //deer animation
            if (SDL_GetTicks() - animationTimer > 200)
            {
                animationTimer = SDL_GetTicks();
                deer.srcrect.x += deerAnimationSize;
                if (deer.srcrect.x >= deerTextureW)
                {
                    deer.srcrect.x = 0;
                }
            }
            //deer acceleration and physics
            move--;
            deer.dstrect.y -= move;
            //collision with the sky or ground
            if (deer.dstrect.y >= ScreenH - deer.dstrect.h - groundH)// - deerAnimationSize)
            {
                deer.dstrect.y = ScreenH - deer.dstrect.h - groundH;// - deerAnimationSize;
                move = 0;
            }
            if (deer.dstrect.y <= 0 - freeSpaceY)
            {
                deer.dstrect.y = 0 - freeSpaceY;
                move = 0;
            }
            //tube1 centre cross check
            if (tube1.rect.x <= (ScreenW - tube1.rect.w) / 2 && !pipe1_crossed_centre && !_pipe2generated)
            {
                if (!pipe2generated)
                    pipe2generated = true;
                pipe1_crossed_centre = true;
            }
            //
            if (tube1.rect.x <= (deerX - (tube1.rect.w -deer.dstrect.w)) && !pipe1_crossed)
            {
                score++;
                if (score > sessionHighscore)
                {
                    sessionHighscore = score;
                }
                if (tube1type == bonus)
                {
                    bonusLvl(sessionHighscore, move, deerX, deerAnimationSize, deerTextureW, gameOver, gamehappening, mainMenu, keyboardState, deer, score, ren, ground, scoreText);
                    bonushappend = true;
                }
                if (tube1type == health)
                {
                    lives++;
                }
                pipe1_crossed = true;
            }
            //tube1 physics
            tube1.rect.x -= tubeSpeed;

            //tube1 failure check
            if (!pipe1_failed && tube1.rect.x < deerX + deer.dstrect.w && tube1.rect.x > deerX - tube1.rect.w && (deer.dstrect.y <= tube1.rect.y + tubeHalfY - freeSpaceY || deer.dstrect.y >= tube1.rect.y + tube1.rect.h - tubeHalfY - deer.dstrect.h))
            {
                lives--;
                if (lives == 0)
                {
                    gameOver = true;
                    gamehappening = false;
                }
                pipe1_failed = true;
            }

            if (tube1.rect.x <= -tube1.rect.w)
            {
                tube1type = generator();
                tube1.texture = tubeTexture[tube1type];
                tube1.rect.x = ScreenW;
                tube1.rect.y = -rand() % groundH * 3;

                pipe1_crossed_centre = false;
                pipe1_crossed = false;
                pipe1_failed = false;
            }
            if (tube2.rect.x <= (ScreenW - tube2.rect.w) / 2 && !pipe2_crossed_centre)
            {
                pipe2_crossed_centre = true;
            }
            if (tube2.rect.x <= (deerX - (tube2.rect.w - deer.dstrect.w)) && !pipe2_crossed)
            {
                score++;
                if (score > sessionHighscore)
                {
                    sessionHighscore = score;
                }
                if (tube2type == bonus)
                {
                    bonusLvl(sessionHighscore ,move, deerX, deerAnimationSize, deerTextureW, gameOver, gamehappening, mainMenu, keyboardState, deer, score, ren, ground, scoreText);
                    bonushappend = true;
                }
                if (tube2type == health)
                {
                    lives++;
                }
                pipe2_crossed = true;
            } 
            if (pipe2generated)
            {
                tube2type = generator();
                tube2.texture = tubeTexture[tube2type];
                tube2.rect.x = ScreenW;
                tube2.rect.y = -rand() % groundH * 3;
                pipe2generated = false;
                _pipe2generated = true;
            }
            //tube2 physics
            tube2.rect.x -= tubeSpeed;

            //tube1 failure check
            if (!pipe2_failed && tube2.rect.x < deerX + deer.dstrect.w  && tube2.rect.x > deerX - tube2.rect.w && (deer.dstrect.y <= tube2.rect.y + tubeHalfY - freeSpaceY || deer.dstrect.y >= tube2.rect.y + tube2.rect.h - tubeHalfY - deer.dstrect.h))
            {
                lives--;
                if (lives == 0)
                {
                    gameOver = true;
                    gamehappening = false;
                }
                pipe2_failed = true;
            }
            if (tube2.rect.x <= -tube2.rect.w)
            {
                tube2type = generator();
                tube2.texture = tubeTexture[tube2type];
                tube2.rect.x = ScreenW;
                tube2.rect.y = -rand() % groundH * 3;

                pipe2_crossed_centre = false;
                pipe2_crossed = false;
                pipe2_failed = false;
            }
            scoreText.texture = renderText(to_string(score), scoreText.color, ren, scoreText.fontSize);
            SDL_QueryTexture(scoreText.texture, nullptr, nullptr, &scoreText.rect.w, &scoreText.rect.h);
            scoreText.rect.x = (ScreenW - scoreText.rect.w) / 2;
            
            livesText.texture = renderText(to_string(lives), livesText.color, ren, livesText.fontSize);
            SDL_QueryTexture(livesText.texture, nullptr, nullptr, &livesText.rect.w, &livesText.rect.h);

            SH.texture = renderText(to_string(sessionHighscore), SH.color, ren, SH.fontSize);
            SDL_QueryTexture(SH.texture, nullptr, nullptr, &SH.rect.w, &SH.rect.h);
            SH.rect.x = ScreenW - SH.rect.w - textCoords;

            SDL_RenderCopy(ren, gameBg, nullptr, nullptr);
            SDL_RenderCopy(ren, tube1.texture, nullptr, &tube1.rect);
            SDL_RenderCopy(ren, tube2.texture, nullptr, &tube2.rect);
            SDL_RenderCopy(ren, deer.texture, &deer.srcrect, &deer.dstrect);
            SDL_RenderCopy(ren, ground[defGr].texture, nullptr, &ground[defGr].rect);
            SDL_RenderCopy(ren, scoreText.texture, nullptr, &scoreText.rect);
            SDL_RenderCopy(ren, livesText.texture, nullptr, &livesText.rect);
            SDL_RenderCopy(ren, SH.texture, nullptr, &SH.rect);
            SDL_RenderPresent(ren);

        }

        SDL_RenderCopy(ren, gameBg, nullptr, nullptr);
        SDL_RenderCopy(ren, tube1.texture, nullptr, &tube1.rect);
        SDL_RenderCopy(ren, tube2.texture, nullptr, &tube2.rect);
        SDL_RenderCopy(ren, deer.texture, &deer.srcrect, &deer.dstrect);
        SDL_RenderCopy(ren, ground[defGr].texture, nullptr, &ground[defGr].rect);
        SDL_RenderPresent(ren);

    }

    SDL_DestroyTexture(gameBg);
    SDL_DestroyTexture(tube1.texture);
    SDL_DestroyTexture(tube2.texture);
    SDL_DestroyTexture(deer.texture);
    SDL_DestroyTexture(scoreText.texture);
    SDL_DestroyTexture(ground[bonusGr].texture);
    SDL_DestroyTexture(ground[defGr].texture);

}


void settingsMenu(bool& mainMenu, SDL_Renderer *ren, SDL_Texture *menuBg, const Uint8 *keyboardState, short frameX, short frameY, short frameW, short frameH, short dY, obj *frame, Mix_Music *avaria, Mix_Music *blestyashie, Mix_Music *digi, Mix_Music *elka, Mix_Music *verka)
{
    enum blue {CTBS, AvariaBlue, BlestyashieBlue, DigiBlue, ElkaBlue, VerkaBlue};
    enum white {AvariaWhite, BlestyashieWhite, DigiWhite, ElkaWhite, VerkaWhite};
    enum enterCase {Avaria, Blestyashie, Digi, Elka, Verka};


    const short blues = 6,
                whites = 5;

    obj blue[blues];
    obj white[whites];

    frame[FrameBlue].rect = {frameX, 0/*inicialization in da cicle*/, frameW, frameH};
    frame[FrameWhite].rect = {frameX, frameY, frameW, frameH};

    blue[CTBS].texture = IMG_LoadTexture(ren, CTBS_p);    //Choose the background song
    blue[CTBS].rect.w = CTBSw;
    blue[CTBS].rect.h = CTBSh;
    blue[CTBS].rect.x = CTBSx;
    blue[CTBS].rect.y = CTBSy;

    //avaria text
    blue[AvariaBlue].texture = IMG_LoadTexture(ren, mAvariaBlue_p);
    white[AvariaWhite].texture = IMG_LoadTexture(ren, mAvariaWhite_p);
    blue[AvariaBlue].rect = white[AvariaWhite].rect = {frameX, frameY, frameW, frameH};

    //blestyashie text
    blue[BlestyashieBlue].texture = IMG_LoadTexture(ren, mBlestyashieBlue_p);
    white[BlestyashieWhite].texture = IMG_LoadTexture(ren, mBlestyashieWhite_p);
    blue[BlestyashieBlue].rect = white[BlestyashieWhite].rect = {frameX, frameY + frameH - dY, frameW, frameH};
    
    //digi text
    blue[DigiBlue].texture = IMG_LoadTexture(ren, mDigiBlue_p);
    white[DigiWhite].texture = IMG_LoadTexture(ren, mDigiWhite_p);
    blue[DigiBlue].rect = white[DigiWhite].rect = {frameX, frameY + 2*(frameH-dY), frameW, frameH};

    //elka text
    blue[ElkaBlue].texture = IMG_LoadTexture(ren, mElkaBlue_p);
    white[ElkaWhite].texture = IMG_LoadTexture(ren, mElkaWhite_p);
    blue[ElkaBlue].rect = white[ElkaWhite].rect = {frameX, frameY + 3*(frameH-dY), frameW, frameH};

    //verka text
    blue[VerkaBlue].texture = IMG_LoadTexture(ren, mVerkaBlue_p);
    white[VerkaWhite].texture = IMG_LoadTexture(ren, mVerkaWhite_p);
    blue[VerkaBlue].rect = white[VerkaWhite].rect = {frameX, frameY + 4*(frameH-dY), frameW, frameH};
    
    bool settingsMenu = true;
    short   buttons = 5,
            whiteTextPos = 0;

    SDL_Event SME;  //settings menu event

    while (settingsMenu)
    {
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, menuBg, nullptr, nullptr);
        SDL_RenderCopy(ren, blue[CTBS].texture, nullptr, &blue[CTBS].rect);

        frame[FrameBlue].rect.y = frameY;
        while(SDL_PollEvent(&SME))
        {
            if(SME.type == SDL_KEYDOWN)
            {
                if (keyboardState[SDL_SCANCODE_ESCAPE])
                {
                    settingsMenu = false;
                }
                if (keyboardState[SDL_SCANCODE_DOWN]||keyboardState[SDL_SCANCODE_S])
                {
                    if (frame[FrameWhite].rect.y <= frameY + 3*(frameH - dY))
                    {
                        frame[FrameWhite].rect.y += frame[FrameWhite].rect.h - dY;
                        whiteTextPos++;
                    }
                    else
                    {
                        whiteTextPos = 0;
                        frame[FrameWhite].rect.y = frameY;
                    }
                }
                if (keyboardState[SDL_SCANCODE_W]||keyboardState[SDL_SCANCODE_UP])
                {   
                    if (frame[FrameWhite].rect.y >= frameY + frameH - dY)
                    {
                        frame[FrameWhite].rect.y -= frame[FrameWhite].rect.h -dY;
                        whiteTextPos--;
                    }
                    else
                    {
                        frame[FrameWhite].rect.y = frameY + 4*(frameH - dY);
                        whiteTextPos = buttons - 1;
                    }
                }
                if (keyboardState[SDL_SCANCODE_RETURN]||keyboardState[SDL_SCANCODE_KP_ENTER])
                {
                    Mix_HaltMusic();
                    switch(whiteTextPos)
                    {
                    case(Avaria):
                        Mix_PlayMusic(avaria , -1);
                        break;
                    case(Blestyashie):
                        Mix_PlayMusic(blestyashie , -1);
                        break;
                    case(Digi):
                        Mix_PlayMusic(digi , -1);
                        break;
                    case(Elka):
                        Mix_PlayMusic(elka , -1);
                        break;
                    case(Verka):
                        Mix_PlayMusic(verka , -1);
                    }
                }
            }/*
            if (SME.type == SDL_QUIT)
            {
                settingsMenu = false;
                mainMenu = false;
            }*/
        }
        /*for (int i = 0; i < buttons; i++)
        {
            SDL_RenderCopy(ren, frame[FrameBlue].texture, nullptr, &frame[FrameBlue].rect);
            frame[FrameBlue].rect.y += frame[FrameBlue].rect.h - dY;
        }*/
        SDL_RenderCopy(ren, blue[AvariaBlue].texture, nullptr, &blue[AvariaBlue].rect);
        SDL_RenderCopy(ren, blue[BlestyashieBlue].texture, nullptr, &blue[BlestyashieBlue].rect);
        SDL_RenderCopy(ren, blue[DigiBlue].texture, nullptr, &blue[DigiBlue].rect);
        SDL_RenderCopy(ren, blue[ElkaBlue].texture, nullptr, &blue[ElkaBlue].rect);
        SDL_RenderCopy(ren, blue[VerkaBlue].texture, nullptr, &blue[VerkaBlue].rect);
        SDL_RenderCopy(ren, white[whiteTextPos].texture, nullptr, &white[whiteTextPos].rect);
        //SDL_RenderCopy(ren, frame[FrameWhite].texture, nullptr, &frame[FrameWhite].rect);
        SDL_RenderPresent(ren);
    }

    for (int i = 0; i < blues; i++)
        SDL_DestroyTexture(blue[i].texture);
    for (int i = 0; i < whites; i++)
        SDL_DestroyTexture(white[i].texture);
}


menuExitCode startMenu(const Uint8 *keyboardState, bool& mainMenu, SDL_Renderer *ren, Mix_Music *avaria, Mix_Music *blestyashie, Mix_Music *digi, Mix_Music *elka, Mix_Music *verka)
{
    ////
    SDL_Texture *menuBg = IMG_LoadTexture(ren, menubg_p);

    enum blue {WTTCDG, PlayBlue, SettingsBlue, QuitBlue}; //WTTCDG = Welcome to the christmas deer game
    enum white {PlayWhite, SettingsWhite, QuitWhite};
    enum enterCase {game, settings, quit};

    const short blues = 4,
                whites = 3,
                frames = 2;

    obj blue[blues];
    obj white[whites];
    obj frame[frames];

    //blue frame
    frame[FrameBlue].texture = IMG_LoadTexture(ren, frameBlue_p);
    frame[FrameBlue].rect = {frameX, 0/*inicialization in da cicle*/, frameW, frameH};

    //white frame
    frame[FrameWhite].texture = IMG_LoadTexture(ren, frameWhite_p);
    frame[FrameWhite].rect = {frameX, frameY, frameW, frameH};

    //head text
    blue[WTTCDG].texture = IMG_LoadTexture(ren, textWTTCDG_p);
    blue[WTTCDG].rect.w = WTTCDGw;
    blue[WTTCDG].rect.h = WTTCDGh;
    blue[WTTCDG].rect.x = WTTCDGx;
    blue[WTTCDG].rect.y = WTTCDGy;

    //play text
    blue[PlayBlue].texture = IMG_LoadTexture(ren, textPlayBlue_p);
    white[PlayWhite].texture = IMG_LoadTexture(ren, textPlayWhite_p);
    blue[PlayBlue].rect = white[PlayWhite].rect = {frameX, frameY, frameW, frameH};

    //settings text
    blue[SettingsBlue].texture = IMG_LoadTexture(ren, textSettingsBlue_p);
    white[SettingsWhite].texture = IMG_LoadTexture(ren, textSettingsWhite_p);
    blue[SettingsBlue].rect = white[SettingsWhite].rect = {frameX, frameY + frameH - dY, frameW, frameH};
    
    //quit text
    blue[QuitBlue].texture = IMG_LoadTexture(ren, textQuitBlue_p);
    white[QuitWhite].texture = IMG_LoadTexture(ren, textQuitWhite_p);
    blue[QuitBlue].rect = white[QuitWhite].rect = {frameX, frameY + 2*(frameH-dY), frameW, frameH};
    
    short   buttons = 3,
            whiteTextPos = 0;

    SDL_Event menuEvent;

    while (mainMenu)
    {
        SDL_ShowCursor(SDL_DISABLE);
        SDL_RenderClear(ren);
        frame[FrameBlue].rect.y = frameY;
        while(SDL_PollEvent(&menuEvent))
        {
            if(menuEvent.type == SDL_KEYDOWN)
            {
                if (keyboardState[SDL_SCANCODE_DOWN]||keyboardState[SDL_SCANCODE_S])
                {
                    if (frame[FrameWhite].rect.y <= frameY + frameH - dY)
                    {
                        frame[FrameWhite].rect.y += frame[FrameWhite].rect.h - dY;
                        whiteTextPos++;
                    }
                    else
                    {
                        whiteTextPos = 0;
                        frame[FrameWhite].rect.y = frameY;
                    }
                }
                if (keyboardState[SDL_SCANCODE_W]||keyboardState[SDL_SCANCODE_UP])
                {   
                    if (frame[FrameWhite].rect.y >= frameY + frameH - dY)
                    {
                        frame[FrameWhite].rect.y -= frame[FrameWhite].rect.h -dY;
                        whiteTextPos--;
                    }
                    else
                    {
                        frame[FrameWhite].rect.y = frameY + 2*(frameH - dY);
                        whiteTextPos = buttons - 1;
                    }
                }
                if (keyboardState[SDL_SCANCODE_RETURN]||keyboardState[SDL_SCANCODE_KP_ENTER])
                {
                    switch(whiteTextPos)
                    {
                    case(game):
                        return gameStart;
                        break;
                    case(settings):
                        settingsMenu(mainMenu, ren, menuBg, keyboardState, frameX, frameY, frameW, frameH, dY, frame, avaria, blestyashie, digi, elka, verka);
                        whiteTextPos = 0;
                        frame[FrameWhite].rect.y = frameY;
                        break;
                    case(quit):
                        return quitApp;
                    }
                }
            }
        }
        SDL_RenderCopy(ren, menuBg, nullptr, nullptr);
        SDL_RenderCopy(ren, blue[WTTCDG].texture, nullptr, &blue[WTTCDG].rect);
        //button shown
        /*for (int i = 0; i < buttons; i++)
        {
            SDL_RenderCopy(ren, frame[FrameBlue].texture, nullptr, &frame[FrameBlue].rect);
            frame[FrameBlue].rect.y += frame[FrameBlue].rect.h - dY;
        }*/
        //text render
        SDL_RenderCopy(ren, blue[PlayBlue].texture, nullptr, &blue[PlayBlue].rect);
        SDL_RenderCopy(ren, blue[SettingsBlue].texture, nullptr, &blue[SettingsBlue].rect);
        SDL_RenderCopy(ren, blue[QuitBlue].texture, nullptr, &blue[QuitBlue].rect);
        SDL_RenderCopy(ren, white[whiteTextPos].texture, nullptr, &white[whiteTextPos].rect);
        //SDL_RenderCopy(ren, frame[FrameWhite].texture, nullptr, &frame[FrameWhite].rect);
        SDL_RenderPresent(ren);
    }
    
    SDL_DestroyTexture(menuBg);
    for (int i = 0; i < blues; i++)
        SDL_DestroyTexture(blue[i].texture);
    for (int i = 0; i < whites; i++)
        SDL_DestroyTexture(white[i].texture);

    return quitApp;
}


int main()
{
    srand(time(nullptr));

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        cout << SDL_GetError() << endl;
        return 1;
    }

    if (TTF_Init() != 0)
    {
        cout << TTF_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
    {
        cout << Mix_GetError() << endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Christmas Deer", 0, 0, ScreenW, ScreenH, SDL_WINDOW_FULLSCREEN);

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    Mix_Music *avaria = Mix_LoadMUS("audio/avaria.mp3");
    Mix_Music *blestyashie = Mix_LoadMUS("audio/blestyashie.mp3");
    Mix_Music *digi = Mix_LoadMUS("audio/digi.mp3");
    Mix_Music *elka = Mix_LoadMUS("audio/elka.mp3");
    Mix_Music *verka = Mix_LoadMUS("audio/verka.mp3");


    SDL_Event event;
    bool quit = false;
    const Uint8 *keyboardState = SDL_GetKeyboardState(nullptr);
    bool mainMenu = true;
    unsigned int sessionHighscore = 0;

    while (!quit)
    {
        if(startMenu(keyboardState, mainMenu, ren, avaria, blestyashie, digi, elka, verka))
        {
            gameLoop(sessionHighscore, ren, keyboardState, mainMenu);
        }
        else
        {
            quit = true;
        }
    }

    Mix_FreeMusic(avaria);
    Mix_FreeMusic(blestyashie);
    Mix_FreeMusic(digi);
    Mix_FreeMusic(elka);
    Mix_FreeMusic(verka);

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);

    SDL_Quit();
    TTF_Quit();
    Mix_Quit();

    return 0;
}