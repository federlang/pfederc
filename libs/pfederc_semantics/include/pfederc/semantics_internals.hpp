#ifndef PFEDERC_SEMANTICS_SEMANTICS_INTERNALS_HPP
#define PFEDERC_SEMANTICS_SEMANTICS_INTERNALS_HPP

#include "pfederc/core.hpp"
#include "pfederc/errors.hpp"
#include "pfederc/token.hpp"

namespace pfederc {
  constexpr uint64_t ITID_NOT_INTERNAL = 0;
  constexpr uint64_t ITID_FN_BUILDIN_PRINTSTR = 1;
  constexpr uint64_t ITID_FN_BUILDIN_PRINTI8= 2;
  constexpr uint64_t ITID_FN_BUILDIN_PRINTU8 = 3;
  constexpr uint64_t ITID_FN_BUILDIN_PRINTI16 = 4;
  constexpr uint64_t ITID_FN_BUILDIN_PRINTU16 = 5;
  constexpr uint64_t ITID_FN_BUILDIN_PRINTI32 = 6;
  constexpr uint64_t ITID_FN_BUILDIN_PRINTU32 = 7;
  constexpr uint64_t ITID_FN_BUILDIN_PRINTI64 = 8;
  constexpr uint64_t ITID_FN_BUILDIN_PRINTU64 = 9;

  constexpr uint64_t ITID_CL_STR = 1000000;
  constexpr uint64_t ITID_CL_I8 = 1000001;
  constexpr uint64_t ITID_CL_U8 = 1000002;
  constexpr uint64_t ITID_CL_I16 = 1000003;
  constexpr uint64_t ITID_CL_U16 = 1000004;
  constexpr uint64_t ITID_CL_I32 = 1000005;
  constexpr uint64_t ITID_CL_U32 = 1000006;
  constexpr uint64_t ITID_CL_I64 = 1000007;
  constexpr uint64_t ITID_CL_U64 = 1000008;

  constexpr uint64_t ITID_TR_BASE = 100000000;

}

#endif /* PFEDERC_SEMANTICS_SEMANTICS_INTERNALS_HPP */
