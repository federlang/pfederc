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
}

#endif /* PFEDERC_CORE_CORE_HPP */
