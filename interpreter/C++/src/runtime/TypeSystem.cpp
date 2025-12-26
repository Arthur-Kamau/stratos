#include "stratos/TypeSystem.h"
#include <sstream>

namespace stratos {

// ============================================================================
// Type Implementation
// ============================================================================

std::string Type::toString() const {
    std::stringstream ss;

    switch (kind) {
        case TypeKind::Primitive:
        case TypeKind::Class:
        case TypeKind::Interface:
            ss << name;
            break;

        case TypeKind::Generic:
            ss << name;
            break;

        case TypeKind::Parameterized:
            ss << name << "<";
            for (size_t i = 0; i < typeParams.size(); i++) {
                if (i > 0) ss << ", ";
                ss << typeParams[i]->toString();
            }
            ss << ">";
            break;

        case TypeKind::Optional:
            ss << elementType->toString() << "?";
            break;

        case TypeKind::Array:
            ss << "Array<" << elementType->toString() << ">";
            break;

        case TypeKind::Function:
            ss << "Function<";
            for (size_t i = 0; i < paramTypes.size(); i++) {
                if (i > 0) ss << ", ";
                ss << paramTypes[i]->toString();
            }
            if (returnType) {
                ss << ", " << returnType->toString();
            }
            ss << ">";
            break;

        case TypeKind::Result:
            ss << "Result<";
            if (typeParams.size() > 0) {
                ss << typeParams[0]->toString();
            }
            if (typeParams.size() > 1) {
                ss << ", " << typeParams[1]->toString();
            }
            ss << ">";
            break;
    }

    return ss.str();
}

std::string Type::toLLVMType() const {
    switch (kind) {
        case TypeKind::Primitive:
            if (name == "int") return "i32";
            if (name == "double") return "double";
            if (name == "bool") return "i1";
            if (name == "string") return "i8*";
            if (name == "void") return "void";
            return "i8*"; // Default to pointer

        case TypeKind::Generic:
            // Generic types are erased to pointers in LLVM
            return "i8*";

        case TypeKind::Parameterized:
        case TypeKind::Class:
        case TypeKind::Interface:
            // Class instances are pointers to structs
            return "%struct." + name + "*";

        case TypeKind::Optional:
            // Optionals are represented as pointers (null = None)
            return elementType->toLLVMType();

        case TypeKind::Array:
            // Arrays are pointers to array structures
            return "%struct.Array*";

        case TypeKind::Function:
            // Function pointers
            return "i8*";

        case TypeKind::Result:
            // Results are structs
            return "%struct.Result*";
    }

    return "i8*";
}

// ============================================================================
// TypeSystem Implementation
// ============================================================================

TypeSystem& TypeSystem::getInstance() {
    static TypeSystem instance;
    return instance;
}

std::shared_ptr<Type> TypeSystem::intType() {
    if (typeCache_.find("int") == typeCache_.end()) {
        typeCache_["int"] = std::make_shared<Type>(TypeKind::Primitive, "int");
    }
    return typeCache_["int"];
}

std::shared_ptr<Type> TypeSystem::doubleType() {
    if (typeCache_.find("double") == typeCache_.end()) {
        typeCache_["double"] = std::make_shared<Type>(TypeKind::Primitive, "double");
    }
    return typeCache_["double"];
}

std::shared_ptr<Type> TypeSystem::boolType() {
    if (typeCache_.find("bool") == typeCache_.end()) {
        typeCache_["bool"] = std::make_shared<Type>(TypeKind::Primitive, "bool");
    }
    return typeCache_["bool"];
}

std::shared_ptr<Type> TypeSystem::stringType() {
    if (typeCache_.find("string") == typeCache_.end()) {
        typeCache_["string"] = std::make_shared<Type>(TypeKind::Primitive, "string");
    }
    return typeCache_["string"];
}

std::shared_ptr<Type> TypeSystem::voidType() {
    if (typeCache_.find("void") == typeCache_.end()) {
        typeCache_["void"] = std::make_shared<Type>(TypeKind::Primitive, "void");
    }
    return typeCache_["void"];
}

std::shared_ptr<Type> TypeSystem::genericType(const std::string& name) {
    return std::make_shared<Type>(TypeKind::Generic, name);
}

std::shared_ptr<Type> TypeSystem::parameterizedType(
    const std::string& name,
    const std::vector<std::shared_ptr<Type>>& params) {

    auto type = std::make_shared<Type>(TypeKind::Parameterized, name);
    type->typeParams = params;
    return type;
}

std::shared_ptr<Type> TypeSystem::arrayType(std::shared_ptr<Type> elementType) {
    auto type = std::make_shared<Type>(TypeKind::Array, "Array");
    type->elementType = elementType;
    return type;
}

std::shared_ptr<Type> TypeSystem::optionalType(std::shared_ptr<Type> baseType) {
    auto type = std::make_shared<Type>(TypeKind::Optional, baseType->name);
    type->elementType = baseType;
    return type;
}

std::shared_ptr<Type> TypeSystem::functionType(
    const std::vector<std::shared_ptr<Type>>& params,
    std::shared_ptr<Type> returnType) {

    auto type = std::make_shared<Type>(TypeKind::Function, "Function");
    type->paramTypes = params;
    type->returnType = returnType;
    return type;
}

bool TypeSystem::isAssignable(std::shared_ptr<Type> from, std::shared_ptr<Type> to) {
    // Same type
    if (from->name == to->name && from->kind == to->kind) {
        return true;
    }

    // Generic type parameters can match anything
    if (to->kind == TypeKind::Generic) {
        return true;
    }

    // Check parameterized types
    if (from->kind == TypeKind::Parameterized && to->kind == TypeKind::Parameterized) {
        if (from->name != to->name) return false;
        if (from->typeParams.size() != to->typeParams.size()) return false;

        for (size_t i = 0; i < from->typeParams.size(); i++) {
            if (!isAssignable(from->typeParams[i], to->typeParams[i])) {
                return false;
            }
        }
        return true;
    }

    // Optional types
    if (to->kind == TypeKind::Optional) {
        return isAssignable(from, to->elementType);
    }

    return false;
}

std::shared_ptr<Type> TypeSystem::unify(std::shared_ptr<Type> t1, std::shared_ptr<Type> t2) {
    if (isAssignable(t1, t2)) return t2;
    if (isAssignable(t2, t1)) return t1;

    // If one is generic, substitute with the concrete type
    if (t1->kind == TypeKind::Generic) {
        substitutions[t1->name] = t2;
        return t2;
    }
    if (t2->kind == TypeKind::Generic) {
        substitutions[t2->name] = t1;
        return t1;
    }

    // Cannot unify
    return nullptr;
}

std::shared_ptr<Type> TypeSystem::instantiate(
    std::shared_ptr<Type> genericType,
    const std::vector<std::shared_ptr<Type>>& concreteTypes) {

    if (genericType->kind != TypeKind::Parameterized) {
        return genericType;
    }

    // Create a new type with concrete type parameters
    auto instantiated = std::make_shared<Type>(TypeKind::Parameterized, genericType->name);
    instantiated->typeParams = concreteTypes;

    return instantiated;
}

// ============================================================================
// Monomorphizer Implementation
// ============================================================================

std::string Monomorphizer::specialize(
    const std::string& genericName,
    const std::vector<std::shared_ptr<Type>>& typeArgs) {

    // Generate specialized name: genericName_Type1_Type2_...
    std::stringstream ss;
    ss << genericName;
    for (const auto& typeArg : typeArgs) {
        ss << "_" << typeArg->name;
    }

    std::string specializedName = ss.str();
    registerInstance(genericName, specializedName, typeArgs);

    return specializedName;
}

void Monomorphizer::registerInstance(
    const std::string& original,
    const std::string& specialized,
    const std::vector<std::shared_ptr<Type>>& typeArgs) {

    MonomorphizedInstance instance{original, specialized, typeArgs};
    instances_[original].push_back(instance);
}

std::vector<Monomorphizer::MonomorphizedInstance>
Monomorphizer::getInstances(const std::string& genericName) const {
    auto it = instances_.find(genericName);
    if (it != instances_.end()) {
        return it->second;
    }
    return {};
}

} // namespace stratos
