#pragma once
#include <fstream>
#include <string_view>

namespace tui {

// Set to true/false via CMake (-DENABLE_LOGGING) or manual compile flag
#ifdef ENABLE_LOGGING
constexpr bool is_logging_enabled = true;
#else
constexpr bool is_logging_enabled = false;
#endif

class Logger {
 public:
  static Logger& instance() {
    static Logger inst;
    return inst;
  }

  void enable(std::string_view path = "tui_debug.log") {
    if constexpr (is_logging_enabled) {
      if (!file_.is_open()) {
        file_.open(std::string(path), std::ios::out | std::ios::app);
      }
    }
  }

  void log(std::string_view message) {
    if constexpr (is_logging_enabled) {
      if (file_.is_open()) {
        file_ << message << "\n";
        file_.flush();  // Immediate write to disk for crash debugging
      }
    }
  }

 private:
  Logger() = default;

  ~Logger() {
    if constexpr (is_logging_enabled) {
      if (file_.is_open()) {
        file_.close();
      }
    }
  }

  // Disable copy / move semantics for singleton
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  std::ofstream file_;
};

// Global inline helper function
inline void log_debug(std::string_view message) {
  if constexpr (is_logging_enabled) {
    Logger::instance().log(message);
  }
}

}  // namespace tui
