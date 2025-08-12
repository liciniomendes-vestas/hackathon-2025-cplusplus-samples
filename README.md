# Vestas Platform Hackathon C++ Sample Application

This project is a sample C++ application for Platform hackathon that helps demonstrating key platform features. 

The application runs a continuous loop that:
- Logs informational messages (in plain text or JSON format).
- Optionally connects to a PostgreSQL database to write and read data.
- Optionally exports metrics using the OpenTelemetry standard.

## Prerequisites

- A C++17 compatible compiler (e.g., GCC, Clang)
- CMake (version 3.12 or higher)
- Conan (the C++ package manager)
- Python (required for Conan)

## Configuration

The application is configured using environment variables:

> Note: all of the configuration options are optional. The application will run without any of them and will only produce plain text logs to standard output.

- `OTEL_EXPORTER_OTLP_ENDPOINT`: The endpoint for the OpenTelemetry collector. If set, metrics will be exported.
  - **Example**: `export OTEL_EXPORTER_OTLP_ENDPOINT="http://localhost:4318/v1/metrics"`
- `CONNECTION_STRING`: The connection string for the PostgreSQL database. If set, the application will interact with the database.
  - **Example**: `export CONNECTION_STRING="postgresql://user:password@localhost:5432/mydatabase"`
- `STRUCTURED_LOGS_ENABLED`: Set to `true` to enable structured (JSON) logging.
  - **Example**: `export STRUCTURED_LOGS_ENABLED="true"`

## Build Instructions

1.  **Detect Conan Profile**:
    ```bash
    conan profile detect
    ```

2.  **Install Dependencies**:
    ```bash
    conan install -of bld --build=missing .
    ```

3.  **Configure with CMake**:
    ```bash
    cmake --preset conan-release
    ```

4.  **Build the Project**:
    ```bash
    cmake --build --preset conan-release
    ```

The final executable will be located at `bld/build/Release/hackathon-sample`.
