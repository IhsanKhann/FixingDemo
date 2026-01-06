# Social Media System

## Overview
A console-based social media management system implemented in C++. This application provides user management, notification handling, and browsing history features.

## Project Structure
- `Main.cpp` - Main entry point with menu selection
- `MasterHeader.h` - Header file declaring menu functions
- `User_manage.cpp` - User management system (registration, login, follow/unfollow)
- `Notification TEst 2.cpp` - Notification system with priority queue
- `HISTORY TEST 2.cpp` - Browsing history system using stack

## Building
The project uses clang++ to compile:
```bash
clang++ -o SocialMediaApp Main.cpp User_manage.cpp "HISTORY TEST 2.cpp" "Notification TEst 2.cpp"
```

## Running
Execute the compiled binary:
```bash
./SocialMediaApp
```

## Features
1. **User Management** - Register, login, profile management, follow/unfollow
2. **Notifications** - Priority queue implementation for notifications (Comments > Likes > Follows)
3. **History** - Stack-based browsing history for profiles and posts

## Dependencies
- C++ compiler (clang++)
- Standard C++ libraries

## Recent Changes
- January 2026: Fixed merge conflicts in History and Notification files
- Configured for Replit environment
