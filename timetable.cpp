#include "timetable.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <limits>

//globals
Course courses[200];
Professor professors[200];
ScheduleEntry timetable[1000];

int courseCount = 0;
int professorCount = 0;
int scheduleCount = 0;

Room allRooms[60];
int totalRooms = 0;
int currentTheoryRoom = 0;
int currentLabRoom = 0;


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <limits>     // FIX: for numeric_limits
#include <algorithm>  // FIX: for std::max
#include "timetable.h"
using namespace std;

// ============= HELPER FUNCTIONS =============

int stringToInt(string str) {
    int result = 0;
    for (int i = 0; i < (int)str.length(); i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
        }
    }
    return result;
}

bool stringToBool(string str) {
    return (str == "TRUE" || str == "true" || str == "1");
}

vector<string> split(string str, char delimiter) {
    vector<string> tokens;
    string token;
    stringstream ss(str);
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool contains(string str, string substring) {
    return str.find(substring) != string::npos;
}

string allocateTheoryRoom() {
    for (int i = currentTheoryRoom; i < 39; i++) {
        if (allRooms[i].type == "Theory") {
            string roomCode = allRooms[i].code;
            currentTheoryRoom = i + 1;
            if (currentTheoryRoom >= 39) currentTheoryRoom = 0;
            return roomCode;
        }
    }
    return "G01";
}

string allocateLabRoom() {
    for (int i = 39 + currentLabRoom; i < totalRooms; i++) {
        if (allRooms[i].type == "Lab") {
            string roomCode = allRooms[i].code;
            currentLabRoom = (i - 39) + 1;
            if (currentLabRoom >= 17) currentLabRoom = 0;
            return roomCode;
        }
    }
    return "L01";
}

void resetRoomCounters() {
    currentTheoryRoom = 0;
    currentLabRoom = 0;
}

void initializeRooms() {
    totalRooms = 0;

    // Ground Floor Theory Rooms
    string groundRooms[] = {"G01", "G02", "G03", "G04", "G05", "G06", "G07",
                            "G08", "G09", "G10", "G11", "G12", "G13"};
    for (int i = 0; i < 13; i++) {
        allRooms[totalRooms].code = groundRooms[i];
        allRooms[totalRooms].type = "Theory";
        totalRooms++;
    }

    // First Floor Theory Rooms
    string firstFloorRooms[] = {"101", "102", "103", "104", "105", "106", "107",
                                "108", "109", "110", "111", "112", "113"};
    for (int i = 0; i < 13; i++) {
        allRooms[totalRooms].code = firstFloorRooms[i];
        allRooms[totalRooms].type = "Theory";
        totalRooms++;
    }

    // Second Floor Theory Rooms
    string secondFloorRooms[] = {"201", "202", "203", "204", "205", "206", "207",
                                 "208", "209", "210", "211", "212", "213"};
    for (int i = 0; i < 13; i++) {
        allRooms[totalRooms].code = secondFloorRooms[i];
        allRooms[totalRooms].type = "Theory";
        totalRooms++;
    }

    // Lab Rooms
    string labRooms[] = {"L01", "L02", "L03", "L04", "L05", "L06", "L07", "L08",
                         "L09", "L10", "L11", "L12", "L13", "L14", "L15", "L16", "L17"};
    for (int i = 0; i < 17; i++) {
        allRooms[totalRooms].code = labRooms[i];
        allRooms[totalRooms].type = "Lab";
        totalRooms++;
    }

    cout << "   Initialized " << totalRooms << " rooms" << endl;
}
//cause we dont  want calsses from 8:30-5:30 continuous they should either be 8:30-2:30 or 11:30 to 5:30
// Decide allowed block for a semester

bool isMorningBlock(int semester) {
    return (semester % 2 == 1); // odd semesters morning, even semesters afternoon
}

// Check if a slot belongs to the allowed block
bool slotAllowed(int semester, int slotIndex) {
    if (isMorningBlock(semester)) {
        // Morning block = slots 0..2 (8:30-1:00) + slot 3 (1:30-2:30)
        return (slotIndex >= 0 && slotIndex <= 3);
    } else {
        // Afternoon block = slots 2..5 (11:30-5:30)
        return (slotIndex >= 2 && slotIndex <= 5);
    }
}


// ============= DATA LOADER FUNCTIONS =============

void loadCourses(string filename) {
    ifstream file(filename.c_str());
    if (!file) {
        cout << "Error: Cannot open courses.csv" << endl;
        return;
    }

    string line;
    getline(file, line); // header

    while (getline(file, line) && courseCount < MAX_COURSES) {
        vector<string> tokens = split(line, ',');
        if (tokens.size() >= 12) {
            courses[courseCount].code = tokens[0];
            courses[courseCount].name = tokens[1];
            courses[courseCount].credits = stringToInt(tokens[2]);
            courses[courseCount].theoryHours = stringToInt(tokens[3]);
            courses[courseCount].labHours = stringToInt(tokens[4]);
            courses[courseCount].category = tokens[5];
            courses[courseCount].prerequisites = tokens[6];
            courses[courseCount].mandatory = stringToBool(tokens[7]);
            courses[courseCount].electiveGroup = tokens[8];
            courses[courseCount].domain = tokens[9];
            courses[courseCount].scheduleType = tokens[10];
            courses[courseCount].semester = stringToInt(tokens[11]);
            courses[courseCount].isAssigned = false;
            courseCount++;
        }
    }
    file.close();
    cout << "   Loaded " << courseCount << " courses" << endl;
}

void loadProfessors(string filename) {
    ifstream file(filename.c_str());
    if (!file) {
        cout << "Error: Cannot open professors.csv" << endl;
        return;
    }

    string line;
    getline(file, line); // header

    while (getline(file, line) && professorCount < MAX_PROFESSORS) {
        vector<string> tokens = split(line, ',');
        if (tokens.size() >= 8) {
            professors[professorCount].id = tokens[0];
            professors[professorCount].name = tokens[1];
            professors[professorCount].department = tokens[2];
            professors[professorCount].rank = tokens[3];
            professors[professorCount].expertise = tokens[4];
            professors[professorCount].preferredCourses = tokens[5];
            professors[professorCount].maxLoad = stringToInt(tokens[6]);
            professors[professorCount].status = tokens[7];
            professorCount++;
        }
    }
    file.close();
    cout << "   Loaded " << professorCount << " professors" << endl;
}

int findProfessorForCourse(const Course& course, int professorCourseCount[]) {
    for (int i = 0; i < professorCount; i++) {
        if (professors[i].status != "ACTIVE") continue;
        if (professorCourseCount[i] >= 3) continue; // NEW: enforce max 3 courses

        if (contains(professors[i].preferredCourses, course.code)) {
            return i;
        }
    }

    for (int i = 0; i < professorCount; i++) {
        if (professors[i].status != "ACTIVE") continue;
        if (professorCourseCount[i] >= 3) continue; // NEW: enforce max 3 courses

        if ((course.domain == "CS" || course.domain == "Cyber") &&
            professors[i].department == "Computer Science") {
            return i;
        } else if (course.domain == "Math" &&
                   professors[i].department == "Mathematics") {
            return i;
        } else if (course.domain == "Humanities" &&
                   professors[i].department == "Humanities") {
            return i;
        } else if ((course.domain == "Science" || course.domain == "Biology" || course.domain == "Chemistry") &&
                   (professors[i].department == "Physics" ||
                    professors[i].department == "Bioscience" ||
                    professors[i].department == "Chemistry")) {
            return i;
        } else if (course.domain == "Management" &&
                   professors[i].department == "Management Sciences") {
            return i;
        }
    }

    return -1;
}

// ============= SCHEDULING HELPERS =============

bool scheduleSingleSession(int semester,
                           const string& day,
                           const string& timeSlot,
                           const string& room,
                           const Course& course,
                           const Professor& prof,
                           const string& type) {
    if (scheduleCount >= MAX_SCHEDULES) return false;
    timetable[scheduleCount].courseCode = course.code;
    timetable[scheduleCount].courseName = course.name;
    timetable[scheduleCount].professorName = prof.name;
    timetable[scheduleCount].professorId = prof.id;
    timetable[scheduleCount].day = day;
    timetable[scheduleCount].timeSlot = timeSlot;
    timetable[scheduleCount].room = room;
    timetable[scheduleCount].type = type;
    timetable[scheduleCount].semester = semester;
    scheduleCount++;
    return true;
}

// ============= SCHEDULER =============
void generateSemesterTimetable(int semester) {
    resetRoomCounters();

    string morningSlots[]   = {"8:30-10:00", "10:00-11:30", "11:30-1:00"};
    string afternoonSlots[] = {"1:30-2:30", "2:30-4:00", "4:30-5:30"};
    string days[]           = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

    int professorLoad[200] = {0};          // credit hours
    int professorCourseCount[200] = {0};   // number of distinct courses
    bool timeSlotUsed[5][6] = {false};     // 5 days × 6 slots
    bool courseScheduled[200] = {false};

    cout << "\n   Scheduling Semester " << semester << " courses:" << endl;

    for (int i = 0; i < courseCount; i++) {
        if (courses[i].semester != semester) continue;
        if (courseScheduled[i]) continue;
        if (courses[i].scheduleType == "ONLINE" || courses[i].scheduleType == "PROJECT") continue;

        int profIndex = findProfessorForCourse(courses[i], professorCourseCount);
        if (profIndex == -1) continue;

        // Enforce both credit load and max 3 courses
        if (professorLoad[profIndex] >= professors[profIndex].maxLoad) continue;
        if (professorCourseCount[profIndex] >= 3) continue;

        // --- declare these variables here ---
        int requiredTheorySessions = 0;
        if (courses[i].credits == 4) requiredTheorySessions = 3;
        else if (courses[i].credits == 3) requiredTheorySessions = 2;
        else if (courses[i].credits == 2) requiredTheorySessions = 2;
        else requiredTheorySessions = (courses[i].theoryHours > 0 ? courses[i].theoryHours : 1);

        int sessionsScheduled = 0;
        // -----------------------------------

        // Schedule theory sessions
        for (int session = 0; session < requiredTheorySessions; session++) {
            bool foundSlot = false;
            for (int dayIndex = 0; dayIndex < 5 && !foundSlot; dayIndex++) {
                for (int slotIndex = 0; slotIndex < 6 && !foundSlot; slotIndex++) {
                    if (!timeSlotUsed[dayIndex][slotIndex] && slotAllowed(semester, slotIndex)) {
                        string timeSlot = (slotIndex < 3) ? morningSlots[slotIndex]
                                                          : afternoonSlots[slotIndex - 3];
                        string room = allocateTheoryRoom();

                        scheduleSingleSession(semester, days[dayIndex], timeSlot, room,
                                              courses[i], professors[profIndex], "Theory");
                        timeSlotUsed[dayIndex][slotIndex] = true;
                        sessionsScheduled++;
                        foundSlot = true;
                    }
                }
            }
        }

        // Schedule lab if needed (2 consecutive slots)
        if (courses[i].labHours > 0 && sessionsScheduled >= requiredTheorySessions) {
            bool labScheduled = false;
            for (int dayIndex = 0; dayIndex < 5 && !labScheduled; dayIndex++) {
                for (int slotIndex = 0; slotIndex < 5 && !labScheduled; slotIndex++) {
                    if (!timeSlotUsed[dayIndex][slotIndex] &&
                        !timeSlotUsed[dayIndex][slotIndex+1] &&
                        slotAllowed(semester, slotIndex) &&
                        slotAllowed(semester, slotIndex+1)) {

                        string labRoom = allocateLabRoom();
                        string timeSlot1 = (slotIndex < 3) ? morningSlots[slotIndex]
                                                           : afternoonSlots[slotIndex - 3];
                        string timeSlot2 = ((slotIndex+1) < 3) ? morningSlots[slotIndex+1]
                                                                 : afternoonSlots[(slotIndex+1) - 3];

                        scheduleSingleSession(semester, days[dayIndex], timeSlot1, labRoom,
                                              courses[i], professors[profIndex], "Lab");
                        scheduleSingleSession(semester, days[dayIndex], timeSlot2, labRoom,
                                              courses[i], professors[profIndex], "Lab");

                        timeSlotUsed[dayIndex][slotIndex] = true;
                        timeSlotUsed[dayIndex][slotIndex+1] = true;
                        labScheduled = true;
                    }
                }
            }
        }

        // Finalize assignment
        if (sessionsScheduled >= requiredTheorySessions) {
            professorLoad[profIndex] += courses[i].credits;
            professorCourseCount[profIndex]++;   // count distinct course assignment
            courseScheduled[i] = true;
        }
    }
}

void generateAllTimetables() {
    scheduleCount = 0;

    cout << "\n   Generating timetables for all semesters..." << endl;
    cout << "   ----------------------------------------" << endl;

    for (int sem = 1; sem <= 8; sem++) {
        cout << "\n   Generating Semester " << sem << "..." << endl;

        int coursesInSemester = 0;
        for (int i = 0; i < courseCount; i++) {
            if (courses[i].semester == sem) {
                coursesInSemester++;
            }
        }

        if (coursesInSemester == 0) {
            cout << "   No courses in Semester " << sem << ", skipping..." << endl;
            continue;
        }

        cout << "   Found " << coursesInSemester << " courses for Semester " << sem << endl;

        generateSemesterTimetable(sem);
        cout << "   Semester " << sem << " timetable generated" << endl;
    }

    cout << "\n   Total scheduled sessions: " << scheduleCount << endl;
}

// ============= DISPLAY FUNCTIONS =============

void displaySemesterTimetable(int semester)  {
    cout << "\n\n";
    cout << "========================================================" << endl;
    cout << "           BCT SEMESTER " << semester << " TIMETABLE           " << endl;
    cout << "========================================================\n" << endl;

    string days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    bool semesterHasClasses = false;

    for (int d = 0; d < 5; d++) {
        bool dayHasClasses = false;

        for (int i = 0; i < scheduleCount; i++) {
            if (timetable[i].semester == semester && timetable[i].day == days[d]) {
                dayHasClasses = true;
                semesterHasClasses = true;
                break;
            }
        }

        if (!dayHasClasses) continue;

        cout << "\n" << days[d] << ":" << endl;
        cout << "--------------------------------------------------------" << endl;
        cout << " Time       | Course       | Professor          | Room   " << endl;
        cout << "--------------------------------------------------------" << endl;

        for (int i = 0; i < scheduleCount; i++) {
            if (timetable[i].semester == semester && timetable[i].day == days[d]) {
                string profName = timetable[i].professorName.substr(0, 15);
                cout << " " << timetable[i].timeSlot << " | "
                     << timetable[i].courseCode << " (" << timetable[i].type << ") | "
                     << profName << " | "
                     << timetable[i].room << endl;
            }
        }
    }

    if (!semesterHasClasses) {
        cout << "\n   No courses scheduled for Semester " << semester << endl;
    }

    // Unique course count
    set<string> uniqueCourses;
    for (int i = 0; i < scheduleCount; i++) {
        if (timetable[i].semester == semester) {
            uniqueCourses.insert(timetable[i].courseCode);
        }
    }

    cout << "\n========================================================" << endl;
    cout << "   Total courses in Semester " << semester << ": " << uniqueCourses.size() << endl;
    cout << "========================================================\n" << endl;
}

void displayAllTimetables() {
    for (int sem = 1; sem <= 8; sem++) {
        displaySemesterTimetable(sem);
    }
}

void showCourseSemesterAssignment() {

    cout << "\n\n";
    cout << "========================================================" << endl;
    cout << "         COURSE SEMESTER ASSIGNMENT REPORT             " << endl;
    cout << "========================================================\n" << endl;

    cout << " Course Code | Course Name                     | Semester | Credits" << endl;
    cout << "-------------------------------------------------------------------" << endl;

    for (int i = 0; i < courseCount; i++) {
        cout << " " << courses[i].code;
        for (int s = (int)courses[i].code.length(); s < 12; s++) cout << " ";

        string nameTrim = courses[i].name.substr(0, 30);
        cout << "| " << nameTrim;
        for (int s = (int)nameTrim.length(); s < 30; s++) cout << " ";

        cout << "|     " << courses[i].semester << "     | ";
        cout << courses[i].credits << endl;
    }

    cout << "\n========================================================" << endl;
    cout << "   Total Courses: " << courseCount << endl;
    cout << "========================================================\n" << endl;
}

void generateReports() {
    cout << "\n\n";
    cout << "========================================================" << endl;
    cout << "           PROFESSOR WORKLOAD REPORT                   " << endl;
    cout << "========================================================\n" << endl;

    // Count unique courses per professor (not sessions)
    set<string> profCourses[200];
    int profCreditCount[200] = {0};

    for (int i = 0; i < scheduleCount; i++) {
        for (int p = 0; p < professorCount; p++) {
            if (professors[p].id == timetable[i].professorId) {
                string courseCode = timetable[i].courseCode;
                if (!profCourses[p].count(courseCode)) {
                    profCourses[p].insert(courseCode);
                    // add credits once per course
                    for (int c = 0; c < courseCount; c++) {
                        if (courses[c].code == courseCode) {
                            profCreditCount[p] += courses[c].credits;
                            break;
                        }
                    }
                }
                break;
            }
        }
    }

    cout << " Professor Name                  | Courses | Credit Hours" << endl;
    cout << "--------------------------------------------------------" << endl;

    for (int p = 0; p < professorCount; p++) {
        if (!profCourses[p].empty()) {
            cout << " " << professors[p].name;
            for (int s = (int)professors[p].name.length(); s < 30; s++) {
                cout << " ";
            }
            cout << "|    " << profCourses[p].size() << "    |      "
                 << profCreditCount[p] << endl;
        }
    }

    cout << "\n\n";
    cout << "========================================================" << endl;
    cout << "           SEMESTER-WISE COURSE COUNT                  " << endl;
    cout << "========================================================" << endl;

    for (int sem = 1; sem <= 8; sem++) {
        set<string> uniqueSemesterCourses;
        for (int i = 0; i < scheduleCount; i++) {
            if (timetable[i].semester == sem) {
                uniqueSemesterCourses.insert(timetable[i].courseCode);
            }
        }
        cout << " Semester " << sem << ": " << uniqueSemesterCourses.size() << " courses" << endl;
    }
}

