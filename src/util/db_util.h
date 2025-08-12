#pragma once

#include <pqxx/pqxx>

#include "log_util.h"

class db_util {
public:
    explicit db_util(const log_util log, const char *connectionString) : connectionString(connectionString), conn(nullptr), log(log) {}

    void init();
    void writeRow();
    int getRowCount() const;
private:
    const char *connectionString;
    std::unique_ptr<pqxx::connection> conn;
    log_util log;
};
