#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <random>
#include <string.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
bool life = true;
bool run = true;
int score = 0;
int best_score = 0;
int compare_score = 1;
bool true_jump_state = false;
bool tap_show = true;
bool tap_change = false;
int jump_count = 0;
bool jump_state = false;
int g_force = 1;
int collected_grav = 0;
int alt_g_force = 5;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distrib(110, 333);
SDL_Color color = { 0, 0, 0, 0 };
SDL_Color color1 = {255, 255, 255, 255};
SDL_Window   *win            = nullptr;
SDL_Surface  *scr            = nullptr;
SDL_Renderer *renderer       = nullptr;
SDL_Surface  *background     = nullptr;
SDL_Surface  *background_rev = nullptr;
SDL_Surface  *land           = nullptr;
SDL_Surface  *player         = nullptr;
SDL_Surface  *pipe           = nullptr;
SDL_Surface  *pipe_rev       = nullptr;
SDL_Surface  *tap            = nullptr;
SDL_Surface  *over_surf      = nullptr;

Mix_Music   *backMusic       = nullptr;
Mix_Music   *menuMusic       = nullptr;
Mix_Chunk   *hopSound        = nullptr;
Mix_Chunk   *bonkSound       = nullptr;


int pl_x = 140;
int pl_y = 180;

int pi_x = 640;
int pi_y = 230;
int pi2_x = 640;
int pi2_y = -300;

int ba_x = 0;
int ba_y = 0;
int ba2_x = 640;

int l1_x = 0;
int l_y = 430;
int l2_x = 640;

SDL_Event e;
SDL_Rect land1_coords;
SDL_Rect land2_coords;
SDL_Rect pipe_coords;
SDL_Rect pipe2_coords;
SDL_Rect player_coords;
SDL_Rect background_coords;
SDL_Rect background2_coords;
SDL_Rect tap_coords;
SDL_Rect txt_coords;
SDL_Rect over_coords;
SDL_Rect best_coords;
SDL_Rect space_coords;
int rand_y(){
    return distrib(gen);
}

int init() {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    win = SDL_CreateWindow("Flappy Cat!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (win == NULL) return 1;
    scr = SDL_GetWindowSurface(win);
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT,2,2048);
    return 0;
}
int quit() {
    SDL_FreeSurface(background);
    SDL_FreeSurface(background_rev);
    SDL_FreeSurface(land);
    SDL_FreeSurface(player);
    SDL_FreeSurface(pipe);
    SDL_FreeSurface(pipe_rev);
    SDL_FreeSurface(tap);
    SDL_FreeSurface(over_surf);
    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}

void print_ttf(SDL_Surface *sDest, const char* message, char* font, int size, SDL_Color color, SDL_Rect dest){
    TTF_Font *fnt = TTF_OpenFont(font, size);
    SDL_Surface *sText = TTF_RenderText_Blended( fnt, message, color);
    SDL_BlitSurface( sText,NULL, sDest,&dest );
    SDL_FreeSurface( sText );
    TTF_CloseFont( fnt );
}

int load_assets() {
    background = SDL_LoadBMP("src/backgr.bmp");
    background_rev = SDL_LoadBMP("src/backgr_rev.bmp");
    land = SDL_LoadBMP("src/land.bmp");
    player = SDL_LoadBMP("src/player.bmp");
    pipe = SDL_LoadBMP("src/pipe.bmp");
    pipe_rev = SDL_LoadBMP("src/pipe_rev.bmp");
    tap = SDL_LoadBMP("src/Start.bmp");
    over_surf = SDL_LoadBMP("src/gameOver.bmp");

    backMusic = Mix_LoadMUS("src/sound/music_main.wav");
    menuMusic = Mix_LoadMUS("src/sound/music_menu.wav");
    hopSound = Mix_LoadWAV("src/sound/sfx_bounce.wav");
    bonkSound = Mix_LoadWAV("src/sound/sfx_bonk.wav");

    Mix_VolumeChunk(hopSound, 20);
    Mix_VolumeChunk(bonkSound, 200);
    Mix_VolumeMusic(75);
    return 0;
}

int blit_assets(){
    SDL_BlitSurface(background, NULL, scr, &background_coords);
    SDL_BlitSurface(background_rev, NULL, scr, &background2_coords);

    SDL_BlitSurface(player, NULL, scr, &player_coords);
    SDL_BlitSurface(pipe, NULL, scr, &pipe_coords);
    SDL_BlitSurface(pipe_rev, NULL, scr, &pipe2_coords);
    SDL_BlitSurface(land, NULL, scr, &land1_coords);
    SDL_BlitSurface(land, NULL, scr, &land2_coords);
    SDL_BlitSurface(tap, NULL, scr, &tap_coords);
    return 0;
}

void give_coords_to_assets(){
    player_coords.x = pl_x;
    player_coords.y = pl_y;
    pipe_coords.x = pi_x;
    pipe_coords.y = pi_y;
    pipe2_coords.x = pi2_x;
    pipe2_coords.y = pi2_y;

    background_coords.x = ba_x;
    background_coords.y = ba_y;
    background2_coords.x = ba2_x;
    background2_coords.y = ba_y;
    land1_coords.x = l1_x;
    land1_coords.y = l_y;
    land2_coords.x = l2_x;
    land2_coords.y = l_y;

    over_coords.x = 195;
    over_coords.y = 136;

    space_coords.x = 150;
    space_coords.y = 340;
}

void move_background(){
    ba_x -= 1;
    ba2_x -= 1;
    l1_x -= 1;
    l2_x -= 1;
    if(ba_x <= -SCREEN_WIDTH) ba_x = SCREEN_WIDTH;
    if(ba2_x <= -SCREEN_WIDTH) ba2_x = SCREEN_WIDTH;
    if(l1_x <= -SCREEN_WIDTH) l1_x = SCREEN_WIDTH;
    if(l2_x <= -SCREEN_WIDTH) l2_x = SCREEN_WIDTH;
}

void move_pipes(){
    pi_x -= 1;
    pi2_x -= 1;
    int r_y;
    if(pi2_x <= -65){
        r_y = rand_y();
        pi2_x = SCREEN_WIDTH;
        pi2_y = r_y * -1;
    }
    if(pi_x <= -65){
        pi_x = SCREEN_WIDTH;
        pi_y = (373 - r_y) + 160;
    }
}

void jump(){
    jump_state = true;
    if(jump_state == true){
        g_force = 1;
        collected_grav = 1;
    }
    pl_y -= alt_g_force;
    jump_state = false;
}

void slowing_the_jump(){
    if(jump_count < 20 && true_jump_state == true){
        jump();
        jump_count += 1;
    }else{
        true_jump_state = false;
        jump_count = 0;
    }
}

void gravity(){
    pl_y += g_force;
    collected_grav += 1;
    if(collected_grav % 20 == 0) g_force += 1;
}

void is_game_over(){
    if((pl_x == pi_x+65) && (pl_y+66 <= pi_y && pi_y-180 <= pl_y) && life) score += 1;

    if((pl_x >= pi_x-65 && pl_x <= pi_x) && (pl_y+66 >= pi_y || pi_y-180 >= pl_y)){ life = false; Mix_PlayChannel(2, bonkSound, 0); pl_y = pi_y - 67; }
    if(pl_y >= 370){ life = false; Mix_PlayChannel(2, bonkSound, 0); pl_y = 369; }

    if(!life){
        if(score > best_score) best_score = score;

        SDL_BlitSurface(over_surf, NULL, scr, &over_coords);
        txt_coords.x = 390;
        txt_coords.y = 252;
        best_coords.x = 390;
        best_coords.y = 298;
        print_ttf(scr, std::to_string(score).c_str(), "FlappyF.ttf", 30, color, txt_coords);
        print_ttf(scr, std::to_string(best_score).c_str(), "FlappyF.ttf", 30, color, best_coords);
        print_ttf(scr, "Hit The Space!", "KarenFat.ttf", 48, color1, space_coords);

    }
    if(life){
        txt_coords.x = 300;
        txt_coords.y = 20;
        print_ttf(scr, std::to_string(score).c_str(), "FlappyF.ttf", 80, color, txt_coords);
    }
}

void restart(){
    score = 0;
    life = true;
    pl_x = 140;
    pl_y = 180;
    pi_x = 640;
    pi_y = 230;
    pi2_x = 640;
    pi2_y = -300;
    ba_x = 0;
    ba_y = 0;
    ba2_x = 640;
    l1_x = 0;
    l_y = 430;
    l2_x = 640;

    over_coords.x = 640;
    over_coords.y = 0;
    space_coords.x = 640;
    space_coords.y = 0;
    tap_show = true;
}

void show_tap(){
    tap_change = !tap_change;
    if(!tap_change){
        tap_coords.x = 640;
        tap_coords.y = 0;
    }else{
        tap_coords.x = 200;
        tap_coords.y = 200;
    }
}
