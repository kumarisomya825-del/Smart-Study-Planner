
#include <QApplication>
#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDate>
#include <QVector>
#include <QStringList>
#include <QSettings>
#include <QSet>
#include <QProgressBar>
#include <QTabWidget>
#include <QSignalBlocker>
#include <QColor>
#include <QGroupBox>
#include <QScrollArea>
#include <QMap>
#include <QTimer>

struct Subject {
    QString name;
    QStringList topics;
    int difficulty = 0;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    QSettings settings("Somya", "SmartStudyPlanner");

    QWidget window;
    window.setWindowTitle("Smart Study Planner");
    window.resize(1250, 850);

    window.setStyleSheet(R"(
        QWidget {
            background-color: #F4F6FC;
            color: #26334D;
            font-family: "Segoe UI";
            font-size: 14px;
        }

        QLabel {
            background: transparent;
        }

        QLineEdit, QComboBox, QDateEdit {
            background: white;
            border: 1px solid #DCE3F0;
            border-radius: 8px;
            padding: 10px;
            min-height: 22px;
            color: #26334D;
        }

        QLineEdit:focus, QComboBox:focus, QDateEdit:focus {
            border: 2px solid #687BEA;
        }

        QPushButton {
            background-color: #596FE8;
            color: white;
            border: none;
            border-radius: 9px;
            padding: 12px;
            font-weight: bold;
        }

        QPushButton:hover {
            background-color: #4057D2;
        }

        QListWidget {
            background-color: white;
            border: 1px solid #E0E6F0;
            border-radius: 10px;
            padding: 8px;
            outline: none;
        }

        QListWidget::item {
            padding: 11px;
            margin: 3px;
            border-radius: 6px;
        }

        QListWidget::item:selected {
            background-color: #E6EBFF;
            color: #4057D2;
        }

        QTabWidget::pane {
            border: none;
            background: #F4F6FC;
        }

        QTabBar::tab {
            background: #E8ECF7;
            color: #46536D;
            padding: 12px 22px;
            margin-right: 5px;
            border-radius: 7px;
        }

        QTabBar::tab:selected {
            background: #596FE8;
            color: white;
        }

        QProgressBar {
            background-color: #E4E9F5;
            border: none;
            border-radius: 8px;
            height: 20px;
            text-align: center;
            color: #26334D;
        }

        QProgressBar::chunk {
            background-color: #596FE8;
            border-radius: 8px;
        }

        QScrollBar:vertical {
            background: #F4F6FC;
            width: 10px;
        }

        QScrollBar::handle:vertical {
            background: #B8C7E8;
            border-radius: 5px;
            min-height: 25px;
        }
    )");

    QVector<Subject> subjects;
    QSet<QString> completedSessions;

    QStringList savedCompleted =
        settings.value("completedSessions").toStringList();

    for (const QString &key : savedCompleted) {
        completedSessions.insert(key);
    }

    // LOAD SAVED SUBJECTS

    int savedCount = settings.beginReadArray("subjects");

    for (int i = 0; i < savedCount; i++) {
        settings.setArrayIndex(i);

        Subject subject;
        subject.name = settings.value("name").toString();
        subject.topics = settings.value("topics").toStringList();
        subject.difficulty = settings.value("difficulty").toInt();

        subjects.append(subject);
    }

    settings.endArray();

    // MAIN WINDOW LAYOUT

    QWidget *central = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // SIDEBAR

    QFrame *sidebar = new QFrame();
    sidebar->setFixedWidth(225);

    sidebar->setStyleSheet(
        "QFrame { background-color: #202C50; }"
        "QLabel { color: white; }"
        "QPushButton {"
        "background-color: transparent;"
        "color: #DCE4FF;"
        "text-align: left;"
        "padding: 15px;"
        "border-radius: 8px;"
        "font-size: 14px;"
        "}"
        "QPushButton:hover { background-color: #354577; }"
    );

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(15, 25, 15, 20);
    sidebarLayout->setSpacing(12);

    QLabel *logo = new QLabel("STUDY PLANNER");
    logo->setStyleSheet(
        "font-size: 20px;"
        "font-weight: bold;"
        "padding: 10px 3px 25px 3px;"
    );

    QPushButton *dashboardButton =
        new QPushButton("  Dashboard");

    QPushButton *subjectsButton =
        new QPushButton("  My Subjects");

    QPushButton *timetableButton =
        new QPushButton("  My Timetable");

    QLabel *sidebarFooter =
        new QLabel("Plan Smart.\nStudy Better.");

    sidebarFooter->setStyleSheet(
        "color: #AAB9E5;"
        "padding: 15px;"
    );

    sidebarLayout->addWidget(logo);
    sidebarLayout->addWidget(dashboardButton);
    sidebarLayout->addWidget(subjectsButton);
    sidebarLayout->addWidget(timetableButton);
    sidebarLayout->addStretch();
    sidebarLayout->addWidget(sidebarFooter);

    // CONTENT AREA

    QWidget *content = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(content);

    contentLayout->setContentsMargins(25, 22, 25, 22);
    contentLayout->setSpacing(15);

    QLabel *welcome = new QLabel("Welcome to Your Study Space!");

    welcome->setStyleSheet(
        "font-size: 27px;"
        "font-weight: bold;"
        "color: #4058D6;"
    );

    QLabel *subtitle =
        new QLabel("Organize your learning and track your daily progress.");

    subtitle->setStyleSheet("color: #78859E;");

    QTabWidget *tabs = new QTabWidget();

    // DASHBOARD PAGE

    QWidget *dashboardPage = new QWidget();
    QScrollArea *dashboardScroll = new QScrollArea();
    dashboardScroll->setWidgetResizable(true);
    dashboardScroll->setFrameShape(QFrame::NoFrame);
    QWidget *dashboardContent = new QWidget();
    QVBoxLayout *dashboardLayout =
        new QVBoxLayout(dashboardContent);
    QVBoxLayout *dashboardPageLayout =
        new QVBoxLayout(dashboardPage);
    dashboardPageLayout->setContentsMargins(0, 0, 0, 0);
    dashboardScroll->setWidget(dashboardContent);
    dashboardPageLayout->addWidget(dashboardScroll);

    QLabel *dashboardTitle =
        new QLabel("Your Dashboard");

    dashboardTitle->setStyleSheet(
        "font-size: 22px;"
        "font-weight: bold;"
        "color: #34446A;"
    );

    QHBoxLayout *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(12);

    QLabel *subjectCountLabel = new QLabel("0");
    QLabel *daysCountLabel = new QLabel("0");
    QLabel *completedCountLabel = new QLabel("0");
    QLabel *percentageLabel = new QLabel("0%");
    QLabel *pendingCountLabel = new QLabel("0");

    auto createCard = [](const QString &title,
                         const QString &color,
                         QLabel *valueLabel) -> QFrame * {

        QFrame *card = new QFrame();

        card->setStyleSheet(
            "QFrame {"
            "background-color: white;"
            "border: 1px solid #E5EAF4;"
            "border-radius: 13px;"
            "}"
        );

        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(16, 18, 16, 18);

        QLabel *titleLabel = new QLabel(title);

        titleLabel->setStyleSheet(
            "color: #78859E;"
            "font-size: 13px;"
            "border: none;"
        );

        valueLabel->setStyleSheet(
            QString(
                "font-size: 30px;"
                "font-weight: bold;"
                "color: %1;"
                "border: none;"
            ).arg(color)
        );

        cardLayout->addWidget(titleLabel);
        cardLayout->addWidget(valueLabel);

        return card;
    };

    cardsLayout->addWidget(
        createCard("Total Subjects", "#526BE2", subjectCountLabel)
    );

    cardsLayout->addWidget(
        createCard("Preparation Days", "#159B78", daysCountLabel)
    );

    cardsLayout->addWidget(
        createCard("Completed Sessions", "#D58A32", completedCountLabel)
    );

    cardsLayout->addWidget(
        createCard("Study Progress", "#8959CF", percentageLabel)
    );

    cardsLayout->addWidget(
        createCard("Pending Sessions", "#D45D6D", pendingCountLabel)
    );

    QLabel *progressTitle =
        new QLabel("Overall Study Progress");

    progressTitle->setStyleSheet(
        "font-size: 17px;"
        "font-weight: bold;"
        "color: #34446A;"
        "padding-top: 20px;"
    );

    QProgressBar *progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setFormat("%p% completed");

    QGroupBox *subjectAnalyticsBox =
        new QGroupBox("Subject-wise Progress");
    subjectAnalyticsBox->setStyleSheet(
        "QGroupBox { background: white; border: 1px solid #E5EAF4;"
        " border-radius: 10px; margin-top: 12px; padding: 12px;"
        " font-weight: bold; color: #34446A; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 5px; }"
    );
    QVBoxLayout *subjectAnalyticsLayout =
        new QVBoxLayout(subjectAnalyticsBox);

    QGroupBox *dailyAnalyticsBox =
        new QGroupBox("Daily Progress (Last 7 Days)");
    dailyAnalyticsBox->setStyleSheet(
        "QGroupBox { background: white; border: 1px solid #E5EAF4;"
        " border-radius: 10px; margin-top: 12px; padding: 12px;"
        " font-weight: bold; color: #34446A; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 5px; }"
    );
    QVBoxLayout *dailyAnalyticsLayout =
        new QVBoxLayout(dailyAnalyticsBox);

    QGroupBox *streakBox = new QGroupBox("Study Streak & Daily Goal");
    streakBox->setStyleSheet(
        "QGroupBox { background: white; border: 1px solid #E5EAF4;"
        " border-radius: 10px; margin-top: 12px; padding: 12px;"
        " font-weight: bold; color: #34446A; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 5px; }"
    );
    QVBoxLayout *streakLayout = new QVBoxLayout(streakBox);
    QLabel *streakCountLabel = new QLabel("Current streak: 0 days");
    QLabel *completedDaysLabel = new QLabel("Total study days: 0");
    QLabel *dailyGoalLabel = new QLabel("Today's study time: 0 / 240 minutes");
    QProgressBar *dailyGoalBar = new QProgressBar();
    dailyGoalBar->setRange(0, 100);
    dailyGoalBar->setValue(0);
    dailyGoalBar->setFormat("%p% of daily 4-hour goal");
    QLabel *achievementLabel = new QLabel("Achievements: not unlocked yet");
    achievementLabel->setWordWrap(true);
    streakLayout->addWidget(streakCountLabel);
    streakLayout->addWidget(completedDaysLabel);
    streakLayout->addWidget(dailyGoalLabel);
    streakLayout->addWidget(dailyGoalBar);
    streakLayout->addWidget(achievementLabel);

    QLabel *motivation = new QLabel(
        "Every small step brings you closer to your goal!"
    );

    motivation->setStyleSheet(
        "background-color: white;"
        "border-radius: 12px;"
        "padding: 20px;"
        "color: #647493;"
        "font-size: 15px;"
    );

    dashboardLayout->setSpacing(18);
    dashboardLayout->addWidget(dashboardTitle);
    dashboardLayout->addLayout(cardsLayout);
    dashboardLayout->addWidget(progressTitle);
    dashboardLayout->addWidget(progressBar);
    dashboardLayout->addWidget(subjectAnalyticsBox);
    dashboardLayout->addWidget(dailyAnalyticsBox);
    dashboardLayout->addWidget(streakBox);
    dashboardLayout->addWidget(motivation);
    dashboardLayout->addStretch();

    // SUBJECTS PAGE

    QWidget *subjectsPage = new QWidget();
    QVBoxLayout *subjectsLayout =
        new QVBoxLayout(subjectsPage);

    QLabel *subjectTitle =
        new QLabel("Manage Your Subjects");

    subjectTitle->setStyleSheet(
        "font-size: 22px;"
        "font-weight: bold;"
        "color: #34446A;"
    );

    QLineEdit *subjectName = new QLineEdit();
    subjectName->setPlaceholderText("Enter subject name");

    QLineEdit *topicsInput = new QLineEdit();
    topicsInput->setPlaceholderText(
        "Enter topics separated by commas"
    );

    QComboBox *difficulty = new QComboBox();
    difficulty->addItems({"Easy", "Medium", "Hard"});

    QFormLayout *subjectForm = new QFormLayout();
    subjectForm->setSpacing(12);
    subjectForm->addRow("Subject Name:", subjectName);
    subjectForm->addRow("Topics:", topicsInput);
    subjectForm->addRow("Difficulty:", difficulty);

    QPushButton *addButton =
        new QPushButton("+ Add Subject");

    QListWidget *subjectsList = new QListWidget();

    for (const Subject &subject : subjects) {
        subjectsList->addItem(
            subject.name + " (" +
            QString::number(subject.topics.size()) + " topics)"
        );
    }

    subjectsLayout->addWidget(subjectTitle);
    subjectsLayout->addLayout(subjectForm);
    subjectsLayout->addWidget(addButton);
    subjectsLayout->addWidget(new QLabel("Your Added Subjects"));
    subjectsLayout->addWidget(subjectsList);

    // TIMETABLE PAGE

    QWidget *timetablePage = new QWidget();
    QVBoxLayout *timetableLayout =
        new QVBoxLayout(timetablePage);

    QLabel *timetableTitle =
        new QLabel("Your Daily Study Timetable");

    timetableTitle->setStyleSheet(
        "font-size: 22px;"
        "font-weight: bold;"
        "color: #34446A;"
    );

    QDateEdit *startDate = new QDateEdit(
        settings.value(
            "startDate",
            QDate::currentDate()
        ).toDate()
    );

    startDate->setCalendarPopup(true);
    startDate->setDisplayFormat("dd MMM yyyy");

    QDateEdit *endDate = new QDateEdit(
        settings.value(
            "endDate",
            QDate::currentDate().addDays(43)
        ).toDate()
    );

    endDate->setCalendarPopup(true);
    endDate->setDisplayFormat("dd MMM yyyy");

    QFormLayout *dateForm = new QFormLayout();
    dateForm->addRow("Start Date:", startDate);
    dateForm->addRow("End Date:", endDate);

    QPushButton *generateButton =
        new QPushButton("Generate My Daily Study Plan");

    QListWidget *scheduleList = new QListWidget();

    timetableLayout->addWidget(timetableTitle);
    timetableLayout->addLayout(dateForm);
    timetableLayout->addWidget(generateButton);
    timetableLayout->addWidget(scheduleList);

    // POMODORO TIMER PAGE

    QWidget *pomodoroPage = new QWidget();
    QVBoxLayout *pomodoroLayout = new QVBoxLayout(pomodoroPage);
    pomodoroLayout->setContentsMargins(30, 25, 30, 25);
    pomodoroLayout->setSpacing(18);

    QLabel *pomodoroTitle = new QLabel("Pomodoro Focus Timer");
    pomodoroTitle->setStyleSheet("font-size: 23px; font-weight: bold; color: #34446A;");

    QLabel *pomodoroSubtitle = new QLabel("Focus for 25 minutes, then take a 5-minute break.");
    pomodoroSubtitle->setStyleSheet("color: #78859E; font-size: 14px;");

    QLabel *timerModeLabel = new QLabel("FOCUS SESSION");
    timerModeLabel->setAlignment(Qt::AlignCenter);
    timerModeLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #596FE8;");

    QLabel *timerDisplay = new QLabel("25:00");
    timerDisplay->setAlignment(Qt::AlignCenter);
    timerDisplay->setStyleSheet("font-size: 64px; font-weight: bold; color: #26334D; background: white; border: 1px solid #E5EAF4; border-radius: 18px; padding: 25px;");

    QLabel *focusCountLabel = new QLabel("Completed focus sessions: 0");
    focusCountLabel->setAlignment(Qt::AlignCenter);
    focusCountLabel->setStyleSheet("font-size: 15px; color: #647493;");

    QPushButton *startPauseButton = new QPushButton("Start");
    QPushButton *resetTimerButton = new QPushButton("Reset");
    QPushButton *switchModeButton = new QPushButton("Switch to Break (5 min)");

    QHBoxLayout *timerButtonsLayout = new QHBoxLayout();
    timerButtonsLayout->addWidget(startPauseButton);
    timerButtonsLayout->addWidget(resetTimerButton);

    pomodoroLayout->addWidget(pomodoroTitle);
    pomodoroLayout->addWidget(pomodoroSubtitle);
    pomodoroLayout->addStretch();
    pomodoroLayout->addWidget(timerModeLabel);
    pomodoroLayout->addWidget(timerDisplay);
    pomodoroLayout->addWidget(focusCountLabel);
    pomodoroLayout->addLayout(timerButtonsLayout);
    pomodoroLayout->addWidget(switchModeButton);
    pomodoroLayout->addStretch();

    QTimer *pomodoroTimer = new QTimer(&window);
    pomodoroTimer->setInterval(1000);
    int remainingSeconds = 25 * 60;
    int focusSessionsCompleted = settings.value("pomodoro/focusSessions", 0).toInt();
    bool isBreakMode = false;
    bool timerRunning = false;

    auto updateTimerDisplay = [&]() {
        int minutes = remainingSeconds / 60;
        int seconds = remainingSeconds % 60;
        timerDisplay->setText(QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0')));
    };

    focusCountLabel->setText(QString("Completed focus sessions: %1").arg(focusSessionsCompleted));
    updateTimerDisplay();

    QObject::connect(pomodoroTimer, &QTimer::timeout, [&]() {
        if (remainingSeconds > 0) {
            remainingSeconds--;
            updateTimerDisplay();
        }

        if (remainingSeconds == 0) {
            pomodoroTimer->stop();
            timerRunning = false;
            startPauseButton->setText("Start");

            if (!isBreakMode) {
                focusSessionsCompleted++;
                settings.setValue("pomodoro/focusSessions", focusSessionsCompleted);
                settings.sync();
                focusCountLabel->setText(QString("Completed focus sessions: %1").arg(focusSessionsCompleted));
                QMessageBox::information(&window, "Focus completed", "Great work! Your 25-minute focus session is complete. Take a 5-minute break.");
                isBreakMode = true;
                timerModeLabel->setText("BREAK TIME");
                timerModeLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #159B78;");
                switchModeButton->setText("Switch to Focus (25 min)");
                remainingSeconds = 5 * 60;
            } else {
                QMessageBox::information(&window, "Break completed", "Break is over. Ready for another focus session?");
                isBreakMode = false;
                timerModeLabel->setText("FOCUS SESSION");
                timerModeLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #596FE8;");
                switchModeButton->setText("Switch to Break (5 min)");
                remainingSeconds = 25 * 60;
            }
            updateTimerDisplay();
        }
    });

    QObject::connect(startPauseButton, &QPushButton::clicked, [&]() {
        if (timerRunning) {
            pomodoroTimer->stop();
            timerRunning = false;
            startPauseButton->setText("Resume");
        } else {
            pomodoroTimer->start();
            timerRunning = true;
            startPauseButton->setText("Pause");
        }
    });

    QObject::connect(resetTimerButton, &QPushButton::clicked, [&]() {
        pomodoroTimer->stop();
        timerRunning = false;
        startPauseButton->setText("Start");
        remainingSeconds = isBreakMode ? 5 * 60 : 25 * 60;
        updateTimerDisplay();
    });

    QObject::connect(switchModeButton, &QPushButton::clicked, [&]() {
        pomodoroTimer->stop();
        timerRunning = false;
        startPauseButton->setText("Start");
        isBreakMode = !isBreakMode;
        if (isBreakMode) {
            remainingSeconds = 5 * 60;
            timerModeLabel->setText("BREAK TIME");
            timerModeLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #159B78;");
            switchModeButton->setText("Switch to Focus (25 min)");
        } else {
            remainingSeconds = 25 * 60;
            timerModeLabel->setText("FOCUS SESSION");
            timerModeLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #596FE8;");
            switchModeButton->setText("Switch to Break (5 min)");
        }
        updateTimerDisplay();
    });

    // ADD TABS

    tabs->addTab(dashboardPage, "Dashboard");
    tabs->addTab(subjectsPage, "My Subjects");
    tabs->addTab(timetablePage, "My Timetable");
    tabs->addTab(pomodoroPage, "Pomodoro Timer");

    contentLayout->addWidget(welcome);
    contentLayout->addWidget(subtitle);
    contentLayout->addWidget(tabs);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(content, 1);

    QVBoxLayout *windowLayout = new QVBoxLayout(&window);
windowLayout->setContentsMargins(0, 0, 0, 0);
windowLayout->setSpacing(0);
windowLayout->addWidget(central);

    // SAVE SUBJECTS

    auto saveSubjects = [&]() {

        settings.beginWriteArray("subjects");

        for (int i = 0; i < subjects.size(); i++) {
            settings.setArrayIndex(i);
            settings.setValue("name", subjects[i].name);
            settings.setValue("topics", subjects[i].topics);
            settings.setValue("difficulty", subjects[i].difficulty);
        }

        settings.endArray();
        settings.sync();
    };

    // UPDATE DASHBOARD

    auto clearAnalyticsLayout = [](QVBoxLayout *layout) {
        while (QLayoutItem *item = layout->takeAt(0)) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
    };

    auto updateDashboard = [&]() {

        int total = 0;
        int completed = 0;
        QMap<QString, QPair<int, int>> dailyStats;

        QVector<int> subjectTotal(subjects.size(), 0);
        QVector<int> subjectCompleted(subjects.size(), 0);

        for (int i = 0; i < scheduleList->count(); i++) {
            QListWidgetItem *item = scheduleList->item(i);
            QString key = item->data(Qt::UserRole).toString();

            if (key.isEmpty()) {
                continue;
            }

            total++;
            bool isDone = (item->checkState() == Qt::Checked);
            if (isDone) {
                completed++;
            }

            QString dateKey = key.left(10);
            dailyStats[dateKey].first++;
            if (isDone) {
                dailyStats[dateKey].second++;
            }

            QString itemText = item->text();
            for (int subjectIndex = 0;
                 subjectIndex < subjects.size();
                 subjectIndex++) {
                if (itemText.contains(subjects[subjectIndex].name + " - ")) {
                    subjectTotal[subjectIndex]++;
                    if (isDone) {
                        subjectCompleted[subjectIndex]++;
                    }
                    break;
                }
            }
        }

        int percentage = total > 0 ? (completed * 100 / total) : 0;
        int pending = total - completed;

        subjectCountLabel->setText(QString::number(subjects.size()));

        int days = startDate->date().daysTo(endDate->date()) + 1;
        if (days < 0) days = 0;
        daysCountLabel->setText(QString::number(days));
        completedCountLabel->setText(QString::number(completed));
        percentageLabel->setText(QString::number(percentage) + "%");
        pendingCountLabel->setText(QString::number(pending));
        progressBar->setValue(percentage);

        // STUDY STREAK AND DAILY GOAL
        QSet<QString> studyDays;
        int todayMinutes = 0;
        QDate today = QDate::currentDate();

        for (const QString &sessionKey : completedSessions) {
            if (sessionKey.size() < 10) {
                continue;
            }

            QString datePart = sessionKey.left(10);
            QDate sessionDate = QDate::fromString(datePart, "yyyy-MM-dd");
            if (!sessionDate.isValid()) {
                continue;
            }

            studyDays.insert(datePart);

            if (sessionDate == today) {
                if (sessionKey.contains("_leetcode")) {
                    todayMinutes += 30;
                } else if (sessionKey.contains("_slot_")) {
                    bool ok = false;
                    int slotNumber = sessionKey.section("_slot_", 1, 1).toInt(&ok);
                    if (ok) {
                        const int slotMinutes[] = {45, 40, 50, 50, 50, 50};
                        if (slotNumber >= 0 && slotNumber < 6) {
                            todayMinutes += slotMinutes[slotNumber];
                        }
                    }
                }
            }
        }

        int currentStreak = 0;
        QDate streakDate = today;
        if (!studyDays.contains(today.toString("yyyy-MM-dd"))) {
            streakDate = today.addDays(-1);
        }
        while (studyDays.contains(streakDate.toString("yyyy-MM-dd"))) {
            currentStreak++;
            streakDate = streakDate.addDays(-1);
        }

        streakCountLabel->setText(
            QString("🔥 Current study streak: %1 day(s)").arg(currentStreak));
        completedDaysLabel->setText(
            QString("📅 Total study days completed: %1").arg(studyDays.size()));
        dailyGoalLabel->setText(
            QString("Today's study time: %1 / 240 minutes")
                .arg(todayMinutes));
        dailyGoalBar->setValue(qMin(100, todayMinutes * 100 / 240));

        QStringList achievements;
        if (currentStreak >= 3) achievements << "🥉 3-Day Streak";
        if (currentStreak >= 7) achievements << "🥈 7-Day Streak";
        if (currentStreak >= 30) achievements << "🥇 30-Day Streak";
        achievementLabel->setText(
            achievements.isEmpty()
                ? "🏆 Achievements: Complete study sessions on 3 consecutive days to unlock your first badge."
                : "🏆 Unlocked: " + achievements.join("   |  "));

        clearAnalyticsLayout(subjectAnalyticsLayout);
        if (subjects.isEmpty()) {
            subjectAnalyticsLayout->addWidget(
                new QLabel("Add subjects to see subject-wise analytics.")
            );
        } else if (total == 0) {
            subjectAnalyticsLayout->addWidget(
                new QLabel("Generate your timetable to see subject progress.")
            );
        } else {
            for (int i = 0; i < subjects.size(); i++) {
                QWidget *row = new QWidget();
                QVBoxLayout *rowLayout = new QVBoxLayout(row);
                rowLayout->setContentsMargins(0, 5, 0, 5);
                QLabel *nameLabel = new QLabel(subjects[i].name);
                int subjectPercent = subjectTotal[i] > 0
                    ? subjectCompleted[i] * 100 / subjectTotal[i] : 0;
                QLabel *detailLabel = new QLabel(
                    QString("%1 of %2 sessions completed (%3%)")
                        .arg(subjectCompleted[i])
                        .arg(subjectTotal[i])
                        .arg(subjectPercent)
                );
                detailLabel->setStyleSheet("color: #78859E; font-size: 12px;");
                QProgressBar *bar = new QProgressBar();
                bar->setRange(0, 100);
                bar->setValue(subjectPercent);
                bar->setFormat("%p%");
                rowLayout->addWidget(nameLabel);
                rowLayout->addWidget(bar);
                rowLayout->addWidget(detailLabel);
                subjectAnalyticsLayout->addWidget(row);
            }
        }

        clearAnalyticsLayout(dailyAnalyticsLayout);
        if (dailyStats.isEmpty()) {
            dailyAnalyticsLayout->addWidget(
                new QLabel("Generate your timetable to see the daily progress chart.")
            );
        } else {
            QStringList dates = dailyStats.keys();
            int first = qMax(0, dates.size() - 7);

            QWidget *chartWidget = new QWidget();
            QHBoxLayout *chartLayout = new QHBoxLayout(chartWidget);
            chartLayout->setContentsMargins(8, 12, 8, 8);
            chartLayout->setSpacing(18);
            chartLayout->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

            for (int i = first; i < dates.size(); i++) {
                QString date = dates[i];
                int dayTotal = dailyStats[date].first;
                int dayDone = dailyStats[date].second;
                int dayPercent = dayTotal > 0 ? dayDone * 100 / dayTotal : 0;

                QWidget *barColumn = new QWidget();
                QVBoxLayout *columnLayout = new QVBoxLayout(barColumn);
                columnLayout->setContentsMargins(2, 0, 2, 0);
                columnLayout->setSpacing(6);
                columnLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

                QLabel *percentLabel = new QLabel(QString("%1%").arg(dayPercent));
                percentLabel->setAlignment(Qt::AlignCenter);
                percentLabel->setStyleSheet("font-weight: bold; color: #4058D6;");

                QProgressBar *bar = new QProgressBar();
                bar->setOrientation(Qt::Vertical);
                bar->setRange(0, 100);
                bar->setValue(dayPercent);
                bar->setTextVisible(false);
                bar->setFixedSize(42, 145);
                bar->setStyleSheet(
                    "QProgressBar { background: #E4E9F5; border: none; border-radius: 7px; }"
                    "QProgressBar::chunk { background: #596FE8; border-radius: 7px; }"
                );

                QLabel *countLabel = new QLabel(
                    QString("%1/%2").arg(dayDone).arg(dayTotal)
                );
                countLabel->setAlignment(Qt::AlignCenter);
                countLabel->setStyleSheet("color: #78859E; font-size: 11px;");

                QLabel *dateLabel = new QLabel(
                    QDate::fromString(date, "yyyy-MM-dd").toString("dd MMM")
                );
                dateLabel->setAlignment(Qt::AlignCenter);
                dateLabel->setStyleSheet("color: #34446A; font-size: 11px;");

                columnLayout->addWidget(percentLabel);
                columnLayout->addWidget(bar, 0, Qt::AlignHCenter);
                columnLayout->addWidget(countLabel);
                columnLayout->addWidget(dateLabel);
                chartLayout->addWidget(barColumn);
            }

            dailyAnalyticsLayout->addWidget(chartWidget);
            QLabel *chartHint = new QLabel("Completed sessions / total sessions for each day");
            chartHint->setStyleSheet("color: #78859E; font-size: 11px;");
            dailyAnalyticsLayout->addWidget(chartHint);
        }
    };

    // SIDEBAR NAVIGATION

    QObject::connect(
        dashboardButton,
        &QPushButton::clicked,
        [&]() {
            tabs->setCurrentIndex(0);
        }
    );

    QObject::connect(
        subjectsButton,
        &QPushButton::clicked,
        [&]() {
            tabs->setCurrentIndex(1);
        }
    );

    QObject::connect(
        timetableButton,
        &QPushButton::clicked,
        [&]() {
            tabs->setCurrentIndex(2);
        }
    );

    // ADD SUBJECT

    QObject::connect(
        addButton,
        &QPushButton::clicked,
        [&]() {

        QString name = subjectName->text().trimmed();
        QString topicText = topicsInput->text().trimmed();

        if (name.isEmpty() || topicText.isEmpty()) {
            QMessageBox::warning(
                &window,
                "Missing Details",
                "Please enter subject name and topics."
            );
            return;
        }

        for (const Subject &subject : subjects) {

            if (subject.name.compare(
                    name, Qt::CaseInsensitive
                ) == 0) {

                QMessageBox::warning(
                    &window,
                    "Duplicate Subject",
                    "This subject is already added."
                );
                return;
            }
        }

        QStringList topics =
            topicText.split(",", Qt::SkipEmptyParts);

        for (QString &topic : topics) {
            topic = topic.trimmed();
        }

        topics.removeAll("");

        if (topics.isEmpty()) {
            QMessageBox::warning(
                &window,
                "Invalid Topics",
                "Please enter at least one topic."
            );
            return;
        }

        Subject subject;
        subject.name = name;
        subject.topics = topics;
        subject.difficulty = difficulty->currentIndex();

        subjects.append(subject);

        subjectsList->addItem(
            name + " (" +
            QString::number(topics.size()) + " topics)"
        );

        saveSubjects();
        updateDashboard();

        subjectName->clear();
        topicsInput->clear();
        difficulty->setCurrentIndex(0);
    });

    // SAVE DATES

    QObject::connect(
        startDate,
        &QDateEdit::dateChanged,
        [&](const QDate &date) {
            settings.setValue("startDate", date);
            settings.sync();
            updateDashboard();
        }
    );

    QObject::connect(
        endDate,
        &QDateEdit::dateChanged,
        [&](const QDate &date) {
            settings.setValue("endDate", date);
            settings.sync();
            updateDashboard();
        }
    );

    // SAVE COMPLETED CHECKBOXES

    QObject::connect(
        scheduleList,
        &QListWidget::itemChanged,
        [&](QListWidgetItem *item) {

        QString key = item->data(Qt::UserRole).toString();

        if (key.isEmpty()) {
            return;
        }

        if (item->checkState() == Qt::Checked) {
            completedSessions.insert(key);
        } else {
            completedSessions.remove(key);
        }

        QStringList savedKeys;

        for (const QString &savedKey : completedSessions) {
            savedKeys.append(savedKey);
        }

        settings.setValue("completedSessions", savedKeys);
        settings.sync();

        updateDashboard();
    });

    // GENERATE DAILY TIMETABLE

    QObject::connect(
        generateButton,
        &QPushButton::clicked,
        [&]() {

        if (subjects.isEmpty()) {
            QMessageBox::warning(
                &window,
                "No Subjects",
                "Please add your subjects first."
            );
            return;
        }

        QDate firstDate = startDate->date();
        QDate lastDate = endDate->date();

        if (lastDate < firstDate) {
            QMessageBox::warning(
                &window,
                "Invalid Dates",
                "End date cannot be before start date."
            );
            return;
        }

        QSignalBlocker blocker(scheduleList);

        scheduleList->clear();

        QStringList timeSlots = {
            "6:45 AM - 7:30 AM",
            "7:00 PM - 7:40 PM",
            "8:20 PM - 9:10 PM",
            "9:10 PM - 10:00 PM",
            "10:20 PM - 11:10 PM",
            "11:10 PM - 12:00 AM"
        };

        QVector<int> topicIndexes(subjects.size(), 0);
        int subjectCursor = 0;

        for (QDate date = firstDate;
             date <= lastDate;
             date = date.addDays(1)) {

            QListWidgetItem *dateItem =
                new QListWidgetItem(
                    date.toString("dddd, dd MMMM yyyy")
                );

            dateItem->setFlags(Qt::ItemIsEnabled);
            dateItem->setBackground(QColor("#DCE7FF"));
            dateItem->setForeground(QColor("#304FC0"));

            scheduleList->addItem(dateItem);

            for (int slot = 0;
                 slot < timeSlots.size();
                 slot++) {

                int selectedSubject = -1;

                for (int count = 0;
                     count < subjects.size();
                     count++) {

                    int currentSubject =
                        subjectCursor % subjects.size();

                    subjectCursor++;

                    if (topicIndexes[currentSubject] <
                        subjects[currentSubject].topics.size()) {

                        selectedSubject = currentSubject;
                        break;
                    }
                }

                QString sessionText = timeSlots[slot] + " | ";

                QString sessionKey =
                    date.toString("yyyy-MM-dd") +
                    "_slot_" + QString::number(slot);

                if (selectedSubject == -1) {

                    sessionText += "Revision / Practice";

                } else {

                    QString topic =
                        subjects[selectedSubject].topics[
                            topicIndexes[selectedSubject]
                        ];

                    sessionText +=
                        subjects[selectedSubject].name +
                        " - " + topic;

                    topicIndexes[selectedSubject]++;
                }

                QListWidgetItem *item =
                    new QListWidgetItem(sessionText);

                item->setData(Qt::UserRole, sessionKey);

                item->setFlags(
                    item->flags() | Qt::ItemIsUserCheckable
                );

                item->setCheckState(
                    completedSessions.contains(sessionKey)
                    ? Qt::Checked
                    : Qt::Unchecked
                );

                scheduleList->addItem(item);
            }

            QString leetcodeKey =
                date.toString("yyyy-MM-dd") + "_leetcode";

            QListWidgetItem *leetcodeItem =
                new QListWidgetItem(
                    "7:30 AM - 8:00 AM | LeetCode Practice"
                );

            leetcodeItem->setData(
                Qt::UserRole,
                leetcodeKey
            );

            leetcodeItem->setFlags(
                leetcodeItem->flags() | Qt::ItemIsUserCheckable
            );

            leetcodeItem->setCheckState(
                completedSessions.contains(leetcodeKey)
                ? Qt::Checked
                : Qt::Unchecked
            );

            scheduleList->addItem(leetcodeItem);
        }

        updateDashboard();

        QMessageBox::information(
            &window,
            "Success",
            "Your daily timetable has been generated!"
        );
    });

    updateDashboard();

    window.show();

    return app.exec();
}
