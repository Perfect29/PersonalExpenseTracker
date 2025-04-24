cmake_minimum_required(VERSION 3.10)
project(PersonalExpenseTracker)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

include_directories(${PQXX_INCLUDE_DIRS})
link_directories(${PQXX_LIBRARY_DIRS})

add_executable(expense_tracker
    main.cpp
    ExpenseManager.cpp
)

target_link_libraries(expense_tracker ${PQXX_LIBRARIES})
