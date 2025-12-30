# Database Engine

A learning project to build a database engine from scratch in C++.

## Project Phases

- [x] **Phase 1** — Mini Disk-Backed Key-Value Store
- [ ] **Phase 2** — Add Indexing (B+ Tree)
- [ ] **Phase 3** — Durability & Crash Recovery
- [ ] **Phase 4** — Buffer Pool / Page Cache
- [ ] **Phase 5** — Optional Concurrency / Multithreading

## Building

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage

```bash
./db_engine
```

## Architecture

### Phase 1: Key-Value Store

Implements a simple disk-backed key-value store with:
- `put(key, value)` - Store a key-value pair
- `get(key)` - Retrieve a value by key
- `remove(key)` - Delete a key-value pair

Storage format: Simple log-structured storage with in-memory index.

