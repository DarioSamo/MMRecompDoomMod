#include "doomkeys.h"

#include "doomgeneric.h"
#include "helpers.hpp"

#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#define usleep(x) Sleep((x) / 1000)
#else
#include <unistd.h>
#include <limits.h>
#endif

#define KEYQUEUE_SIZE 16
#ifdef _WIN32
#define MAX_PATH_LENGTH MAX_PATH
#else
#define MAX_PATH_LENGTH PATH_MAX
#endif

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;
static char s_WadPath[MAX_PATH_LENGTH] = "";
static const char *s_Argv[] = { "", "-iwad", s_WadPath };

static void addKeyToQueue(int pressed, unsigned char doomKey) {
    unsigned short keyData = (pressed << 8) | doomKey;
    s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
    s_KeyQueueWriteIndex++;
    s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

extern "C" {
    void DG_Init() {
        memset(s_KeyQueue, 0, KEYQUEUE_SIZE * sizeof(unsigned short));
    }

    void DG_DrawFrame() {
        // Does nothing.
    }

    void DG_SleepMs(uint32_t ms) {
        usleep(ms * 1000);
    }

    uint32_t DG_GetTicksMs() {
#ifdef _WIN32
        return GetTickCount();
#else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
    }

    int DG_GetKey(int *pressed, unsigned char *doomKey) {
        if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex) {
            // Key queue is empty.
            return 0;
        } else {
            unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
            s_KeyQueueReadIndex = (s_KeyQueueReadIndex + 1) % KEYQUEUE_SIZE;
            *pressed = keyData >> 8;
            *doomKey = keyData & 0xFF;
            return 1;
        }
    }

    uint32_t recomp_api_version = 1;

    void DG_SetWindowTitle(const char *title) {
        // Does nothing.
    }

    void DoomDLL_Initialize(uint8_t *rdram, recomp_context *ctx) {
#ifdef _WIN32
        if (!GetModuleFileNameA(NULL, s_WadPath, sizeof(s_WadPath))) {
            return;
        }
#else
        ssize_t len = readlink("/proc/self/exe", s_WadPath, sizeof(s_WadPath) - 1);
        if (len == -1) {
            return;
        }
        s_WadPath[len] = '\0';
#endif

        char *lastSlash = strrchr(s_WadPath, '/');
#ifdef _WIN32
        if (!lastSlash) {
            lastSlash = strrchr(s_WadPath, '\\');
        }
#endif
        if (lastSlash == nullptr) {
            return;
        }

        lastSlash++;
        *lastSlash = '\0';
        strcat(s_WadPath, "DOOM1.WAD");

        doomgeneric_Create(sizeof(s_Argv) / sizeof(char *), (char **)(s_Argv));
    }

    void DoomDLL_Tick(uint8_t *rdram, recomp_context *ctx) {
        doomgeneric_Tick();
    }

    void DoomDLL_Input(uint8_t *rdram, recomp_context *ctx) {
        unsigned int doomKey = _arg<0, unsigned int>(rdram, ctx);
        unsigned int pressed = _arg<1, unsigned int>(rdram, ctx);
        addKeyToQueue(pressed, doomKey);
    }

    void DoomDLL_ScreenCopy(uint8_t *rdram, recomp_context *ctx) {
        uint8_t *dstScreenBuffer = (uint8_t *)(_arg<0, void *>(rdram, ctx));
        for (uint32_t y = 0; y < DOOMGENERIC_RESY; y++) {
            for (uint32_t x = 0; x < DOOMGENERIC_RESX; x++) {
                pixel_t pixel = DG_ScreenBuffer[y * DOOMGENERIC_RESX + x];
                *(dstScreenBuffer++) = (pixel >> 24) & 0xFF;
                *(dstScreenBuffer++) = (pixel >> 0) & 0xFF;
                *(dstScreenBuffer++) = (pixel >> 8) & 0xFF;
                *(dstScreenBuffer++) = (pixel >> 16) & 0xFF;
            }
        }
    }

    void DoomDLL_ScreenWidth(uint8_t *rdram, recomp_context *ctx) {
        _return(ctx, (unsigned int)(DOOMGENERIC_RESX));
    }

    void DoomDLL_ScreenHeight(uint8_t *rdram, recomp_context *ctx) {
        _return(ctx, (unsigned int)(DOOMGENERIC_RESY));
    }
};
