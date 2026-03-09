#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>

namespace stratos {

/**
 * Type kinds in the Stratos type system
 */
enum class TypeKind {
    Primitive,    // int, double, bool, string, void
    Generic,      // T, U, V, etc. (type parameters)
    Parameterized,// List<T>, Map<K,V>, etc. (generic types with concrete parameters)
    Function,     // Function types
    Class,        // User-defined classes
    Interface,    // Interface types
    Optional,     // T? (nullable types)
    Array,        // Array<T>
    Result        // Result<T, E>
};

/**
 * Represents a type in the Stratos type system
 */
class Type {
public:
    TypeKind kind;
    std::string name;
    std::vector<std::shared_ptr<Type>> typeParams; // For generic types
    std::shared_ptr<Type> elementType; // For arrays and optionals
    std::vector<std::shared_ptr<Type>> paramTypes; // For function types
    std::shared_ptr<Type> returnType; // For function types

    Type(TypeKind k, const std::string& n) : kind(k), name(n) {}

    // Check if this is a generic type (has type parameters)
    bool isGeneric() const {
        return !typeParams.empty() || kind == TypeKind::Generic;
    }

    // Check if this is a concrete type (no type parameters)
    bool isConcrete() const {
        return !isGeneric();
    }

    // Get string representation
    std::string toString() const;

    // Get LLVM type representation
    std::string toLLVMType() const;
};

/**
 * Type system for handling generic types and type inference
 */
class TypeSystem {
public:
    static TypeSystem& getInstance();

    // Create primitive types
    std::shared_ptr<Type> intType();
    std::shared_ptr<Type> doubleType();
    std::shared_ptr<Type> boolType();
    std::shared_ptr<Type> stringType();
    std::shared_ptr<Type> voidType();

    // Create generic type parameter
    std::shared_ptr<Type> genericType(const std::string& name);

    // Create parameterized type (e.g., List<int>)
    std::shared_ptr<Type> parameterizedType(const std::string& name,
                                            const std::vector<std::shared_ptr<Type>>& params);

    // Create array type
    std::shared_ptr<Type> arrayType(std::shared_ptr<Type> elementType);

    // Create optional type
    std::shared_ptr<Type> optionalType(std::shared_ptr<Type> baseType);

    // Create function type
    std::shared_ptr<Type> functionType(const std::vector<std::shared_ptr<Type>>& params,
                                       std::shared_ptr<Type> returnType);

    // Type checking and inference
    bool isAssignable(std::shared_ptr<Type> from, std::shared_ptr<Type> to);
    std::shared_ptr<Type> unify(std::shared_ptr<Type> t1, std::shared_ptr<Type> t2);

    // Generic instantiation
    std::shared_ptr<Type> instantiate(std::shared_ptr<Type> genericType,
                                      const std::vector<std::shared_ptr<Type>>& concreteTypes);

    // Type substitution for generics
    std::unordered_map<std::string, std::shared_ptr<Type>> substitutions;

private:
    TypeSystem() = default;
    std::unordered_map<std::string, std::shared_ptr<Type>> typeCache_;
};

/**
 * Monomorphization - Convert generic types to concrete types
 * This is used to generate specialized versions of generic functions/classes
 */
class Monomorphizer {
public:
    struct MonomorphizedInstance {
        std::string originalName;
        std::string specializedName;
        std::vector<std::shared_ptr<Type>> typeArgs;
    };

    // Generate specialized version of a generic function
    std::string specialize(const std::string& genericName,
                          const std::vector<std::shared_ptr<Type>>& typeArgs);

    // Track monomorphized instances
    void registerInstance(const std::string& original,
                         const std::string& specialized,
                         const std::vector<std::shared_ptr<Type>>& typeArgs);

    // Get all instances of a generic
    std::vector<MonomorphizedInstance> getInstances(const std::string& genericName) const;

private:
    std::unordered_map<std::string, std::vector<MonomorphizedInstance>> instances_;
};

/**
 * Generic constraint system (for future use)
 */
class GenericConstraint {
public:
    enum class Kind {
        Extends,    // T extends SomeClass
        Implements, // T implements SomeInterface
        Equals      // T == SomeType
    };

    Kind kind;
    std::string typeParam;
    std::shared_ptr<Type> constraint;

    GenericConstraint(Kind k, const std::string& param, std::shared_ptr<Type> c)
        : kind(k), typeParam(param), constraint(c) {}
};

} // namespace stratos
