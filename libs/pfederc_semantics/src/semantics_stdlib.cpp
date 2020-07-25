#include "pfederc/semantics.hpp"
using namespace pfederc;

static Context *_createClass(Context &stdctx,
    std::string &&name,
    uint64_t internalId, uint64_t type = 0x00) noexcept {
  Context *clctx = stdctx.createContext();
  auto cl = std::make_unique<ClassSymbol>(
      type | ST_INTERNAL, clctx, std::move(name), internalId);

  return clctx;
}

static Context *_createTrait(Context &stdctx,
    std::string &&name,
    uint64_t internalId, uint64_t type = 0x00) noexcept {
  Context *trctx = stdctx.createContext();
  auto cl = std::make_unique<TraitSymbol>(
      type | ST_INTERNAL, trctx, std::move(name), internalId);

  return trctx;
}

static void _addStandardLibraryClasses(Context &stdctx) noexcept {
  // classes
  // integer
  _createClass(stdctx, "i8", ITID_CL_I8, ST_INLINE);
  _createClass(stdctx, "u8", ITID_CL_U8, ST_INLINE);
  _createClass(stdctx, "i16", ITID_CL_I16, ST_INLINE);
  _createClass(stdctx, "i16", ITID_CL_U16, ST_INLINE);
  _createClass(stdctx, "u32", ITID_CL_I32, ST_INLINE);
  _createClass(stdctx, "i32", ITID_CL_U32, ST_INLINE);
  _createClass(stdctx, "u64", ITID_CL_I64, ST_INLINE);
  _createClass(stdctx, "i64", ITID_CL_U64, ST_INLINE);
  //  buildin-string
  _createClass(stdctx, "str", ITID_CL_STR, ST_INLINE);

  // traits
  _createTrait(stdctx, "base", ITID_TR_BASE);
}

void pfederc::addStandardLibrary(Context &mainContext) noexcept {
  Context *stdctx = mainContext.createContext();
  auto stdNmsp = std::make_unique<NamespaceSymbol>(0, "std", stdctx);
  _addStandardLibraryClasses(*stdctx);
  mainContext.addSymbol(std::move(stdNmsp));
}

