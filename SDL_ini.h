/*
 * SDL_ini.h - Single-header INI file library for SDL3
 *
 * Website:
 *   https://github.com/robloach/SDL_ini
 *
 * Usage:
 *   In exactly one C source file, define the implementation before including:
 *
 *     #define SDL_INI_IMPLEMENTATION
 *     #include "SDL_ini.h"
 *
 *   In all other files, just include the header normally:
 *
 *     #include "SDL_ini.h"
 *
 * License:
 *   SDL_ini: Single-header C library for reading/writing INI files with SDL3
 *   Copyright (C) 2026 Rob Loach (https://robloach.net)
 *
 *   This software is provided 'as-is', without any express or implied
 *   warranty.  In no event will the authors be held liable for any damages
 *   arising from the use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose,
 *   including commercial applications, and to alter it and redistribute it
 *   freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not
 *       claim that you wrote the original software. If you use this software
 *       in a product, an acknowledgment in the product documentation would be
 *       appreciated but is not required.
 *   2. Altered source versions must be plainly marked as such, and must not be
 *       misrepresented as being the original software.
 *   3. This notice may not be removed or altered from any source distribution.
 */

#ifndef SDL_INI_H_
#define SDL_INI_H_

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * An SDL_ini instance to handle the data for an INI file.
 * \see INI_Create()
 * \see INI_Destroy()
 */
typedef struct SDL_ini SDL_ini;

#define SDL_INI_MAJOR_VERSION 1
#define SDL_INI_MINOR_VERSION 1
#define SDL_INI_MICRO_VERSION 0
#define SDL_INI_VERSION \
    SDL_VERSIONNUM(SDL_INI_MAJOR_VERSION, SDL_INI_MINOR_VERSION, SDL_INI_MICRO_VERSION)
#define SDL_INI_VERSION_ATLEAST(X, Y, Z) \
    (SDL_INI_VERSION >= SDL_VERSIONNUM(X, Y, Z))

/**
 * \defgroup SDL_ini SDL_ini
 * @{
 */

/**
 * Get the version of SDL_ini that is linked against.
 *
 * \returns the version number as SDL_VERSIONNUM(major, minor, micro).
 */
int INI_GetVersion(void);

/**
 * Create an empty INI object.
 *
 * \returns a new SDL_ini on success, or NULL on failure; call SDL_GetError() for more information.
 * \see INI_Destroy()
 */
SDL_ini *INI_Create(void);

/**
 * Load an INI file from an SDL_IOStream.
 *
 * \param src the SDL_IOStream to read from.
 * \param closeio if true, the stream is closed after reading.
 * \returns a new SDL_ini on success, or NULL on failure; call SDL_GetError() for more information.
 * \see INI_Save_IO()
 * \see INI_Load()
 */
SDL_ini *INI_Load_IO(SDL_IOStream *src, bool closeio);

/**
 * Load an INI file from a filesystem path.
 *
 * \param file the path to the INI file.
 * \returns a new SDL_ini on success, or NULL on failure; call SDL_GetError() for more information.
 * \see INI_Save()
 */
SDL_ini *INI_Load(const char *file);

/**
 * Load an INI from the given string.
 *
 * \param text the string containing the ini data.
 * \returns a new SDL_ini on success, or NULL on failure; call SDL_GetError() for more information.
 * \see INI_Load()
 */
SDL_ini *INI_LoadString(const char *text);

/**
 * Save an INI object to an SDL_IOStream.
 *
 * \param ini the SDL_ini to write.
 * \param dst the SDL_IOStream to write to.
 * \param closeio if true, the stream is closed after writing.
 * \returns true on success or false on failure; call SDL_GetError() for more information.
 * \see INI_Load_IO()
 * \see INI_Save()
 */
bool INI_Save_IO(const SDL_ini *ini, SDL_IOStream *dst, bool closeio);

/**
 * Save an INI object to a filesystem path.
 *
 * \param ini the SDL_ini to write.
 * \param file the path to write to.
 * \returns true on success or false on failure; call SDL_GetError() for more information.
 * \see INI_Load()
 */
bool INI_Save(const SDL_ini *ini, const char *file);

/**
 * Free an INI object and all associated memory.
 *
 * \param ini the SDL_ini to destroy. NULL is safely ignored.
 * \see INI_Create()
 * \see INI_Load()
 */
void INI_Destroy(SDL_ini *ini);

/**
 * Get a string value from the INI.
 *
 * \param ini the SDL_ini to query.
 * \param section section name (NULL or "" for the global section).
 * \param key the key to look up.
 * \param default_value returned when the key is not found.
 * \returns the value string, or default_value if not found.
 */
const char *INI_GetString(const SDL_ini *ini, const char *section, const char *key, const char *default_value);

/**
 * Get a signed 64-bit integer value from the INI.
 *
 * \param ini the SDL_ini to query.
 * \param section section name (NULL or "" for the global section).
 * \param key the key to look up.
 * \param default_value returned when the key is not found.
 * \returns the parsed value, or default_value if not found.
 * \see INI_GetString()
 */
Sint64 INI_GetInt(const SDL_ini *ini, const char *section, const char *key, Sint64 default_value);

/**
 * Get a floating-point value from the INI.
 *
 * \param ini the SDL_ini to query.
 * \param section section name (NULL or "" for the global section).
 * \param key the key to look up.
 * \param default_value returned when the key is not found.
 * \returns the parsed value, or default_value if not found.
 * \see INI_GetString()
 */
float INI_GetFloat(const SDL_ini *ini, const char *section, const char *key, float default_value);

/**
 * Get a double-precision floating-point value from the INI.
 *
 * \param ini the SDL_ini to query.
 * \param section section name (NULL or "" for the global section).
 * \param key the key to look up.
 * \param default_value returned when the key is not found.
 * \returns the parsed value, or default_value if not found.
 * \see INI_GetString()
 */
double INI_GetDouble(const SDL_ini *ini, const char *section, const char *key, double default_value);

/**
 * Get a boolean value from the INI.
 *
 * Recognises "1", "true", "yes", "on" as true and "0", "false", "no", "off"
 * as false (case-insensitive).
 *
 * \param ini the SDL_ini to query.
 * \param section section name (NULL or "" for the global section).
 * \param key the key to look up.
 * \param default_value returned when the key is not found or is unrecognised.
 * \returns the parsed value, or default_value if not found.
 * \see INI_GetString()
 */
bool INI_GetBoolean(const SDL_ini *ini, const char *section, const char *key, bool default_value);

/**
 * Set a string value in the INI.
 *
 * The section is created if it does not exist.
 *
 * \param ini the SDL_ini to modify.
 * \param section section name (NULL or "" for the global section).
 * \param key the key to set.
 * \param value the string value.
 * \returns true on success or false on failure.
 */
bool INI_SetString(SDL_ini *ini, const char *section, const char *key, const char *value);

/**
 * Set a signed 64-bit integer value in the INI.
 *
 * \param ini the SDL_ini to modify.
 * \param section section name (NULL or "" for the global section).
 * \param key the key to set.
 * \param value the integer value.
 * \returns true on success or false on failure.
 * \see INI_SetString()
 */
bool INI_SetInt(SDL_ini *ini, const char *section, const char *key, Sint64 value);

/**
 * Set a single-precision floating-point value in the INI.
 *
 * \param ini the SDL_ini to modify.
 * \param section section name (NULL or "" for the global section).
 * \param key the key to set.
 * \param value the float value.
 * \returns true on success or false on failure.
 * \see INI_SetString()
 */
bool INI_SetFloat(SDL_ini *ini, const char *section, const char *key, float value);

/**
 * Set a double-precision floating-point value in the INI.
 *
 * \param ini the SDL_ini to modify.
 * \param section section name (NULL or "" for the global section).
 * \param key the key to set.
 * \param value the double value.
 * \returns true on success or false on failure.
 * \see INI_SetString()
 */
bool INI_SetDouble(SDL_ini *ini, const char *section, const char *key, double value);

/**
 * Set a boolean value in the INI.
 *
 * Written as "true" or "false".
 *
 * \param ini the SDL_ini to modify.
 * \param section section name (NULL or "" for the global section).
 * \param key the key to set.
 * \param value the boolean value.
 * \returns true on success or false on failure.
 * \see INI_SetString()
 */
bool INI_SetBoolean(SDL_ini *ini, const char *section, const char *key, bool value);

/**
 * Check whether a key exists in a section.
 *
 * \param ini the SDL_ini to query.
 * \param section section name (NULL or "" for the global section).
 * \param key the key to look up.
 * \returns true if the key exists, false otherwise.
 * \see INI_GetString()
 * \see INI_RemoveKey()
 */
bool INI_HasKey(const SDL_ini *ini, const char *section, const char *key);

/**
 * Check whether a section exists and contains at least one key.
 *
 * Sections that hold only comments or blank lines are not considered present.
 *
 * \param ini the SDL_ini to query.
 * \param section the section name to check for.
 * \returns true if the section has at least one key, false otherwise.
 * \see INI_HasKey()
 * \see INI_RemoveSection()
 */
bool INI_HasSection(const SDL_ini *ini, const char *section);

/**
 * Delete a key from a section.
 *
 * \param ini the SDL_ini to modify.
 * \param section section name (NULL or "" for the global section).
 * \param key the key to remove.
 * \returns true if the key was found and removed, false otherwise.
 * \see INI_RemoveSection()
 */
bool INI_RemoveKey(SDL_ini *ini, const char *section, const char *key);

/**
 * Delete an entire section and all its keys.
 *
 * \param ini the SDL_ini to modify.
 * \param section section name (NULL or "" for the global section).
 * \returns true if the section was found and removed, false otherwise.
 * \see INI_RemoveKey()
 */
bool INI_RemoveSection(SDL_ini *ini, const char *section);

/**
 * Callback invoked for each section name during enumeration.
 *
 * \param userdata user-provided pointer.
 * \param section the section name (empty string for the global section).
 */
typedef void (SDLCALL *INI_EnumerateSectionsCallback)(void *userdata, const SDL_ini *ini, const char *section);

/**
 * Callback invoked for each key/value pair during enumeration.
 *
 * \param userdata user-provided pointer.
 * \param key the key name.
 * \param value the associated value.
 */
typedef void (SDLCALL *INI_EnumerateKeysCallback)(void *userdata, const SDL_ini *ini, const char* section, const char *key, const char *value);

/**
 * Enumerate all sections in the INI.
 *
 * \param ini the SDL_ini to enumerate.
 * \param callback called once per section.
 * \param userdata passed through to the callback.
 */
void INI_EnumerateSections(const SDL_ini *ini, INI_EnumerateSectionsCallback callback, void *userdata);

/**
 * Enumerate all key/value pairs in a section.
 *
 * \param ini the SDL_ini to enumerate.
 * \param section section name (NULL or "" for the global section).
 * \param callback called once per key.
 * \param userdata passed through to the callback.
 */
void INI_EnumerateKeys(const SDL_ini *ini, const char *section, INI_EnumerateKeysCallback callback, void *userdata);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#ifdef SDL_INI_IMPLEMENTATION
#ifndef SDL_INI_IMPLEMENTATION_ONCE
#define SDL_INI_IMPLEMENTATION_ONCE

/**
 * The type of a given SDL_ini_item.
 *
 * \see SDL_ini_item
 * \internal
 */
typedef enum SDL_ini_item_type {
    SDL_INI_ITEM_ENTRY, /* The SDL_ini_item is something like "key = value". */
    SDL_INI_ITEM_COMMENT, /* The SDL_ini_item is a comment, the the ; or # prefix. */
    SDL_INI_ITEM_BLANK /* The SDL_ini_item is an empty line, because empty lines are preserved. */
} SDL_ini_item_type;

/**
 * A single SDL_ini item, held within a section.
 * \see SDL_ini_section
 * \internal
 */
typedef struct SDL_ini_item {
    SDL_ini_item_type type;
    char *key; /* When the entry is a SDL_INI_ITEM_ENTRY, includes the key. */
    char *value; /* When the entry is a SDL_INI_ITEM_ENTRY, provides the value. */
    char *comment; /* When the entry is a SDL_INI_ITEM_COMMENT, this includes the full text, including ; or # prefix. */
} SDL_ini_item;

/**
 * An SDL_ini section, holding any number of items.
 * \see SDL_ini
 * \internal
 */
typedef struct SDL_ini_section {
    char *name; /** Empty string for the global section. */
    SDL_ini_item *items; /** The array of items that are held within this section. */
    int item_count; /** The number of items. */
    int item_capacity; /** The maximum capacity of the items array. */
} SDL_ini_section;

/**
 * An SDL_ini instance to handle the data for an INI file.
 * \see INI_Create()
 * \see INI_Destroy()
 */
struct SDL_ini {
    SDL_ini_section *sections;
    int section_count;
    int section_capacity;
};

/**
 * Normalise NULL section name to empty string for internal use.
 *
 * \internal
 */
static const char *INI__section_name(const char *section)
{
    return section ? section : "";
}

/**
 * Trim leading and trailing whitespace in-place.
 *
 * \returns pointer into the original buffer (does NOT allocate).
 *
 * \internal
 */
static char *INI__trim(char *s)
{
    while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') {
        ++s;
    }
    if (*s == '\0') {
        return s;
    }
    char *end = s + SDL_strlen(s) - 1;
    while (end > s && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) {
        --end;
    }
    *(end + 1) = '\0';
    return s;
}

/**
 * Un-escape backslash sequences in-place: \\ \" \n \r \t.
 *
 * The result is never longer than the input, so this is safe to do in place.
 *
 * \internal
 */
static void INI__unescape(char *s)
{
    char *d = s;
    for (char *p = s; *p; ++p) {
        if (*p == '\\' && p[1] != '\0') {
            ++p;
            switch (*p) {
            case 'n':  *d++ = '\n'; break;
            case 'r':  *d++ = '\r'; break;
            case 't':  *d++ = '\t'; break;
            case '\\': *d++ = '\\'; break;
            case '"':  *d++ = '"';  break;
            default:   *d++ = *p;   break;  // unknown escape: keep literal
            }
        } else {
            *d++ = *p;
        }
    }
    *d = '\0';
}

/**
 * Strip surrounding double quotes from a value in-place, un-escaping the contents.
 *
 * '"hello world"' becomes 'hello world'; '"a\"b"' becomes 'a"b'.
 *
 * \returns pointer into the original buffer (does NOT allocate).
 *
 * \internal
 */
static char *INI__unquote(char *s)
{
    size_t len = SDL_strlen(s);
    if (len >= 2 && s[0] == '"' && s[len - 1] == '"') {
        s[len - 1] = '\0';
        char *inner = s + 1;
        INI__unescape(inner);
        return inner;
    }
    return s;
}

/**
 * Allocate an escaped copy of s suitable for writing between double quotes.
 *
 * Escapes \\, ", newline, carriage return and tab so the value round-trips  through INI__unquote().
 *
 * \returns a newly allocated string (free with SDL_free), or NULL on OOM.
 *
 * \internal
 */
static char *INI__escape(const char *s)
{
    size_t len = SDL_strlen(s);
    char *out = (char *)SDL_malloc(len * 2 + 1);
    if (!out) {
        SDL_OutOfMemory();
        return NULL;
    }
    char *d = out;
    for (const char *p = s; *p; ++p) {
        switch (*p) {
        case '\\': *d++ = '\\'; *d++ = '\\'; break;
        case '"':  *d++ = '\\'; *d++ = '"';  break;
        case '\n': *d++ = '\\'; *d++ = 'n';  break;
        case '\r': *d++ = '\\'; *d++ = 'r';  break;
        case '\t': *d++ = '\\'; *d++ = 't';  break;
        default:   *d++ = *p; break;
        }
    }
    *d = '\0';
    return out;
}

/**
 * Checks if the value needs to be wrapped in double quotes when serializing.
 *
 * Empty string, leading/trailing whitespace, or any character must be
 * escaped (", backslash, newline, carriage return, tab).
 *
 * \returns true if the value needs to be wrapped in double quotes.
 *
 * \internal
 */
static bool INI__needs_quoting(const char *s)
{
    if (s[0] == '\0') {
        return true;
    }
    if (s[0] == ' ' || s[0] == '\t') {
        return true;
    }
    size_t len = SDL_strlen(s);
    if (s[len - 1] == ' ' || s[len - 1] == '\t') {
        return true;
    }
    for (const char *p = s; *p; ++p) {
        if (*p == '"' || *p == '\\' ||
            *p == '\n' || *p == '\r' || *p == '\t') {
            return true;
        }
    }
    return false;
}

/**
 * Find a section by name (case-insensitive).
 *
 * \returns NULL if not found.
 *
 * \internal
 */
static SDL_ini_section *INI__find_section(const SDL_ini *ini, const char *name)
{
    name = INI__section_name(name);
    for (int i = 0; i < ini->section_count; ++i) {
        if (SDL_strcasecmp(ini->sections[i].name, name) == 0) {
            return &ini->sections[i];
        }
    }
    return NULL;
}

/**
 * Find an ENTRY item by key within a section (case-insensitive).
 *
 * \internal
 */
static SDL_ini_item *INI__find_entry(const SDL_ini_section *sec, const char *key)
{
    for (int i = 0; i < sec->item_count; ++i) {
        if (sec->items[i].type == SDL_INI_ITEM_ENTRY &&
            SDL_strcasecmp(sec->items[i].key, key) == 0) {
            return &sec->items[i];
        }
    }
    return NULL;
}

/**
 * Ensure capacity for one more section.
 *
 * \returns false on OOM.
 *
 * \internal
 */
static bool INI__grow_sections(SDL_ini *ini)
{
    if (ini->section_count < ini->section_capacity) {
        return true;
    }
    int new_cap = ini->section_capacity ? ini->section_capacity * 2 : 4;
    SDL_ini_section *tmp = (SDL_ini_section *)SDL_realloc(
        ini->sections, (size_t)new_cap * sizeof(*tmp));
    if (!tmp) {
        return SDL_OutOfMemory();
    }
    ini->sections = tmp;
    ini->section_capacity = new_cap;
    return true;
}

/**
 * Ensure capacity for one more item in a section.
 *
 * \returns false on OOM.
 *
 * \internal
 */
static bool INI__grow_items(SDL_ini_section *sec)
{
    if (sec->item_count < sec->item_capacity) {
        return true;
    }
    int new_cap = sec->item_capacity ? sec->item_capacity * 2 : 8;
    SDL_ini_item *tmp = (SDL_ini_item *)SDL_realloc(
        sec->items, (size_t)new_cap * sizeof(*tmp));
    if (!tmp) {
        return SDL_OutOfMemory();
    }
    sec->items = tmp;
    sec->item_capacity = new_cap;
    return true;
}

/**
 * Add or get a section by name.
 *
 * \returns NULL on OOM.
 *
 * \internal
 */
static SDL_ini_section *INI__get_or_create_section(SDL_ini *ini, const char *name)
{
    name = INI__section_name(name);
    SDL_ini_section *sec = INI__find_section(ini, name);
    if (sec) {
        return sec;
    }
    if (!INI__grow_sections(ini)) {
        return NULL;
    }
    sec = &ini->sections[ini->section_count];
    SDL_memset(sec, 0, sizeof(*sec));
    sec->name = SDL_strdup(name);
    if (!sec->name) {
        SDL_OutOfMemory();
        return NULL;
    }
    ini->section_count++;
    return sec;
}

/**
 * Free all memory owned by a section (not the section struct itself).
 *
 * \internal
 */
static void INI__free_section_contents(SDL_ini_section *sec)
{
    for (int i = 0; i < sec->item_count; ++i) {
        if (sec->items[i].type == SDL_INI_ITEM_ENTRY) {
            SDL_free(sec->items[i].key);
            SDL_free(sec->items[i].value);
        } else if (sec->items[i].type == SDL_INI_ITEM_COMMENT) {
            SDL_free(sec->items[i].comment);
        }
        // BLANK items have no allocated memory.
    }
    SDL_free(sec->items);
    SDL_free(sec->name);
}

int INI_GetVersion(void)
{
    return SDL_INI_VERSION;
}

SDL_ini *INI_Create(void)
{
    SDL_ini *ini = (SDL_ini *)SDL_calloc(1, sizeof(*ini));
    if (!ini) {
        SDL_OutOfMemory();
    }
    return ini;
}

SDL_ini *INI_Load_IO(SDL_IOStream *src, bool closeio)
{
    if (!src) {
        SDL_SetError("INI_Load_IO: src is NULL");
        return NULL;
    }

    size_t datasize = 0;
    char *data = (char *)SDL_LoadFile_IO(src, &datasize, closeio);
    if (!data) {
        return NULL;  // SDL_GetError() already set
    }

    SDL_ini *ini = INI_Create();
    if (!ini) {
        SDL_free(data);
        return NULL;
    }

    // Current section name (empty = global).
    const char *cur_section = "";

    char *line = data;
    while (line && *line) {
        // End of Line
        char *eol = SDL_strchr(line, '\n');
        if (eol) {
            *eol = '\0';
        }

        char *trimmed = INI__trim(line);

        // Blank Line
        if (*trimmed == '\0') {
            SDL_ini_section *sec = INI__get_or_create_section(ini, cur_section);
            if (!sec) {
                INI_Destroy(ini);
                SDL_free(data);
                return NULL;
            }
            cur_section = sec->name;
            if (!INI__grow_items(sec)) {
                INI_Destroy(ini);
                SDL_free(data);
                return NULL;
            }
            SDL_ini_item *item = &sec->items[sec->item_count];
            SDL_memset(item, 0, sizeof(*item));
            item->type = SDL_INI_ITEM_BLANK;
            sec->item_count++;
            line = eol ? eol + 1 : NULL;
            continue;
        }

        // Comment
        if (*trimmed == '#' || *trimmed == ';') {
            SDL_ini_section *sec = INI__get_or_create_section(ini, cur_section);
            if (!sec) {
                INI_Destroy(ini);
                SDL_free(data);
                return NULL;
            }
            cur_section = sec->name;
            if (!INI__grow_items(sec)) {
                INI_Destroy(ini);
                SDL_free(data);
                return NULL;
            }
            SDL_ini_item *item = &sec->items[sec->item_count];
            SDL_memset(item, 0, sizeof(*item));
            item->type = SDL_INI_ITEM_COMMENT;
            item->comment = SDL_strdup(trimmed);
            if (!item->comment) {
                SDL_OutOfMemory();
                INI_Destroy(ini);
                SDL_free(data);
                return NULL;
            }
            sec->item_count++;
            line = eol ? eol + 1 : NULL;
            continue;
        }

        // Section
        if (*trimmed == '[') {
            char *close = SDL_strchr(trimmed, ']');
            if (close) {
                *close = '\0';
                char *sec_name = INI__trim(trimmed + 1);

                // Ensure section exists.
                SDL_ini_section *sec = INI__get_or_create_section(ini, sec_name);
                if (!sec) {
                    INI_Destroy(ini);
                    SDL_free(data);
                    return NULL;
                }
                cur_section = sec->name;  // point at the owned copy
            }
            line = eol ? eol + 1 : NULL;
            continue;
        }

        // Key Value
        char *eq = SDL_strchr(trimmed, '=');
        if (eq) {
            *eq = '\0';
            char *key   = INI__trim(trimmed);
            char *value = INI__unquote(INI__trim(eq + 1));

            // Make sure the current section exists.
            SDL_ini_section *sec = INI__get_or_create_section(ini, cur_section);
            if (!sec) {
                INI_Destroy(ini);
                SDL_free(data);
                return NULL;
            }

            // Grab the owned name pointer so cur_section stays valid.
            cur_section = sec->name;

            // Duplicate key in the same section: overwrite the existing value
            // instead of appending a second entry (matches set-path semantics).
            SDL_ini_item *existing = INI__find_entry(sec, key);
            if (existing) {
                char *new_value = SDL_strdup(value);
                if (!new_value) {
                    SDL_OutOfMemory();
                    INI_Destroy(ini);
                    SDL_free(data);
                    return NULL;
                }
                SDL_free(existing->value);
                existing->value = new_value;
                line = eol ? eol + 1 : NULL;
                continue;
            }

            if (!INI__grow_items(sec)) {
                INI_Destroy(ini);
                SDL_free(data);
                return NULL;
            }

            SDL_ini_item *item = &sec->items[sec->item_count];
            SDL_memset(item, 0, sizeof(*item));
            item->type  = SDL_INI_ITEM_ENTRY;
            item->key   = SDL_strdup(key);
            item->value = SDL_strdup(value);
            if (!item->key || !item->value) {
                SDL_free(item->key);
                SDL_free(item->value);
                SDL_OutOfMemory();
                INI_Destroy(ini);
                SDL_free(data);
                return NULL;
            }
            sec->item_count++;
        }
        // Lines without '=' are silently ignored.

        line = eol ? eol + 1 : NULL;
    }

    SDL_free(data);
    return ini;
}

SDL_ini *INI_Load(const char *file)
{
    SDL_IOStream *io = SDL_IOFromFile(file, "r");
    if (!io) {
        return NULL;
    }
    return INI_Load_IO(io, true);
}

SDL_ini *INI_LoadString(const char *text)
{
    return INI_Load_IO(SDL_IOFromConstMem(text, SDL_strlen(text)), true);
}

bool INI_Save_IO(const SDL_ini *ini, SDL_IOStream *dst, bool closeio)
{
    if (!ini || !dst) {
        if (closeio && dst) {
            SDL_CloseIO(dst);
        }
        return SDL_SetError("INI_Save_IO: invalid arguments");
    }

    bool wrote_any = false;
    bool last_was_blank = false;

    for (int s = 0; s < ini->section_count; ++s) {
        const SDL_ini_section *sec = &ini->sections[s];
        bool is_global = (sec->name[0] == '\0');

        // Skip empty global section entirely.
        if (is_global && sec->item_count == 0) {
            continue;
        }

        if (!is_global) {
            // Add a blank line separator before section headers when needed.
            if (wrote_any && !last_was_blank) {
                SDL_IOprintf(dst, "\n");
            }
            SDL_IOprintf(dst, "[%s]\n", sec->name);
            last_was_blank = false;
            wrote_any = true;
        }

        for (int i = 0; i < sec->item_count; ++i) {
            const SDL_ini_item *item = &sec->items[i];
            switch (item->type) {
            case SDL_INI_ITEM_ENTRY: {
                const char *val = item->value;
                if (INI__needs_quoting(val)) {
                    char *esc = INI__escape(val);
                    if (!esc) {
                        if (closeio) {
                            SDL_CloseIO(dst);
                        }
                        return false; // SDL_OutOfMemory() already set
                    }
                    SDL_IOprintf(dst, "%s = \"%s\"\n", item->key, esc);
                    SDL_free(esc);
                } else {
                    SDL_IOprintf(dst, "%s = %s\n", item->key, val);
                }
                last_was_blank = false;
                break;
            }
            case SDL_INI_ITEM_COMMENT:
                SDL_IOprintf(dst, "%s\n", item->comment);
                last_was_blank = false;
                break;
            case SDL_INI_ITEM_BLANK:
                SDL_IOprintf(dst, "\n");
                last_was_blank = true;
                break;
            }
            wrote_any = true;
        }
    }

    if (closeio) {
        if (!SDL_CloseIO(dst)) {
            return false;
        }
    }
    return true;
}

bool INI_Save(const SDL_ini *ini, const char *file)
{
    SDL_IOStream *io = SDL_IOFromFile(file, "w");
    if (!io) {
        return false;
    }
    return INI_Save_IO(ini, io, true);
}

void INI_Destroy(SDL_ini *ini)
{
    if (!ini) {
        return;
    }
    for (int i = 0; i < ini->section_count; ++i) {
        INI__free_section_contents(&ini->sections[i]);
    }
    SDL_free(ini->sections);
    SDL_free(ini);
}

const char *INI_GetString(const SDL_ini *ini, const char *section, const char *key, const char *default_value)
{
    if (!ini || !key) {
        return default_value;
    }
    const SDL_ini_section *sec = INI__find_section(ini, section);
    if (!sec) {
        return default_value;
    }
    const SDL_ini_item *item = INI__find_entry(sec, key);
    if (!item) {
        return default_value;
    }
    return item->value;
}

bool INI_HasKey(const SDL_ini *ini, const char *section, const char *key)
{
    if (!ini || !key) {
        return false;
    }
    const SDL_ini_section *sec = INI__find_section(ini, section);
    if (!sec) {
        return false;
    }
    return INI__find_entry(sec, key) != NULL;
}

bool INI_HasSection(const SDL_ini *ini, const char *section)
{
    if (!ini) {
        return false;
    }
    const SDL_ini_section *sec = INI__find_section(ini, section);
    if (!sec) {
        return false;
    }
    // A section counts as present only if it holds at least one key/value.
    for (int i = 0; i < sec->item_count; ++i) {
        // Comments or blank lines are not considered as filling a section.
        if (sec->items[i].type == SDL_INI_ITEM_ENTRY) {
            return true;
        }
    }
    return false;
}

Sint64 INI_GetInt(const SDL_ini *ini, const char *section, const char *key, Sint64 default_value)
{
    const char *str = INI_GetString(ini, section, key, NULL);
    if (!str) {
        return default_value;
    }
    // Detect a 0x/0X prefix for hex; otherwise parse base 10 so a leading
    // zero ("010") is decimal 10, not octal 8.
    const char *p = str;
    while (*p == ' ' || *p == '\t') {
        ++p;
    }
    if (*p == '+' || *p == '-') {
        ++p;
    }
    int base = (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) ? 16 : 10;
    char *endp = NULL;
    Sint64 val = SDL_strtoll(str, &endp, base);
    if (endp == str) {
        return default_value;  // no digits parsed
    }
    return val;
}

double INI_GetDouble(const SDL_ini *ini, const char *section, const char *key, double default_value)
{
    const char *str = INI_GetString(ini, section, key, NULL);
    if (!str) {
        return default_value;
    }
    char *endp = NULL;
    double val = SDL_strtod(str, &endp);
    if (endp == str) {
        return default_value;
    }
    return val;
}

float INI_GetFloat(const SDL_ini *ini, const char *section, const char *key, float default_value)
{
    return (float)INI_GetDouble(ini, section, key, (double)default_value);
}

bool INI_GetBoolean(const SDL_ini *ini, const char *section, const char *key, bool default_value)
{
    const char *str = INI_GetString(ini, section, key, NULL);
    if (!str) {
        return default_value;
    }
    if (SDL_strcasecmp(str, "1") == 0 ||
        SDL_strcasecmp(str, "true") == 0 ||
        SDL_strcasecmp(str, "t") == 0 ||
        SDL_strcasecmp(str, "yes") == 0 ||
        SDL_strcasecmp(str, "y") == 0 ||
        SDL_strcasecmp(str, "on") == 0 ||
        SDL_strcasecmp(str, "enabled") == 0 ||
        SDL_strcasecmp(str, "enable") == 0 ||
        SDL_strcasecmp(str, "active") == 0)
    {
        return true;
    }

    if (SDL_strcasecmp(str, "0") == 0 ||
        SDL_strcasecmp(str, "false") == 0 ||
        SDL_strcasecmp(str, "f") == 0 ||
        SDL_strcasecmp(str, "no") == 0 ||
        SDL_strcasecmp(str, "n") == 0 ||
        SDL_strcasecmp(str, "off") == 0 ||
        SDL_strcasecmp(str, "disabled") == 0 ||
        SDL_strcasecmp(str, "disable") == 0 ||
        SDL_strcasecmp(str, "inactive") == 0 ||
        SDL_strcasecmp(str, "none") == 0)
    {
        return false;
    }
    return default_value;
}

bool INI_SetString(SDL_ini *ini, const char *section, const char *key, const char *value)
{
    if (!ini || !key || !value) {
        return SDL_SetError("INI_SetString: invalid arguments");
    }

    const char *sec_name = INI__section_name(section);
    SDL_ini_section *sec = INI__get_or_create_section(ini, sec_name);
    if (!sec) {
        return false;
    }

    // Update existing key if it exists.
    SDL_ini_item *item = INI__find_entry(sec, key);
    if (item) {
        char *new_value = SDL_strdup(value);
        if (!new_value) {
            return SDL_OutOfMemory();
        }
        SDL_free(item->value);
        item->value = new_value;
        return true;
    }

    // Append new entry.
    if (!INI__grow_items(sec)) {
        return false;
    }
    item = &sec->items[sec->item_count];
    SDL_memset(item, 0, sizeof(*item));
    item->type  = SDL_INI_ITEM_ENTRY;
    item->key   = SDL_strdup(key);
    item->value = SDL_strdup(value);
    if (!item->key || !item->value) {
        SDL_free(item->key);
        SDL_free(item->value);
        return SDL_OutOfMemory();
    }
    sec->item_count++;
    return true;
}

bool INI_SetInt(SDL_ini *ini, const char *section, const char *key, Sint64 value)
{
    char *str = NULL;
    if (SDL_asprintf(&str, "%" SDL_PRIs64, value) < 0 || !str) {
        return SDL_OutOfMemory();
    }
    bool ok = INI_SetString(ini, section, key, str);
    SDL_free(str);
    return ok;
}

bool INI_SetDouble(SDL_ini *ini, const char *section, const char *key, double value)
{
    char *str = NULL;
    if (SDL_asprintf(&str, "%.17g", value) < 0 || !str) {
        return SDL_OutOfMemory();
    }
    bool ok = INI_SetString(ini, section, key, str);
    SDL_free(str);
    return ok;
}

bool INI_SetFloat(SDL_ini *ini, const char *section, const char *key, float value)
{
    char *str = NULL;
    if (SDL_asprintf(&str, "%.8g", (double)value) < 0 || !str) {
        return SDL_OutOfMemory();
    }
    bool ok = INI_SetString(ini, section, key, str);
    SDL_free(str);
    return ok;
}

bool INI_SetBoolean(SDL_ini *ini, const char *section, const char *key, bool value)
{
    return INI_SetString(ini, section, key, value ? "true" : "false");
}

bool INI_RemoveKey(SDL_ini *ini, const char *section, const char *key)
{
    if (!ini || !key) {
        return false;
    }
    SDL_ini_section *sec = INI__find_section(ini, section);
    if (!sec) {
        return false;
    }
    for (int i = 0; i < sec->item_count; ++i) {
        if (sec->items[i].type == SDL_INI_ITEM_ENTRY &&
            SDL_strcasecmp(sec->items[i].key, key) == 0) {
            SDL_free(sec->items[i].key);
            SDL_free(sec->items[i].value);
            // Shift remaining items down to preserve order.
            SDL_memmove(&sec->items[i], &sec->items[i + 1], (size_t)(sec->item_count - 1 - i) * sizeof(sec->items[0]));
            sec->item_count--;
            return true;
        }
    }
    return false;
}

bool INI_RemoveSection(SDL_ini *ini, const char *section)
{
    if (!ini) {
        return false;
    }
    const char *sec_name = INI__section_name(section);
    for (int i = 0; i < ini->section_count; ++i) {
        if (SDL_strcasecmp(ini->sections[i].name, sec_name) == 0) {
            INI__free_section_contents(&ini->sections[i]);
            SDL_memmove(&ini->sections[i], &ini->sections[i + 1],
                        (size_t)(ini->section_count - 1 - i) * sizeof(ini->sections[0]));
            ini->section_count--;
            return true;
        }
    }
    return false;
}

void INI_EnumerateSections(const SDL_ini *ini, INI_EnumerateSectionsCallback callback, void *userdata)
{
    if (!ini || !callback) {
        return;
    }
    for (int i = 0; i < ini->section_count; ++i) {
        callback(userdata, ini, ini->sections[i].name);
    }
}

void INI_EnumerateKeys(const SDL_ini *ini, const char *section, INI_EnumerateKeysCallback callback, void *userdata)
{
    if (!ini || !callback) {
        return;
    }
    const SDL_ini_section *sec = INI__find_section(ini, section);
    if (!sec) {
        return;
    }
    for (int i = 0; i < sec->item_count; ++i) {
        if (sec->items[i].type == SDL_INI_ITEM_ENTRY) {
            callback(userdata, ini, section, sec->items[i].key, sec->items[i].value);
        }
    }
}

#endif /* SDL_INI_IMPLEMENTATION_ONCE */
#endif /* SDL_INI_IMPLEMENTATION */

#ifdef __DOXYGEN
/**
 * In exactly one C source file, define \c SDL_INI_IMPLEMENTATION before including `SDL_ini.h`.
 *
 * \code
 * #define SDL_INI_IMPLEMENTATION
 * #include "SDL_ini.h"
 * \endcode
 *
 * In all other files, just include the header normally:
 * \code
 * #include "SDL_ini.h"
 * \endcode
 *
 * \see INI_Create()
 */
#define SDL_INI_IMPLEMENTATION
#endif /* __DOXYGEN */

#endif /* SDL_INI_H_ */
