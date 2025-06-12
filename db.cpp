#include "db.hpp"
#include <iostream>
#include <sqlite3.h>

sqlite3 *DB::db = nullptr;

const char *SQLITE_DB_FILENAME = "vanity.db";

sqlite3 *DB::get() {
  if (!db) {
    sqlite3_open(SQLITE_DB_FILENAME, &db);

    atexit([] {
      if (db)
        sqlite3_close(db);
    });
  }
  return db;
}

void create_db_client() {
  int rc;
  char *err = 0;

  sqlite3 *db = DB::get();

  rc = sqlite3_exec(
      db,
      "CREATE TABLE IF NOT EXISTS volunteers(id TEXT, token TEXT, password "
      "TEXT, name TEXT, username TEXT, email TEXT, mfa_secret TEXT, PRIMARY "
      "KEY (token));",
      0, 0, &err);

  if (rc != SQLITE_OK) {
    std::cout << "Some Shi Happened: " << sqlite3_errmsg(db) << std::endl;
  }
}

void synchronise_config() {
  char *err = 0;

  sqlite3 *db = DB::get();

  sqlite3_exec(db,
               "CREATE TABLE IF NOT EXISTS config_sync (id INTEGER PRIMARY KEY "
               "CHECK (id = 1), config_hash TEXT NOT NULL);",
               nullptr, nullptr, &err);

  // sqlite3_exec(DB::get(), "SOMETHIN", 0, 0, &err);
}