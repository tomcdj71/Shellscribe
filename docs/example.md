# Shellscribe Examples

This document provides examples of how to document your shell scripts for Shellscribe and how to use the tool.

## Basic Usage

### Running Shellscribe

```bash
# Process a single shell script
scribe path/to/script.sh

# Process a directory (recursively)
scribe path/to/directory/

# Specify a custom configuration file
scribe --config-file=my-config.scribeconf path/to/script.sh
```

### Output

By default, Shellscribe generates Markdown documentation in the `./docs` directory. The output preserves the original directory structure of your scripts.

## Example Shell Script Documentation

Here's an example of a well-documented shell script that works with Shellscribe:

```bash
#!/bin/bash

# @file         example.sh
# @brief        Example script demonstrating Shellscribe annotations
# @description  This script demonstrates how to use Shellscribe annotations
#               to generate comprehensive documentation for shell scripts.
# @version      1.0.0
# @author       John Doe <john.doe@example.com>
# @license      MIT
# @copyright    Copyright (C) 2025, Example Corp.

# shellcheck disable=SC2154
# shellcheck disable=SC2034 # Unused variables should be detected
# @description Checks if a file exists and is readable
# @arg $1 Path to the file to check
# @exitcode 0 File exists and is readable
# @exitcode 1 File does not exist or is not readable
# @example
#   if check_file "/etc/passwd"; then
#     echo "File exists and is readable"
#   fi
check_file() {
    [ -r "$1" ]
}

# @description Prints a message with the specified log level
# @arg $1 Log level (info, warn, error)
# @arg $2 Message to print
# @example
#   log_message "info" "Script started"
#   log_message "error" "Could not open file"
log_message() {
    local level="$1"
    local message="$2"
    
    case "$level" in
        info)
            echo "[INFO] $message"
            ;;
        warn)
            echo "[WARNING] $message"
            ;;
        error)
            echo "[ERROR] $message"
            ;;
        *)
            echo "[UNKNOWN] $message"
            ;;
    esac
}

# @description Processes data from input file and writes to output file
# @arg $1 Input file path
# @arg $2 Output file path
# @option --verbose Display detailed information
# @option --strict Apply strict validation
# @example
#   process_data input.txt output.txt
# @example
#   process_data --verbose input.txt output.txt
# @requires check_file
# @provides data_processing
# @calls grep
# @used-by main
# @exitcode 0 Processing successful
# @exitcode 1 Input file could not be read
# @exitcode 2 Output file could not be written
process_data() {
    # Function implementation...
    echo "Processing data..."
}

# @alert note
# This is a note that will be highlighted in the documentation.
# It's useful for adding important information for users.

# Main execution
main() {
    check_file "$1" || {
        log_message "error" "Input file not found or not readable"
        exit 1
    }
    
    process_data "$1" "$2"
    exit $?
}

main "$@"
```

## Special Annotations

### File-Level Annotations

These annotations provide metadata about the entire script:

```bash
# @file         script_name.sh
# @brief        One-line summary of the script
# @description  Detailed description of the script
# @version      1.0.0
# @author       Name <email>
# @license      License type (MIT, GPL, etc.)
# @copyright    Copyright information
# @skip         Add this to skip documentation generation for this file
```

### Function Annotations

These annotations document individual functions:

```bash
# @description  Detailed description of what the function does
# @arg $1       Description of first argument
# @arg $2       Description of second argument
# @option --x   Description of flag
# @exitcode 0   Description of exit code
# @example
#   example_command arg1 arg2
#   # This shows how to use the function
# @requires     other_function
# @provides     feature_name
# @calls        external_command
# @used-by      calling_function
# @internal     Add this for internal/private functions
```

### Special Blocks

```bash
# @alert note
# This creates a note block in the documentation

# @alert warning
# This creates a warning block in the documentation

# @alert danger
# This creates a danger/error block in the documentation

# @section Advanced Usage
# This creates a section in the documentation
```

### ShellCheck Directives

ShellCheck directives in your code are automatically detected and can be included in the documentation:

```bash
# shellcheck disable=SC2086
# This disables the "double quote to prevent globbing" warning

# shellcheck disable=SC2181
# Disable SC2181 (Check exit code directly with e.g. 'if mycmd;')
```

To include ShellCheck directives in the documentation, enable the `show_shellcheck` option in your configuration:

```
show_shellcheck = true
```

You can control the display format by setting the `shellcheck_display` option to either `table` or `sequential`:

```
shellcheck_display = table   # Display in table format
shellcheck_display = sequential  # Display in sequential format (default)
```

## Example Generated Documentation

The generated documentation for the example script would look like this:

---

# example.sh (v1.0.0)

## About

**Version:** 1.0.0

**License:** MIT

**Copyright:** Copyright (C) 2025, Example Corp.

**Description:** This script demonstrates how to use Shellscribe annotations to generate comprehensive documentation for shell scripts.

**Author:** John Doe (john.doe@example.com)

---

## Index

- [check_file](#check_file)
- [log_message](#log_message)
- [process_data](#process_data)

### check_file

Checks if a file exists and is readable

#### Arguments

* $1 (Path)
  Path to the file to check

#### Example

```bash
if check_file "/etc/passwd"; then
  echo "File exists and is readable"
fi
```

#### Return Values

* 0 - File exists and is readable
* 1 - File does not exist or is not readable

### log_message

Prints a message with the specified log level

#### Arguments

* $1 (string)
  Log level (info, warn, error)
* $2 (string)
  Message to print

#### Example

```bash
log_message "info" "Script started"
log_message "error" "Could not open file"
```

### process_data

Processes data from input file and writes to output file

#### Arguments

* $1 (string)
  Input file path
* $2 (string)
  Output file path

#### Options
* `--verbose`: Display detailed information
* `--strict`: Apply strict validation

#### Dependencies

##### Required Dependencies

* `check_file`

##### Provides

* data_processing

##### External Calls

* `grep`

##### Used By

* `main`

#### Return Values

* 0 - Processing successful
* 1 - Input file could not be read
* 2 - Output file could not be written

#### Example

```bash
process_data input.txt output.txt
process_data --verbose input.txt output.txt
```

> **NOTE:**
> 
> This is a note that will be highlighted in the documentation. It's useful for adding important information for users.

---

## Advanced Techniques

### Custom Configuration

You can customize various aspects of the documentation generation process using the `.scribeconf` file. A sample file named `.scribeconf-example` is provided at the root of the project, which you can use as a starting point for your own configuration.

Just copy it to the root of your project and edit it how you want.

### Ignoring Files

To exclude files from documentation, you can:

1. Add the `@skip` annotation to the file's metadata
2. Mark files as ELF binaries (automatically detected)
