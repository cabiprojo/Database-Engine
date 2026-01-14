# Database Engine

A learning project to build a database engine from scratch in C++.

## Project Phases

- [x] **Phase 0** — Storage Layer (Page abstraction, FileManager, PageManager)
- [x] **Phase 1** — Disk-Backed Key-Value Store (page-based storage)
- [x] **Phase 2** — Buffer Pool / Page Cache (LRU eviction, pin/unpin, dirty tracking)
- [x] **Phase 3** — B+ Tree Indexing (page-based B+ Tree with insert/search)
- [ ] **Phase 4** — Durability & Crash Recovery (WAL, LSNs, recovery)
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

### Phase 0: Storage Layer
- **Page**: Fixed-size 4KB page abstraction with byte-level read/write operations
- **FileManager**: Low-level file I/O with page-level read/write operations
- **PageManager**: High-level page allocation and management

### Phase 1: Disk-Backed Key-Value Store
Implements a page-based key-value store with:
- `put(key, value)` - Store a key-value pair
- `get(key)` - Retrieve a value by key
- `remove(key)` - Delete a key-value pair

Storage format: Page-based storage with in-memory index mapping keys to (page_id, offset).

### Phase 2: Buffer Pool / Page Cache
- LRU (Least Recently Used) eviction policy
- Pin/unpin semantics for page retention
- Dirty page tracking and flushing
- Integrated with KVStore for cached page I/O

### Phase 3: B+ Tree Indexing
- Page-based B+ Tree implementation
- Node serialization/deserialization to/from pages
- Insert with automatic node splitting
- Search operations
- Integrated with BufferPool for efficient page caching
- Persistence support (root page ID stored in metadata page)

