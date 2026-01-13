# Modern C++ String View

## Overview

This project implements a **modern C++ string view abstraction** that provides **filtered, non-owning access** to character sequences.

The implementation is designed to behave similarly to standard library views, allowing users to iterate over a string while applying filtering logic, **without copying or owning the underlying data**.

The project focuses on **clean interface design, memory safety, and modern C++ best practices**.

---

## Project Structure

Modern-Cpp-String-View/
├─ include/
│  └─ filtered_string_view.h
├─ src/
│  └─ filtered_string_view.cpp
├─ .gitignore
└─ README.md

---

## Key Concepts

- **Non-owning view**: The view does not manage or copy the underlying string data
- **Zero-copy design**: Filtering is applied logically during iteration
- **STL-style interface**: Supports iteration and standard usage patterns
- **Separation of interface and implementation**: Header and source files are clearly separated

---

## Features

- Filtered access to string data using user-defined predicates
- Iterator-based traversal of filtered characters
- Strong const-correctness guarantees
- Safe lifetime management following RAII principles
- No unnecessary memory allocation or data copying

---

## Design Highlights

- Designed as a lightweight abstraction similar to `std::string_view`
- Uses iterators to apply filtering logic lazily during traversal
- Emphasises clean API boundaries and predictable object semantics
- Written with correctness and maintainability in mind

---

## Requirements

- C++17 or later
- Standard C++ library

No external dependencies are required.

---

## Usage Notes

This project focuses on **core abstraction and implementation**, rather than build systems or external frameworks.

The code is intended to be read and evaluated directly, making it suitable for:
- Code review
- Technical interviews
- Demonstrating modern C++ design skills

---
