/*
 * SDL_ini_test.c - Comprehensive tests for SDL_ini.h
 *
 * Built on the SDL_test harness. Build via the project's CMake, or:
 *   cc SDL_ini_test.c -o SDL_ini_test $(pkg-config --cflags --libs sdl3) -lSDL3_test
 */
#define SDL_INI_IMPLEMENTATION
#include "SDL_ini.h"

#include <SDL3/SDL_test.h>

/* Check a boolean condition. */
#define TEST(cond, msg) SDLTest_AssertCheck((cond) ? 1 : 0, "%s", (msg))

/* Check string equality, reporting the actual and expected values on failure. */
#define TEST_STR(a, b, msg)                                             \
    do {                                                                \
        const char *sa_ = (a);                                          \
        const char *sb_ = (b);                                          \
        SDLTest_AssertCheck(sa_ && sb_ && SDL_strcmp(sa_, sb_) == 0,     \
                            "%s (got \"%s\", expected \"%s\")", (msg),   \
                            sa_ ? sa_ : "(null)", sb_ ? sb_ : "(null)"); \
    } while (0)

/* Collects names/values seen during enumeration, passed via the callback's
 * userdata. Sections use only the names; keys use names and values. */
typedef struct {
    int count;
    char names[32][64];
    char values[32][128];
} Collector;

static void SDLCALL collect_section(void *userdata, const SDL_ini *ini, const char *section)
{
    Collector *c = (Collector *)userdata;
    (void)ini;
    if (c->count < 32) {
        SDL_strlcpy(c->names[c->count], section, sizeof(c->names[0]));
    }
    c->count++;
}

static void SDLCALL collect_key(void *userdata, const SDL_ini *ini, const char *section, const char *key, const char *value)
{
    Collector *c = (Collector *)userdata;
    (void)ini;
    (void)section;
    if (c->count < 32) {
        SDL_strlcpy(c->names[c->count], key, sizeof(c->names[0]));
        SDL_strlcpy(c->values[c->count], value, sizeof(c->values[0]));
    }
    c->count++;
}

/* Save an INI to a freshly allocated, NUL-terminated string (caller frees with
 * SDL_free). Optionally reports the byte count via *size. */
static char *save_string(SDL_ini *ini, size_t *size)
{
    SDL_IOStream *out = SDL_IOFromDynamicMem();
    if (!out || !INI_Save_IO(ini, out, false)) {
        if (out) {
            SDL_CloseIO(out);
        }
        return NULL;
    }
    SDL_SeekIO(out, 0, SDL_IO_SEEK_SET);
    return (char *)SDL_LoadFile_IO(out, size, true);
}

static int SDLCALL test_create_destroy(void *arg)
{
    (void)arg;
    SDL_ini *ini = INI_Create();
    TEST(ini != NULL, "INI_Create returns non-NULL");
    INI_Destroy(ini);

    // Destroying NULL should be a no-op.
    INI_Destroy(NULL);
    TEST(true, "INI_Destroy(NULL) does not crash");
    return TEST_COMPLETED;
}

static int SDLCALL test_set_get_string(void *arg)
{
    (void)arg;
    SDL_ini *ini = INI_Create();

    // Global section.
    TEST(INI_SetString(ini, NULL, "name", "hello") == true, "set global key");
    TEST_STR(INI_GetString(ini, NULL, "name", "x"), "hello", "get global key");

    // Named section.
    TEST(INI_SetString(ini, "Video", "width", "1920") == true, "set named section key");
    TEST_STR(INI_GetString(ini, "Video", "width", "0"), "1920", "get named section key");

    // Case-insensitive lookup.
    TEST_STR(INI_GetString(ini, "video", "WIDTH", "0"), "1920", "case-insensitive lookup");

    // Default value for missing key.
    TEST_STR(INI_GetString(ini, "Video", "missing", "default"), "default", "missing key returns default");

    // Default value for missing section.
    TEST_STR(INI_GetString(ini, "NoSuch", "key", "fallback"), "fallback", "missing section returns default");

    // Overwrite existing key.
    INI_SetString(ini, "Video", "width", "2560");
    TEST_STR(INI_GetString(ini, "Video", "width", "0"), "2560", "overwrite existing key");

    INI_Destroy(ini);
    return TEST_COMPLETED;
}

static int SDLCALL test_set_get_int(void *arg)
{
    (void)arg;
    SDL_ini *ini = INI_Create();

    INI_SetInt(ini, "Numbers", "decimal", 42);
    TEST(INI_GetInt(ini, "Numbers", "decimal", 0) == 42, "int round-trip (decimal)");

    INI_SetString(ini, "Numbers", "hex", "0xFF");
    TEST(INI_GetInt(ini, "Numbers", "hex", 0) == 255, "int parse hex 0xFF");

    INI_SetString(ini, "Numbers", "negative", "-100");
    TEST(INI_GetInt(ini, "Numbers", "negative", 0) == -100, "int parse negative");

    // Leading zero is decimal, not octal.
    INI_SetString(ini, "Numbers", "leading_zero", "010");
    TEST(INI_GetInt(ini, "Numbers", "leading_zero", 0) == 10, "int parse leading zero is decimal");

    // Non-numeric string returns default.
    INI_SetString(ini, "Numbers", "text", "abc");
    TEST(INI_GetInt(ini, "Numbers", "text", 999) == 999, "non-numeric returns default");

    // Missing key returns default.
    TEST(INI_GetInt(ini, "Numbers", "missing", -1) == -1, "missing key returns default");

    INI_Destroy(ini);
    return TEST_COMPLETED;
}

static int SDLCALL test_set_get_float(void *arg)
{
    (void)arg;
    SDL_ini *ini = INI_Create();

    INI_SetFloat(ini, "Physics", "gravity", 9.81f);
    float val = INI_GetFloat(ini, "Physics", "gravity", 0.0f);
    TEST(val > 9.80f && val < 9.82f, "float round-trip 9.81");

    INI_SetString(ini, "Physics", "pi", "3.14159");
    val = INI_GetFloat(ini, "Physics", "pi", 0.0f);
    TEST(val > 3.14f && val < 3.15f, "float parse 3.14159");

    // Non-numeric
    INI_SetString(ini, "Physics", "bad", "xyz");
    TEST(INI_GetFloat(ini, "Physics", "bad", -1.0f) == -1.0f, "non-numeric float returns default");

    INI_Destroy(ini);
    return TEST_COMPLETED;
}

static int SDLCALL test_set_get_double(void *arg)
{
    (void)arg;
    SDL_ini *ini = INI_Create();

    // Double-precision round-trip.
    INI_SetDouble(ini, "Physics", "precise_pi", 3.141592653589793);
    double dval = INI_GetDouble(ini, "Physics", "precise_pi", 0.0);
    TEST(dval > 3.14159265358979 && dval < 3.14159265358980, "double round-trip precise pi");

    INI_SetString(ini, "Physics", "large", "1.23456789012345e+100");
    dval = INI_GetDouble(ini, "Physics", "large", 0.0);
    TEST(dval > 1.2e+100, "double parse large exponent");

    // Non-numeric
    INI_SetString(ini, "Physics", "bad", "xyz");
    TEST(INI_GetDouble(ini, "Physics", "bad", -1.0) == -1.0, "non-numeric double returns default");

    // Missing key
    TEST(INI_GetDouble(ini, "Physics", "missing", 42.5) == 42.5, "missing key returns default");

    INI_Destroy(ini);
    return TEST_COMPLETED;
}

static int SDLCALL test_set_get_boolean(void *arg)
{
    (void)arg;
    SDL_ini *ini = INI_Create();

    INI_SetBoolean(ini, "Flags", "fullscreen", true);
    TEST(INI_GetBoolean(ini, "Flags", "fullscreen", false) == true, "boolean round-trip true");

    INI_SetBoolean(ini, "Flags", "vsync", false);
    TEST(INI_GetBoolean(ini, "Flags", "vsync", true) == false, "boolean round-trip false");

    // Various truthy/falsy strings.
    INI_SetString(ini, "Flags", "a", "YES");
    TEST(INI_GetBoolean(ini, "Flags", "a", false) == true, "YES -> true");

    INI_SetString(ini, "Flags", "b", "on");
    TEST(INI_GetBoolean(ini, "Flags", "b", false) == true, "on -> true");

    INI_SetString(ini, "Flags", "c", "No");
    TEST(INI_GetBoolean(ini, "Flags", "c", true) == false, "No -> false");

    INI_SetString(ini, "Flags", "d", "OFF");
    TEST(INI_GetBoolean(ini, "Flags", "d", true) == false, "OFF -> false");

    INI_SetString(ini, "Flags", "e", "1");
    TEST(INI_GetBoolean(ini, "Flags", "e", false) == true, "1 -> true");

    INI_SetString(ini, "Flags", "f", "0");
    TEST(INI_GetBoolean(ini, "Flags", "f", true) == false, "0 -> false");

    // Unrecognised string returns default.
    INI_SetString(ini, "Flags", "g", "maybe");
    TEST(INI_GetBoolean(ini, "Flags", "g", true) == true, "unrecognised -> default (true)");
    TEST(INI_GetBoolean(ini, "Flags", "g", false) == false, "unrecognised -> default (false)");

    INI_Destroy(ini);
    return TEST_COMPLETED;
}

static int SDLCALL test_deletion(void *arg)
{
    (void)arg;
    SDL_ini *ini = INI_Create();

    INI_SetString(ini, "Sec", "a", "1");
    INI_SetString(ini, "Sec", "b", "2");
    INI_SetString(ini, "Sec", "c", "3");

    // Delete middle key.
    TEST(INI_RemoveKey(ini, "Sec", "b") == true, "delete existing key");
    TEST_STR(INI_GetString(ini, "Sec", "b", "gone"), "gone", "deleted key returns default");

    // Remaining keys are intact.
    TEST_STR(INI_GetString(ini, "Sec", "a", "?"), "1", "key a intact");
    TEST_STR(INI_GetString(ini, "Sec", "c", "?"), "3", "key c intact");

    // Delete non-existent key.
    TEST(INI_RemoveKey(ini, "Sec", "z") == false, "delete non-existent key returns false");

    // Delete entire section.
    INI_SetString(ini, "Other", "x", "10");
    TEST(INI_RemoveSection(ini, "Sec") == true, "delete section");
    TEST_STR(INI_GetString(ini, "Sec", "a", "gone"), "gone", "deleted section key returns default");

    // Other section unaffected.
    TEST_STR(INI_GetString(ini, "Other", "x", "?"), "10", "other section intact");

    // Delete non-existent section.
    TEST(INI_RemoveSection(ini, "NoSuch") == false, "delete non-existent section returns false");

    INI_Destroy(ini);
    return TEST_COMPLETED;
}

static int SDLCALL test_enumeration(void *arg)
{
    (void)arg;
    SDL_ini *ini = INI_Create();
    Collector got;

    INI_SetString(ini, NULL, "global_key", "gv");
    INI_SetString(ini, "Alpha", "a1", "v1");
    INI_SetString(ini, "Alpha", "a2", "v2");
    INI_SetString(ini, "Beta",  "b1", "v3");

    // Enumerate sections.
    got.count = 0;
    INI_EnumerateSections(ini, collect_section, &got);
    TEST(got.count == 3, "3 sections enumerated");

    // Enumerate keys in Alpha.
    got.count = 0;
    INI_EnumerateKeys(ini, "Alpha", collect_key, &got);
    TEST(got.count == 2, "2 keys in Alpha");
    TEST_STR(got.names[0], "a1", "first key is a1");
    TEST_STR(got.values[0], "v1", "first value is v1");

    // Enumerate keys in global section.
    got.count = 0;
    INI_EnumerateKeys(ini, NULL, collect_key, &got);
    TEST(got.count == 1, "1 key in global section");
    TEST_STR(got.names[0], "global_key", "global key name");

    // Enumerate keys in non-existent section.
    got.count = 0;
    INI_EnumerateKeys(ini, "NoSuch", collect_key, &got);
    TEST(got.count == 0, "0 keys in non-existent section");

    INI_Destroy(ini);
    return TEST_COMPLETED;
}

static int SDLCALL test_save_and_load(void *arg)
{
    (void)arg;
    SDL_ini *ini = INI_Create();

    // Build some data.
    INI_SetString(ini,  NULL,      "app", "SDL_ini_test");
    INI_SetString(ini,  "Video",   "width", "1920");
    INI_SetString(ini,  "Video",   "height", "1080");
    INI_SetBoolean(ini, "Video",   "fullscreen", true);
    INI_SetInt(ini,     "Audio",   "volume", 85);
    INI_SetFloat(ini,   "Physics", "gravity", 9.81f);

    // Save to file.
    const char *path = "test_output.ini";
    TEST(INI_Save(ini, path) == true, "save to file");
    INI_Destroy(ini);

    // Load it back.
    SDL_ini *loaded = INI_Load(path);
    TEST(loaded != NULL, "load from file");

    if (loaded) {
        TEST_STR(INI_GetString(loaded, NULL, "app", "?"), "SDL_ini_test", "global key round-trip");
        TEST_STR(INI_GetString(loaded, "Video", "width", "0"), "1920", "video width round-trip");
        TEST_STR(INI_GetString(loaded, "Video", "height", "0"), "1080", "video height round-trip");
        TEST(INI_GetBoolean(loaded, "Video", "fullscreen", false) == true, "fullscreen round-trip");
        TEST(INI_GetInt(loaded, "Audio", "volume", 0) == 85, "volume round-trip");

        float grav = INI_GetFloat(loaded, "Physics", "gravity", 0.0f);
        TEST(grav > 9.80f && grav < 9.82f, "gravity round-trip");

        INI_Destroy(loaded);
    }

    // Clean up test file.
    SDL_RemovePath(path);
    return TEST_COMPLETED;
}

static int SDLCALL test_parse_edge_cases(void *arg)
{
    (void)arg;
    // Build an INI string with tricky formatting.
    SDL_ini *ini = INI_LoadString(
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
        "x = 1\n");
    TEST(ini != NULL, "parse edge-case INI");

    if (ini) {
        Collector got;

        // Global key.
        TEST_STR(INI_GetString(ini, NULL, "global", "?"), "value", "global key parsed");

        // Spaced section and trimmed values.
        TEST_STR(INI_GetString(ini, "Spaced Section", "key1", "?"), "value with spaces", "trimmed key1");
        TEST_STR(INI_GetString(ini, "Spaced Section", "key2", "?"), "nospace", "key2 no-space");

        // Lines without '=' should be ignored.
        TEST_STR(INI_GetString(ini, "Spaced Section", "noequals_line", "missing"), "missing", "no-equals line ignored");

        // Empty section exists but has no keys.
        got.count = 0;
        INI_EnumerateKeys(ini, "Empty", collect_key, &got);
        TEST(got.count == 0, "empty section has 0 keys");

        // HasKeys section.
        TEST_STR(INI_GetString(ini, "HasKeys", "x", "?"), "1", "HasKeys.x parsed");

        INI_Destroy(ini);
    }
    return TEST_COMPLETED;
}

static int SDLCALL test_io_stream(void *arg)
{
    (void)arg;
    SDL_ini *ini = INI_Create();
    INI_SetString(ini, "Test", "key", "value");

    // Save to a dynamic memory stream.
    SDL_IOStream *out = SDL_IOFromDynamicMem();
    TEST(out != NULL, "create dynamic mem IOStream");
    TEST(INI_Save_IO(ini, out, false) == true, "save to dynamic mem");

    // Seek back to start and load.
    SDL_SeekIO(out, 0, SDL_IO_SEEK_SET);
    SDL_ini *loaded = INI_Load_IO(out, true);
    TEST(loaded != NULL, "load from dynamic mem");
    if (loaded) {
        TEST_STR(INI_GetString(loaded, "Test", "key", "?"), "value", "IOStream round-trip");
        INI_Destroy(loaded);
    }

    INI_Destroy(ini);
    return TEST_COMPLETED;
}

static int SDLCALL test_quoted_values(void *arg)
{
    (void)arg;
    // Parse: double-quoted values should have quotes stripped.
    SDL_ini *ini = INI_LoadString(
        "[Quoted]\n"
        "plain = hello\n"
        "quoted = \"world\"\n"
        "spaced = \"  leading and trailing  \"\n"
        "empty = \"\"\n"
        "single_quote = \"only one side\n"
        "inner_quotes = \"she said \"hi\"\"\n");
    TEST(ini != NULL, "parse quoted INI");

    if (ini) {
        // Unquoted value unchanged.
        TEST_STR(INI_GetString(ini, "Quoted", "plain", "?"), "hello", "unquoted value");

        // Simple quoted value: quotes stripped.
        TEST_STR(INI_GetString(ini, "Quoted", "quoted", "?"), "world", "simple quoted value");

        // Quoted value preserves inner whitespace.
        TEST_STR(INI_GetString(ini, "Quoted", "spaced", "?"), "  leading and trailing  ", "quoted preserves whitespace");

        // Empty quoted value.
        TEST_STR(INI_GetString(ini, "Quoted", "empty", "?"), "", "empty quoted value");

        INI_Destroy(ini);
    }

    // Round-trip: values with leading/trailing whitespace get auto-quoted.
    SDL_ini *ini2 = INI_Create();
    INI_SetString(ini2, "RT", "normal", "abc");
    INI_SetString(ini2, "RT", "spaces", "  padded  ");
    INI_SetString(ini2, "RT", "empty", "");
    // Values that are themselves quoted or contain quotes/backslashes must
    // round-trip exactly (regression: previously silently lost the quotes).
    INI_SetString(ini2, "RT", "literal_quotes", "\"quoted\"");
    INI_SetString(ini2, "RT", "inner_quote", "she said \"hi\"");
    INI_SetString(ini2, "RT", "backslash", "C:\\path\\file");

    const char *path = "test_quoted_rt.ini";
    TEST(INI_Save(ini2, path) == true, "save quoted round-trip");
    INI_Destroy(ini2);

    SDL_ini *loaded = INI_Load(path);
    TEST(loaded != NULL, "load quoted round-trip");
    if (loaded) {
        TEST_STR(INI_GetString(loaded, "RT", "normal", "?"), "abc", "normal value round-trip");
        TEST_STR(INI_GetString(loaded, "RT", "spaces", "?"), "  padded  ", "whitespace value round-trip");
        TEST_STR(INI_GetString(loaded, "RT", "empty", "?"), "", "empty value round-trip");
        TEST_STR(INI_GetString(loaded, "RT", "literal_quotes", "?"), "\"quoted\"", "literal-quotes value round-trip");
        TEST_STR(INI_GetString(loaded, "RT", "inner_quote", "?"), "she said \"hi\"", "inner-quote value round-trip");
        TEST_STR(INI_GetString(loaded, "RT", "backslash", "?"), "C:\\path\\file", "backslash value round-trip");
        INI_Destroy(loaded);
    }
    SDL_RemovePath(path);
    return TEST_COMPLETED;
}

static int SDLCALL test_newline_values(void *arg)
{
    (void)arg;
    // Values containing newlines/tabs must be escaped so they don't break the
    // file structure on reload.
    SDL_ini *ini = INI_Create();
    INI_SetString(ini, "Multi", "line", "first\nsecond");
    INI_SetString(ini, "Multi", "tabbed", "a\tb\tc");
    INI_SetString(ini, "Multi", "after", "sentinel");

    const char *path = "test_newline.ini";
    TEST(INI_Save(ini, path) == true, "save newline values");
    INI_Destroy(ini);

    SDL_ini *loaded = INI_Load(path);
    TEST(loaded != NULL, "load newline values");
    if (loaded) {
        Collector got;

        TEST_STR(INI_GetString(loaded, "Multi", "line", "?"), "first\nsecond", "newline value round-trip");
        TEST_STR(INI_GetString(loaded, "Multi", "tabbed", "?"), "a\tb\tc", "tab value round-trip");
        TEST_STR(INI_GetString(loaded, "Multi", "after", "?"), "sentinel", "value after newline value intact");

        // The embedded newline must not have created extra keys.
        got.count = 0;
        INI_EnumerateKeys(loaded, "Multi", collect_key, &got);
        TEST(got.count == 3, "newline did not split into extra keys");

        INI_Destroy(loaded);
    }
    SDL_RemovePath(path);
    return TEST_COMPLETED;
}

static int SDLCALL test_duplicate_keys(void *arg)
{
    (void)arg;
    // A key repeated within one section collapses to a single entry with the
    // last value.
    SDL_ini *ini = INI_LoadString(
        "[Sec]\n"
        "key = first\n"
        "key = second\n"
        "key = third\n");
    TEST(ini != NULL, "load duplicate keys");

    if (ini) {
        Collector got;

        TEST_STR(INI_GetString(ini, "Sec", "key", "?"), "third", "duplicate key takes last value");

        got.count = 0;
        INI_EnumerateKeys(ini, "Sec", collect_key, &got);
        TEST(got.count == 1, "duplicate keys collapse to one entry");

        INI_Destroy(ini);
    }
    return TEST_COMPLETED;
}

static int SDLCALL test_comment_preservation(void *arg)
{
    (void)arg;
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

    SDL_ini *ini = INI_LoadString(ini_text);
    TEST(ini != NULL, "load INI with comments");

    if (ini) {
        // Verify values are still correct.
        TEST_STR(INI_GetString(ini, NULL, "app", "?"), "MyApp", "global key with comments");
        TEST_STR(INI_GetString(ini, "Video", "width", "?"), "1920", "video width with comments");
        TEST(INI_GetBoolean(ini, "Video", "fullscreen", false) == true, "fullscreen with comments");
        TEST(INI_GetInt(ini, "Audio", "volume", 0) == 85, "volume with comments");

        // Save to a string and compare output.
        size_t outsize = 0;
        char *output = save_string(ini, &outsize);
        TEST(output != NULL, "read back saved INI");

        if (output) {
            TEST(outsize == SDL_strlen(ini_text), "output size matches");
            TEST(SDL_strcmp(output, ini_text) == 0, "round-trip preserves comments and blanks");
            if (SDL_strcmp(output, ini_text) != 0) {
                SDL_Log("  EXPECTED:\n%s", ini_text);
                SDL_Log("  GOT:\n%s", output);
            }
            SDL_free(output);
        }

        INI_Destroy(ini);
    }
    return TEST_COMPLETED;
}

static int SDLCALL test_comment_types(void *arg)
{
    (void)arg;
    // Both ; and # comments are preserved.
    const char *ini_text =
        "; semicolon comment\n"
        "# hash comment\n"
        "\n"
        "[Sec]\n"
        "key = val\n";

    SDL_ini *ini = INI_LoadString(ini_text);
    TEST(ini != NULL, "parse both comment types");

    if (ini) {
        // Save and verify both comment styles are preserved.
        char *output = save_string(ini, NULL);
        if (output) {
            TEST(SDL_strcmp(output, ini_text) == 0, "both comment styles round-trip");
            SDL_free(output);
        }
        INI_Destroy(ini);
    }
    return TEST_COMPLETED;
}

static int SDLCALL test_null_section(void *arg)
{
    (void)arg;
    SDL_ini *ini = INI_Create();

    // NULL section stores keys at the top level.
    INI_SetString(ini, NULL, "key1", "val1");
    INI_SetString(ini, NULL, "key2", "val2");
    INI_SetString(ini, "Named", "key3", "val3");

    TEST_STR(INI_GetString(ini, NULL, "key1", "?"), "val1", "NULL section get");
    TEST_STR(INI_GetString(ini, "", "key1", "?"), "val1", "empty-string section get (same as NULL)");

    // Save and verify global keys appear at top without a header.
    char *output = save_string(ini, NULL);
    if (output) {
        // Output should start with keys, not a section header.
        TEST(SDL_strncmp(output, "key1 = val1\n", 12) == 0, "global keys at top of file");
        TEST(SDL_strstr(output, "[Named]") != NULL, "named section present");
        SDL_free(output);
    }

    // Delete from NULL section.
    TEST(INI_RemoveKey(ini, NULL, "key1") == true, "delete from NULL section");
    TEST_STR(INI_GetString(ini, NULL, "key1", "gone"), "gone", "deleted NULL-section key returns default");

    INI_Destroy(ini);
    return TEST_COMPLETED;
}

static int SDLCALL test_has_key(void *arg)
{
    (void)arg;
    SDL_ini *ini = INI_Create();

    INI_SetString(ini, "Sec", "exists", "value");
    INI_SetString(ini, NULL, "global", "gv");

    TEST(INI_HasKey(ini, "Sec", "exists") == true, "existing key found");
    TEST(INI_HasKey(ini, "Sec", "missing") == false, "missing key not found");
    TEST(INI_HasKey(ini, "NoSuch", "exists") == false, "missing section not found");
    TEST(INI_HasKey(ini, NULL, "global") == true, "global section key found");
    TEST(INI_HasKey(ini, NULL, "nope") == false, "global section missing key");
    TEST(INI_HasKey(NULL, "Sec", "exists") == false, "NULL ini returns false");
    TEST(INI_HasKey(ini, "Sec", NULL) == false, "NULL key returns false");

    INI_SetString(ini, "Sec", "exists", "updated");
    TEST(INI_HasKey(ini, "Sec", "exists") == true, "key still found after update");

    INI_RemoveKey(ini, "Sec", "exists");
    TEST(INI_HasKey(ini, "Sec", "exists") == false, "key gone after removal");

    INI_Destroy(ini);
    return TEST_COMPLETED;
}

static int SDLCALL test_has_section(void *arg)
{
    (void)arg;
    SDL_ini *ini = INI_Create();

    INI_SetString(ini, "Sec", "exists", "value");
    INI_SetString(ini, NULL, "global", "gv");

    TEST(INI_HasSection(ini, "Sec") == true, "existing section found");
    TEST(INI_HasSection(ini, "NoSuch") == false, "missing section not found");
    TEST(INI_HasSection(ini, "sec") == true, "section lookup is case-insensitive");
    TEST(INI_HasSection(ini, NULL) == true, "global section found");
    TEST(INI_HasSection(ini, "") == true, "global section found via empty string");
    TEST(INI_HasSection(NULL, "Sec") == false, "NULL ini returns false");

    INI_RemoveSection(ini, "Sec");
    TEST(INI_HasSection(ini, "Sec") == false, "section gone after removal");

    INI_Destroy(ini);

    // A section that holds only comments/blank lines is not "present".
    ini = INI_LoadString("[Empty]\n; just a comment\n\n[Real]\nkey = value\n");
    TEST(INI_HasSection(ini, "Empty") == false, "comment-only section not present");
    TEST(INI_HasSection(ini, "Real") == true, "section with a key is present");

    INI_Destroy(ini);
    return TEST_COMPLETED;
}

static int SDLCALL test_version(void *arg)
{
    (void)arg;
    int ver = INI_GetVersion();
    TEST(ver > 0, "INI_GetVersion returns positive value");
    TEST(ver == SDL_INI_VERSION, "INI_GetVersion matches SDL_INI_VERSION macro");
    TEST(SDL_INI_MAJOR_VERSION >= 1, "major version is >= 1");
    TEST(SDL_INI_MINOR_VERSION >= 1, "minor version is >= 1");
    TEST(SDL_INI_MICRO_VERSION >= 0, "micro version is >= 0");
    TEST(SDL_INI_VERSION_ATLEAST(1, 0, 0), "version at least 1.0.0");
    TEST(!SDL_INI_VERSION_ATLEAST(2, 0, 0), "version not at least 2.0.0");
    return TEST_COMPLETED;
}

static int SDLCALL test_crlf(void *arg)
{
    (void)arg;

    // Load a CRLF-encoded string and verify values parse correctly.
    SDL_ini *ini = INI_LoadString("[Section]\r\nkey = value\r\nnum = 42\r\n");
    TEST(ini != NULL, "parse CRLF input");
    if (ini) {
        TEST_STR(INI_GetString(ini, "Section", "key", "?"), "value", "CRLF value parsed");
        TEST(INI_GetInt(ini, "Section", "num", 0) == 42, "CRLF int parsed");

        // Save and verify output uses CRLF.
        SDL_IOStream *out = SDL_IOFromDynamicMem();
        TEST(INI_Save_IO(ini, out, false) == true, "save CRLF");
        size_t len = (size_t)SDL_TellIO(out);
        SDL_SeekIO(out, 0, SDL_IO_SEEK_SET);
        char *buf = (char *)SDL_malloc(len + 1);
        SDL_ReadIO(out, buf, len);
        buf[len] = '\0';
        SDL_CloseIO(out);

        TEST(SDL_strstr(buf, "\r\n") != NULL, "output contains CRLF");
        TEST(buf[len - 1] == '\n' && buf[len - 2] == '\r', "lines end with CRLF");

        // Reload the CRLF output and verify round-trip.
        SDL_ini *reloaded = INI_LoadString(buf);
        TEST(reloaded != NULL, "reload CRLF output");
        if (reloaded) {
            TEST_STR(INI_GetString(reloaded, "Section", "key", "?"), "value", "CRLF round-trip");
            INI_Destroy(reloaded);
        }
        SDL_free(buf);
        INI_Destroy(ini);
    }

    // LF-only input should save with LF.
    ini = INI_LoadString("[Section]\nkey = value\n");
    TEST(ini != NULL, "parse LF input");
    if (ini) {
        SDL_IOStream *out = SDL_IOFromDynamicMem();
        INI_Save_IO(ini, out, false);
        size_t len = (size_t)SDL_TellIO(out);
        SDL_SeekIO(out, 0, SDL_IO_SEEK_SET);
        char *buf = (char *)SDL_malloc(len + 1);
        SDL_ReadIO(out, buf, len);
        buf[len] = '\0';
        SDL_CloseIO(out);

        TEST(SDL_strstr(buf, "\r\n") == NULL, "LF input produces LF output");
        SDL_free(buf);
        INI_Destroy(ini);
    }

    return TEST_COMPLETED;
}

static int SDLCALL test_merge(void *arg)
{
    (void)arg;

    SDL_ini *a = INI_LoadString("[S1]\nk1 = v1\nk2 = v2\n[S2]\nx = 10\n");
    SDL_ini *b = INI_LoadString("[S1]\nk2 = overwritten\nk3 = new\n[S3]\ny = 20\n");
    TEST(a != NULL && b != NULL, "parse merge inputs");

    TEST(INI_Merge(a, b) == true, "merge succeeds");

    // Existing key preserved.
    TEST_STR(INI_GetString(a, "S1", "k1", "?"), "v1", "k1 preserved");
    // Duplicate key overwritten by src.
    TEST_STR(INI_GetString(a, "S1", "k2", "?"), "overwritten", "k2 overwritten");
    // New key added.
    TEST_STR(INI_GetString(a, "S1", "k3", "?"), "new", "k3 added");
    // Original section preserved.
    TEST(INI_GetInt(a, "S2", "x", 0) == 10, "S2.x preserved");
    // New section added.
    TEST(INI_GetInt(a, "S3", "y", 0) == 20, "S3.y added");

    INI_Destroy(a);
    INI_Destroy(b);

    // NULL safety.
    TEST(INI_Merge(NULL, NULL) == false, "Merge(NULL,NULL) fails");

    // Merge_IO via dynamic mem.
    a = INI_Create();
    INI_SetString(a, "A", "key", "original");
    const char *src_text = "[A]\nkey = merged\nnew = yes\n";
    SDL_IOStream *io = SDL_IOFromConstMem(src_text, SDL_strlen(src_text));
    TEST(INI_Merge_IO(a, io, true) == true, "Merge_IO succeeds");
    TEST_STR(INI_GetString(a, "A", "key", "?"), "merged", "Merge_IO overwrites");
    TEST_STR(INI_GetString(a, "A", "new", "?"), "yes", "Merge_IO adds key");
    INI_Destroy(a);
  
    return TEST_COMPLETED;
}

static int SDLCALL test_dirty_flag(void *arg)
{
    (void)arg;

    // Fresh and loaded INIs start clean.
    SDL_ini *ini = INI_Create();
    TEST(INI_IsDirty(ini) == false, "new INI is not dirty");
    TEST(INI_IsDirty(NULL) == false, "IsDirty(NULL) is false");

    // SetString marks dirty.
    INI_SetString(ini, "s", "k", "v");
    TEST(INI_IsDirty(ini) == true, "dirty after SetString");

    // Manual clear.
    INI_SetDirty(ini, false);
    TEST(INI_IsDirty(ini) == false, "clean after SetDirty(false)");

    // Updating existing key marks dirty.
    INI_SetString(ini, "s", "k", "v2");
    TEST(INI_IsDirty(ini) == true, "dirty after update");
    INI_SetDirty(ini, false);

    // SetInt marks dirty.
    INI_SetInt(ini, "s", "n", 42);
    TEST(INI_IsDirty(ini) == true, "dirty after SetInt");
    INI_SetDirty(ini, false);

    // RemoveKey marks dirty.
    INI_RemoveKey(ini, "s", "n");
    TEST(INI_IsDirty(ini) == true, "dirty after RemoveKey");
    INI_SetDirty(ini, false);

    // RemoveSection marks dirty.
    INI_RemoveSection(ini, "s");
    TEST(INI_IsDirty(ini) == true, "dirty after RemoveSection");

    INI_Destroy(ini);

    // Loaded INI starts clean.
    ini = INI_LoadString("[Test]\nkey = value\n");
    TEST(ini != NULL, "load INI");
    if (ini) {
        TEST(INI_IsDirty(ini) == false, "loaded INI is not dirty");
        INI_Destroy(ini);
    }

    // SetDirty(NULL) doesn't crash.
    INI_SetDirty(NULL, true);

    return TEST_COMPLETED;
}

static int SDLCALL test_parse_errors(void *arg)
{
    (void)arg;
    SDL_ini *ini;

    // Unclosed section bracket on line 1 should fail.
    ini = INI_LoadString("[NoClose\nkey = value\n");
    TEST(ini == NULL, "unclosed bracket returns NULL");
    if (ini == NULL) {
        const char *err = SDL_GetError();
        TEST(SDL_strstr(err, "line 1") != NULL, "error mentions line 1");
        TEST(SDL_strstr(err, "missing closing bracket") != NULL, "error mentions missing bracket");
    }
    INI_Destroy(ini);

    // Unclosed bracket on line 3.
    ini = INI_LoadString("[ok]\nkey = value\n[bad\n");
    TEST(ini == NULL, "unclosed bracket on line 3 returns NULL");
    if (ini == NULL) {
        const char *err = SDL_GetError();
        TEST(SDL_strstr(err, "line 3") != NULL, "error mentions line 3");
    }
    INI_Destroy(ini);

    // Valid INI should still parse correctly.
    ini = INI_LoadString("[section]\nkey = value\n");
    TEST(ini != NULL, "valid INI loads ok");
    INI_Destroy(ini);

    return TEST_COMPLETED;
}

#define CASE(fn, desc) &(const SDLTest_TestCaseReference){ fn, #fn, desc, TEST_ENABLED }

static const SDLTest_TestCaseReference *iniTestCases[] = {
    CASE(test_version,              "Version macros and INI_GetVersion"),
    CASE(test_create_destroy,       "Create and destroy lifecycle"),
    CASE(test_set_get_string,       "Set/get strings, sections, defaults"),
    CASE(test_set_get_int,          "Set/get and parse integers"),
    CASE(test_set_get_float,        "Set/get and parse floats"),
    CASE(test_set_get_double,       "Set/get and parse doubles"),
    CASE(test_set_get_boolean,      "Set/get and parse booleans"),
    CASE(test_deletion,             "Remove keys and sections"),
    CASE(test_enumeration,          "Enumerate sections and keys"),
    CASE(test_save_and_load,        "Save to and load from a file"),
    CASE(test_parse_edge_cases,     "Parse whitespace/comment edge cases"),
    CASE(test_io_stream,            "Save/load through an IOStream"),
    CASE(test_quoted_values,        "Quoted value parsing and round-trip"),
    CASE(test_newline_values,       "Escaped newline/tab values"),
    CASE(test_duplicate_keys,       "Duplicate keys collapse to last"),
    CASE(test_comment_preservation, "Comments and blanks round-trip"),
    CASE(test_comment_types,        "Both ; and # comment styles"),
    CASE(test_null_section,         "NULL/global section handling"),
    CASE(test_has_key,              "INI_HasKey"),
    CASE(test_has_section,          "INI_HasSection"),
    CASE(test_crlf,                 "CRLF detection and round-trip"),
    CASE(test_merge,                "Merge INI files"),
    CASE(test_dirty_flag,           "Dirty flag tracking"),
    CASE(test_parse_errors,         "Parse error line numbers"),
    NULL
};

static SDLTest_TestSuiteReference iniSuite = {
    "INI", NULL, iniTestCases, NULL
};

static SDLTest_TestSuiteReference *testSuites[] = {
    &iniSuite,
    NULL
};

int main(int argc, char *argv[])
{
    SDLTest_CommonState *state;
    SDLTest_TestSuiteRunner *runner;
    int result;

    state = SDLTest_CommonCreateState(argv, 0);
    if (!state) {
        return 1;
    }

    /* Create the runner before parsing args so its CLI options are recognised. */
    runner = SDLTest_CreateTestSuiteRunner(state, testSuites);

    if (!SDLTest_CommonDefaultArgs(state, argc, argv)) {
        SDLTest_DestroyTestSuiteRunner(runner);
        SDLTest_CommonDestroyState(state);
        return 1;
    }

    result = SDLTest_ExecuteTestSuiteRunner(runner);

    SDLTest_DestroyTestSuiteRunner(runner);
    SDLTest_CommonDestroyState(state);
    return result;
}
