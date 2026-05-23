# elf-parser

Dynamic ELF loader for embedded systems targeting the Xtensa ABI.
`elf-parser` loads relocatable/shared ELF objects at runtime, performs symbol resolution, applies relocations, and enables cross-binary linking through a global symbol table.

Designed for constrained embedded platforms such as the Espressif Systems ESP32 family, the loader supports dynamic module execution directly from executable memory regions.


## Implementation Details

The parser operates in the following stages:

1. **ELF Header Parsing**: Reads and validates the ELF header to extract metadata including architecture, entry point, and program header information.

2. **Segment Loading**: Iterates through program headers and loads each segment into the appropriate memory region:
    - **IRAM**: Instruction memory for executable code
    - **DRAM**: Data memory for initialized and uninitialized data sections

3. **Memory Management**: Handles section-to-segment mapping and resolves relocation entries if present.

4. **Execution Transfer**: Sets up the processor state and transfers control to the entry point address specified in the ELF header.



## Features

* ELF32 parser for Xtensa ABI binaries
* Runtime loading of ELF objects
* Dynamic symbol resolution
* Global symbol table for cross-module linking
* Shared library style symbol exports/imports
* RELA relocation handling
* Runtime executable allocation using IRAM
* Dynamic function export/import between binaries
* Lightweight embedded-focused implementation
* Compatible with ESP-IDF environments

---

## Supported ELF Components

| Component                    | Status    |
| ---------------------------- | --------- |
| ELF32                        | Supported |
| Xtensa ABI                   | Supported |
| `.dynsym`                    | Supported |
| `.dynstr`                    | Supported |
| `SHT_RELA` relocations       | Supported |
| Shared symbol exports        | Supported |
| Cross-binary linking         | Supported |
| Executable memory allocation | Supported |

---

## Architecture

```text
+--------------------+
| ELF Binary         |
| (.dynsym/.rela)    |
+---------+----------+
          |
          v
+--------------------+
| ELF Parser         |
| Section Extraction |
+---------+----------+
          |
          v
+--------------------+
| Global Symbol Table|
| name -> address    |
+---------+----------+
          |
          +------------------+
          |                  |
          v                  v
+----------------+   +----------------+
| Relocations    |   | Export Symbols |
| Symbol Imports |   | Shared APIs    |
+----------------+   +----------------+
          |
          v
+--------------------+
| Executable IRAM    |
| Runtime Execution  |
+--------------------+
```

---


## Example Workflow

### Exporting symbols from Binary A

```cpp
extern "C" void led_toggle() {
    // hardware code
}
```

When Binary A is loaded:

```cpp
symbol_table["led_toggle"] = function_address;
```

---

### Importing symbols in Binary B

```cpp
extern "C" void led_toggle();

void app_main() {
    led_toggle();
}
```

During relocation:

```text
led_toggle -> resolved from global symbol table
```

The relocation entry is patched with the actual runtime address.

---

## Runtime Linking Model

The loader maintains a global runtime symbol registry:

```cpp
std::unordered_map<std::string, void*> executable::symbol_table;
```

This allows:

* Symbol sharing between independently loaded ELF objects
* Shared-library-like behavior
* Dynamic runtime composition of applications
* Plugin/module architectures on embedded systems

---

## Memory Model

The executable image is copied into executable IRAM memory using:

```cpp
heap_caps_malloc(size, MALLOC_CAP_EXEC);
```

This allows runtime execution of dynamically loaded code.

---

## Example Usage

```cpp
uint8_t* elf_binary = load_binary();
int size = get_binary_size();

executable app(elf_binary, size);

app();
```

## Build Environment

* C++
* ESP-IDF
* Xtensa toolchain
* ELF32 ABI