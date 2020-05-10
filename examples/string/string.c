#include "string.h"

static void string_ensure_space(string *sb, size_t add_len) {
    if(sb == NULL || add_len == 0) return;

    /* If out allocated space is big enough */
    if(sb->alloced >= sb->len+add_len + 1) return;

    while (sb->alloced < sb->len+add_len + 1) {
        /* Doubling growth strategy */
        sb->alloced <<= 1;
        if(sb->alloced == 0) {
            /* Left shift of max bits will go to 0. An unsigned type set to
             * -1 will return the maximum possible size. However, we should
             *  have run out of memory well before we need to do this. Since
             *  this is the theoretical maximum total system memory we don't
             *  have a flag saying we can't grow any more because it should
             *  be impossible to get to this point */
            sb->alloced--;
        }
    }
    sb->str = realloc(sb->str, sb->alloced);
}

string *string_create(char *initial_string) {
    string *sb;

    /* If a persitence flag is passed do not store on the garbage collector */
    sb = calloc(1, sizeof(*sb));
    sb->str = malloc(string_init_capacity);

    /* NULL terminate the string */
    *sb->str = '\0';

    sb->alloced = string_init_capacity;
    sb->len = 0;

    string_add_str(sb, initial_string);
    return sb;
}

void string_add_str(string *sb, const char *str) {
    if(sb == NULL || str == NULL || *str == '\0') return;

    size_t len = strlen(str);
    string_ensure_space(sb, len);

    /* Copy the value into memory */
    memmove(sb->str+sb->len, str, len);

    /* Reset length and NULL terminate */
    sb->len += len;
    sb->str[sb->len] = '\0';
}

void string_add_char(string *sb, char c) {
    if(sb == NULL) return;

    /* In any case we try to overflow the input */
    if(c > 255 || c < 0) return;

    string_ensure_space(sb, 1);

    sb->str[sb->len] = c;
    sb->len++;
    sb->str[sb->len] = '\0';
}

void string_add_int(string *sb, int val) {
    char str[1024];

    if(sb == NULL) return;

    snprintf(str, sizeof(str), "%d", val);
    string_add_str(sb, str);
}

void string_add_double_precision(string *sb, double val) {
    char str[1024];

    if(sb == NULL) return;

    /* Use %g for minimum precision on printing floats */
    snprintf(str, sizeof(str), "%g", val);
    string_add_str(sb, str);
}

char *string_get(string *sb) {
    if(sb == NULL) return NULL;
    return sb->str;
}

char string_get_char_at_index(string *sb, size_t index) {
    if(sb == NULL || index < 0) return '\0';
    return sb->str[index];
}

void string_shorten(string *sb, size_t len) {
    if(sb == NULL || len >= sb->len) return;

    /* Reset the length and NULL terminate, ingoring
        all values right to the NULL from memory */
    sb->len = len;
    sb->str[sb->len] = '\0';
}

void string_delete(string *sb) {
    if(sb == NULL) return;

    /* Call truncate with 0, clearing out the string */
    string_shorten(sb, 0);
}

void string_skip(string *sb, size_t len) {
    if(sb == NULL || len == 0) return;

    if(len >= sb->len) {
        /* If we choose to drop more bytes than the
            string has simply clear the string */
        string_delete(sb);
        return;
    }

    sb->len -= len;

    /* +1 to move the NULL. */
    memmove(sb->str, sb->str + len, sb->len + 1);
}

size_t string_length(string *sb) {
    if(sb == NULL) return 0;
    return sb->len;
}
