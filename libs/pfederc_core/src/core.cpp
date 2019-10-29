#include "pfederc/core.hpp"
using namespace pfederc;

LanguageConfiguration pfederc::createDefaultLanguageConfiguration() noexcept {
  return LanguageConfiguration{false, true};
}
