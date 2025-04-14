# Shellscribe

![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)
![License](https://img.shields.io/github/license/saltstack/salt)

**Shellscribe** is a documentation generator for shell scripts. It extracts specially formatted comments from your bash scripts and generates clear, structured, and professional documentation in Markdown format.

## Features

- Generates elegant Markdown documentation
- Automatically extracts script metadata (author, version, license, etc.)
- Supports documentation of functions, arguments, and examples
- Supports exit codes and stdout/stderr messages
- Transforms GitHub usernames into links to their profiles
- Detects and documents ShellCheck directives
- Supports dependency documentation (requires, provides, calls, used-by)
- Optimized for memory efficiency with large projects
- Recursive processing of directories
- Supports excluding scripts from documentation using @skip annotation

## Table of Contents

- [Shellscribe](#shellscribe)
  - [Features](#features)
  - [Table of Contents](#table-of-contents)
  - [Installation](#installation)
    - [Prerequisites](#prerequisites)
    - [Quick Installation](#quick-installation)
      - [Using the installation script](#using-the-installation-script)
    - [Using pre-built package (Debian/Ubuntu)](#using-pre-built-package-debianubuntu)
- [Download the latest .deb package](#download-the-latest-deb-package)
    - [Compile from source](#compile-from-source)
  - [Usage](#usage)
    - [Basic Usage](#basic-usage)
    - [Directory Processing](#directory-processing)
    - [Options](#options)
    - [Configuration File](#configuration-file)
  - [Documentation Format](#documentation-format)
    - [File Metadata](#file-metadata)
    - [Function Documentation](#function-documentation)
    - [Dependency Documentation](#dependency-documentation)
    - [ShellCheck Directives](#shellcheck-directives)
    - [Excluding Scripts](#excluding-scripts)
  - [Examples](#examples)
  - [Contribute](#contribute)
  - [License](#license)

## Installation

### Prerequisites

- A C compiler (GCC or Clang)
- CMake (>= 3.10)

### Quick Installation

#### Using the installation script

You can quickly install Shellscribe using our installation script available in the GitHub releases:

```bash
curl -sL https://github.com/tomcdj71/shellscribe/releases/latest/download/scribe
mv scribe /usr/local/bin
```

### Using pre-built package (Debian/Ubuntu)

For Debian-based distributions, you can download and install the pre-built .deb package:

# Download the latest .deb package

```bash
RELEASE_URL=$(curl -s https://api.github.com/repos/tomcdj71/shellscribe/releases/latest | 
             grep "browser_download_url.*deb" | 
             cut -d : -f 2,3 | 
             tr -d \")
wget "$RELEASE_URL" -O shellscribe.deb
sudo dpkg -i shellscribe.deb
```

Instructions on how the .deb package is built 

### Compile from source

```bash
# Clone the repository
git clone https://github.com/tomcdj71/shellscribe.git
cd shellscribe

# Create the build directory
mkdir build && cd build

# Configure and compile
cmake ..
cmake --build .

# Install (optional)
sudo cmake --install .
```

## Usage

### Basic Usage

```bash
scribe path/to/your_script.sh
```

By default, Shellscribe outputs to stdout, which can be redirected to a file:

### Directory Processing

Shellscribe can process entire directories of shell scripts recursively:

```bash
scribe path/to/scripts_directory
```

The documentation will be saved to the configured output directory (default: `./docs`).

### Options

```
Usage: scribe [OPTIONS] FILE...
Options:
  --help, -h             Display this help message
  --version, -v          Display the version
  --config-file=FILE     Specify a custom configuration file
```

### Configuration File

Shellscribe uses a configuration file named `.scribeconf` in the current directory by default. For a complete list of configuration options, see [Configuration Reference](docs/configuration_references.md).

Example `.scribeconf`:
```
doc_path=./docs
show_shellcheck=true
shellcheck_display=sequential
arguments_display=table
```

## Documentation Format

Shellscribe uses a specific comment format to extract documentation:

### File Metadata

```bash
#!/usr/bin/env bash
# @file modules/example.sh
# @project MyProject
# @version 1.0.0
# @description An example module with useful functions
# @author John Doe (johndoe)
# @author_contact johndoe@example.com
# @license BSD-3 Clause
# @copyright Copyright (C) 2025, MyOrg
```

### Function Documentation

```bash
# @brief Installs a package with apt
# @description Attempts to install a package using apt-get
# @arg $1 string Name of the package to install
# @requires root_access
# @provides package_installation
# @calls apt-get
# @example
# install_package "curl"
# @exitcode 0 Installation successful
# @exitcode 1 Installation failed
# @stdout Installation status message
install_package() {
    local package=$1
    echo "Installing $package..."
    apt-get install -y "$package" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "Installation of $package successful"
        return 0
    else
        echo "Installation of $package failed" >&2
        return 1
    fi
}
```

### Dependency Documentation

Shellscribe supports documenting dependencies between functions:

- `@requires`: Functions or components required by this function
- `@provides`: Services or features provided by this function
- `@calls`: External commands or tools called by this function
- `@used-by`: Functions that use this function

```bash
# @function prepare_environment
# @brief Sets up the execution environment
# @requires bash
# @provides environment_setup
# @calls mkdir
prepare_environment() {
    mkdir -p /tmp/workspace
}
```

### ShellCheck Directives

Shellscribe can detect and document ShellCheck directives in your code:

```bash
# shellcheck disable=SC2086
# @function join_arguments
# @brief Joins all arguments with a space
join_arguments() {
    echo $*
}
```

To include ShellCheck directives in the documentation, enable the `show_shellcheck` option in your configuration. You can control the display format by setting the `shellcheck_display` option to either `table` or `sequential`.

### Excluding Scripts

You can exclude specific scripts from documentation generation by adding the `@skip` annotation in the file metadata:

```bash
#!/usr/bin/env bash
# @file internal_script.sh
# @skip
# @description This script will not be included in the documentation
```

Scripts with the `@skip` annotation will be ignored during documentation generation, which is useful for utility scripts, tests, or other internal files that don't need public documentation.

## Examples

Here's an example of the generated documentation:
docs: [example](docs/example.md)

## Contribute

Contributions are welcome! Here's how to participate:

1. Fork the project
2. Create a branch for your feature (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the Apache 2.0 Clause - see the [LICENSE](LICENSE) file for details.

---

© tomcdj71 2025
