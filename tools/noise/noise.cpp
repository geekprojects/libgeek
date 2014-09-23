
#include <signal.h>

#include <SDL.h>
#include <SDL_audio.h>
#include <geek/core-random.h>

Geek::Core::Random g_random;
static bool g_done = false;

void done(int sig)
{
    g_done = true;
}

void SDLCALL audio_callback(void *unused, Uint8 *stream, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        uint32_t r = g_random.rand32();
        stream[i] = r & 0xff;
    }
}

int main(int argc, char** argv)
{

    int res;
    res = SDL_Init(SDL_INIT_AUDIO);
    if (res < 0)
    {
        fprintf(stderr, "noise: Couldn't initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_AudioSpec spec;
    spec.callback = audio_callback;
    spec.freq = 22050;
    spec.format = AUDIO_U8;
    spec.channels = 2;
    spec.silence = 0;

    res = SDL_OpenAudio(&spec, NULL);
    if (res < 0)
    {
        fprintf(stderr, "noise: Couldn't initialize audio: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    signal(SIGHUP, done);
    signal(SIGINT, done);
    signal(SIGQUIT, done);
    signal(SIGTERM, done);

    SDL_PauseAudio(0);

    while (!g_done)
    {
        SDL_Delay(100);
    }

    SDL_CloseAudio();
    SDL_Quit();

    return 0;
}

