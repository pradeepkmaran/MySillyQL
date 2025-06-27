# MySillyQL 🗃️

A lightweight, educational database management system with a custom SQL-like query language. Built in C++ with a focus on simplicity and learning.

## 🚀 Features

- **Custom Query Language**: Uses "silly" syntax for database operations
- **Type System**: Supports INTEGER, TEXT, DECIMAL, BOOLEAN, and BLOB data types
- **File Persistence**: Automatic saving and loading of databases and schemas
- **Modular Architecture**: Clean separation of concerns with organized codebase
- **Memory Safe**: Proper resource management and error handling

## 🛠️ Building

### Prerequisites
- C++17 compatible compiler (GCC, Clang, etc.)
- Make

### Compilation
```bash
# Build the main application
make all

# Build and run tests
make test
./test_sillyql

# Clean build artifacts
make clean
```

## 📖 Usage

### Starting the Database
```bash
./sillyql
```

### Creating a Database
```sql
silly make db my_database;
silly use my_database;
```

### Creating Tables
```sql
silly make table users with columns 
  id of integer as primary, 
  name of text, 
  email of text, 
  age of integer, 
  active of boolean;
```

### Inserting Data
```sql
silly put 1 "John-Doe" "john@example.com" 25 true inside table users;
silly put 2 "Jane-Smith" "jane@example.com" 30 false inside table users;
```

### Querying Data
```sql
silly get users;
```

### Saving Changes
```sql
silly save;
```

### Exiting
```sql
silly exit;
```

## 📁 Project Structure

```
MySillyQL/
├── src/
│   ├── common/          # Shared types and utilities
│   │   ├── types.hpp    # Core enums and type definitions
│   │   ├── utils.hpp    # Helper functions
│   │   └── utils.cpp
│   ├── storage/         # Data storage layer
│   │   ├── value.hpp    # Value class for data storage
│   │   ├── schema.hpp   # Table schema definitions
│   │   ├── row.hpp      # Row data structure
│   │   └── table.hpp    # Table management
│   ├── database/        # Database management
│   │   ├── database.hpp # Database operations
│   │   └── database.cpp
│   ├── query/          # Query processing
│   │   ├── parser.hpp   # SQL parser
│   │   ├── parser.cpp
│   │   ├── executor.hpp # Query executor
│   │   └── executor.cpp
│   ├── io/             # Input/Output handling
│   │   ├── input_output.hpp
│   │   └── input_output.cpp
│   └── main.cpp        # Application entry point
├── tests/
│   └── test_main.cpp   # Unit tests
├── Makefile            # Build configuration
└── README.md
```

## 🎯 Design Philosophy

MySillyQL was designed with these principles:

- **Educational Focus**: Easy to understand codebase for learning database concepts
- **Simplicity**: Minimal dependencies, straightforward implementation
- **Modularity**: Clean separation between parsing, storage, and execution
- **Type Safety**: Strong typing with proper error handling

## 🔧 Supported Data Types

| Type    | Description | Example |
|---------|-------------|---------|
| INTEGER | 64-bit signed integers | `42`, `-123` |
| TEXT    | Variable-length strings | `"Hello World"` |
| DECIMAL | Double-precision floats | `3.14`, `-2.5` |
| BOOLEAN | True/false values | `true`, `false` |
| BLOB    | Binary data | *(future implementation)* |

## 📝 Query Language Reference

### Database Operations
- `silly make db <name>;` - Create a new database
- `silly use <name>;` - Switch to a database
- `silly save;` - Save all changes to disk

### Table Operations
- `silly make table <name> with columns <column_definitions>;` - Create table
- `silly get <table_name>;` - Select all rows from table

### Data Operations
- `silly put <values> inside table <table_name>;` - Insert row
- `silly exit;` - Exit the application

### Column Definition Syntax
```
<column_name> of <type> [as primary]
```

## 🧪 Testing

The project includes unit tests to verify functionality:

```bash
make test
./test_sillyql
```

Tests cover:
- Value creation and type handling
- Schema and table operations
- Row insertion and retrieval
- File persistence

## 🤝 Contributing

This is primarily an educational project, but contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## 📄 License

This project is open source and available under the [MIT License](LICENSE).

## 🎓 Learning Goals

This project demonstrates:
- Database storage engine basics
- Custom language parsing and execution
- File-based persistence
- Memory management in C++
- Modular software architecture
- Test-driven development

## 🔮 Future Enhancements

- [ ] WHERE clause support for selective queries
- [ ] UPDATE and DELETE operations
- [ ] Indexing for faster queries
- [ ] JOIN operations between tables
- [ ] Transaction support
- [ ] Network protocol for client-server architecture
- [ ] Query optimization
- [ ] BLOB data type implementation
- [ ] Support multi-word values for strings

## 🏗️ Architecture Overview

MySillyQL follows a layered architecture:

1. **Presentation Layer** (`io/`): Handles user input/output
2. **Query Layer** (`query/`): Parses and executes commands
3. **Storage Layer** (`storage/`): Manages data structures and schemas
4. **Database Layer** (`database/`): Coordinates database operations
5. **Common Layer** (`common/`): Shared utilities and types

## 📈 Performance Characteristics

- **Memory Usage**: Efficient with dynamic allocation
- **Storage**: File-based persistence with schema separation
- **Scalability**: Suitable for educational and small-scale applications
- **Concurrency**: Single-threaded design (future enhancement opportunity)
