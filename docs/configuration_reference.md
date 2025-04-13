# Shellscribe Configuration Reference

This document provides a comprehensive reference for all available configuration options in Shellscribe. Configuration can be set in a `.scribeconf` file in your project directory.

## Configuration File Format

The configuration file uses a simple key-value format:

```
# This is a comment
option_name = value
another_option = true
```

## General Options

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `doc_path` | String | `./docs` | Directory where generated documentation will be stored |
| `output_file` | String | `null` | Specific output file for single-file processing (overrides doc_path) |
| `traverse_symlinks` | Boolean | `false` | Whether to follow symbolic links when processing directories |
| `verbose` | Boolean | `false` | Enable verbose output during processing |
| `memory_tracking` | Boolean | `false` | Enable memory usage tracking |
| `memory_stats` | Boolean | `false` | Display memory statistics after processing |

## Documentation Style Options

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `project_name` | String | `""` | Project name to display in documentation |
| `toc_depth` | Integer | `3` | Maximum heading level to include in table of contents |
| `show_toc` | Boolean | `true` | Whether to include a table of contents |
| `show_footer` | Boolean | `true` | Whether to include a footer in documentation pages |
| `footer_text` | String | `"Generated with Shellscribe {version}"` | Custom footer text |
| `show_license` | Boolean | `true` | Whether to include license information |
| `show_authors` | Boolean | `true` | Whether to display author information |
| `linkify_usernames` | Boolean | `true` | Convert GitHub usernames to links in author information |

## Content Options

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `show_shellcheck` | Boolean | `false` | Include shellcheck directives in documentation |
| `shellcheck_display` | String | `"sequential"` | Display style for shellcheck directives (`"table"` or `"sequential"`) |
| `show_alerts` | Boolean | `false` | Include alert blocks in documentation |
| `show_internal` | Boolean | `false` | Include functions marked as @internal |
| `table_arguments` | Boolean | `false` | Display function arguments in table format |
| `max_description_length` | Integer | `0` | Limit description length (0 = no limit) |
| `include_pattern` | String | `"*.sh"` | Pattern for files to include |
| `exclude_pattern` | String | `""` | Pattern for files to exclude |

## Example Configuration

A complete example of a `.scribeconf` file:

```
# Output settings
doc_path = ./documentation
traverse_symlinks = true
verbose = true

# Documentation style
project_name = My Shell Project
show_toc = true
toc_depth = 3
footer_text = Generated with Shellscribe by the MyTeam team

# Content settings
show_shellcheck = true
shellcheck_display = sequential
show_alerts = true
table_arguments = true
```
