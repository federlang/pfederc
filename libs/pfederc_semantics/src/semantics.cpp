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

  for (auto &objs : multipleSemantics.at(SemanticMultipleType::OBJS)) {
    if (!hasAllSemanticType(type, ST_OBJ))
      return false;
  }

  return true;
}

bool Semantic::isTypeTupleVariables() const noexcept {
  if (!hasAllSemanticType(type, ST_TUPLE | ST_VAR | ST_OBJ))
    return false;

  for (auto &objs : multipleSemantics.at(SemanticMultipleType::OBJS)) {
    if (!hasAllSemanticType(type, ST_OBJ | ST_VAR))
      return false;
  }

  return true;
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
	if (singleSemantics.find(type) != singleSemantics.end())
		singleSemantics[type] = semantic;
	else
		singleSemantics.insert(std::pair<SemanticSingleType, Semantic*>(type, semantic));
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

// SafeSemantic

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
  return SafeSemantic::findChild(name, enablechildren);
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
}

TypeAnalyzer::~TypeAnalyzer() {
}

bool TypeAnalyzer::addSemantic(const std::string &mangle, std::unique_ptr<Semantic> &&semantic) noexcept {
	std::lock_guard<std::mutex> lck(mtxSemantics);
	auto pair = semantics.insert(std::pair<std::string, std::unique_ptr<Semantic>>(
				mangle, std::move(semantic)));
	return pair.second;
}

Semantic *TypeAnalyzer::getSemantic(const std::string &mangle) noexcept {
	std::lock_guard<std::mutex> lck(mtxSemantics);
	auto it = semantics.find(mangle);
	if (it == semantics.end())
		return nullptr;

	return it->second.get();
}
