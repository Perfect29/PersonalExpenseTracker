#include "ExpenseManager.hpp"
#include <iostream>

using namespace std;
using namespace pqxx;

ExpenseManager::ExpenseManager(connection& c) : conn(c) {}

void ExpenseManager::prepareStatements() {
    conn.prepare("insert_expense", "INSERT INTO expenses (amount, category, description) VALUES ($1, $2, $3)");
    conn.prepare("delete_expense", "DELETE FROM expenses WHERE id = $1");
    conn.prepare("select_all", "SELECT * FROM expenses");
    conn.prepare("sum_expenses", "SELECT SUM(amount) FROM expenses");

    conn.prepare("select_by_date", "SELECT * FROM expenses WHERE date BETWEEN $1 AND $2");
    conn.prepare("sum_by_category", "SELECT category, SUM(amount) FROM expenses GROUP BY category ORDER BY SUM(amount) DESC");
    conn.prepare("search_description", "SELECT * FROM expenses WHERE description ILIKE $1");
    conn.prepare("most_expensive", "SELECT * FROM expenses ORDER BY amount DESC LIMIT 1");
}

void ExpenseManager::addExpense() {
    double amount;
    string category, description;

    cout << "Enter amount: ";
    while (!(cin >> amount) || amount < 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid amount. Please enter a positive number: ";
    }
    cin.ignore();

    cout << "Enter category: ";
    getline(cin, category);

    cout << "Enter description: ";
    getline(cin, description);

    try {
        work W(conn);
        W.exec_prepared("insert_expense", amount, category, description);
        W.commit();
        cout << "Expense added!\n";
    } catch (const exception& e) {
        cerr << "Error adding expense: " << e.what() << endl;
    }
}

void ExpenseManager::showAllExpenses() {
    try {
        nontransaction N(conn);
        result R = N.exec_prepared("select_all");

        cout << "ID\tAmount\tCategory\tDescription\n";
        cout << "------------------------------------------\n";

        for (auto row : R) {
            cout << row[0].as<int>() << "\t"
                 << row[1].as<double>() << "\t"
                 << row[2].c_str() << "\t"
                 << row[3].c_str() << "\n";
        }
    } catch (const exception& e) {
        cerr << "Error showing expenses: " << e.what() << endl;
    }
}

void ExpenseManager::deleteExpense() {
    int id;
    cout << "Enter ID to delete: ";
    cin >> id;

    try {
        work W(conn);
        W.exec_prepared("delete_expense", id);
        W.commit();
        cout << "Deleted!\n";
    } catch (const exception& e) {
        cerr << "Error deleting expense: " << e.what() << endl;
    }
}

void ExpenseManager::calculateTotalSpending() {
    try {
        nontransaction N(conn);
        result R = N.exec_prepared("sum_expenses");

        cout << "Total spent: ";
        if (!R.empty() && !R[0][0].is_null()) {
            cout << R[0][0].as<double>() << " kzt\n";
        } else {
            cout << "0 kzt\n";
        }
    } catch (const exception& e) {
        cerr << "Error calculating total: " << e.what() << endl;
    }
}

void ExpenseManager::showByDateRange() {
    string start_date, end_date;
    cout << "Enter start date (YYYY-MM-DD): ";
    cin >> start_date;
    cout << "Enter end date (YYYY-MM-DD): ";
    cin >> end_date;

    try {
        nontransaction N(conn);
        result R = N.exec_prepared("select_by_date", start_date, end_date);

        cout << "ID\tAmount\tCategory\tDescription\n";
        cout << "------------------------------------------\n";

        for (auto row : R) {
            cout << row[0].as<int>() << "\t"
                 << row[1].as<double>() << "\t"
                 << row[2].c_str() << "\t"
                 << row[3].c_str() << "\n";
        }
    } catch (const exception& e) {
        cerr << "Error filtering by date: " << e.what() << endl;
    }
}

void ExpenseManager::showTotalByCategory() {
    try {
        nontransaction N(conn);
        result R = N.exec_prepared("sum_by_category");

        cout << "Category\tTotal\n";
        cout << "------------------------\n";

        for (auto row : R) {
            cout << row[0].c_str() << "\t" << row[1].as<double>() << "\n";
        }
    } catch (const exception& e) {
        cerr << "Error showing total by category: " << e.what() << endl;
    }
}

void ExpenseManager::searchByDescription() {
    string keyword;
    cout << "Enter keyword to search in description: ";
    cin.ignore();
    getline(cin, keyword);
    keyword = "%" + keyword + "%";

    try {
        nontransaction N(conn);
        result R = N.exec_prepared("search_description", keyword);

        cout << "ID\tAmount\tCategory\tDescription\n";
        cout << "------------------------------------------\n";

        for (auto row : R) {
            cout << row[0].as<int>() << "\t"
                 << row[1].as<double>() << "\t"
                 << row[2].c_str() << "\t"
                 << row[3].c_str() << "\n";
        }
    } catch (const exception& e) {
        cerr << "Error searching description: " << e.what() << endl;
    }
}

void ExpenseManager::mostExpensiveExpense() {
    try {
        nontransaction N(conn);
        result R = N.exec_prepared("most_expensive");

        if (!R.empty()) {
            auto row = R[0];
            cout << "Most expensive expense:\n";
            cout << "ID: " << row[0].as<int>() << ", Amount: " << row[1].as<double>()
                 << ", Category: " << row[2].c_str() << ", Description: " << row[3].c_str() << "\n";
        } else {
            cout << "No expenses found.\n";
        }
    } catch (const exception& e) {
        cerr << "Error fetching most expensive expense: " << e.what() << endl;
    }
}
