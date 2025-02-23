#include <pqxx/pqxx>  // PostgreSQL C++ Library
#include <iostream>
#include <cstdlib>  // For getenv()
#include <string>

using namespace std;
using namespace pqxx;

void addExpense(connection &C) {
    double amount;
    string category, description;

    cout << "Enter amount: ";
    cin >> amount;
    cin.ignore(); // To avoid issues with getline

    cout << "Enter category: ";
    getline(cin, category);

    cout << "Enter description: ";
    getline(cin, description);

    try {
        work W(C);
        string query = "INSERT INTO expenses (amount, category, description) VALUES (" +
                       to_string(amount) + ", " + W.quote(category) + ", " + W.quote(description) + ");";
        W.exec(query);
        W.commit();
        cout << "Expense added successfully!\n";
    } catch (const exception &e) {
        cerr << "Error adding expense: " << e.what() << endl;
    }
}

void listExpenses(connection &C) {
    try {
        nontransaction N(C);
        result R = N.exec("SELECT * FROM expenses;");
        
        cout << "ID\tAmount\tCategory\tDescription\n";
        cout << "------------------------------------------\n";
        for (const auto &row : R) {
            cout << row[0].as<int>() << "\t"
                 << row[1].as<double>() << "\t"
                 << row[2].c_str() << "\t"
                 << row[3].c_str() << "\n";
        }
    } catch (const exception &e) {
        cerr << "Error listing expenses: " << e.what() << endl;
    }
}

void deleteExpense(connection &C) {
    int id;
    cout << "Enter expense ID to delete: ";
    cin >> id;

    try {
        work W(C);
        string query = "DELETE FROM expenses WHERE id = " + to_string(id) + ";";
        W.exec(query);
        W.commit();
        cout << "Expense deleted successfully!\n";
    } catch (const exception &e) {
        cerr << "Error deleting expense: " << e.what() << endl;
    }
}

void calculateTotalSpending(connection &C) {
    try {
        nontransaction N(C);
        result R = N.exec("SELECT SUM(amount) FROM expenses;");

        if (!R.empty() && !R[0][0].is_null()) {
            cout << "Total spending: " << R[0][0].as<double>() << " kzt" << endl;
        } else {
            cout << "Total spending: $0.00\n";
        }
    } catch (const exception &e) {
        cerr << "Error calculating total spending: " << e.what() << endl;
    }
}

int main() {
    const char* db_password = getenv("DB_PASSWORD"); // Get password from environment variable

    if (!db_password) {
        cerr << "Error: Database password not set! Please set DB_PASSWORD environment variable.\n";
        return 1;
    }

    try {
        connection C("dbname=expense_tracker user=postgres password=" + string(db_password) + " hostaddr=127.0.0.1 port=5432");

        if (C.is_open()) {
            cout << "Connected to database successfully!\n";
        } else {
            cerr << "Failed to connect to database.\n";
            return 1;
        }

        int choice;
        while (true) {
            cout << "\nExpense Tracker\n";
            cout << "1. Add Expense\n";
            cout << "2. List Expenses\n";
            cout << "3. Delete Expense\n";
            cout << "4. Calculate Total Spending\n";
            cout << "5. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    addExpense(C);
                    break;
                case 2:
                    listExpenses(C);
                    break;
                case 3:
                    deleteExpense(C);
                    break;
                case 4:
                    calculateTotalSpending(C);
                    break;
                case 5:
                    cout << "Exiting...\n";
                    return 0;
                default:
                    cout << "Invalid choice. Try again.\n";
            }
        }
    } catch (const exception &e) {
        cerr << "Database connection failed: " << e.what() << endl;
        return 1;
    }

    return 0;
}
