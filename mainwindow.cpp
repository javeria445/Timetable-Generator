#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QHeaderView>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentTheoryRoom(0)
    , currentLabRoom(0)
    , currentSemesterView(1)
{
    ui->setupUi(this);

    initializeRooms();

    ui->stackedPages->setCurrentWidget(ui->pageHome);

    for (int i = 1; i <= 8; i++) {
        ui->comboSemester->addItem(QString("Semester %1").arg(i));
    }

    ui->tableDataPreview->setColumnCount(4);
    ui->tableDataPreview->setHorizontalHeaderLabels({"Code", "Name", "Credits", "Semester"});
    ui->tableDataPreview->horizontalHeader()->setStretchLastSection(true);

    ui->tableSemester->setColumnCount(6);
    ui->tableSemester->setHorizontalHeaderLabels({"", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"});
    ui->tableSemester->horizontalHeader()->setStretchLastSection(true);

    ui->tableProfessorWorkload->setColumnCount(3);
    ui->tableProfessorWorkload->setHorizontalHeaderLabels({"Professor", "Courses", "Status"});
    ui->tableProfessorWorkload->horizontalHeader()->setStretchLastSection(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeRooms()
{
    allRooms.clear();

    QStringList groundRooms = {"G01", "G02", "G03", "G04", "G05", "G06", "G07", "G08", "G09", "G10", "G11", "G12", "G13"};
    for (const QString& code : groundRooms) {
        Room r; r.code = code; r.type = "Theory"; allRooms.append(r);
    }

    QStringList firstFloorRooms = {"101", "102", "103", "104", "105", "106", "107", "108", "109", "110", "111", "112", "113"};
    for (const QString& code : firstFloorRooms) {
        Room r; r.code = code; r.type = "Theory"; allRooms.append(r);
    }

    QStringList secondFloorRooms = {"201", "202", "203", "204", "205", "206", "207", "208", "209", "210", "211", "212", "213"};
    for (const QString& code : secondFloorRooms) {
        Room r; r.code = code; r.type = "Theory"; allRooms.append(r);
    }

    QStringList labRooms = {"L01", "L02", "L03", "L04", "L05", "L06", "L07", "L08", "L09", "L10", "L11", "L12", "L13", "L14", "L15", "L16", "L17"};
    for (const QString& code : labRooms) {
        Room r; r.code = code; r.type = "Lab"; allRooms.append(r);
    }
}

QString MainWindow::allocateTheoryRoom()
{
    for (int i = 0; i < 39; i++) {
        int index = (currentTheoryRoom + i) % 39;
        if (allRooms[index].type == "Theory") {
            currentTheoryRoom = (index + 1) % 39;
            return allRooms[index].code;
        }
    }
    return "G01";
}

QString MainWindow::allocateLabRoom()
{
    int labStart = 39;
    int labCount = allRooms.size() - labStart;
    for (int i = 0; i < labCount; i++) {
        int index = labStart + ((currentLabRoom + i) % labCount);
        if (allRooms[index].type == "Lab") {
            currentLabRoom = (currentLabRoom + 1) % labCount;
            return allRooms[index].code;
        }
    }
    return "L01";
}

void MainWindow::resetRoomCounters()
{
    currentTheoryRoom = 0;
    currentLabRoom = 0;
}

bool MainWindow::isMorningBlock(int semester)
{
    return (semester % 2 == 1);
}

bool MainWindow::slotAllowed(int semester, int slotIndex)
{
    if (isMorningBlock(semester)) {
        return (slotIndex >= 0 && slotIndex <= 3);
    } else {
        return (slotIndex >= 2 && slotIndex <= 5);
    }
}

void MainWindow::loadCourses(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Cannot open courses file.");
        return;
    }
    courses.clear();
    QTextStream in(&file);
    if (!in.atEnd()) in.readLine();
    while (!in.atEnd()) {
        QStringList tokens = in.readLine().split(',');
        if (tokens.size() >= 12) {
            Course c;
            c.code = tokens[0].trimmed(); c.name = tokens[1].trimmed();
            c.credits = tokens[2].toInt(); c.theoryHours = tokens[3].toInt();
            c.labHours = tokens[4].toInt(); c.category = tokens[5].trimmed();
            c.prerequisites = tokens[6].trimmed(); c.mandatory = (tokens[7].toLower() == "true");
            c.electiveGroup = tokens[8].trimmed(); c.domain = tokens[9].trimmed();
            c.scheduleType = tokens[10].trimmed(); c.semester = tokens[11].toInt();
            c.isAssigned = false;
            courses.append(c);
        }
    }
    file.close();
    ui->lblCoursesCount->setText(QString("Loaded: %1 courses").arg(courses.size()));
    updateDataPreview();
}

void MainWindow::loadProfessors(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Cannot open professors file.");
        return;
    }
    professors.clear();
    QTextStream in(&file);
    if (!in.atEnd()) in.readLine();
    while (!in.atEnd()) {
        QStringList tokens = in.readLine().split(',');
        if (tokens.size() >= 8) {
            Professor p;
            p.id = tokens[0].trimmed(); p.name = tokens[1].trimmed();
            p.department = tokens[2].trimmed(); p.rank = tokens[3].trimmed();
            p.expertise = tokens[4].trimmed(); p.preferredCourses = tokens[5].trimmed();
            p.maxLoad = tokens[6].toInt(); p.status = tokens[7].trimmed().toUpper();
            professors.append(p);
        }
    }
    file.close();
    ui->lblProfessorsCount->setText(QString("Loaded: %1 professors").arg(professors.size()));
    ui->comboProfessor->clear();
    ui->comboProfessor->addItem("All Professors");
    for (const Professor& p : professors) ui->comboProfessor->addItem(p.name);
}

void MainWindow::updateDataPreview()
{
    ui->tableDataPreview->setRowCount(courses.size());
    for (int i = 0; i < courses.size(); i++) {
        ui->tableDataPreview->setItem(i, 0, new QTableWidgetItem(courses[i].code));
        ui->tableDataPreview->setItem(i, 1, new QTableWidgetItem(courses[i].name));
        ui->tableDataPreview->setItem(i, 2, new QTableWidgetItem(QString::number(courses[i].credits)));
        ui->tableDataPreview->setItem(i, 3, new QTableWidgetItem(QString::number(courses[i].semester)));
    }
}

int MainWindow::findProfessorForCourse(const Course& course, QVector<int>& professorCourseCount)
{
    for (int i = 0; i < professors.size(); i++) {
        if (professors[i].status != "ACTIVE") continue;
        if (professorCourseCount[i] >= 3) continue;
        if (professors[i].preferredCourses.contains(course.code)) return i;
    }
    for (int i = 0; i < professors.size(); i++) {
        if (professors[i].status != "ACTIVE") continue;
        if (professorCourseCount[i] >= 3) continue;
        if ((course.domain == "CS" && professors[i].department == "Computer Science") ||
            (course.domain == "Math" && professors[i].department == "Mathematics")) return i;
    }
    return -1;
}

bool MainWindow::isProfessorAvailable(const QString& professorId, int semester, int dayIndex, int slotIndex, bool timeSlotUsed[5][6])
{
    QStringList days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    QString currentDay = days[dayIndex];
    QVector<int> dailySlots;

    for (const ScheduleEntry& entry : timetable) {
        if (entry.professorId == professorId && entry.day == currentDay) {
            QStringList allSlots = {"8:30-10:00", "10:00-11:30", "11:30-1:00", "1:30-2:30", "2:30-4:00", "4:30-5:30"};
            int entrySlot = allSlots.indexOf(entry.timeSlot);
            if (entrySlot == slotIndex) return false;
            dailySlots.append(entrySlot);
        }
    }

    dailySlots.append(slotIndex);
    std::sort(dailySlots.begin(), dailySlots.end());

    int consecutive = 1;
    for (int i = 1; i < dailySlots.size(); i++) {
        if (dailySlots[i] == dailySlots[i-1] + 1) {
            consecutive++;
            if (consecutive > 2) return false;
        } else {
            consecutive = 1;
        }
    }
    return true;
}

bool MainWindow::scheduleSingleSession(int semester, const QString& day, const QString& timeSlot, const QString& room, const Course& course, const Professor& prof, const QString& type)
{
    ScheduleEntry entry;
    entry.courseCode = course.code; entry.courseName = course.name;
    entry.professorName = prof.name; entry.professorId = prof.id;
    entry.day = day; entry.timeSlot = timeSlot; entry.room = room;
    entry.type = type; entry.semester = semester;
    timetable.append(entry);
    return true;
}

void MainWindow::generateSemesterTimetable(int semester)
{
    QStringList morningSlots = {"8:30-10:00", "10:00-11:30", "11:30-1:00"};
    QStringList afternoonSlots = {"1:30-2:30", "2:30-4:00", "4:30-5:30"};
    QStringList days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

    QVector<int> professorCourseCount(professors.size(), 0);
    bool timeSlotUsed[5][6] = {false};

    auto semesterCourses = courses;
    std::sort(semesterCourses.begin(), semesterCourses.end(), [](const Course& a, const Course& b) {
        return a.credits > b.credits;
    });

    for (const auto& course : semesterCourses) {
        if (course.semester != semester || course.scheduleType == "ONLINE") continue;

        int profIdx = findProfessorForCourse(course, professorCourseCount);
        if (profIdx == -1) continue;

        int theoryNeeded = (course.credits >= 3) ? 2 : 1;
        int scheduled = 0;

        for (int d = 0; d < 5 && scheduled < theoryNeeded; d++) {
            for (int s = 0; s < 6 && scheduled < theoryNeeded; s++) {
                if (!timeSlotUsed[d][s] && slotAllowed(semester, s) && isProfessorAvailable(professors[profIdx].id, semester, d, s, timeSlotUsed)) {
                    QString slotStr = (s < 3) ? morningSlots[s] : afternoonSlots[s-3];
                    scheduleSingleSession(semester, days[d], slotStr, allocateTheoryRoom(), course, professors[profIdx], "Theory");
                    timeSlotUsed[d][s] = true;
                    scheduled++;
                }
            }
        }

        if (course.labHours > 0) {
            bool labDone = false;
            for (int d = 0; d < 5 && !labDone; d++) {
                for (int s = 0; s < 5 && !labDone; s++) {
                    if (!timeSlotUsed[d][s] && !timeSlotUsed[d][s+1] && slotAllowed(semester, s) && slotAllowed(semester, s+1) &&
                        isProfessorAvailable(professors[profIdx].id, semester, d, s, timeSlotUsed)) {
                        QString slot1 = (s < 3) ? morningSlots[s] : afternoonSlots[s-3];
                        QString slot2 = (s+1 < 3) ? morningSlots[s+1] : afternoonSlots[s+1-3];
                        QString lRoom = allocateLabRoom();
                        scheduleSingleSession(semester, days[d], slot1, lRoom, course, professors[profIdx], "Lab");
                        scheduleSingleSession(semester, days[d], slot2, lRoom, course, professors[profIdx], "Lab");
                        timeSlotUsed[d][s] = true; timeSlotUsed[d][s+1] = true;
                        labDone = true;
                    }
                }
            }
        }
        professorCourseCount[profIdx]++;
    }
}

void MainWindow::generateAllTimetables()
{
    timetable.clear();
    resetRoomCounters();
    for (int sem = 1; sem <= 8; sem++) generateSemesterTimetable(sem);
    ui->txtGenerationSummary->setText(QString("Total Sessions Scheduled: %1").arg(timetable.size()));
    QMessageBox::information(this, "Success", "Timetables generated with all constraints applied.");
}

void MainWindow::displaySemesterTimetableGrid(int semester)
{
    QStringList timeSlots = {"8:30-10:00", "10:00-11:30", "11:30-1:00", "1:30-2:30", "2:30-4:00", "4:30-5:30"};
    QStringList days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

    // Basic Table Setup
    ui->tableSemester->setRowCount(6);
    ui->tableSemester->setColumnCount(6);

    // Set Headers
    ui->tableSemester->setHorizontalHeaderLabels({"", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"});
    for (int i = 0; i < timeSlots.size(); i++) {
        ui->tableSemester->setItem(i, 0, new QTableWidgetItem(timeSlots[i]));
        ui->tableSemester->item(i, 0)->setBackground(QColor(240, 240, 240)); // Gray out time column
    }

    // Clear previous data in day columns (1 to 5)
    for (int row = 0; row < 6; row++) {
        for (int col = 1; col <= 5; col++) {
            ui->tableSemester->setItem(row, col, new QTableWidgetItem(""));
        }
    }

    // Populate data from the 'timetable' container
    bool foundData = false;
    for (const auto& entry : timetable) {
        if (entry.semester == semester) {
            int rowIndex = timeSlots.indexOf(entry.timeSlot);
            int colIndex = days.indexOf(entry.day) + 1; // +1 because col 0 is Time

            if (rowIndex >= 0 && colIndex > 0) {
                foundData = true;
                QString cellText = QString("%1\n%2\n%3\nRoom: %4")
                                       .arg(entry.courseCode)
                                       .arg(entry.type)
                                       .arg(entry.professorName)
                                       .arg(entry.room);

                QTableWidgetItem* item = new QTableWidgetItem(cellText);
                item->setTextAlignment(Qt::AlignCenter);

                // Color coding as per report v5.0
                if (entry.type == "Lab") {
                    item->setBackground(QColor(255, 240, 200)); // Light Orange
                } else {
                    item->setBackground(QColor(200, 240, 255)); // Light Blue
                }

                ui->tableSemester->setItem(rowIndex, colIndex, item);
            }
        }
    }

    // UI Adjustments
    ui->tableSemester->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (int i = 0; i < 6; i++) {
        ui->tableSemester->setRowHeight(i, 90);
    }

    if (!foundData) {
        qDebug() << "No data found for Semester" << semester;
    }
}
void MainWindow::displayProfessorWorkload()
{
    QMap<QString, int> counts;
    ui->tableProfessorWorkload->setRowCount(professors.size());
    for (int i = 0; i < professors.size(); i++) {
        int c = 0;
        QSet<QString> unique;
        for (const auto& e : timetable) if (e.professorId == professors[i].id) unique.insert(e.courseCode);
        ui->tableProfessorWorkload->setItem(i, 0, new QTableWidgetItem(professors[i].name));
        ui->tableProfessorWorkload->setItem(i, 1, new QTableWidgetItem(QString::number(unique.size())));
        ui->tableProfessorWorkload->setItem(i, 2, new QTableWidgetItem(professors[i].status));
    }
}

void MainWindow::on_btnGetStarted_clicked() { ui->stackedPages->setCurrentWidget(ui->pageLoadData); }
void MainWindow::on_btnLoadCourses_clicked() { loadCourses(QFileDialog::getOpenFileName(this, "Courses CSV", "", "CSV (*.csv)")); }
void MainWindow::on_btnLoadProfessors_clicked() { loadProfessors(QFileDialog::getOpenFileName(this, "Professors CSV", "", "CSV (*.csv)")); }
void MainWindow::on_btnGoGenerate_clicked() { ui->stackedPages->setCurrentWidget(ui->pageGenerate); }
// ============= UPDATED NAVIGATION & DISPLAY LOGIC =============

void MainWindow::on_btnGenerateTimetable_clicked()
{
    if (courses.isEmpty() || professors.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please load data first!");
        return;
    }

    // Step 1: Clear old data and reset counters
    timetable.clear();
    resetRoomCounters();

    // Step 2: Generate for all 8 semesters
    for (int sem = 1; sem <= 8; sem++) {
        generateSemesterTimetable(sem);
    }

    // Step 3: Update the summary text immediately
    ui->txtGenerationSummary->setText(QString("Generation Complete!\nTotal Sessions: %1\nClick 'View Results' to see the grid.")
                                          .arg(timetable.size()));

    QMessageBox::information(this, "Success", "Timetables generated successfully for all semesters.");
}

void MainWindow::on_btnGoResults_clicked()
{
    if (timetable.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please generate timetables first!");
        return;
    }

    // Switch to the results page
    ui->stackedPages->setCurrentWidget(ui->pageViewResults);

    // Reset to first semester view and FORCE the grid to refresh
    currentSemesterView = 1;
    ui->comboSemester->setCurrentIndex(0); // This triggers on_comboSemester_currentIndexChanged

    displaySemesterTimetableGrid(currentSemesterView);
    displayProfessorWorkload();
}

void MainWindow::on_btnNextSemester_clicked() { if (currentSemesterView < 8) displaySemesterTimetableGrid(++currentSemesterView); }
void MainWindow::on_btnPrevSemester_clicked() { if (currentSemesterView > 1) displaySemesterTimetableGrid(--currentSemesterView); }
void MainWindow::on_comboSemester_currentIndexChanged(int index) { if (index >= 0) displaySemesterTimetableGrid(index + 1); }
void MainWindow::on_btnBackHomeFromResults_clicked() { ui->stackedPages->setCurrentWidget(ui->pageHome); }
void MainWindow::on_btnBackHome_clicked() { ui->stackedPages->setCurrentWidget(ui->pageHome); }
void MainWindow::on_btnBackHomeFromLoad_clicked() { ui->stackedPages->setCurrentWidget(ui->pageHome); }
void MainWindow::on_comboProfessor_currentIndexChanged(int index) { Q_UNUSED(index); }
