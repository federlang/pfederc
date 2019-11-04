#ifndef PFEDERC_CORE_CORE_HPP
#define PFEDERC_CORE_CORE_HPP

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <istream>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <tuple>
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

  LanguageConfiguration createDefaultLanguageConfiguration() noexcept;
}

#endif /* PFEDERC_CORE_CORE_HPP */
