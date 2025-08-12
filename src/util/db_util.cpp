//
// Created by ed on 8/8/25.
//

#include "db_util.h"

#include <fmt/format.h>

#include "log_util.h"

void db_util::init() {
    conn = std::make_unique<pqxx::connection>(connectionString);

    if (!conn->is_open()) {
        throw std::runtime_error("Failed to open DB connection");
    }

    log.info(fmt::format("Connected to database: {}", conn->dbname()));
    log.info("Ensuring that table 'public.hackathon' exists..");
    pqxx::work transaction(*conn);

    pqxx::result result = transaction.exec("CREATE TABLE IF NOT EXISTS public.hackathon (timestamp TEXT);");
    transaction.commit();
}

void db_util::writeRow() {
    if (!conn) {
        throw std::runtime_error("DB connection not initialized");
    }

    // Get current time as ISO8601 string
    const auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &now_c);
#else
    localtime_r(&now_c, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");

    pqxx::work transaction(*conn);
    transaction.exec(pqxx::zview{"INSERT INTO public.hackathon (timestamp) VALUES ($1)"}, pqxx::params(oss.str()));
    transaction.commit();
}


int db_util::getRowCount() const {
    if (!conn) {
        throw std::runtime_error("DB connection not initialized");
    }

    pqxx::work transaction(*conn);
    const pqxx::result result = transaction.exec("SELECT COUNT(*) FROM public.hackathon");

    return result[0][0].as<int>();
}
