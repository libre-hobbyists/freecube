# Contributing to FreeCube

Thank you for your interest in contributing to FreeCube! We're building something special here, and every contribution helps preserve gaming history.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How Can I Contribute?](#how-can-i-contribute)
- [Development Setup](#development-setup)
- [Coding Standards](#coding-standards)
- [Commit Guidelines](#commit-guidelines)
- [Pull Request Process](#pull-request-process)
- [Documentation](#documentation)

---

## Code of Conduct

Be kind, be respectful, be constructive. We're all here because we love games and preservation. Treat others the way you'd want to be treated.

A full Code of Conduct is coming soon, but the short version: No harassment, no bigotry, no toxicity. We're here to build cool stuff together.

---

## How Can I Contribute?

### Reporting Bugs

Found a bug? Great! Open an issue and include:
- Clear description of the problem
- Steps to reproduce
- Expected vs. actual behavior
- Your environment (OS, compiler, etc.)

> [!NOTE]
> When the emulator crashes, the errors provided should include common environment variables like the compiler used, the version of the project, the targeted CPU architecture and OS. Please provide all of these when reporting bugs!

### Suggesting Features

Have an idea? We'd love to hear it! Open a discussion or issue with:
- Clear description of the feature
- Why it would be useful
- Any implementation ideas (optional psuedocode)

### Contributing Code

We need help with:
- **CPU emulation** - PowerPC interpreter and JIT
- **Graphics** - Flipper GPU emulation
- **Audio** - DSP and audio processing
- **Input** - Controller handling
- **Testing** - Unit tests and compatibility testing
- **Documentation** - Code docs, guides, hardware research

Check our [issues labeled "good first issue"](../../issues?q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22) if you're new!

---

## Development Setup

### Prerequisites

- **Compiler**: GCC 9+, Clang 10+, or MSVC 2019+ (C++17 support required)
- **Build System**: CMake 4.1.2+
- **Git**: For version control

### Building
```bash
# Clone the repository
git clone https://github.com/libre-hobbyists/freecube.git
cd freecube

# Create build directory
mkdir build && cd build

# Configure and build
cmake <configuration flags> ..
cmake --build .

# Run tests (when available)
...
```

---

## Coding Standards

We follow modern C++ best practices to keep the codebase maintainable and portable.

### Language Standards

- **C++**: C++17 standard compliance
- **C**: C17 standard compliance (for C components)
- **No compiler extensions**: Code must be portable (no `-std=gnu++17`, use `-std=c++17`)

**Exception**: Platform-specific code is allowed when clearly marked:
```cpp
#ifdef _WIN32
    // Windows-specific implementation
#elif defined(__linux__)
    // Linux-specific implementation
#else
    #error "Unsupported platform"
#endif
```

### Code Style

We use `.editorconfig` for automatic formatting. Key points:

- **Indentation**: 4 spaces (no tabs) for C/C++
- **Line length**: 120 characters maximum
- **Encoding**: UTF-8
- **Line endings**: LF (Unix-style)
- **Trailing whitespace**: Remove it
- **Final newline**: Always include

### Naming Conventions
```cpp
// Classes and structs: PascalCase
class CpuInterpreter {
    // Public members: snake_case
public:
    void execute_instruction();
    
    // Private members: snake_case with trailing underscore
private:
    uint32_t program_counter_;
    bool is_running_;
};

// Functions: snake_case
void initialize_emulator();

// Constants: SCREAMING_SNAKE_CASE
constexpr uint32_t MEMORY_SIZE = 0x1800000;

// Namespaces: snake_case
namespace freecube::core {
    // ...
}

// Enums: PascalCase for type, SCREAMING_SNAKE_CASE for values
enum class CpuState {
    RUNNING,
    HALTED,
    EXCEPTION
};

// Private functions: snake_case with prefixed underscore
static void _do_something();
```

### Modern C++ Practices

**Do:**
- Use smart pointers (`std::unique_ptr`, `std::shared_ptr`) over raw pointers
- Use `auto` when type can be quickly derived from context
- Use range-based for loops when possible
- Use `constexpr` for compile-time constants
- Use `nullptr` instead of `NULL`
- Use scoped enums (`enum class`)
- Prefer `std::array` over C-style arrays
- Use RAII for resource management

**Don't:**
- Use raw `new`/`delete` (use smart pointers)
- Use C-style casts (use `static_cast`, `reinterpret_cast`, etc.)
- Use `using namespace` in headers
- Leave TODO comments without an issue reference
```cpp
// Good
auto memory = std::make_unique<uint8_t[]>(MEMORY_SIZE);
for (const auto& instruction : instruction_list) {
    process(instruction);
}

// Bad
uint8_t* memory = new uint8_t[MEMORY_SIZE];  // Memory leak risk!
for (int i = 0; i < instruction_list.size(); i++) {
    process(instruction_list[i]);
}
```

### Error Handling

- Use exceptions for exceptional circumstances
- Use `std::optional` for values that may not exist
- Use `std::variant` or tagged unions for error types when appropriate
- Document what exceptions a function may throw
```cpp
// Good
std::optional<Instruction> decode_instruction(uint32_t opcode);

// Also good
class EmulatorException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};
```

---

## Documentation

### Code Comments

We use **Doxygen** for documentation generation. All public APIs must be documented.
```cpp
/**
 * @brief Executes a single CPU instruction
 * 
 * Fetches the instruction at the current program counter, decodes it,
 * and executes the corresponding operation.
 * 
 * @throws CpuException if instruction is invalid
 * @return true if execution should continue, false if halted
 */
bool execute_instruction();

/**
 * @brief Reads a value from memory
 * 
 * @param address The memory address to read from
 * @param size Number of bytes to read (1, 2, or 4)
 * @return The value read from memory
 * @throws MemoryException if address is out of bounds
 */
uint32_t read_memory(uint32_t address, size_t size);
```

**Comment guidelines:**
- Public APIs: Full Doxygen comments
- Private/internal: Brief comments explaining "why", not "what"
- Complex algorithms: Detailed explanation of approach
- Hacks/workarounds: Explain why they're necessary (with issue reference)
```cpp
// Good comment - explains WHY
// Using spinlock here because mutex overhead is too high for this hot path
// See issue #0 for benchmarks

// Bad comment - states the obvious
// Increment the counter
counter++;

// Bad comment #2 - out of date or incorrect information
// Returns bool
uint32_t helpful_function();
```

### Documentation Files

- Keep `docs/` updated with technical documentation
- Document hardware quirks and reverse engineering findings
- Include references to source material when available

---

## Commit Guidelines

Write clear, descriptive commit messages that explain what you changed and why.

### Good commit messages:
```
Add PowerPC branch instruction support

Implements bc, bclr, and bcctr variants. Closes #42
```
```
Fix endianness bug in memory reads

GameCube is big-endian, we were reading little-endian.
```
```
Document Flipper GPU registers

Based on hardware testing. See docs/hardware/gpu.md
```

### Tips:

- First line: Brief summary (50-72 characters ideal)
- Blank line, then details if needed
- Reference issues when relevant (`Closes #123`, `Fixes #456`)
- Explain *why*, not just *what*
- Keep commits focused on one logical change

That's it! No strict format required - just be clear and helpful.
---

## Pull Request Process

### Before Submitting

1. **Test your changes** - Make sure everything compiles and works
2. **Follow code style** - Run any formatters/linters
3. **Update documentation** - If you changed APIs or behavior
4. **Write tests** - If applicable (when we have a test framework)
5. **Rebase on main** - Keep your branch up to date

### Submitting

1. Push your changes to a feature branch
2. Open a Pull Request with a clear title and description
3. Link any related issues
4. Be responsive to feedback

### PR Template

When you open a PR, you'll see our template. Please fill it out completely:

- **Description**: What does this PR do?
- **Motivation**: Why is this change needed?
- **Testing**: How did you test this?
- **Related Issues**: Link to any related issues

### Review Process

- At least one maintainer must approve
- CI checks must pass (when we have them)
- No merge conflicts
- Code follows our standards

We'll try to review PRs promptly, but please be patient. This is a volunteer heavy project!

---

## Legal Considerations

### Clean Room Development

> [!CAUTION]
> FreeCube is developed through clean-room reverse engineering only.

**Never contribute:**
- Leaked source code
- Decompiled code from official software
- Copyrighted materials from Nintendo
- Code derived from proprietary SDKs

**Always:**
- Document your reverse engineering methodology
- Work from hardware observation and testing
- Use publicly available documentation
- Cite your sources

If you're unsure whether something is acceptable, ask first!

### License

By contributing, you agree that your contributions will be licensed under GPLv3, the same license as the project.

---

## Getting Help

- **Questions?** Open a [Discussion](../../discussions)
- **Stuck?** Ask in your PR or issue
- **Want to chat?** (TBD on community)

We're friendly and want to help you succeed!

---

## Recognition

Contributors will be recognized in:
- `AUTHORS.md` file
- Release notes
- Project documentation

We appreciate every contribution, no matter how small!

---

> *Thank you for helping preserve gaming history!*
