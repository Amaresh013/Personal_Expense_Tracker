# 💰 Personal Expense Tracker

A menu-driven, terminal-based expense tracker written in **C**.

## Features
- Admin login, logout, add new admin, forgot username/password
- Add, view, search, update and delete expenses
- Add and delete categories
- Monthly budget with a warning when it is crossed
- Summary report with category-wise spending
- Data saved in files, with input validation

## How to Run
```bash
gcc -Wall -Wextra -o expense_tracker *.c
./expense_tracker
```
On Windows, run `expense_tracker.exe`.

On the first run, choose **2. Register Admin** to create the first admin, then **1. Login**.

## Files
| File | Purpose |
|---|---|
| `main.c` | Main menu and program flow |
| `auth.c` | Login, register, password recovery |
| `expense.c` | Expense add, view, search, update, delete |
| `category.c` | Category management |
| `report.c` | Summary report and budget |
| `input.c` / `storage.c` | Safe input and file saving |
| `expense.h` | Structures and function list |

## Note
This is a beginner learning project built to practice C programming: structures, file handling, modular code and input validation. Passwords are hashed with a simple method, so it is not meant for real-world security.