# Readers-Writers Problem Implementation

A simple implementation of the classic Readers-Writers synchronization problem in Rust, demonstrating two different approaches:
- Using Rust's standard library synchronization primitives
- Using direct pthread bindings

## Implementations

```bash
cargo b
cargo r
# Follow instruction
```

### Standard Rust Implementation
Uses `Arc`, `Mutex`, and `Condvar` from Rust's standard library. Provides memory safety and ergonomic APIs.

```bash
  cargo run --example std_impl
```

### Pthread Implementation
Direct bindings to system pthreads. Lower-level implementation demonstrating raw thread handling.

```bash
  cargo run --example pthread_impl
```

## Usage

1. Clone the repository
2. Run either implementation using the commands above
3. Enter the number of readers and writers when prompted

## Requirements

1. Rust 1.75+
2. Cargo
3. Unix-like operating system (for pthread implementation)

## Dependencies

1. rand
2. libc
3. pthread