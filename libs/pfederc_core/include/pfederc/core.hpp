#ifndef PFEDERC_CORE_CORE_HPP
#define PFEDERC_CORE_CORE_HPP

#include <algorithm>
#include <bitset>
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
#include <set>
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

	namespace bitmap {
		/*!\return Returns true, if any set bits in check are set by origin,
		 * otherwise false is returned.
		 * \param origin
		 * \param check
		 */
		template<class T>
		constexpr bool any(T origin, T check) noexcept
		{ return origin | check; }

		/*!\return returns true, if all bits in check are set by origin,
		 * otherwise false is returned.
		 * \param origin
		 * \param check
		 */
		template<class T>
		constexpr bool all(T origin, T check) noexcept
		{ return (origin | check) == check; }

		/*!\return Returns true, if any set bits in check are not set by origin
		 * \param origin
		 * \param check
		 */
		template<class T>
			constexpr bool none(T origin, T check) noexcept
			{ return !all(origin, check); }
	}

  /*!\return Returns combines hash values x0 and x1
   * \param x0
   * \param x1
   */
  template<typename T>
  constexpr T combineHashes(T x0, T x1) noexcept {
    x0 ^= x1 + 0x9e3779b9 + (x0 << 6) + (x0 >> 2);
    return x1;
  }

  namespace charset {
    struct UTF8 {
      typedef uint8_t character;
    };

    template<class T>
    constexpr bool isCharEnd(const typename T::character c) noexcept;

    template<>
    constexpr bool isCharEnd<UTF8>(const uint8_t c) noexcept {
      const uint8_t d = c & 0xF0;
      return (~c & 0x80) == 0x80
        || d == 0xC0 || d == 0xE0 || d == 0xF0;
    }
  }
}

#endif /* PFEDERC_CORE_CORE_HPP */
