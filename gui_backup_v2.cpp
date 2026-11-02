
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QDate>
#include <QStringList>
#include <QVector>
#include <algorithm>

struct Subject {
    QString name;
    QStringList topics;
    QDate examDate;
    QString difficulty;
};

struct StudySession {
    QDate date;
    QString subject;
    QString topic;
    int hour;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QVector<Subject> subjects;
    QVector<StudySession> schedule;

    QWidget window;
    window.setWindowTitle("Smart Study Planner");
    window.resize(1000, 750);

    window.setStyleSheet(
        "QWidget { background: #f4f7fb; color: #1e293b; font-size: 14px; }"
        "QLineEdit, QComboBox, QDateEdit, QSpinBox { background: white; padding: 8px; border: 1px solid #cbd5e1; border-radius: 6px; }"
        "QPushButton { background: #2563eb; color: white; padding: 10px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background: #1d4ed8; }"
        "QListWidget, QTextEdit { background: white; border: 1px solid #cbd5e1; border-radius: 8px; padding: 8px; }"
    );

    auto *mainLayout = new QVBoxLayout(&window);

    auto *title = new QLabel("SMART STUDY PLANNER");
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: #2563eb; padding: 10px;");
    mainLayout->addWidget(title);

    auto *subtitle = new QLabel("Plan your studies. Manage your time. Achieve your goals.");
    mainLayout->addWidget(subtitle);

    auto *formBox = new QGroupBox("Add Subject and Syllabus");
    auto *form = new QFormLayout(formBox);

    auto *subjectInput = new QLineEdit;
    subjectInput->setPlaceholderText("Example: Data Structures");

    auto *topicsInput = new QLineEdit;
    topicsInput->setPlaceholderText("Example: Arrays, Linked List, Trees");

    auto *examInput = new QDateEdit(QDate::currentDate().addDays(14));
    examInput->setCalendarPopup(true);
    examInput->setDisplayFormat("dd-MM-yyyy");

    auto *difficultyInput = new QComboBox;
    difficultyInput->addItems({"Easy", "Medium", "Hard"});

    auto *hoursInput = new QSpinBox;
    hoursInput->setRange(1, 12);
    hoursInput->setValue(4);
    hoursInput->setSuffix(" hours per day");

    form->addRow("Subject Name:", subjectInput);
    form->addRow("Syllabus Topics (comma separated):", topicsInput);
    form->addRow("Exam Date:", examInput);
    form->addRow("Difficulty:", difficultyInput);
    form->addRow("Daily Study Time:", hoursInput);

    mainLayout->addWidget(formBox);

    auto *addButton = new QPushButton("Add Subject");
    mainLayout->addWidget(addButton);

    auto *subjectHeading = new QLabel("My Subjects");
    subjectHeading->setStyleSheet("font-size: 18px; font-weight: bold;");
    mainLayout->addWidget(subjectHeading);

    auto *subjectList = new QListWidget;
    subjectList->setMaximumHeight(150);
    mainLayout->addWidget(subjectList);

    auto *generateButton = new QPushButton("Generate My Study Plan");
    generateButton->setStyleSheet("background: #16a34a; font-weight: bold; padding: 14px;");
    mainLayout->addWidget(generateButton);

    auto *planHeading = new QLabel("My Weekly Study Timetable");
    planHeading->setStyleSheet("font-size: 19px; font-weight: bold;");
    mainLayout->addWidget(planHeading);

    auto *planList = new QListWidget;
    mainLayout->addWidget(planList);

    auto *status = new QLabel("Add your subjects to get started!");
    mainLayout->addWidget(status);

    QObject::connect(addButton, &QPushButton::clicked, [&]() {
        QString name = subjectInput->text().trimmed();
        QStringList topics = topicsInput->text().split(',', Qt::SkipEmptyParts);

        for (QString &topic : topics) {
            topic = topic.trimmed();
        }

        topics.removeAll("");

        if (name.isEmpty() || topics.isEmpty()) {
            QMessageBox::warning(&window, "Missing Information",
                                 "Enter a subject name and at least one topic.");
            return;
        }

        if (examInput->date() < QDate::currentDate()) {
            QMessageBox::warning(&window, "Invalid Exam Date",
                                 "Please select today or a future exam date.");
            return;
        }

        for (const Subject &s : subjects) {
            if (s.name.compare(name, Qt::CaseInsensitive) == 0) {
                QMessageBox::warning(&window, "Duplicate Subject",
                                     "This subject has already been added.");
                return;
            }
        }

        Subject subject;
        subject.name = name;
        subject.topics = topics;
        subject.examDate = examInput->date();
        subject.difficulty = difficultyInput->currentText();

        subjects.append(subject);

        subjectList->addItem(
            name + " | " + QString::number(topics.size()) +
            " topics | Exam: " + examInput->date().toString("dd-MM-yyyy") +
            " | " + subject.difficulty
        );

        subjectInput->clear();
        topicsInput->clear();

        schedule.clear();
        planList->clear();

        status->setText("Subject added successfully!");
    });

    QObject::connect(generateButton, &QPushButton::clicked, [&]() {
        if (subjects.isEmpty()) {
            QMessageBox::information(&window, "No Subjects",
                                     "Please add subjects before generating a plan.");
            return;
        }

        QVector<Subject> orderedSubjects = subjects;

        std::stable_sort(orderedSubjects.begin(), orderedSubjects.end(),
            [](const Subject &a, const Subject &b) {
                if (a.examDate != b.examDate) {
                    return a.examDate < b.examDate;
                }

                auto rank = [](const QString &d) {
                    if (d == "Hard") return 3;
                    if (d == "Medium") return 2;
                    return 1;
                };

                return rank(a.difficulty) > rank(b.difficulty);
            }
        );

        struct TopicTask {
            QString subject;
            QString topic;
        };

        QVector<TopicTask> pendingTopics;

        for (const Subject &s : orderedSubjects) {
            for (const QString &topic : s.topics) {
                pendingTopics.append({s.name, topic});
            }
        }

        schedule.clear();
        planList->clear();

        int dailyHours = hoursInput->value();
        int topicIndex = 0;

        for (int day = 0; day < 7; day++) {
            QDate studyDate = QDate::currentDate().addDays(day);

            planList->addItem(
                "========== " +
                studyDate.toString("dddd, dd MMMM yyyy") +
                " =========="
            );

            for (int hour = 0; hour < dailyHours; hour++) {
                StudySession session;
                session.date = studyDate;
                session.hour = hour + 1;

                if (topicIndex < pendingTopics.size()) {
                    session.subject = pendingTopics[topicIndex].subject;
                    session.topic = pendingTopics[topicIndex].topic;
                    topicIndex++;
                } else {
                    session.subject = "Revision";
                    session.topic = "Revise previously studied topics";
                }

                schedule.append(session);

                planList->addItem(
                    QString("  %1:00 - %2:00 | %3 | %4")
                    .arg(9 + hour, 2, 10, QChar('0'))
                    .arg(10 + hour, 2, 10, QChar('0'))
                    .arg(session.subject)
                    .arg(session.topic)
                );
            }

            planList->addItem("");
        }

        status->setText(
            QString("Weekly timetable generated! %1 study sessions planned.")
            .arg(schedule.size())
        );
    });

    window.show();

    return app.exec();
}
