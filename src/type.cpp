bool Type::is_equal(const Type &type) const {
    return true;
}

void Type::print_node(std::ostream& os) const {
    os << "[BASE TYPE]";
}

std::ostream &operator<<(std::ostream& os, const Type& type) {
    type.print_node(os);
    return os;
}

PrimitiveType::PrimitiveType(Primitive prim) : prim(prim) {}

PrimitiveType::PrimitiveType(TokenType type) {
    switch(type) {
        case TokenType::INT:
        case TokenType::HEX:
        case TokenType::OCTAL: {
            prim = Primitive::INT;
            break;
        }
        
        case TokenType::FLOAT: 
            prim = Primitive::FLOAT; 
            break;
        
        case TokenType::BOOL_TRUE:
        case TokenType::BOOL_FALSE:
            prim = Primitive::BOOL;
            break;

        default:
            std::cout << "\"" << type << "\" is not a primitive type"; 
            assert(false);
    }
}

bool PrimitiveType::is_equal(const Type &type) const {
    auto t = static_cast<const PrimitiveType &>(type);
    return prim == t.prim;
}

void PrimitiveType::print_node(std::ostream& os) const {
    switch(prim) {
        case Primitive::I8: os << "i8"; break;
        case Primitive::I16: os << "i16"; break;
        case Primitive::I32: os << "i32"; break;
        case Primitive::I64: os << "i64"; break;
        case Primitive::INT: os << "int"; break;
        case Primitive::F32: os << "f32"; break;
        case Primitive::F64: os << "f64"; break;
        case Primitive::FLOAT: os << "float"; break;
        case Primitive::BOOL: os << "bool"; break;
    }
}