#include "ExpenseManager.hpp"
#include <iostream>
#include <cstdlib>
#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

int main() {
    const char* db_password = getenv("DB_PASSWORD");
    if (!db_password) {
        cout << "Set DB_PASSWORD first!\n";
        return 1;
    }

    connection C("dbname=expense_tracker user=postgres password=" + string(db_password) + " hostaddr=127.0.0.1 port=5432");
    cout << "DB Connected!\n";

    ExpenseManager manager(C);
    manager.prepareStatements();

    int choice;
    while (true) {
        cout << "\nExpense Tracker Menu:\n";
        cout << "1. Add Expense\n";
        cout << "2. Show All Expenses\n";
        cout << "3. Delete Expense\n";
        cout << "4. Total Spending\n";
        cout << "5. Expenses by Date Range\n";
        cout << "6. Total by Category\n";
        cout << "7. Search in Description\n";
        cout << "8. Most Expensive Expense\n";
        cout << "9. Quit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: manager.addExpense(); break;
            case 2: manager.showAllExpenses(); break;
            case 3: manager.deleteExpense(); break;
            case 4: manager.calculateTotalSpending(); break;
            case 5: manager.showByDateRange(); break;
            case 6: manager.showTotalByCategory(); break;
            case 7: manager.searchByDescription(); break;
            case 8: manager.mostExpensiveExpense(); break;
            case 9: cout << "Bye!\n"; return 0;
            default: cout << "Invalid option!\n";
        }
    }
}
