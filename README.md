# Student Database Management System
A robust C++ application for managing student records using PostgreSQL. This console-based CRUD application features input validation, transaction management, and a clean singleton database connection pattern.
## 🎯 Features
- **Create** – Add new students with name, group, date of birth, and average score
- **Read** – Display all students in a formatted list
- **Update** – Edit any field of an existing student record
- **Delete** – Remove students by ID
- **Input Validation** – Strict checks for:
  - Date format (DD.MM.YYYY)
  - Average score range (0.0 – 5.0)
  - Non‑empty text fields
  - Integer ranges
- **Transaction Support** – All operations use BEGIN/COMMIT/ROLLBACK
- **Error Handling** – Custom exceptions for database errors, invalid input, and missing records
## 🛠️ Tech Stack
| Component       | Technology                          |
|----------------|-------------------------------------|
| Language       | C++17                               |
| Database       | PostgreSQL                          |
| Connection API | libpq (PostgreSQL C API)            |
| Build System   | Any C++ compiler (g++/clang/MSVC)   |
## 📋 Prerequisites
- PostgreSQL server (running and accessible)
- `libpq` development libraries
- C++ compiler with C++17 support
## 📄 License
The project is intended for educational purposes. It can be used and modified freely.