/*
 * test_SDL_ini.c - Comprehensive tests for SDL_ini.h
 *
 * Build:
 *   cc test_SDL_ini.c -o test_SDL_ini $(pkg-config --cflags --libs sdl3)
 */

#define SDL_INI_IMPLEMENTATION
#include "SDL_ini.h"

#include <stdio.h>

static int g_pass = 0;
static int g_fail = 0;

/**
 * Tests against the given condition, and outputs the msg on failure.
 *
 * @param cond The condition to check against.
 * @param msg The message to dipslay if it fails.
 */
#define TEST(cond, msg)                                                 \
    do {                                                                \
        if (cond) {                                                     \
            g_pass++;                                                   \
        } else {                                                        \
            g_fail++;                                                   \
            SDL_Log("FAIL [%s:%d]: %s", __FILE__, __LINE__, msg);       \
        }                                                               \
    } while (0)

#define TEST_STR(a, b, msg)                                             \
    do {                                                                \
        if ((a) && (b) && SDL_strcmp((a), (b)) == 0) {                   \
            g_pass++;                                                   \
        } else {                                                        \
            g_fail++;                                                   \
            SDL_Log("FAIL [%s:%d]: %s (got \"%s\", expected \"%s\")",   \
                    __FILE__, __LINE__, msg,                             \
                    (a) ? (a) : "(null)", (b) ? (b) : "(null)");        \
        }                                                               \
    } while (0)

static int g_section_count;
static char g_section_names[32][64];

static void SDLCALL count_sections(const SDL_ini* ini, const char *section, void *userdata)
{
    (void)ini;
    (void)userdata;
    if (g_section_count < 32) {
        SDL_strlcpy(g_section_names[g_section_count], section,
                    sizeof(g_section_names[0]));
    }
    g_section_count++;
}

static int g_key_count;
static char g_keys[32][64];
static char g_values[32][128];

static void SDLCALL count_keys(const SDL_ini* ini, const char *key, const char *value, void *userdata)
{
    (void)userdata;
    if (g_key_count < 32) {
        SDL_strlcpy(g_keys[g_key_count], key, sizeof(g_keys[0]));
        SDL_strlcpy(g_values[g_key_count], value, sizeof(g_values[0]));
    }
    g_key_count++;
}

static void test_create_destroy(void)
{
    SDL_Log("test_create_destroy");
    SDL_ini *ini = SDL_CreateIni();
    TEST(ini != NULL, "SDL_CreateIni returns non-NULL");
    SDL_DestroyIni(ini);

    // Destroying NULL should be a no-op. */
    SDL_DestroyIni(NULL);
    TEST(true, "SDL_DestroyIni(NULL) does not crash");
}

static void test_set_get_string(void)
{
    SDL_Log("test_set_get_string");
    SDL_ini *ini = SDL_CreateIni();

    // Global section.
    TEST(SDL_SetIniString(ini, NULL, "name", "hello") == true, "set global key");
    TEST_STR(SDL_GetIniString(ini, NULL, "name", "x"), "hello", "get global key");

    // Named section.
    TEST(SDL_SetIniString(ini, "Video", "width", "1920") == true, "set named section key");
    TEST_STR(SDL_GetIniString(ini, "Video", "width", "0"), "1920", "get named section key");

    // Case-insensitive lookup.
    TEST_STR(SDL_GetIniString(ini, "video", "WIDTH", "0"), "1920", "case-insensitive lookup");

    // Default value for missing key.
    TEST_STR(SDL_GetIniString(ini, "Video", "missing", "default"), "default", "missing key returns default");

    // Default value for missing section.
    TEST_STR(SDL_GetIniString(ini, "NoSuch", "key", "fallback"), "fallback", "missing section returns default");

    // Overwrite existing key.
    SDL_SetIniString(ini, "Video", "width", "2560");
    TEST_STR(SDL_GetIniString(ini, "Video", "width", "0"), "2560", "overwrite existing key");

    SDL_DestroyIni(ini);
}

static void test_set_get_int(void)
{
    SDL_Log("test_set_get_int");
    SDL_ini *ini = SDL_CreateIni();

    SDL_SetIniInt(ini, "Numbers", "decimal", 42);
    TEST(SDL_GetIniInt(ini, "Numbers", "decimal", 0) == 42, "int round-trip (decimal)");

    SDL_SetIniString(ini, "Numbers", "hex", "0xFF");
    TEST(SDL_GetIniInt(ini, "Numbers", "hex", 0) == 255, "int parse hex 0xFF");

    SDL_SetIniString(ini, "Numbers", "negative", "-100");
    TEST(SDL_GetIniInt(ini, "Numbers", "negative", 0) == -100, "int parse negative");

    // Leading zero is decimal, not octal.
    SDL_SetIniString(ini, "Numbers", "leading_zero", "010");
    TEST(SDL_GetIniInt(ini, "Numbers", "leading_zero", 0) == 10, "int parse leading zero is decimal");

    // Non-numeric string returns default.
    SDL_SetIniString(ini, "Numbers", "text", "abc");
    TEST(SDL_GetIniInt(ini, "Numbers", "text", 999) == 999, "non-numeric returns default");

    // Missing key returns default.
    TEST(SDL_GetIniInt(ini, "Numbers", "missing", -1) == -1, "missing key returns default");

    SDL_DestroyIni(ini);
}

static void test_set_get_float(void)
{
    SDL_Log("test_set_get_float");
    SDL_ini *ini = SDL_CreateIni();

    SDL_SetIniFloat(ini, "Physics", "gravity", 9.81f);
    float val = SDL_GetIniFloat(ini, "Physics", "gravity", 0.0f);
    TEST(val > 9.80f && val < 9.82f, "float round-trip 9.81");

    SDL_SetIniString(ini, "Physics", "pi", "3.14159");
    val = SDL_GetIniFloat(ini, "Physics", "pi", 0.0f);
    TEST(val > 3.14f && val < 3.15f, "float parse 3.14159");

    // Non-numeric
    SDL_SetIniString(ini, "Physics", "bad", "xyz");
    TEST(SDL_GetIniFloat(ini, "Physics", "bad", -1.0f) == -1.0f, "non-numeric float returns default");

    SDL_DestroyIni(ini);
}

static void test_set_get_double(void)
{
    SDL_Log("test_set_get_double");
    SDL_ini *ini = SDL_CreateIni();

    // Double-precision round-trip.
    SDL_SetIniDouble(ini, "Physics", "precise_pi", 3.141592653589793);
    double dval = SDL_GetIniDouble(ini, "Physics", "precise_pi", 0.0);
    TEST(dval > 3.14159265358979 && dval < 3.14159265358980, "double round-trip precise pi");

    SDL_SetIniString(ini, "Physics", "large", "1.23456789012345e+100");
    dval = SDL_GetIniDouble(ini, "Physics", "large", 0.0);
    TEST(dval > 1.2e+100, "double parse large exponent");

    // Non-numeric
    SDL_SetIniString(ini, "Physics", "bad", "xyz");
    TEST(SDL_GetIniDouble(ini, "Physics", "bad", -1.0) == -1.0, "non-numeric double returns default");

    // Missing key
    TEST(SDL_GetIniDouble(ini, "Physics", "missing", 42.5) == 42.5, "missing key returns default");

    SDL_DestroyIni(ini);
}

static void test_set_get_boolean(void)
{
    SDL_Log("test_set_get_boolean");
    SDL_ini *ini = SDL_CreateIni();

    SDL_SetIniBoolean(ini, "Flags", "fullscreen", true);
    TEST(SDL_GetIniBoolean(ini, "Flags", "fullscreen", false) == true, "boolean round-trip true");

    SDL_SetIniBoolean(ini, "Flags", "vsync", false);
    TEST(SDL_GetIniBoolean(ini, "Flags", "vsync", true) == false, "boolean round-trip false");

    // Various truthy/falsy strings.
    SDL_SetIniString(ini, "Flags", "a", "YES");
    TEST(SDL_GetIniBoolean(ini, "Flags", "a", false) == true, "YES -> true");

    SDL_SetIniString(ini, "Flags", "b", "on");
    TEST(SDL_GetIniBoolean(ini, "Flags", "b", false) == true, "on -> true");

    SDL_SetIniString(ini, "Flags", "c", "No");
    TEST(SDL_GetIniBoolean(ini, "Flags", "c", true) == false, "No -> false");

    SDL_SetIniString(ini, "Flags", "d", "OFF");
    TEST(SDL_GetIniBoolean(ini, "Flags", "d", true) == false, "OFF -> false");

    SDL_SetIniString(ini, "Flags", "e", "1");
    TEST(SDL_GetIniBoolean(ini, "Flags", "e", false) == true, "1 -> true");

    SDL_SetIniString(ini, "Flags", "f", "0");
    TEST(SDL_GetIniBoolean(ini, "Flags", "f", true) == false, "0 -> false");

    // Unrecognised string returns default.
    SDL_SetIniString(ini, "Flags", "g", "maybe");
    TEST(SDL_GetIniBoolean(ini, "Flags", "g", true) == true, "unrecognised -> default (true)");
    TEST(SDL_GetIniBoolean(ini, "Flags", "g", false) == false, "unrecognised -> default (false)");

    SDL_DestroyIni(ini);
}

static void test_deletion(void)
{
    SDL_Log("test_deletion");
    SDL_ini *ini = SDL_CreateIni();

    SDL_SetIniString(ini, "Sec", "a", "1");
    SDL_SetIniString(ini, "Sec", "b", "2");
    SDL_SetIniString(ini, "Sec", "c", "3");

    // Delete middle key.
    TEST(SDL_DeleteIniKey(ini, "Sec", "b") == true, "delete existing key");
    TEST_STR(SDL_GetIniString(ini, "Sec", "b", "gone"), "gone", "deleted key returns default");

    // Remaining keys are intact.
    TEST_STR(SDL_GetIniString(ini, "Sec", "a", "?"), "1", "key a intact");
    TEST_STR(SDL_GetIniString(ini, "Sec", "c", "?"), "3", "key c intact");

    // Delete non-existent key.
    TEST(SDL_DeleteIniKey(ini, "Sec", "z") == false, "delete non-existent key returns false");

    // Delete entire section.
    SDL_SetIniString(ini, "Other", "x", "10");
    TEST(SDL_DeleteIniSection(ini, "Sec") == true, "delete section");
    TEST_STR(SDL_GetIniString(ini, "Sec", "a", "gone"), "gone", "deleted section key returns default");

    // Other section unaffected.
    TEST_STR(SDL_GetIniString(ini, "Other", "x", "?"), "10", "other section intact");

    // Delete non-existent section.
    TEST(SDL_DeleteIniSection(ini, "NoSuch") == false, "delete non-existent section returns false");

    SDL_DestroyIni(ini);
}

static void test_enumeration(void)
{
    SDL_Log("test_enumeration");
    SDL_ini *ini = SDL_CreateIni();

    SDL_SetIniString(ini, NULL, "global_key", "gv");
    SDL_SetIniString(ini, "Alpha", "a1", "v1");
    SDL_SetIniString(ini, "Alpha", "a2", "v2");
    SDL_SetIniString(ini, "Beta",  "b1", "v3");

    // Enumerate sections.
    g_section_count = 0;
    SDL_EnumerateIniSections(ini, count_sections, NULL);
    TEST(g_section_count == 3, "3 sections enumerated");

    // Enumerate keys in Alpha.
    g_key_count = 0;
    SDL_EnumerateIniKeys(ini, "Alpha", count_keys, NULL);
    TEST(g_key_count == 2, "2 keys in Alpha");
    TEST_STR(g_keys[0], "a1", "first key is a1");
    TEST_STR(g_values[0], "v1", "first value is v1");

    // Enumerate keys in global section.
    g_key_count = 0;
    SDL_EnumerateIniKeys(ini, NULL, count_keys, NULL);
    TEST(g_key_count == 1, "1 key in global section");
    TEST_STR(g_keys[0], "global_key", "global key name");

    // Enumerate keys in non-existent section.
    g_key_count = 0;
    SDL_EnumerateIniKeys(ini, "NoSuch", count_keys, NULL);
    TEST(g_key_count == 0, "0 keys in non-existent section");

    SDL_DestroyIni(ini);
}

static void test_save_and_load(void)
{
    SDL_Log("test_save_and_load");
    SDL_ini *ini = SDL_CreateIni();

    // Build some data.
    SDL_SetIniString(ini,  NULL,      "app", "test_SDL_ini");
    SDL_SetIniString(ini,  "Video",   "width", "1920");
    SDL_SetIniString(ini,  "Video",   "height", "1080");
    SDL_SetIniBoolean(ini, "Video",   "fullscreen", true);
    SDL_SetIniInt(ini,     "Audio",   "volume", 85);
    SDL_SetIniFloat(ini,   "Physics", "gravity", 9.81f);

    // Save to file.
    const char *path = "test_output.ini";
    TEST(SDL_SaveIni(ini, path) == true, "save to file");
    SDL_DestroyIni(ini);

    // Load it back.
    SDL_ini *loaded = SDL_LoadIni(path);
    TEST(loaded != NULL, "load from file");

    if (loaded) {
        TEST_STR(SDL_GetIniString(loaded, NULL, "app", "?"), "test_SDL_ini", "global key round-trip");
        TEST_STR(SDL_GetIniString(loaded, "Video", "width", "0"), "1920", "video width round-trip");
        TEST_STR(SDL_GetIniString(loaded, "Video", "height", "0"), "1080", "video height round-trip");
        TEST(SDL_GetIniBoolean(loaded, "Video", "fullscreen", false) == true, "fullscreen round-trip");
        TEST(SDL_GetIniInt(loaded, "Audio", "volume", 0) == 85, "volume round-trip");

        float grav = SDL_GetIniFloat(loaded, "Physics", "gravity", 0.0f);
        TEST(grav > 9.80f && grav < 9.82f, "gravity round-trip");

        SDL_DestroyIni(loaded);
    }

    // Clean up test file.
    SDL_RemovePath(path);
}

static void test_parse_edge_cases(void)
{
    SDL_Log("test_parse_edge_cases");

    // Build an INI string with tricky formatting.
    const char *ini_text =
        "  ; This is a comment  \n"
        "# Another comment\n"
        "   \n"
        "global = value\n"
        "\n"
        "  [  Spaced Section  ]  \n"
        "  key1   =   value with spaces  \n"
        "key2=nospace\n"
        "noequals_line\n"
        "[Empty]\n"
        "[HasKeys]\n"
        "x = 1\n";

    SDL_IOStream *mem = SDL_IOFromConstMem(ini_text, SDL_strlen(ini_text));
    TEST(mem != NULL, "create IOStream from const mem");

    SDL_ini *ini = SDL_LoadIni_IO(mem, true);
    TEST(ini != NULL, "parse edge-case INI");

    if (ini) {
        // Global key.
        TEST_STR(SDL_GetIniString(ini, NULL, "global", "?"),
                 "value", "global key parsed");

        // Spaced section and trimmed values.
        TEST_STR(SDL_GetIniString(ini, "Spaced Section", "key1", "?"), "value with spaces", "trimmed key1");
        TEST_STR(SDL_GetIniString(ini, "Spaced Section", "key2", "?"),
                 "nospace", "key2 no-space");

        // Lines without '=' should be ignored.
        TEST_STR(SDL_GetIniString(ini, "Spaced Section", "noequals_line", "missing"),
                 "missing", "no-equals line ignored");

        // Empty section exists but has no keys.
        g_key_count = 0;
        SDL_EnumerateIniKeys(ini, "Empty", count_keys, NULL);
        TEST(g_key_count == 0, "empty section has 0 keys");

        // HasKeys section.
        TEST_STR(SDL_GetIniString(ini, "HasKeys", "x", "?"),
                 "1", "HasKeys.x parsed");

        SDL_DestroyIni(ini);
    }
}

static void test_io_stream(void)
{
    SDL_Log("test_io_stream");
    SDL_ini *ini = SDL_CreateIni();
    SDL_SetIniString(ini, "Test", "key", "value");

    // Save to a dynamic memory stream.
    SDL_IOStream *out = SDL_IOFromDynamicMem();
    TEST(out != NULL, "create dynamic mem IOStream");
    TEST(SDL_SaveIni_IO(ini, out, false) == true, "save to dynamic mem");

    // Seek back to start and load.
    SDL_SeekIO(out, 0, SDL_IO_SEEK_SET);
    SDL_ini *loaded = SDL_LoadIni_IO(out, true);
    TEST(loaded != NULL, "load from dynamic mem");
    if (loaded) {
        TEST_STR(SDL_GetIniString(loaded, "Test", "key", "?"), "value", "IOStream round-trip");
        SDL_DestroyIni(loaded);
    }

    SDL_DestroyIni(ini);
}

static void test_quoted_values(void)
{
    SDL_Log("test_quoted_values");

    // Parse: double-quoted values should have quotes stripped.
    const char *ini_text =
        "[Quoted]\n"
        "plain = hello\n"
        "quoted = \"world\"\n"
        "spaced = \"  leading and trailing  \"\n"
        "empty = \"\"\n"
        "single_quote = \"only one side\n"
        "inner_quotes = \"she said \"hi\"\"\n";

    SDL_IOStream *mem = SDL_IOFromConstMem(ini_text, SDL_strlen(ini_text));
    SDL_ini *ini = SDL_LoadIni_IO(mem, true);
    TEST(ini != NULL, "parse quoted INI");

    if (ini) {
        // Unquoted value unchanged.
        TEST_STR(SDL_GetIniString(ini, "Quoted", "plain", "?"), "hello", "unquoted value");

        // Simple quoted value: quotes stripped.
        TEST_STR(SDL_GetIniString(ini, "Quoted", "quoted", "?"), "world", "simple quoted value");

        // Quoted value preserves inner whitespace.
        TEST_STR(SDL_GetIniString(ini, "Quoted", "spaced", "?"), "  leading and trailing  ", "quoted preserves whitespace");

        // Empty quoted value. */
        TEST_STR(SDL_GetIniString(ini, "Quoted", "empty", "?"), "", "empty quoted value");

        SDL_DestroyIni(ini);
    }

    // Round-trip: values with leading/trailing whitespace get auto-quoted.
    SDL_ini *ini2 = SDL_CreateIni();
    SDL_SetIniString(ini2, "RT", "normal", "abc");
    SDL_SetIniString(ini2, "RT", "spaces", "  padded  ");
    SDL_SetIniString(ini2, "RT", "empty", "");
    // Values that are themselves quoted or contain quotes/backslashes must
    // round-trip exactly (regression: previously silently lost the quotes).
    SDL_SetIniString(ini2, "RT", "literal_quotes", "\"quoted\"");
    SDL_SetIniString(ini2, "RT", "inner_quote", "she said \"hi\"");
    SDL_SetIniString(ini2, "RT", "backslash", "C:\\path\\file");

    const char *path = "test_quoted_rt.ini";
    TEST(SDL_SaveIni(ini2, path) == true, "save quoted round-trip");
    SDL_DestroyIni(ini2);

    SDL_ini *loaded = SDL_LoadIni(path);
    TEST(loaded != NULL, "load quoted round-trip");
    if (loaded) {
        TEST_STR(SDL_GetIniString(loaded, "RT", "normal", "?"), "abc", "normal value round-trip");
        TEST_STR(SDL_GetIniString(loaded, "RT", "spaces", "?"), "  padded  ", "whitespace value round-trip");
        TEST_STR(SDL_GetIniString(loaded, "RT", "empty", "?"), "", "empty value round-trip");
        TEST_STR(SDL_GetIniString(loaded, "RT", "literal_quotes", "?"), "\"quoted\"", "literal-quotes value round-trip");
        TEST_STR(SDL_GetIniString(loaded, "RT", "inner_quote", "?"), "she said \"hi\"", "inner-quote value round-trip");
        TEST_STR(SDL_GetIniString(loaded, "RT", "backslash", "?"), "C:\\path\\file", "backslash value round-trip");
        SDL_DestroyIni(loaded);
    }
    SDL_RemovePath(path);
}

static void test_newline_values(void)
{
    SDL_Log("test_newline_values");

    // Values containing newlines/tabs must be escaped so they don't break the
    // file structure on reload.
    SDL_ini *ini = SDL_CreateIni();
    SDL_SetIniString(ini, "Multi", "line", "first\nsecond");
    SDL_SetIniString(ini, "Multi", "tabbed", "a\tb\tc");
    SDL_SetIniString(ini, "Multi", "after", "sentinel");

    const char *path = "test_newline.ini";
    TEST(SDL_SaveIni(ini, path) == true, "save newline values");
    SDL_DestroyIni(ini);

    SDL_ini *loaded = SDL_LoadIni(path);
    TEST(loaded != NULL, "load newline values");
    if (loaded) {
        TEST_STR(SDL_GetIniString(loaded, "Multi", "line", "?"), "first\nsecond", "newline value round-trip");
        TEST_STR(SDL_GetIniString(loaded, "Multi", "tabbed", "?"), "a\tb\tc", "tab value round-trip");
        TEST_STR(SDL_GetIniString(loaded, "Multi", "after", "?"), "sentinel", "value after newline value intact");

        // The embedded newline must not have created extra keys.
        g_key_count = 0;
        SDL_EnumerateIniKeys(loaded, "Multi", count_keys, NULL);
        TEST(g_key_count == 3, "newline did not split into extra keys");

        SDL_DestroyIni(loaded);
    }
    SDL_RemovePath(path);
}

static void test_duplicate_keys(void)
{
    SDL_Log("test_duplicate_keys");

    // A key repeated within one section collapses to a single entry with the
    // last value.
    const char *ini_text =
        "[Sec]\n"
        "key = first\n"
        "key = second\n"
        "key = third\n";

    SDL_IOStream *mem = SDL_IOFromConstMem(ini_text, SDL_strlen(ini_text));
    SDL_ini *ini = SDL_LoadIni_IO(mem, true);
    TEST(ini != NULL, "load duplicate keys");

    if (ini) {
        TEST_STR(SDL_GetIniString(ini, "Sec", "key", "?"), "third", "duplicate key takes last value");

        g_key_count = 0;
        SDL_EnumerateIniKeys(ini, "Sec", count_keys, NULL);
        TEST(g_key_count == 1, "duplicate keys collapse to one entry");

        SDL_DestroyIni(ini);
    }
}

static void test_comment_preservation(void)
{
    SDL_Log("test_comment_preservation");

    // Build an INI with comments and blank lines.
    const char *ini_text =
        "; Global header comment\n"
        "app = MyApp\n"
        "\n"
        "[Video]\n"
        "; Resolution settings\n"
        "width = 1920\n"
        "height = 1080\n"
        "\n"
        "# Flags\n"
        "fullscreen = true\n"
        "\n"
        "[Audio]\n"
        "; Volume from 0 to 100\n"
        "volume = 85\n";

    // Load from memory.
    SDL_IOStream *mem = SDL_IOFromConstMem(ini_text, SDL_strlen(ini_text));
    SDL_ini *ini = SDL_LoadIni_IO(mem, true);
    TEST(ini != NULL, "load INI with comments");

    if (ini) {
        // Verify values are still correct.
        TEST_STR(SDL_GetIniString(ini, NULL, "app", "?"), "MyApp", "global key with comments");
        TEST_STR(SDL_GetIniString(ini, "Video", "width", "?"), "1920", "video width with comments");
        TEST(SDL_GetIniBoolean(ini, "Video", "fullscreen", false) == true, "fullscreen with comments");
        TEST(SDL_GetIniInt(ini, "Audio", "volume", 0) == 85, "volume with comments");

        // Save to dynamic mem and compare output.
        SDL_IOStream *out = SDL_IOFromDynamicMem();
        TEST(SDL_SaveIni_IO(ini, out, false) == true, "save INI with comments");

        // Read back the written bytes.
        Sint64 written = SDL_TellIO(out);
        SDL_SeekIO(out, 0, SDL_IO_SEEK_SET);
        size_t outsize = 0;
        char *output = (char *)SDL_LoadFile_IO(out, &outsize, true);
        TEST(output != NULL, "read back saved INI");

        if (output) {
            TEST((Sint64)outsize == written, "output size matches");
            TEST(SDL_strcmp(output, ini_text) == 0, "round-trip preserves comments and blanks");
            if (SDL_strcmp(output, ini_text) != 0) {
                SDL_Log("  EXPECTED:\n%s", ini_text);
                SDL_Log("  GOT:\n%s", output);
            }
            SDL_free(output);
        }

        SDL_DestroyIni(ini);
    }
}

static void test_comment_types(void)
{
    SDL_Log("test_comment_types");

    // Both ; and # comments are preserved.
    const char *ini_text =
        "; semicolon comment\n"
        "# hash comment\n"
        "\n"
        "[Sec]\n"
        "key = val\n";

    SDL_IOStream *mem = SDL_IOFromConstMem(ini_text, SDL_strlen(ini_text));
    SDL_ini *ini = SDL_LoadIni_IO(mem, true);
    TEST(ini != NULL, "parse both comment types");

    if (ini) {
        // Save and verify both comment styles are preserved.
        SDL_IOStream *out = SDL_IOFromDynamicMem();
        SDL_SaveIni_IO(ini, out, false);
        SDL_SeekIO(out, 0, SDL_IO_SEEK_SET);
        size_t outsize = 0;
        char *output = (char *)SDL_LoadFile_IO(out, &outsize, true);

        if (output) {
            TEST(SDL_strcmp(output, ini_text) == 0, "both comment styles round-trip");
            SDL_free(output);
        }
        SDL_DestroyIni(ini);
    }
}

static void test_null_section(void)
{
    SDL_Log("test_null_section");
    SDL_ini *ini = SDL_CreateIni();

    // NULL section stores keys at the top level.
    SDL_SetIniString(ini, NULL, "key1", "val1");
    SDL_SetIniString(ini, NULL, "key2", "val2");
    SDL_SetIniString(ini, "Named", "key3", "val3");

    TEST_STR(SDL_GetIniString(ini, NULL, "key1", "?"), "val1", "NULL section get");
    TEST_STR(SDL_GetIniString(ini, "", "key1", "?"), "val1", "empty-string section get (same as NULL)");

    // Save and verify global keys appear at top without a header.
    SDL_IOStream *out = SDL_IOFromDynamicMem();
    SDL_SaveIni_IO(ini, out, false);
    SDL_SeekIO(out, 0, SDL_IO_SEEK_SET);
    size_t outsize = 0;
    char *output = (char *)SDL_LoadFile_IO(out, &outsize, true);

    if (output) {
        // Output should start with keys, not a section header.
        TEST(SDL_strncmp(output, "key1 = val1\n", 12) == 0, "global keys at top of file");
        TEST(SDL_strstr(output, "[Named]") != NULL, "named section present");
        SDL_free(output);
    }

    // Delete from NULL section.
    TEST(SDL_DeleteIniKey(ini, NULL, "key1") == true, "delete from NULL section");
    TEST_STR(SDL_GetIniString(ini, NULL, "key1", "gone"), "gone", "deleted NULL-section key returns default");

    SDL_DestroyIni(ini);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (!SDL_Init(0)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    test_create_destroy();
    test_set_get_string();
    test_set_get_int();
    test_set_get_float();
    test_set_get_double();
    test_set_get_boolean();
    test_deletion();
    test_enumeration();
    test_save_and_load();
    test_parse_edge_cases();
    test_io_stream();
    test_quoted_values();
    test_newline_values();
    test_duplicate_keys();
    test_comment_preservation();
    test_comment_types();
    test_null_section();

    SDL_Log("===========================================");
    SDL_Log("Results: %d passed, %d failed", g_pass, g_fail);
    SDL_Log("===========================================");

    SDL_Quit();
    return g_fail > 0 ? 1 : 0;
}
