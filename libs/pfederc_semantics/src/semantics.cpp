#include "pfederc/semantics.hpp"
using namespace pfederc;

// Semantic

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

// SafeSemantic

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
