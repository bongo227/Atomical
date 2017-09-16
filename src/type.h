#define PRINT_OP(type)                                                              \
friend std::ostream& operator<<(std::ostream& os, const type& obj) {                \
    obj.print_node(os);                                                             \
    return os;                                                                      \
}

#define EQUAL_OP(type)                                                              \
friend bool operator==(const type& lhs, const type& rhs) {                          \
    return (typeid(lhs) == typeid(rhs)) && lhs.is_equal(rhs);                       \
}                                                                                   \

#define NOT_EQUAL_OP(type)                                                          \
friend bool operator!=(const type& lhs, const type& rhs) {                          \
    return (typeid(lhs) != typeid(rhs)) || !lhs.is_equal(rhs);                      \
}                                                                                   \

enum class Primitive {
    I8,
    I16,
    I32,
    I64,
    INT,
    F32,
    F64,
    FLOAT,
    BOOL,
};

class Type {
    virtual bool is_equal(const Type &type) const;

    virtual void print_node(std::ostream& os) const;

    EQUAL_OP(Type);
    NOT_EQUAL_OP(Type);
    PRINT_OP(Type);
};

class PrimitiveType : public Type {
    Primitive prim;

public:
    explicit PrimitiveType(Primitive prim);
    explicit PrimitiveType(TokenType type);

    virtual bool is_equal(const Type& type) const override;
    virtual void print_node(std::ostream& os) const override;
};