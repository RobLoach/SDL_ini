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
    SDL_ini *ini = INI_Create();
    INI_SetString(ini,  NULL,    "title",        "My Game");
    INI_SetInt(ini,     "Video", "width",      1920);
    INI_SetInt(ini,     "Video", "height",     1080);
    INI_SetBoolean(ini, "Video", "fullscreen", true);
    INI_SetFloat(ini,   "Audio", "volume",     0.85f);
    INI_Save(ini, "settings.ini");
    INI_Destroy(ini);

    // Loading
    SDL_ini *cfg = INI_Load("settings.ini");
    int w = (int)INI_GetInt(cfg, "Video", "width", 1280);
    bool fs = INI_GetBoolean(cfg, "Video", "fullscreen", false);
    INI_Destroy(cfg);

    SDL_Quit();
    return 0;
}
```

Produces `settings.ini`:

```ini
app = My Game

[Video]
width = 1920
height = 1080
fullscreen = true

[Audio]
volume = 0.85
```

## API Reference

```c
// SDL_ini

SDL_ini *INI_Create(void);
SDL_ini *INI_Load_IO(SDL_IOStream *src, bool closeio);
SDL_ini *INI_Load(const char *file);
bool INI_Save_IO(const SDL_ini *ini, SDL_IOStream *dst, bool closeio);
bool INI_Save(const SDL_ini *ini, const char *file);
void INI_Destroy(SDL_ini *ini);

// Get

const char *INI_GetString(const SDL_ini *ini, const char *section, const char *key, const char *default_value);
Sint64 INI_GetInt(const SDL_ini *ini, const char *section, const char *key, Sint64 default_value);
float INI_GetFloat(const SDL_ini *ini, const char *section, const char *key, float default_value);
double INI_GetDouble(const SDL_ini *ini, const char *section, const char *key, double default_value);
bool INI_GetBoolean(const SDL_ini *ini, const char *section, const char *key, bool default_value);

// Set

bool INI_SetString(SDL_ini *ini, const char *section, const char *key, const char *value);
bool INI_SetInt(SDL_ini *ini, const char *section, const char *key, Sint64 value);
bool INI_SetFloat(SDL_ini *ini, const char *section, const char *key, float value);
bool INI_SetDouble(SDL_ini *ini, const char *section, const char *key, double value);
bool INI_SetBoolean(SDL_ini *ini, const char *section, const char *key, bool value);
bool INI_HasKey(const SDL_ini *ini, const char *section, const char *key);
bool INI_HasSection(const SDL_ini *ini, const char *section);
bool INI_RemoveKey(SDL_ini *ini, const char *section, const char *key);
bool INI_RemoveSection(SDL_ini *ini, const char *section);

// Enumerate

typedef void (SDLCALL *INI_EnumerateSectionsCallback)(void *userdata, const SDL_ini *ini, const char *section);
typedef void (SDLCALL *INI_EnumerateKeysCallback)(void *userdata, const SDL_ini *ini, const char* section, const char *key,  const char *value);
void INI_EnumerateSections(const SDL_ini *ini, INI_EnumerateSectionsCallback callback, void *userdata);
void INI_EnumerateKeys(const SDL_ini *ini, const char *section, INI_EnumerateKeysCallback callback, void *userdata);
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
cc test/SDL_ini_test.c -o SDL_ini_test $(pkg-config --cflags --libs sdl3)
./SDL_ini_test
```

### Documentation

```bash
doxygen .Doxyfile
```

## License

[zlib](LICENSE)
