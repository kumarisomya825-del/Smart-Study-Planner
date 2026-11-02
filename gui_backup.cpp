
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QDate>
#include <QVector>
#include <algorithm>

struct Task {
    QString name;
    QString priority;
    QDate deadline;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QVector<Task> tasks;

    QWidget window;
    window.setWindowTitle("Smart Student Task Planner");
    window.resize(760, 600);
    window.setStyleSheet(
        "QWidget { background: #f4f7fb; color: #1e293b; font-size: 14px; }"
        "QLineEdit, QComboBox, QDateEdit { background: white; padding: 8px; border: 1px solid #cbd5e1; border-radius: 6px; }"
        "QPushButton { background: #2563eb; color: white; padding: 9px; border: none; border-radius: 6px; }"
        "QPushButton:hover { background: #1d4ed8; }"
        "QListWidget { background: white; border: 1px solid #cbd5e1; border-radius: 8px; padding: 6px; }"
    );

    auto *layout = new QVBoxLayout(&window);

    auto *heading = new QLabel("Smart Student Task Planner");
    heading->setStyleSheet("font-size: 25px; font-weight: bold; color: #1d4ed8; padding: 10px;");
    layout->addWidget(heading);

    auto *subtitle = new QLabel("Organize your study tasks in one place");
    layout->addWidget(subtitle);

    auto *formBox = new QGroupBox("Add a New Task");
    auto *form = new QFormLayout(formBox);

    auto *taskInput = new QLineEdit;
    taskInput->setPlaceholderText("e.g. Complete DSA Assignment");

    auto *priorityInput = new QComboBox;
    priorityInput->addItems({"High", "Medium", "Low"});

    auto *deadlineInput = new QDateEdit(QDate::currentDate());
    deadlineInput->setCalendarPopup(true);
    deadlineInput->setDisplayFormat("dd-MM-yyyy");

    form->addRow("Task Name:", taskInput);
    form->addRow("Priority:", priorityInput);
    form->addRow("Deadline:", deadlineInput);
    layout->addWidget(formBox);

    auto *addButton = new QPushButton("Add Task");
    auto *searchInput = new QLineEdit;
    searchInput->setPlaceholderText("Search task by name...");
    auto *searchButton = new QPushButton("Search");
    auto *showButton = new QPushButton("Show All");
    auto *sortButton = new QPushButton("Sort by Priority");
    auto *deleteButton = new QPushButton("Delete Selected");

    auto *buttonRow = new QHBoxLayout;
    buttonRow->addWidget(addButton);
    buttonRow->addWidget(searchInput);
    buttonRow->addWidget(searchButton);
    layout->addLayout(buttonRow);

    auto *actionRow = new QHBoxLayout;
    actionRow->addWidget(showButton);
    actionRow->addWidget(sortButton);
    actionRow->addWidget(deleteButton);
    layout->addLayout(actionRow);

    auto *taskList = new QListWidget;
    layout->addWidget(taskList);

    auto *status = new QLabel("Ready — add your first task!");
    layout->addWidget(status);

    auto refreshList = [&, taskList, status]() {
        taskList->clear();
        for (int i = 0; i < tasks.size(); ++i) {
            const Task &t = tasks[i];
            taskList->addItem(QString("%1. [%2] %3 — Due: %4")
                .arg(i + 1)
                .arg(t.priority)
                .arg(t.name)
                .arg(t.deadline.toString("dd-MM-yyyy")));
        }
        status->setText(QString("Total tasks: %1").arg(tasks.size()));
    };

    QObject::connect(addButton, &QPushButton::clicked, [&]() {
        QString name = taskInput->text().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(&window, "Missing Task", "Please enter a task name.");
            return;
        }

        tasks.append({name, priorityInput->currentText(), deadlineInput->date()});
        taskInput->clear();
        refreshList();
        status->setText("Task added successfully!");
    });

    QObject::connect(showButton, &QPushButton::clicked, [&]() {
        refreshList();
    });

    QObject::connect(searchButton, &QPushButton::clicked, [&]() {
        QString query = searchInput->text().trimmed();
        taskList->clear();
        for (int i = 0; i < tasks.size(); ++i) {
            if (tasks[i].name.contains(query, Qt::CaseInsensitive)) {
                taskList->addItem(QString("%1. [%2] %3 — Due: %4")
                    .arg(i + 1)
                    .arg(tasks[i].priority)
                    .arg(tasks[i].name)
                    .arg(tasks[i].deadline.toString("dd-MM-yyyy")));
            }
        }
        status->setText(QString("Search results: %1").arg(taskList->count()));
    });

    QObject::connect(sortButton, &QPushButton::clicked, [&]() {
        auto rank = [](const QString &p) {
            if (p == "High") return 0;
            if (p == "Medium") return 1;
            return 2;
        };
        std::stable_sort(tasks.begin(), tasks.end(), [&](const Task &a, const Task &b) {
            return rank(a.priority) < rank(b.priority);
        });
        refreshList();
        status->setText("Tasks sorted: High to Low priority.");
    });

    QObject::connect(deleteButton, &QPushButton::clicked, [&]() {
        int row = taskList->currentRow();
        if (row < 0) {
            QMessageBox::information(&window, "Select Task", "Please select a task from the list first.");
            return;
        }

        QString selectedText = taskList->currentItem()->text();
        int dot = selectedText.indexOf('.');
        bool ok = false;
        int taskNumber = selectedText.left(dot).toInt(&ok);

        if (ok && taskNumber >= 1 && taskNumber <= tasks.size()) {
            tasks.removeAt(taskNumber - 1);
            refreshList();
            status->setText("Task deleted successfully!");
        }
    });

    window.show();
    return app.exec();
}
