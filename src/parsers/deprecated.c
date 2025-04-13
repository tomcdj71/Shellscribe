/**
 * @file deprecated.c
 * @brief Implementation of deprecated tag parser
 *
 * This file provides functionality for parsing and processing deprecation-related
 * tags in documentation blocks. These tags are used to mark functions that are
 * deprecated, document their replacements, and specify end-of-life information.
 * This helps users avoid using deprecated functions and migrate to newer alternatives.
 */

#include "parsers/deprecated.h"
#include "utils/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Check if a tag is a deprecation-related tag
 * 
 * Determines whether a given tag is related to deprecation information.
 * Recognizes 'deprecated', 'replacement', and 'eol' (end-of-life) as
 * valid deprecation-related tags.
 *
 * @param tag The tag name to check, without the '@' prefix
 * 
 * @return bool true if the tag is a deprecation-related tag, false otherwise or if tag is NULL
 *
 * @note This function is case-sensitive, so tags must exactly match one of the
 *       recognized forms.
 * @note These tags are typically used together to provide complete deprecation information:
 *       - deprecated: Marks a function as deprecated, optionally with a version
 *       - replacement: Suggests an alternative function to use instead
 *       - eol: Specifies when the function will be removed
 */
bool is_deprecation_tag(const char *tag) {
    if (tag == NULL) {
        return false;
    }
    
    return (strcmp(tag, "deprecated") == 0 ||
            strcmp(tag, "replacement") == 0 ||
            strcmp(tag, "eol") == 0);
}

/**
 * @brief Process a deprecated tag (@deprecated)
 * 
 * Marks a function as deprecated and optionally records the version
 * when the deprecation was introduced. The tag content may include
 * a "from X.Y.Z" clause to specify the version.
 *
 * @param docblock The documentation block to mark as deprecated
 * @param content The content of the deprecated tag, optionally containing version information
 * 
 * @return bool true if the deprecated information was successfully processed and
 *              added to the docblock, false if an error occurred
 *
 * @note If the content includes the word "from" followed by a version number,
 *       that version is extracted and stored as the deprecation version.
 * @note If no version is specified or the content is empty, the function will
 *       still be marked as deprecated, but without version information.
 * @note Memory is allocated for the version string, which will be freed
 *       when the docblock is freed.
 * 
 * @see process_replacement_tag
 * @see process_eol_tag
 */
bool process_deprecated_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL) {
        return false;
    }
    docblock->deprecation.is_deprecated = true;
    if (content != NULL && *content != '\0') {
        const char *from = strstr(content, "from");
        if (from != NULL) {
            from += 4;
            while (isspace(*from)) {
                from++;
            }
            free(docblock->deprecation.version);
            docblock->deprecation.version = string_duplicate(from);
        } else {
            free(docblock->deprecation.version);
            docblock->deprecation.version = string_duplicate(content);
        }
    }
    
    return true;
}

/**
 * @brief Process a replacement tag (@replacement)
 * 
 * Records the suggested replacement function for a deprecated function.
 * This helps users migrate away from the deprecated function by providing
 * a direct reference to the preferred alternative.
 *
 * @param docblock The documentation block to add the replacement information to
 * @param content The content of the replacement tag, containing the name of the
 *                replacement function
 * 
 * @return bool true if the replacement information was successfully processed and
 *              added to the docblock, false if an error occurred or if required
 *              parameters are NULL or empty
 *
 * @note Memory is allocated for the replacement string, which will be freed
 *       when the docblock is freed.
 * @note If the docblock already had a replacement, the previous value is freed
 *       before the new value is assigned.
 * 
 * @see process_deprecated_tag
 */
bool process_replacement_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL || *content == '\0') {
        return false;
    }
    free(docblock->deprecation.replacement);
    docblock->deprecation.replacement = string_duplicate(content);
    
    return true;
}

/**
 * @brief Process an end-of-life tag (@eol)
 * 
 * Records the end-of-life information for a deprecated function,
 * which specifies when the function will be removed from the codebase.
 * This can be a version number, date, or other timeframe description.
 *
 * @param docblock The documentation block to add the EOL information to
 * @param content The content of the EOL tag, containing the removal timeframe
 * 
 * @return bool true if the EOL information was successfully processed and
 *              added to the docblock, false if an error occurred or if required
 *              parameters are NULL or empty
 *
 * @note Memory is allocated for the EOL string, which will be freed
 *       when the docblock is freed.
 * @note If the docblock already had EOL information, the previous value is freed
 *       before the new value is assigned.
 * 
 * @see process_deprecated_tag
 */
bool process_eol_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL || *content == '\0') {
        return false;
    }
    free(docblock->deprecation.eol);
    docblock->deprecation.eol = string_duplicate(content);
    
    return true;
} 
