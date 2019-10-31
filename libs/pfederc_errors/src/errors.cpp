#include "pfederc/errors.hpp"
using namespace pfederc;

void pfederc::fatal(const char *file,
    int line, const std::string &msg) noexcept {
  std::cerr << file << ':' << line << ": " << msg << std::endl;
  std::exit(1);
}

// LogMessage

LogMessage::LogMessage(Level logLevel,
    const std::string &msg, const std::vector<LogMessage> &msgs) noexcept
    : logLevel{logLevel}, msg(msg), msgs(msgs) {
}

LogMessage::LogMessage(const LogMessage &msg) noexcept
    : logLevel{msg.logLevel}, msg(msg.msg), msgs(msg.msgs) {
}

LogMessage::~LogMessage() {
}

Level LogMessage::getLogLevel() const noexcept {
  return logLevel;
}

const std::string &LogMessage::getMessage() const noexcept {
  return msg;
}

const std::vector<LogMessage> &LogMessage::getMessages() const noexcept {
  return msgs;
}

void LogMessage::log(Logger &log) const noexcept {
  if (!log.log(getLogLevel(), getMessage()))
    return;

  for (const LogMessage &msg : getMessages()) {
    msg.log(log);
  }
}

// BaseLogger

BaseLogger::BaseLogger(std::ostream &out, std::ostream &err) noexcept
    : out{out}, err{err} {
}

BaseLogger::BaseLogger(const BaseLogger &log) noexcept
  : out{log.out}, err{log.err} {
}

BaseLogger::~BaseLogger() {
}

void BaseLogger::info(const std::string &msg) noexcept {
  out << msg << std::endl;
}

void BaseLogger::error(const std::string &msg) noexcept {
  err << msg << std::endl;
}

// Logger

Logger::Logger(Level allowedLevels, const BaseLogger &baseLog) noexcept
    : allowedLevels{allowedLevels}, baseLog(baseLog) {
}

Logger::~Logger() {
}

bool Logger::accept(Level logLevel) const noexcept {
  return (logLevel & allowedLevels) == logLevel;
}

bool Logger::log(Level logLevel, const std::string &msg) noexcept {
  if (!accept(logLevel))
    return false;

  if ((logLevel & (LVL_WARNING | LVL_NOTE | LVL_HELP)) != 0)
    baseLog.info(msg);
  else
    baseLog.error(msg);

  return true;
}
