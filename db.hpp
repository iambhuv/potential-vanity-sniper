#pragma once
// db.hpp
#include <sqlite3.h>

class DB {
public:
    static sqlite3* get();
private:
    static sqlite3* db;
};
