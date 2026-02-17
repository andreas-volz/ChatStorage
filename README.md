# ChatStorage

ChatStorage is a C++ framework designed for local storage, processing, and analysis of chat data from multiple sources.
It enables importing chat logs, media files, and user information, persisting them in a structured way, and providing a clean, consistent API for querying and manipulating chat histories. The framework is suitable for applications such as chat analytics, migration, or building custom chat-based interfaces.

## Dependencies
- **C++17 or later**: Core language features and standard library usage.
- **SQLite3**: Lightweight database backend for persistence.
- **Meson build system**: For building the project across platforms.
- **C++ compiler**: Must support C++17 features.
- Optional dependencies: standard C++ libraries for file I/O, logging, and string utilities.

## How It Works
1. **Import Layer**: Chat logs are ingested via format-specific parsers that implement a common abstract interface. Parsers transform raw files into normalized internal representations of chats, messages, users, and media.
2. **Persistence Layer**: Data is stored in SQLite databases through a repository pattern. Each entity type (Chat, Message, User, Media) has a dedicated repository managing CRUD operations and ensuring consistency.
3. **Core API**: Provides structured access to chat data. Applications interact with `ChatStorage` and `ChatContext` objects, which expose chats, messages, users, and media. The core handles caching, object lifecycle, and orchestrates import and persistence operations.
4. **Utilities**: Common helper classes provide logging, file handling, platform abstractions, and string utilities to ensure cross-platform consistency and maintainable code.

## Architecture Overview
ChatStorage follows a layered architecture designed for separation of concerns:

- **Public API**
  Stable interfaces for external consumption: `ChatStorage`, `ChatContext`, `Chat`, `Message`, `User`, `Media`, `ChatSource`. These define the operations clients can perform without exposing implementation details.

- **Core Domain**
  Implements the business logic for chat storage, retrieval, and manipulation. Handles orchestration between importers and persistence, caching, and API object management.

- **Importer Layer**
  Contains parsers for different chat formats. Each parser implements a common interface, allowing the framework to support multiple input formats seamlessly. Includes `ImportManager` for coordinating parsing, and `ChatImportContext` for maintaining state during imports.

- **Database / Persistence Layer**
  Repositories (`ChatRepository`, `MessageRepository`, `UserRepository`, `MediaRepository`) handle SQLite storage. `PersistenceManager` coordinates transactions and database connections. `Statement` classes encapsulate prepared SQL statements.

- **Common / Infrastructure**
  Cross-cutting utilities: logging (`Logger`), file operations (`FileUtil`), string processing (`StringUtil`), platform abstraction (`Platform`), and command-line option parsing (`OptionParser`). Ensures maintainability and consistency across all layers.

See `docs/ChatStorage_HighLevel_Architecture.puml` for a high-level UML diagram of the architecture.
This diagram illustrates the dependencies and flow between the Public API, Core Domain, Importer, Persistence, and Common layers.


