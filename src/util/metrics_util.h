#pragma once

class metrics_util
{
public:
  static void init(const char *endpoint);
  static void cleanUpMetrics();
  static void writeCounter(const std::string &name, double value);
  static void writeGauge(const std::string &name, double value);
};
