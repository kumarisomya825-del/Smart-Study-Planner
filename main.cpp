
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

struct Task {
    string name;
    string priority;
    string deadline;
};

int priorityValue(string priority) {
    if (priority == "High" || priority == "high") return 1;
    if (priority == "Medium" || priority == "medium") return 2;
    if (priority == "Low" || priority == "low") return 3;
    return 4;
}

int main() {
    vector<Task> tasks;
    int choice;
    string taskName, priority, deadline;

    do {
        cout << "\n===== SMART STUDENT TASK PLANNER =====" << endl;
        cout << "1. Add Task" << endl;
        cout << "2. Display Tasks" << endl;
        cout << "3. Delete Task" << endl;
        cout << "4. Search Task" << endl;
        cout << "5. Sort Tasks by Priority" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            cout << "Enter your task: ";
            getline(cin, taskName);

            cout << "Select Priority (High/Medium/Low): ";
            getline(cin, priority);
            while (priority != "High" && priority != "high" && 
       priority != "Medium" && priority != "medium" &&
       priority != "Low" && priority != "low") {
    cout << "Invalid priority! Enter High, Medium, or Low: ";
    getline(cin, priority);
}

            cout << "Enter Deadline (DD-MM-YYYY): ";
            getline(cin, deadline);

            tasks.push_back({taskName, priority, deadline});
            cout << "Task added successfully!" << endl; 
        }
        else if (choice == 2) {
            if (tasks.empty()) {
                cout << "No tasks available!" << endl;
            } else {
                cout << "\n===== YOUR TASK LIST =====" << endl;
                for (int i = 0; i < tasks.size(); i++) {
                    cout << i + 1 << ". " << tasks[i].name << endl;
                    cout << "   Priority: " << tasks[i].priority << endl;
                    cout << "   Deadline: " << tasks[i].deadline << endl;
                }
            }
        }
        else if (choice == 3) {
            if (tasks.empty()) {
                cout << "No tasks to delete!" << endl;
            } else {
                for (int i = 0; i < tasks.size(); i++) {
                    cout << i + 1 << ". " << tasks[i].name << endl;
                }

                int number;
                cout << "Enter task number to delete: ";
                cin >> number;

                if (number >= 1 && number <= static_cast<int>(tasks.size())) {
                    tasks.erase(tasks.begin() + number - 1);
                    cout << "Task deleted successfully!" << endl;
                } else {
                    cout << "Invalid task number!" << endl;
                }
            }
        }
        else if (choice == 4) {
            string searchName;
            bool found = false;

            cout << "Enter task name to search: ";
            getline(cin, searchName);

            for (const Task& task : tasks) {
                if (task.name == searchName) {
                    cout << "Task found!" << endl;
                    cout << "Priority: " << task.priority << endl;
                    cout << "Deadline: " << task.deadline << endl;
                    found = true;
                    break;
                }
            }

            if (!found) {
                cout << "Task not found!" << endl;
            }
        }
        else if (choice == 5) {
            sort(tasks.begin(), tasks.end(),
                [](const Task& a, const Task& b) {
                    return priorityValue(a.priority) < priorityValue(b.priority);
                });

            cout << "Tasks sorted by priority!" << endl;

            for (int i = 0; i < tasks.size(); i++) {
                cout << i + 1 << ". " << tasks[i].name
                     << " | Priority: " << tasks[i].priority
                     << " | Deadline: " << tasks[i].deadline << endl;
            }
        }
        else if (choice == 6) {
            cout << "Thank you for using Task Planner!" << endl;
        }
        else {
            cout << "Invalid choice! Try again." << endl;
        }

    } while (choice != 6);

    return 0;
}
