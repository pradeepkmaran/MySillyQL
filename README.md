# MySillyQL 
A basic SQLite-like database engine written in C++. This project implements a simple database with a B-tree structure for data storage and retrieval.

## Overview
This project creates a database that:
* Stores records with ID, username, and email fields
* Uses a B-tree data structure for efficient lookups
* Persists data to disk
* Supports basic SQL-like commands (insert, select)
* Includes meta commands for database management

## Getting Started
Building the Project
```bash
g++ -o mysillyql main.cpp
```
Running the Database
```bash
./mysillyql mydb.db
```
You must provide a database filename as an argument.

## Commands
### SQL Commands

insert ID USERNAME EMAIL - Insert a new record
select - Display all records in the database

### Meta Commands

silly exit - Save changes and exit
silly btree - Display the current B-tree structure
silly constants - Show internal constants and configuration

## Project Structure
The database engine consists of:

* B-tree implementation for data storage
* Pager system for memory management and disk I/O
* SQL command parser and executor
* Cursor-based record access

## Limitations

Currently supports only leaf nodes (no internal nodes)
No support for deleting or updating records
No support for indexes or transactions

## Acknowledgements
This project was inspired by and referred from cstack/db_tutorial: Writing a SQLite clone from scratch in C.
