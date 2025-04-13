# Shellscribe Annotations Reference

This document provides a complete reference for all annotations supported by Shellscribe, as well as potential future annotations that could be implemented.

## Currently Supported Annotations

### File Annotations

These annotations are used at the beginning of the script to provide metadata about the file.

| Annotation | Description | Example |
|------------|-------------|---------|
| `@file` | The name of the script file | `# @file backup_script.sh` |
| `@brief` | A brief one-line description of the script | `# @brief Daily backup automation script` |
| `@description` | A detailed description of the script | `# @description This script automates daily backups of critical data` |
| `@version` | The version of the script | `# @version 1.2.3` |
| `@author` | The author(s) of the script | `# @author Jane Doe (janedoe1234)` |
| `@license` | The license under which the script is distributed | `# @license MIT` |
| `@copyright` | Copyright information | `# @copyright (c) 2025 Example Inc.` |
| `@project` | The project to which the script belongs | `# @project Backup Utilities` |
| `@skip` | Indicates that the file should be skipped during documentation generation | `# @skip` |

### Function Annotations

These annotations document individual functions within your script.

| Annotation | Description | Example |
|------------|-------------|---------|
| `@description` | Description of what the function does | `# @description Validates user input` |
| `@arg` | Description of a function argument | `# @arg $1 Username to validate` |
| `@option` | Description of an option flag | `# @option --verbose Show detailed output` |
| `@example` | Example usage of the function | `# @example validate_user "john_doe"` |
| `@exitcode` | Description of an exit code | `# @exitcode 0 Validation successful` |
| `@return` | Description of the return value | `# @return 0 if validated, 1 otherwise` |
| `@internal` | Indicates an internal/private function | `# @internal` |
| `@deprecated` | Indicates a deprecated function with version, replacement, and end-of-life info | `# @deprecated from 1.0.0`<br>`# @replacement new_function`<br>`# @eol 2.0.0` |
| `@alias` | Indicates that the function is an alias for another | `# @alias check_user` |
| `@function` | Declares a function name manually (useful for non-standard declarations) | `# @function validate_user` |
| `@set` | Documents a variable that is set by the function | `# @set CONFIG_PATH Path to the configuration file` |
| `@noargs` | Indicates that the function doesn't take any arguments | `# @noargs` |

### Dependency Annotations

These annotations document function and resource dependencies.

| Annotation | Description | Example |
|------------|-------------|---------|
| `@requires` | Required resources or other functions | `# @requires check_environment` |
| `@provides` | Functionality provided by the function | `# @provides user_validation` |

### I/O Annotations

| Annotation | Description | Example |
|------------|-------------|---------|
| `@stderr` | Describes what the function outputs to standard error | `# @stderr Error messages when validation fails` |
| `@stdin` | Describes what the function reads from standard input | `# @stdin List of usernames, one per line` |
| `@stdout` | Describes what the function outputs to standard output | `# @stdout Validation results in JSON format` |

### Structure and Documentation Annotations

These annotations help structure your documentation.

| Annotation | Description | Example |
|------------|-------------|---------|
| `@section` | Creates a new section in the documentation | `# @section Advanced Configuration` |
| `shellcheck` | Documents a shellcheck directive. Display format controlled by `shellcheck_display` config setting (table or sequential) | `# shellcheck disable=SC2059` |

### Alert Block Types

The `@alert` annotation can be followed by different alert types:

| Alert Type | Purpose | Example |
|------------|---------|---------|
| `note` | Informational notes | `# @alert note` |
| `warning` | Warning messages | `# @alert warning` |
| `danger` | Critical information or errors | `# @alert danger` |
| `info` | General informational content | `# @alert info` |
| `tip` | Helpful tips or best practices | `# @alert tip` |

## Planned Future Annotations

The following annotations are not currently implemented but are planned for future releases:

### Additional Dependency Annotations

| Annotation | Description | Example |
|------------|-------------|---------|
| `@see` | Reference to related functions | `# @see authenticate_user` |
| `@reference` | External references or documentation | `# @reference https://example.com/docs` |

## Annotation Format

- Annotations start with `#` followed by `@` and the annotation name (except for shellcheck which can't contain `@` symbol)
- Multiple annotations of the same type can be used when appropriate
- Multi-line annotations are supported by continuing with `#` on subsequent lines
- File-level annotations should be placed at the beginning of the file
- Function annotations should be placed directly above the function declaration

## Example

```bash
# @file example.sh
# @brief Example script
# @description
# This is a multi-line description
# of the script's purpose.
# @author John Doe <john@example.com>
# @version 1.0.0
# @license MIT

# @description Validates user input
# @arg $1 The username to validate
# @arg $2 Optional validation level (1-3)
# @return 0 if valid, 1 otherwise
# @exitcode 0 Validation successful
# @exitcode 1 Validation failed
# @requires check_environment
# @example
#   validate_user "john_doe"
#   validate_user --strict "john_doe" 2
validate_user() {
    # Function implementation
}

# @description This function is deprecated.
# @deprecated from 1.0.0
# @replacement new_function
# @eol 2.0.0
deprecated_function() {
    # Function implementation
}

# @alert warning
# This function will be deprecated in future versions.
# Please migrate to the new API.
```

## Contributing New Annotations

If you would like to propose a new annotation type or contribute to implementing the planned annotations, please open an issue or pull request on the project's GitHub repository. 
