#ifndef TIMETABLE_H
#define TIMETABLE_H

#define MAX_COURSES 200
#define MAX_PROFESSORS 200
#define MAX_SCHEDULES 1000

#include <string>
#include <vector>
#include <set>
using namespace std;




// ===== STRUCTS =====
struct Course {
    string code, name, category, prerequisites, electiveGroup, domain, scheduleType;
    int credits, theoryHours, labHours, semester;
    bool mandatory, isAssigned;
};

struct Professor {
    string id, name, department, rank, expertise, preferredCourses, status;
    int maxLoad;
};

struct ScheduleEntry {
    string courseCode, courseName, professorName, professorId;
    string day, timeSlot, room, type;
    int semester;
};

struct Room {
    string code, type;
};

// ===== GLOBALS =====
extern Course courses[];
extern Professor professors[];
extern ScheduleEntry timetable[];

extern int courseCount;
extern int professorCount;
extern int scheduleCount;

// ===== FUNCTIONS (we will call from GUI) =====
void initializeRooms();
void loadCourses(string filename);
void loadProfessors(string filename);
void generateAllTimetables();

#endif
