/**
 * @file string.h
 * @brief Utilities for string manipulation
 */

#ifndef SHELLSCRIBE_UTILS_STRING_UTILS_H
#define SHELLSCRIBE_UTILS_STRING_UTILS_H

#include <stddef.h>

/**
 * @brief Duplicates a string
 * 
 * @param str Source string
 * @return char* Newly allocated string (must be freed by the caller), or NULL on failure
 */
char *string_duplicate(const char *str);

/**
 * @brief Concatenates two strings
 * 
 * @param str1 First string
 * @param str2 Second string
 * @return char* Newly allocated string (must be freed by the caller), or NULL on failure
 */
char *string_concat(const char *str1, const char *str2);

/**
 * @brief Trims whitespace from the beginning and end of a string
 * 
 * @param str String to trim
 * @return char* Newly allocated string (must be freed by the caller), or NULL on failure
 */
char *string_trim(const char *str);

/**
 * @brief Searches and replaces all occurrences of a substring
 * 
 * @param src Source string
 * @param pattern Pattern to search for
 * @param replacement Replacement string
 * @return char* Newly allocated string (must be freed by the caller), or NULL on failure
 */
char *string_replace(const char *src, const char *pattern, const char *replacement);

/**
 * @brief Checks if a string starts with a given prefix
 * 
 * @param str String to check
 * @param prefix Prefix to look for
 * @return int 1 if the string starts with the prefix, 0 otherwise
 */
int string_starts_with(const char *str, const char *prefix);

/**
 * @brief Checks if a string matches a regular expression
 * 
 * @param str String to check
 * @param pattern Regular expression
 * @return int 1 if the string matches, 0 otherwise, -1 on error
 */
int string_matches_regex(const char *str, const char *pattern);

/**
 * @brief Extracts groups matching a regular expression
 * 
 * @param str Source string
 * @param pattern Regular expression with capture groups
 * @param matches Array of strings to store matches (must be freed by the caller)
 * @param max_matches Maximum number of matches
 * @return int Number of matches found, or -1 on error
 */
int string_extract_regex(const char *str, const char *pattern, 
                        char **matches, int max_matches);

#endif /* SHELLSCRIBE_UTILS_STRING_UTILS_H */
