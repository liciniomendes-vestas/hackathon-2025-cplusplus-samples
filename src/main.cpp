#include <chrono>
#include <csignal>
#include <thread>
#include <ctime>
#include <iostream>
#include <atomic>
#include <condition_variable>
#include <fmt/printf.h>
#include <mutex>

#include "db_util.h"
#include "log_util.h"
#include "util/metrics_util.h"

static const char *otelEndpoint = std::getenv("OTEL_EXPORTER_OTLP_ENDPOINT");
static const char *connectionString = std::getenv("CONNECTION_STRING");

static const bool metricsEnabled = otelEndpoint && *otelEndpoint != '\0';
static const bool databaseEnabled = connectionString && *connectionString != '\0';
static const bool structuredLogsEnabled = std::getenv("STRUCTURED_LOGS_ENABLED") && std::string(std::getenv("STRUCTURED_LOGS_ENABLED")) == "true";

static std::atomic running(true);
static std::condition_variable condition_variable;
static std::mutex condition_variable_mutex;

static auto logger = log_util(structuredLogsEnabled);
static auto db = db_util(logger, connectionString);

void SignalHandler(int) {
    running = false;
    condition_variable.notify_all();
}

// enables metrics if `OTEL_EXPORTER_OTLP_ENDPOINT` is set
void initMetrics() {
    if (!metricsEnabled) {
        logger.warn("No OTEL_EXPORTER_OTLP_ENDPOINT set. Metrics disabled.");
    } else {
        logger.info("OTEL_EXPORTER_OTLP_ENDPOINT is set. Metrics enabled.");
        metrics_util::init(otelEndpoint);
    }
}
// enabled database interaction if `CONNECTION_String` is set
void initDatabase() {
    if (!databaseEnabled) {
        logger.warn("No CONNECTION_STRING set. Database disabled.");
    }
    else {
        logger.info("CONNECTION_STRING set. Starting database setup.");
        db.init();
    }
}

int main() {
    logger.info("Starting...");

    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    initMetrics();
    initDatabase();

    std::unique_lock lock(condition_variable_mutex);
    while (running) {
        logger.info("Starting worker loop");
        if (metricsEnabled) {
            logger.info("Writing metrics..");
            metrics_util::writeCounter("loops", 1);
        } else {
            logger.warn("Skipping metrics, because they are not configured.");
        }

        if (databaseEnabled) {
            logger.info("Interacting with database..");
            db.writeRow();
            auto rowCount = db.getRowCount();
            if (metricsEnabled) {
                metrics_util::writeCounter("rows_created", 1);
            }
            logger.info(fmt::format("There are {} rows in the database", rowCount));
        } else {
            logger.warn("Skipping database interaction, because it is not configured.");
        }

        logger.info("Worker loop finished, will run again in 15 seconds");
        condition_variable.wait_for(lock, std::chrono::seconds(1));
    }

    logger.info("Shutting down gracefully...");
    metrics_util::cleanUpMetrics();
    return 0;
}
