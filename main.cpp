// g++  -std=c++11  -DmacOS=1 -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2 -I Apps
// -I Source -I . Apps/*cpp Source/*cpp Targets/macOS/*.cpp  -o Aq g++  -c -std=c++11
// -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks   *.cpp
// g++  -std=c++11  -DmacOS=1 -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2  -o Aq
// g++  -std=c++11  -DmacOS=1 -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2  -o Aq
// *.cpp *.o

#include <SDL2/SDL.h>

#include "AquaOS.h"

SDL_Surface*  surface;
SDL_Renderer* renderer = NULL;
SDL_Window*   window = NULL;

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("AquaOS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 320 * 3, 240 * 3,
                              SDL_WINDOW_ALLOW_HIGHDPI);
    surface = SDL_GetWindowSurface(window);
    int       down = 0;
    SDL_Event windowEvent;
    setup();
    SDL_UpdateWindowSurface(window);
    int running = 1;
    while (running) {
        SDL_Delay(20);
        loop();
        SDL_UpdateWindowSurface(window);
        while (SDL_PollEvent(&windowEvent)) {
            switch (windowEvent.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    down = 1;
                    setTouchX(windowEvent.motion.x);
                    setTouchY(windowEvent.motion.y);
                    setMouseUp(1);
                    loop();
                    SDL_UpdateWindowSurface(window);
                    break;
                case SDL_MOUSEMOTION:
                    // This is a piece of garbage
                    if (down && windowEvent.motion.y > 650) {
                        setTouchX(windowEvent.motion.x);
                        setTouchY(windowEvent.motion.y);
                        setMouseUp(1);
                        loop();
                        SDL_UpdateWindowSurface(window);
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    down = 0;
                    break;
            }
        }
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
