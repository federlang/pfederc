#ifndef PFEDERC_ERRORS_ERRORS_HPP
#define PFEDERC_ERRORS_ERRORS_HPP

#include "pfederc/core.hpp"

namespace pfederc {
  class BaseLogger;
  class Logger;
  class LogMessage;

  enum Level : uint16_t {
    /*!\brief Code Warning
     */
    LVL_WARNING  = 0x0001,
    /*!\brief Code error
     */
    LVL_ERROR    = 0x0002,
    /*!\brief Code note
     */
    LVL_NOTE     = 0x0004,
    /*!\brief Code help
     */
    LVL_HELP     = 0x0008,
    /*!\brief Errors, which are not related to Feder code
     *
     * Excluding invalid program states
     */
    LVL_FATAL    = 0x0010,
    /*!\brief Invalid program states
     */
    LVL_FAILURE  = 0x0020
  };

  /*!\brief Bitmask of all log levels
   */
  constexpr Level LVL_ALL = static_cast<Level>(LVL_WARNING | LVL_ERROR
      | LVL_NOTE | LVL_HELP | LVL_FATAL | LVL_FAILURE);

  /*!\brief Wrapping output streams
   */
  class BaseLogger {
    std::ostream &out, &err;
  public:
    BaseLogger(std::ostream &out = std::cout, std::ostream &err = std::cerr) noexcept;
    BaseLogger(const BaseLogger &log) noexcept;
    virtual ~BaseLogger();

    void info(const std::string &msg) noexcept;
    void error(const std::string &msg) noexcept;
  };

  /*!\brief Logger with filtering
   */
  class Logger {
    Level allowedLevels;
    BaseLogger baseLog;
  public:
    Logger(Level allowedLevels = LVL_ALL,
        const BaseLogger &baseLog = BaseLogger()) noexcept;
    virtual ~Logger();

    /*!\return Returns true if logLevels are not filtered by Logger, otherwise
     * false.
     * \param logLevels Log levels to check
     */
    bool accept(Level logLevels) const noexcept;

    /*!\brief Log msg to console if Logger doesn't filter logLevel
     *
     * Message is filtered if accept(logLevel) returns false.
     *
     * \return Returns true if message wasn't filtered, otherwise false.
     */
    bool log(Level logLevel, const std::string &msg) noexcept;
  };

  /*!\brief Log message with optional attached messages
   */
  class LogMessage {
    Level logLevel;
    std::string msg;
    std::vector<LogMessage> msgs;
  public:
    LogMessage(Level logLevel, const std::string &msg,
               const std::vector<LogMessage> &msgs = {}) noexcept;
    LogMessage(const LogMessage &msg) noexcept;
    virtual ~LogMessage();

    Level getLogLevel() const noexcept;
    const std::string &getMessage() const noexcept;

    /*!\return Returns attached messages
     */
    const std::vector<LogMessage> &getMessages() const noexcept;

    /*!\brief Logs message and attached messages
     * \param log
     *
     * Attached messages are only logged in case msg was logged
     */
    void log(Logger &log) const noexcept;
  };

  void fatal(const char *file, int line, const std::string &msg) noexcept;
}

#endif /* PFEDERC_ERRORS_ERRORS_HPP */
