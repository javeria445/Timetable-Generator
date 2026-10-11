# Timetable Management System

A desktop application for creating and managing class timetables, built with **C++** and the **Qt framework**. Developed as my final project for [course name / semester].

## Features

- Add, edit, and delete timetable entries [courses, teachers, rooms, time slots]
- [Clash detection: prevents overlapping classes for the same teacher/room/section]
- [View timetable by day / teacher / section / room]
- [Save and load data from files]
- Clean graphical interface built with Qt Widgets

## Tech Stack

| Category | Details |
|----------|---------|
| Language | C++ |
| Framework | Qt 6.10.1 |
| Build System | CMake |
| Compiler | MinGW 64-bit |
| IDE | Qt Creator |

## Project Structure

```
Timetable Management System/
├── Source Files/    # C++ source code, headers, UI files, CMakeLists.txt
├── Data files/      # [Sample data used by the application]
└── README.md
```

## Getting Started

### Prerequisites
- Qt 6.x (with the MinGW 64-bit kit)
- CMake 3.16 or newer
- Qt Creator (recommended)

### Build and Run
1. Clone the repository:
```
   git clone https://github.com/<your-username>/<repo-name>.git
```
2. Open `Source Files/CMakeLists.txt` in Qt Creator.
3. Select the **Desktop Qt 6.10.1 MinGW 64-bit** kit and click **Configure Project**.
4. Press **Build**, then **Run**.

## Screenshots

[Add 2-3 screenshots here: main window, add-entry form, timetable view]

```
![Main Window](screenshots/main.png)
```

## What I Learned

- Designing a GUI application with Qt Widgets and signals/slots
- Structuring a C++ project with object-oriented principles
- [File handling / data storage]
- [Scheduling logic and conflict handling]

## Future Improvements

- [Export timetable to PDF / Excel]
- [Database support instead of file storage]
- [User login for admin and students]

## Author

**Javeria Anis**
BS Computer Technology (Cybersecurity), COMSATS University Islamabad
[LinkedIn profile link] | [GitHub profile link]
