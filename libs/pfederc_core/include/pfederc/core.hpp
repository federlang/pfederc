#ifndef PFEDERC_CORE_CORE_HPP
#define PFEDERC_CORE_CORE_HPP

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <functional>
#include <future>
#include <iostream>
#include <iterator>
#include <istream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace pfederc {
  /*!\brief Booleans setting how the parser-frontend behaves
   *
   * This structure is for managing different versions of the language
   */
  struct LanguageConfiguration {
    bool multiLineString,
         multiLineStringLeftTrim;
  };

  constexpr LanguageConfiguration createDefaultLanguageConfiguration() noexcept {
    return LanguageConfiguration{false, true};
  }
}

#endif /* PFEDERC_CORE_CORE_HPP */
