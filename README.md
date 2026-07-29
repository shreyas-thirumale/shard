# Shard

A modern, self-documenting command-line shell written in C++20.

Shard introduces a readable command language designed for users who find traditional shell syntax unintuitive. Instead of `ls`, `cd`, `mkdir`, users type:

```
list files
go to Documents
create folder Projects
```

## Status

**v0.1 — Active Development**

- [x] REPL
- [x] Lexer
- [x] Parser + AST
- [x] Filesystem commands (navigation, file ops, directory ops)
- [ ] Help system
- [ ] Autocomplete
- [ ] Fuzzy matching
- [ ] AI intent resolution (v2)

## Building

Requires: CMake 3.20+, C++20 compiler (GCC 11+, Clang 13+, MSVC 2022+)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## Running

```bash
./build/shard
```

## Testing

```bash
cmake --build build
cd build && ctest --output-on-failure
```

## Command Reference

### Navigation
| Command | Example |
|---|---|
| Change directory | `go to Documents` |
| Change directory | `change directory to Downloads` |
| Go back | `go back` |
| List files | `list files` |
| Current directory | `show current directory` |

### File Operations
| Command | Example |
|---|---|
| Copy file | `copy report.pdf to Backup` |
| Move file | `move image.png into Pictures` |
| Delete file | `delete old.txt` |
| Rename file | `rename draft.txt to final.txt` |
| Create file | `create file notes.txt` |

### Directory Operations
| Command | Example |
|---|---|
| Create folder | `create folder Projects` |
| Delete folder | `delete folder Temp` |
| Rename folder | `rename folder OldName to NewName` |

## Architecture

```
User Input
    ↓
Lexer          — tokenizes raw input
    ↓
Parser         — produces typed AST nodes
    ↓
AST            — structured command representation
    ↓
Executor       — walks the AST, performs OS operations
    ↓
Operating System
```

## Project Structure

```
src/
  core/          Shell REPL
  lexer/         Tokenizer
  parser/        Recursive descent parser
  ast/           Command node types
  commands/      ICommand interface
  executor/      AST → OS operations
  utils/         String utilities, edit distance
tests/           Google Test suites
```
