# SDL_ini

A lightweight, single-header C library for reading and writing **INI configuration files** with [SDL3](https://libsdl.org/).

## Features

- **Single-header**: Drop `SDL_ini.h` into your project
- **Pure SDL3**: Uses only SDL3 APIs (`SDL_malloc`, `SDL_IOStream`, `SDL_asprintf`, etc)
- **Typed Properties**: `String`, `Int`, `Float`, `Double`, `Boolean`
- **Case-Insensitive**: Section and key lookups can be any case
- **Comments**: `#` and `;` comments and blank lines are preserved
- **Quoted Values**: Values can be wrapped in `"double quotes"` to preserve leading/trailing whitespace
- **Globals**: A `NULL` section appears before the first `[section]`
- **Enumeration**: Iterate through sections and keys with callbacks

## Usage

In **exactly one** C source file, define the implementation before including the header:

```c
#define SDL_INI_IMPLEMENTATION
#include "SDL_ini.h"
```

In all other files, include the header normally:

```c
#include "SDL_ini.h"
```

## Quick Example

```c
#define SDL_INI_IMPLEMENTATION
#include "SDL_ini.h"

int main(int argc, char *argv[])
{
    SDL_Init(0);

    // Creating and Saving
    SDL_ini *ini = SDL_CreateIni();
    SDL_SetIniString(ini,  NULL,    "app",        "MyGame");
    SDL_SetIniInt(ini,     "Video", "width",      1920);
    SDL_SetIniInt(ini,     "Video", "height",     1080);
    SDL_SetIniBoolean(ini, "Video", "fullscreen", true);
    SDL_SetIniFloat(ini,   "Audio", "volume",     0.85f);
    SDL_SaveIni(ini, "settings.ini");
    SDL_DestroyIni(ini);

    // Loading
    SDL_ini *cfg = SDL_LoadIni("settings.ini");
    int w = (int)SDL_GetIniInt(cfg, "Video", "width", 1280);
    bool fs = SDL_GetIniBoolean(cfg, "Video", "fullscreen", false);
    SDL_DestroyIni(cfg);

    SDL_Quit();
    return 0;
}
```

Produces `settings.ini`:

```ini
app = MyGame

[Video]
width = 1920
height = 1080
fullscreen = true

[Audio]
volume = 0.85
```

## API Reference

``` c
// SDL_ini

SDL_ini *SDL_CreateIni(void);
SDL_ini *SDL_LoadIni_IO(SDL_IOStream *src, bool closeio);
SDL_ini *SDL_LoadIni(const char *file);
bool SDL_SaveIni_IO(const SDL_ini *ini, SDL_IOStream *dst, bool closeio);
bool SDL_SaveIni(const SDL_ini *ini, const char *file);
void SDL_DestroyIni(SDL_ini *ini);

// Get

const char *SDL_GetIniString(const SDL_ini *ini, const char *section, const char *key, const char *default_value);
Sint64 SDL_GetIniInt(const SDL_ini *ini, const char *section, const char *key, Sint64 default_value);
float SDL_GetIniFloat(const SDL_ini *ini, const char *section, const char *key, float default_value);
double SDL_GetIniDouble(const SDL_ini *ini, const char *section, const char *key, double default_value);
bool SDL_GetIniBoolean(const SDL_ini *ini, const char *section, const char *key, bool default_value);

// Set

bool SDL_SetIniString(SDL_ini *ini, const char *section, const char *key, const char *value);
bool SDL_SetIniInt(SDL_ini *ini, const char *section, const char *key, Sint64 value);
bool SDL_SetIniFloat(SDL_ini *ini, const char *section, const char *key, float value);
bool SDL_SetIniDouble(SDL_ini *ini, const char *section, const char *key, double value);
bool SDL_SetIniBoolean(SDL_ini *ini, const char *section, const char *key, bool value);
bool SDL_DeleteIniKey(SDL_ini *ini, const char *section, const char *key);
bool SDL_DeleteIniSection(SDL_ini *ini, const char *section);

// Enumerate

typedef void (SDLCALL *SDL_EnumerateIniSectionsCallback)(const SDL_ini *ini, const char *section, void *userdata);
typedef void (SDLCALL *SDL_EnumerateIniKeysCallback)(const SDL_ini *ini, const char *key,  const char *value, void *userdata);
void SDL_EnumerateIniSections(const SDL_ini *ini, SDL_EnumerateIniSectionsCallback callback, void *userdata);
void SDL_EnumerateIniKeys(const SDL_ini *ini, const char *section, SDL_EnumerateIniKeysCallback callback, void *userdata);
```

## INI File Format

```ini
; Comments start with ; or #
# This is also a comment

; Keys before any [section] belong in the NULL section
app_name = My Application

[Display]
; Values can optionally be double-quoted
width = 1920
title = "  My Game  "

[Flags]
fullscreen = true
vsync = off
```

- **Sections** are delimited by `[name]`
- **Keys** are separated from values by `=`
- **Comments** begin with `;` or `#`
- **Quoted values** — surrounding `"double quotes"` are stripped on load and added on save when the value contains leading/trailing whitespace or is empty
- **Lookups** are case-insensitive for both section names and keys

## Building

### CMake

```bash
cmake -B build
cmake --build build
ctest --test-dir build
```

### Manual

```bash
cc test_SDL_ini.c -o test_SDL_ini $(pkg-config --cflags --libs sdl3)
./test_SDL_ini
```

## License

[zlib](LICENSE)
