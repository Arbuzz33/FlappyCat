#include "settings.h"

int main (int argc, char ** args) {
    init();
    load_assets();

    Mix_PlayMusic(backMusic, -1);

    while(run) {
        while(SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT) run = false;
            if(e.type == SDL_KEYDOWN){
                if (e.key.keysym.sym == SDLK_UP){
                    tap_show = false;
                    true_jump_state = true;
                    Mix_PlayChannel(1, hopSound, 0);
                }
                if(life == false && e.key.keysym.sym == SDLK_SPACE) restart();
            }
        }
    blit_assets();
    give_coords_to_assets();
    is_game_over();
    if(tap_show){
        show_tap();
        SDL_Delay(500);
    }
    if(!tap_show && life){
        tap_coords.x = 640;
        tap_coords.y = 0;
        gravity();
        move_background();
        move_pipes();
        slowing_the_jump();
        SDL_Delay(6);
    }
    SDL_UpdateWindowSurface(win);
    }
    return quit();
}