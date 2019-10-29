#ifndef PFEDERC_CORE_CORE_HPP
#define PFEDERC_CORE_CORE_HPP

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
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
