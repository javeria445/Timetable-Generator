#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVector>
#include <QSet>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// ============= DATA STRUCTURES =============
struct Course {
    QString code;
    QString name;
    int credits;
    int theoryHours;
    int labHours;
    QString category;
    QString prerequisites;
    bool mandatory;
    QString electiveGroup;
    QString domain;
    QString scheduleType;
    int semester;
    bool isAssigned;
};

struct Professor {
    QString id;
    QString name;
    QString department;
    QString rank;
    QString expertise;
    QString preferredCourses;
    int maxLoad;
    QString status;
};

struct ScheduleEntry {
    QString courseCode;
    QString courseName;
    QString professorName;
    QString professorId;
    QString day;
    QString timeSlot;
    QString room;
    QString type;
    int semester;
};

struct Room {
    QString code;
    QString type;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnLoadCourses_clicked();
    void on_btnLoadProfessors_clicked();
    void on_btnGoGenerate_clicked();
    void on_btnGenerateTimetable_clicked();
    void on_btnNextSemester_clicked();
    void on_btnPrevSemester_clicked();
    void on_btnBackHome_clicked();
    void on_btnBackHomeFromLoad_clicked();
    void on_btnBackHomeFromResults_clicked();
    void on_btnGetStarted_clicked();
    void on_btnGoResults_clicked();
    void on_comboSemester_currentIndexChanged(int index);
    void on_comboProfessor_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

    // Data storage
    QVector<Course> courses;
    QVector<Professor> professors;
    QVector<ScheduleEntry> timetable;
    QVector<Room> allRooms;

    int currentTheoryRoom;
    int currentLabRoom;
    int currentSemesterView;

    // Helper functions
    void initializeRooms();
    void loadCourses(const QString& filename);
    void loadProfessors(const QString& filename);
    void generateAllTimetables();
    void generateSemesterTimetable(int semester);
    void displaySemesterTimetableGrid(int semester);
    void displayProfessorWorkload();
    void updateDataPreview();
    void saveTimetableToCSV();
    bool isProfessorAvailable(const QString& professorId, int semester,
                              int dayIndex, int slotIndex,
                              bool timeSlotUsed[5][6]);//to prevent consecutive classes more than 3 hours

    // Scheduling helpers
    int findProfessorForCourse(const Course& course, QVector<int>& professorCourseCount);
    bool scheduleSingleSession(int semester, const QString& day, const QString& timeSlot,
                               const QString& room, const Course& course,
                               const Professor& prof, const QString& type);
    QString allocateTheoryRoom();
    QString allocateLabRoom();
    void resetRoomCounters();
    bool isMorningBlock(int semester);
    bool slotAllowed(int semester, int slotIndex);
};

#endif // MAINWINDOW_H
