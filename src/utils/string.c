/**
 * @file string.c
 * @brief Implementation of string utilities
 */

#include "utils/string.h"
#include "utils/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

/**
 * @brief Duplicates a string
 * 
 * @param str The string to duplicate
 * @return char* A new allocated string containing a copy of the input or NULL if the input is NULL or allocation fails
 */
char *string_duplicate(const char *str) {
    if (str == NULL) {
        return NULL;
    }
    return shell_strdup(str);
}

/**
 * @brief Concatenates two strings
 * 
 * @param str1 First string
 * @param str2 Second string
 * @return char* A new allocated string containing the concatenation or NULL if allocation fails
 */
char *string_concat(const char *str1, const char *str2) {
    if (str1 == NULL) {
        return string_duplicate(str2);
    }
    if (str2 == NULL) {
        return string_duplicate(str1);
    }

    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char *result = (char *)shell_malloc(len1 + len2 + 1);
    if (result == NULL) {
        return NULL;
    }

    strcpy(result, str1);
    strcat(result, str2);
    return result;
}

/**
 * @brief Trims whitespace from the beginning and end of a string
 * 
 * @param str The string to trim
 * @return char* A new allocated string with whitespace removed from both ends or NULL if allocation fails
 */
char *string_trim(const char *str) {
    if (str == NULL) {
        return NULL;
    }
    const char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    if (*start == '\0') {
        return string_duplicate("");
    }
    const char *end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    size_t len = end - start + 1;
    char *result = (char *)shell_malloc(len + 1);
    if (result == NULL) {
        return NULL;
    }
    strncpy(result, start, len);
    result[len] = '\0';
    return result;
}

/**
 * @brief Replaces all occurrences of a pattern in a string with a replacement
 * 
 * @param src The source string
 * @param pattern The pattern to search for
 * @param replacement The replacement string
 * @return char* A new allocated string with replacements or NULL if allocation fails
 */
char *string_replace(const char *src, const char *pattern, const char *replacement) {
    if (src == NULL || pattern == NULL || replacement == NULL) {
        return NULL;
    }
    size_t count = 0;
    const char *pos = src;
    size_t pattern_len = strlen(pattern);
    while ((pos = strstr(pos, pattern)) != NULL) {
        count++;
        pos += pattern_len;
    }
    if (count == 0) {
        return string_duplicate(src);
    }
    size_t src_len = strlen(src);
    size_t replacement_len = strlen(replacement);
    size_t result_len = src_len + count * (replacement_len - pattern_len) + 1;
    char *result = (char *)shell_malloc(result_len);
    if (result == NULL) {
        return NULL;
    }
    char *dest = result;
    const char *remaining = src;
    while ((pos = strstr(remaining, pattern)) != NULL) {
        size_t prefix_len = pos - remaining;
        memcpy(dest, remaining, prefix_len);
        dest += prefix_len;
        memcpy(dest, replacement, replacement_len);
        dest += replacement_len;
        remaining = pos + pattern_len;
    }
    strcpy(dest, remaining);

    return result;
}

/**
 * @brief Checks if a string starts with a given prefix
 * 
 * @param str The string to check
 * @param prefix The prefix to look for
 * @return int 1 if the string starts with the prefix, 0 otherwise
 */
int string_starts_with(const char *str, const char *prefix) {
    if (str == NULL || prefix == NULL) {
        return 0;
    }
    size_t prefix_len = strlen(prefix);
    size_t str_len = strlen(str);
    if (prefix_len > str_len) {
        return 0;
    }

    return (strncmp(str, prefix, prefix_len) == 0) ? 1 : 0;
}

/**
 * @brief Checks if a string matches a regular expression
 * 
 * @param str The string to check
 * @param pattern The regular expression pattern
 * @return int 1 if the string matches, 0 if it doesn't match, -1 on error
 */
int string_matches_regex(const char *str, const char *pattern) {
    if (str == NULL || pattern == NULL) {
        return -1;
    }
    regex_t regex;
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret != 0) {
        char error_buffer[100];
        regerror(ret, &regex, error_buffer, sizeof(error_buffer));
        fprintf(stderr, "Regex compilation error: %s\n", error_buffer);
        return -1;
    }
    ret = regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);
    if (ret == 0) {
        return 1;  // Match
    } else if (ret == REG_NOMATCH) {
        return 0;  // No match
    } else {
        return -1; // Error
    }
}

/**
 * @brief Extracts substrings matching a regular expression pattern
 * 
 * @param str The source string
 * @param pattern The regular expression pattern
 * @param matches Array to store the extracted matches
 * @param max_matches Maximum number of matches to extract
 * @return int Number of matches found, 0 if no matches, -1 on error
 */
int string_extract_regex(const char *str, const char *pattern, char **matches, int max_matches) {
    if (str == NULL || pattern == NULL || matches == NULL || max_matches <= 0) {
        return -1;
    }
    regex_t regex;
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret != 0) {
        char error_buffer[100];
        regerror(ret, &regex, error_buffer, sizeof(error_buffer));
        fprintf(stderr, "Regex compilation error: %s\n", error_buffer);
        return -1;
    }
    int actual_max_groups = (max_matches < 30) ? max_matches + 1 : 30;
    regmatch_t pmatch[30];
    ret = regexec(&regex, str, actual_max_groups, pmatch, 0);
    if (ret != 0) {
        regfree(&regex);
        return 0;
    }
    int match_count = 0;
    for (int i = 1; i < actual_max_groups && match_count < max_matches; i++) {
        if (pmatch[i].rm_so != -1) {
            int start = pmatch[i].rm_so;
            int end = pmatch[i].rm_eo;
            int length = end - start;
            matches[match_count] = (char *)shell_malloc(length + 1);
            if (matches[match_count] == NULL) {
                for (int j = 0; j < match_count; j++) {
                    void *match_ptr = matches[j];
                    shell_free(&match_ptr);
                    matches[j] = NULL;
                }
                regfree(&regex);
                return -1;
            }
            strncpy(matches[match_count], str + start, length);
            matches[match_count][length] = '\0';
            match_count++;
        }
    }
    
    regfree(&regex);
    return match_count;
}
