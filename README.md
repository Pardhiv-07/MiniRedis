# MiniRedis

MiniRedis is a Redis-inspired in-memory key-value database built in C++17. It is a local, single-process systems project designed to demonstrate clean architecture, command parsing, file persistence, and an LRU cache implementation using STL data structures.

This project is not a distributed Redis clone. It focuses on learning core systems programming concepts in a readable, resume-friendly codebase.

## Project Overview

MiniRedis stores string keys and string values in memory and exposes a simple command-line interface.

It supports basic database operations such as `SET`, `GET`, `DEL`, and `KEYS`, along with manual persistence through `SAVE` and `LOAD`. It also includes an LRU cache layer that keeps recently accessed keys available in constant time.

The project is organized into separate layers so each part has a clear responsibility:

- Database storage
- Command parsing
- File persistence
- LRU cache management
- CLI application loop

## Features

- `SET <key> <value>`: Store or update a key-value pair.
- `GET <key>`: Retrieve a value by key.
- `DEL <key>`: Delete a key-value pair.
- `KEYS`: Display all keys currently stored.
- `SAVE`: Save the in-memory database to disk.
- `LOAD`: Load the database from disk.
- `CACHE`: Display current cache contents.
- `CACHE SIZE <capacity>`: Configure cache capacity at runtime.
- `CACHE VISUAL`: Visualize the LRU cache from most recently used to least recently used.
- `HELP`: Display supported commands.
- `EXIT`: Close the application.

## Architecture

### Database Layer

The database layer is implemented by the `Database` class.

It owns the main in-memory key-value store:

```cpp
std::unordered_map<std::string, std::string>
```

Responsibilities:

- Insert or update keys.
- Retrieve values.
- Delete keys.
- Return all keys.
- Expose entries safely for persistence.

The database is the source of truth. The cache is only a performance layer and can be cleared without losing data.

### Persistence Layer

The persistence layer is implemented by the `Persistence` class.

It handles saving and loading database contents from:

```text
data/database.txt
```

Each line stores one key-value pair using a tab-separated format:

```text
key<TAB>value
```

Special characters such as tabs, newlines, carriage returns, and backslashes are escaped before writing to disk.

### Cache Layer

The cache layer is implemented by the `LRUCache` class.

It stores recently accessed values so repeated reads can be served quickly. The cache uses:

- `std::unordered_map` for direct key lookup.
- `std::list` as a doubly linked list to track recency order.

The front of the list is the most recently used entry. The back of the list is the least recently used entry.

When cache capacity is exceeded, the least recently used entry is evicted.

### Command Parser

The command parser is implemented in `command_parser.h` and `command_parser.cpp`.

It converts raw terminal input into a structured command:

```cpp
struct ParsedCommand {
    std::string name;
    std::string key;
    std::string value;
};
```

For example:

```text
SET name Sai
```

becomes:

```text
name  = SET
key   = name
value = Sai
```

## Data Structures Used

### unordered_map

Used in the database layer for storing key-value pairs.

```cpp
std::unordered_map<std::string, std::string>
```

Used in the cache layer for mapping keys directly to linked-list iterators.

```cpp
std::unordered_map<std::string, EntryIterator>
```

This enables average constant-time lookup, insertion, and deletion.

### Doubly Linked List

The LRU cache uses `std::list`, which is a doubly linked list.

It allows cache entries to be moved to the front in constant time when they are accessed.

```cpp
std::list<std::pair<std::string, std::string>>
```

The cache order is:

```text
[MRU] newest <-> ... <-> oldest [LRU]
```

## Time Complexities

| Operation | Layer | Average Time Complexity | Explanation |
|---|---|---:|---|
| `SET` | Database | O(1) | Inserts or updates a key in `unordered_map`. |
| `GET` | Database | O(1) | Looks up a key in `unordered_map`. |
| `DEL` | Database | O(1) | Removes a key from `unordered_map`. |
| `KEYS` | Database | O(n) | Must visit every stored key. |
| `SAVE` | Persistence | O(n) | Writes every key-value pair to disk. |
| `LOAD` | Persistence | O(n) | Reads every key-value pair from disk. |
| Cache `GET` | LRU Cache | O(1) | Map lookup plus constant-time list splice. |
| Cache `PUT` | LRU Cache | O(1) | Map insert/update plus front insertion. |
| Cache `DELETE` | LRU Cache | O(1) | Map lookup plus list erase. |
| Cache Eviction | LRU Cache | O(1) | Removes the back of the list. |

## Example Usage

Compile the project:

```powershell
g++ -std=c++17 src/main.cpp src/database.cpp src/persistence.cpp src/cache.cpp src/command_parser.cpp -o miniredis.exe
```

Run MiniRedis:

```powershell
.\miniredis.exe
```

Example session:

```text
MiniRedis > SET name Sai
OK

MiniRedis > GET name
Sai

MiniRedis > SET city Hyderabad
OK

MiniRedis > KEYS
city
name

MiniRedis > DEL name
OK

MiniRedis > GET name
(nil)
```

Persistence example:

```text
MiniRedis > SET language C++
OK

MiniRedis > SAVE
Database Saved Successfully

MiniRedis > EXIT
Goodbye
```

After restarting:

```text
MiniRedis > GET language
C++
```

LRU cache example:

```text
MiniRedis > CACHE SIZE 3
Cache capacity set to 3

MiniRedis > SET a 1
OK

MiniRedis > SET b 2
OK

MiniRedis > SET c 3
OK

MiniRedis > GET a
1

MiniRedis > CACHE VISUAL
LRU Cache Visualizer
Capacity: 3, Size: 3

[MRU] [a:1] <-> [c:3] <-> [b:2] [LRU]

Slot  Recency  Key  Value
   1      MRU  a  1
   2   middle  c  3
   3      LRU  b  2
```

When a new key is inserted beyond capacity, the least recently used key is evicted:

```text
MiniRedis > SET d 4
OK

MiniRedis > CACHE VISUAL
LRU Cache Visualizer
Capacity: 3, Size: 3

[MRU] [d:4] <-> [a:1] <-> [c:3] [LRU]
```

## Project Structure

```text
Mini redis/
├── src/
│   ├── main.cpp
│   ├── database.h
│   ├── database.cpp
│   ├── cache.h
│   ├── cache.cpp
│   ├── persistence.h
│   ├── persistence.cpp
│   ├── command_parser.h
│   └── command_parser.cpp
├── data/
│   └── database.txt
├── README.md
└── .gitignore
```

### File Responsibilities

| File | Responsibility |
|---|---|
| `src/main.cpp` | Runs the command-line loop and connects database, cache, persistence, and parser layers. |
| `src/database.h` | Declares the `Database` class interface. |
| `src/database.cpp` | Implements in-memory key-value operations. |
| `src/cache.h` | Declares the `LRUCache` class interface. |
| `src/cache.cpp` | Implements LRU cache behavior using `unordered_map` and `std::list`. |
| `src/persistence.h` | Declares persistence result types and the `Persistence` class. |
| `src/persistence.cpp` | Implements file save/load logic using `ifstream` and `ofstream`. |
| `src/command_parser.h` | Declares the parsed command structure and parser function. |
| `src/command_parser.cpp` | Converts raw command-line input into structured commands. |
| `data/database.txt` | Stores saved database contents. |
| `.gitignore` | Excludes build outputs and local editor files. |

## Future Improvements

- TTL support for expiring keys automatically.
- Networking support with a TCP server and client.
- Replication between multiple MiniRedis instances.
- REST API dashboard for browser-based inspection and control.
- Unit tests for database, parser, persistence, and cache behavior.
- Better command validation and structured error reporting.
- Optional JSON or binary persistence format.

## Resume Highlights

- Built a Redis-inspired in-memory key-value database in C++17 using clean layered architecture.
- Implemented file persistence with custom serialization and escaping logic.
- Designed an LRU cache with average O(1) `GET`, `PUT`, and `DELETE` operations using `unordered_map` and a doubly linked list.
- Added a command-line interface with parsing, validation, cache visualization, and persistent storage.

