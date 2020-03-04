#include "pfederc/semantics.hpp"
using namespace pfederc;

// Semantic

Semantic::Semantic(uint32_t type, Expr *expr, Semantic *parent,
    std::unordered_map<SemanticInfoStringType, std::string> &&infoStrings,
		std::unordered_map<SemanticSingleType, Semantic*> &&singleSemantics,
		std::unordered_map<SemanticMultipleType, std::list<Semantic*>> &&multipleSemantics) noexcept
	: type{type}, expr{expr}, parent{parent}, infoStrings(std::move(infoStrings)),
	  singleSemantics(std::move(singleSemantics)),
		multipleSemantics(std::move(multipleSemantics)) {
}

Semantic::~Semantic() {
}

const Semantic *Semantic::findChild(const std::string &name, bool enablechildren) const noexcept {
  if (enablechildren) {
    for (auto &child : children) {
      if (child.first == name)
        return child.second;
    }
  }

  if (hasAllSemanticType(getType(), ST_NAME)
      && getInfoString(SemanticInfoStringType::NAME) == name)
    return this;

  if (!getParent())
    return nullptr;

  return getParent()->findChild(name, enablechildren);
}

bool Semantic::addChild(const std::string &name, Semantic *semantic) noexcept {
  auto pair = children.insert(std::pair<std::string, Semantic*>(name, semantic));
  return pair.second;
}

bool Semantic::setChild(const std::string &name, Semantic *semantic) noexcept {
  auto it = children.find(name);
  if (it == children.end())
    return false;

  it->second = semantic;
  return true;
}

Semantic *Semantic::getChild(const std::string &name) noexcept {
  auto it = children.find(name);
  // nothing found
  if (it == children.end())
    return nullptr;
  // found iterator/associated value
  return it->second;
}


bool Semantic::isTypeTupleObjects() const noexcept {
  if (!hasAllSemanticType(type, ST_TUPLE | ST_VAR | ST_OBJ))
    return false;

	bool result = true;
	forEachSemantics(SemanticMultipleType::OBJS, [&result](Semantic *obj) {
    	if (result && !hasAllSemanticType(obj->getType(), ST_OBJ))
				result = false;
	});

  return result;
}

bool Semantic::isTypeTupleVariables() const noexcept {
  if (!hasAllSemanticType(type, ST_TUPLE | ST_VAR | ST_OBJ))
    return false;

	bool result = true;
	forEachSemantics(SemanticMultipleType::OBJS, [&result](Semantic *obj) {
		if (result && !hasAllSemanticType(obj->getType(), ST_OBJ | ST_VAR))
			result = false;
  });

  return result;
}

bool Semantic::isTypeRegularObject() const noexcept {
  if (!isTypeValidObject())
    return false;

  return hasAnySemanticType(singleSemantics.at(SemanticSingleType::TYPE)->getType(),
      ST_CLASS | ST_TRAIT);
}

bool Semantic::isTypeInternalObject() const noexcept {
  if (!isTypeValidObject())
    return false;

  return hasAnySemanticType(singleSemantics.at(SemanticSingleType::TYPE)->getType(),
      ST_INTERNAL);
}

void Semantic::setSemantic(SemanticSingleType type, Semantic *semantic) noexcept {
  singleSemantics[type] = semantic;
}

Semantic *Semantic::getSemantic(SemanticSingleType type) noexcept {
	return singleSemantics[type];
}

const Semantic *Semantic::getSemantic(SemanticSingleType type) const noexcept {
	return singleSemantics.at(type);
}

bool Semantic::addSemantics(SemanticMultipleType type, Semantic *semantic) noexcept {
	std::list<Semantic*> semantics = multipleSemantics[type];
	if (std::find_if(semantics.begin(), semantics.end(),
				[semantic](Semantic *const toCheck) { return toCheck == semantic; }) != semantics.end())
		return false;

	multipleSemantics[type].push_back(semantic);
	return true;
}

void Semantic::forEachSemantics(SemanticMultipleType type,
				const std::function<void(Semantic *semantic)> &fn) const noexcept {
	const std::list<Semantic *> semantics = multipleSemantics.at(type);
	std::for_each(semantics.begin(), semantics.end(), fn);
}

Semantic *SafeSemantic::getSemantic(SemanticSingleType type) noexcept {
  std::lock_guard<std::mutex> lck(mtxSingle);
  return Semantic::getSemantic(type);
}

const Semantic *SafeSemantic::getSemantic(SemanticSingleType type) const noexcept {
  std::lock_guard<std::mutex> lck(mtxSingle);
  return Semantic::getSemantic(type);
}
                                                                     
SafeSemantic::SafeSemantic(uint32_t type, Expr *expr, Semantic *parent,
    std::unordered_map<SemanticInfoStringType, std::string> &&infoStrings,
		std::unordered_map<SemanticSingleType, Semantic*> &&singleSemantics,
		std::unordered_map<SemanticMultipleType, std::list<Semantic*>> &&multipleSemantics) noexcept
		: Semantic(type, expr, parent, std::move(infoStrings),
			std::move(singleSemantics), std::move(multipleSemantics)),
			mtxChildren(), mtxSingle(), mtxMultiple() {
}

SafeSemantic::~SafeSemantic() {
}

const Semantic *SafeSemantic::findChild(const std::string &name, bool enablechildren) const noexcept {
  std::lock_guard<std::mutex> lck(mtxChildren);
  return Semantic::findChild(name, enablechildren);
}

bool SafeSemantic::addChild(const std::string &name, Semantic *semantic) noexcept {
  std::lock_guard<std::mutex> lck(mtxChildren);
  return Semantic::addChild(name, semantic);
}

bool SafeSemantic::setChild(const std::string &name, Semantic *semantic) noexcept {
  std::lock_guard<std::mutex> lck(mtxChildren);
  return Semantic::setChild(name, semantic);
}

Semantic *SafeSemantic::getChild(const std::string &name) noexcept {
  std::lock_guard<std::mutex> lck(mtxChildren);
  return Semantic::getChild(name);
}

void SafeSemantic::setSemantic(SemanticSingleType type, Semantic *semantic) noexcept {
	std::lock_guard<std::mutex> lck(mtxSingle);
	Semantic::setSemantic(type, semantic);
}

bool SafeSemantic::addSemantics(SemanticMultipleType type, Semantic *semantic) noexcept {
	std::lock_guard<std::mutex> lck(mtxMultiple);
	return Semantic::addSemantics(type, semantic);
}

void SafeSemantic::forEachSemantics(SemanticMultipleType type,
				const std::function<void(Semantic *semantic)> &fn) const noexcept {
	std::lock_guard<std::mutex> lck(mtxMultiple);
	Semantic::forEachSemantics(type, fn);
}

// TypeAnalyzer
TypeAnalyzer::TypeAnalyzer(const size_t maxthreads) noexcept
		: maxthreads{maxthreads},
			mtxSemantics(), semantics() {
  assert(maxthreads > 0);
}

TypeAnalyzer::~TypeAnalyzer() {
}

void TypeAnalyzer::addSemanticToBuilder(const std::string &name, Semantic *semantic) noexcept {
  std::lock_guard<std::mutex> lck(mtxBuildSemantics);
  if (targetBuildSemantics.find(name) != targetBuildSemantics.end()) {
    targetBuildSemantics[name].push_back(semantic);
    return;
  }

  targetBuildSemantics.insert(std::pair<std::string, std::list<Semantic*>>(
        name, std::list<Semantic*>{semantic}));
}

Semantic* TypeAnalyzer::addSemantic(const std::string &mangle,
    std::unique_ptr<Semantic> &&semantic) noexcept {
  Expr *expr = semantic->getExpression();
	std::lock_guard<std::mutex> lck(mtxSemantics);
	auto pair = semantics.insert(std::pair<std::string, std::unique_ptr<Semantic>>(
				mangle, std::move(semantic)));
  Semantic *result = pair.first->second.get();
  if (pair.second) {
#ifndef NDEBUG
    assert(exprToSemantic.insert(std::pair<Expr*,Semantic*>(expr, result)).second);
#else
    exprToSemantic.insert(std::pair<Expr*,Semantic*>(expr, result));
#endif
  }
  return result;
}

Semantic *TypeAnalyzer::addSemanticAlways(const std::string &mangle,
    std::unique_ptr<Semantic> &&semantic) noexcept {
  Expr *expr = semantic->getExpression();
  Semantic *pSemantic = semantic.get();
  Semantic *result = addSemantic(mangle, std::move(semantic));
  if (result != pSemantic) {
    // addSemantic doesn't bind expr to semantic if insertion wasn't
    // successfull, in that case still insert it
#ifndef NDEBUG
    assert(exprToSemantic.insert(std::pair<Expr*,Semantic*>(expr, result)).second);
#else
    exprToSemantic.insert(std::pair<Expr*,Semantic*>(expr, result));
#endif
  }

  return result;
}

Semantic *TypeAnalyzer::getSemantic(const std::string &mangle) const noexcept {
	std::lock_guard<std::mutex> lck(mtxSemantics);
	auto it = semantics.find(mangle);
	if (it == semantics.end())
		return nullptr;

	return it->second.get();
}

void TypeAnalyzer::forEachSemantic(const std::function<void(
      const std::string&,Semantic*)> &fn) const noexcept {
  std::lock_guard<std::mutex> lck(mtxSemantics);
  for (auto &pair : semantics)
    fn(pair.first, pair.second.get());
}
