#pragma once
#include <pqxx/pqxx>
#include <string>

class ExpenseManager {
private:
    pqxx::connection& conn;

public:
    ExpenseManager(pqxx::connection& c);

    void prepareStatements();
    void addExpense();
    void showAllExpenses();
    void deleteExpense();
    void calculateTotalSpending();

    void showByDateRange();
    void showTotalByCategory();
    void searchByDescription();
    void mostExpensiveExpense();
};
