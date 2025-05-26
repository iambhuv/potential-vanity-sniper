#include <iostream>
#include <sqlite3.h>

const char *SQLITE_DB_FILENAME = "vanity.db";

sqlite3 *create_db_client()
{
  sqlite3 *db;
  char *err = 0;

  int rc = sqlite3_open(SQLITE_DB_FILENAME, &db);

  if (rc)
  {
    std::cout << "Can't open DB:" << sqlite3_errmsg(db) << std::endl;
  }

  rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS volunteers(id TEXT, token TEXT, password TEXT, name TEXT, mfa_secret TEXT, PRIMARY KEY (token));", 0, 0, &err);

  if (rc != SQLITE_OK)
  {
    std::cout << "Some Shi Happened: " << sqlite3_errmsg(db) << std::endl;
  }

  return db;
}

int main()
{
  sqlite3 *db;
  char *err = 0;

  int rc = sqlite3_open(SQLITE_DB_FILENAME, &db);

  if (rc)
  {
    std::cout << "Can't open DB:" << sqlite3_errmsg(db) << std::endl;
    return 1;
  }
  std::cout << "SQLite3 DB Open" << std::endl;

  rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS volunteers(id TEXT, token TEXT, password TEXT, name TEXT, mfa_secret TEXT, PRIMARY KEY (token));", 0, 0, &err);

  if (rc != SQLITE_OK)
  {
    std::cout << "Some shi happened" << sqlite3_errmsg(db) << std::endl;
    return 1;
  }

  sqlite3_close(db);
}