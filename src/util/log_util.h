#pragma once
#include <string>

class log_util {
public:
   explicit log_util(const bool useStructuredLogs) : useStructuredLogs(useStructuredLogs) {}
   void info(const std::string &message) const;
   void warn(const std::string &message) const;
   void error(const std::string &message) const;
private:
   bool useStructuredLogs;
};
