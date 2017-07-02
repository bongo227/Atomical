TEST(SsaTest, SsaEmptyFunctionDclNoArguments) {
    const char *src = "proc test :: -> int {}";
    parser *p = new_parser(Lex((char *)src));
    Dcl *dcl = parse_declaration(p);

    Ssa *ssa = new_ssa();
    Procedure *proc = ssa_function(ssa, dcl);
    
    ASSERT_EQ(proc->entryBlock->id, 0);
    ASSERT_EQ(proc->entryBlock->first, NULL);
    ASSERT_EQ(proc->entryBlock->last, NULL);
}

TEST(SsaTest, SsaEmptyFunctionDclOneArgument) {
    const char *src = "proc test :: int a -> int {}";
    parser *p = new_parser(Lex((char *)src));
    Dcl *dcl = parse_declaration(p);

    Ssa *ssa = new_ssa();
    Procedure *proc = ssa_function(ssa, dcl);
    
    ASSERT_EQ(proc->entryBlock->id, 0);
    ASSERT_TRUE(proc->entryBlock->first != NULL);
    ASSERT_TRUE(proc->entryBlock->last != NULL);
    ASSERT_EQ(proc->entryBlock->first, proc->entryBlock->last);
}

TEST(SsaTest, SsaEmptyFunctionDclManyArguments) {
    const char *src = "proc test :: int a, int b -> int {}";
    parser *p = new_parser(Lex((char *)src));
    Dcl *dcl = parse_declaration(p);

    Ssa *ssa = new_ssa();
    Procedure *proc = ssa_function(ssa, dcl);

    ASSERT_EQ(proc->entryBlock->id, 0);
    ASSERT_TRUE(proc->entryBlock->first != NULL);
    ASSERT_TRUE(proc->entryBlock->last != NULL);
    ASSERT_EQ(proc->entryBlock->first->next, proc->entryBlock->last);
    ASSERT_TRUE(proc->entryBlock->first != proc->entryBlock->last);
}