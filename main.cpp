#include <pqxx/pqxx> 
#include <iostream>
#include <cstdlib>  
#include <string>

using namespace std;
using namespace pqxx;

void addExp(connection &C) {
    double amount;
    string category, description;

    cout << "Enter amount: ";
    cin >> amount;
    cin.ignore(); 

    cout << "Enter category: ";
    getline(cin, category);

    cout << "Enter description: ";
    getline(cin, description);

    work W(C);
    string query = "INSERT INTO expenses (amount, category, description) VALUES (" +
                   to_string(amount) + ", '" + category + "', '" + description + "');";
    W.exec(query);
    W.commit();
    cout << "Expense added!\n";
}

void showallExp(connection &C) {
    nontransaction N(C);
    result R = N.exec("SELECT * FROM expenses;");
    
    cout << "ID\tAmount\tCategory\tDescription\n";
    cout << "------------------------------------------\n";
    size_t i = 0;
    while (i < R.size()) {
        cout << R[i][0].as<int>() << "\t"
        << R[i][1].as<double>() << "\t"
        << R[i][2].c_str() << "\t"
        << R[i][3].c_str() << "\n";
        i++;
    }
}

void deleteExp(connection &C) {
    int id;
    cout << "Enter ID to delete: ";
    cin >> id;
    
    work W(C);
    W.exec("DELETE FROM expenses WHERE id = " + to_string(id) + ";");
    W.commit();
    cout << "Deleted!\n";
}

void calculateTotalSpending(connection &C) {
    nontransaction N(C);
    result R = N.exec("SELECT SUM(amount) FROM expenses;");

    cout << "Total spent: ";
    if (!R.empty() && !R[0][0].is_null()) {
        cout << R[0][0].as<double>() << " kzt" << endl;
    } else {
        cout << "0 kzt\n";
    }
}

int main() {
    const char* db_password = getenv("DB_PASSWORD");

    if (!db_password) {
        cout << "Set DB_PASSWORD first!\n";
        return 1;
    }

    connection C("dbname=expense_tracker user=postgres password=" + string(db_password) + " hostaddr=127.0.0.1 port=5432");
    
    cout << "DB Connected!\n";

    int choice;
    while (true) {
        cout << "\nExpense Tracker\n";
        cout << "1. Add\n";
        cout << "2. Show All\n";
        cout << "3. Remove\n";
        cout << "4. Total\n";
        cout << "5. Quit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                addExp(C);
                break;
            case 2:
                showallExp(C);
                break;
            case 3:
                deleteExp(C);
                break;
            case 4:
                calculateTotalSpending(C);
                break;
            case 5:
                cout << "Bye!\n";
                return 0;
            default:
                cout << "Invalid!\n";
        }
    }
}
