#define STRING_HEADER(s) ((string_header *)s - 1)

TEST(StringTest, CreateNewString) {
    string s = string_new("test");
    ASSERT_EQ(STRING_HEADER(s)->length, 4);
    ASSERT_EQ(STRING_HEADER(s)->capacity, 4);
    ASSERT_EQ(0, strcmp("test", s));
    string_free(s);
}

TEST(StringTest, CreateNewStringLength) {
    string s = string_new_length("testtesttest", 4);
    ASSERT_EQ(STRING_HEADER(s)->length, 4);
    ASSERT_EQ(STRING_HEADER(s)->capacity, 4);
    ASSERT_EQ(0, strcmp("test", s));
    string_free(s);
}

TEST(StringTest, CreateNewStringFile) {
    FILE *f = fopen("/tmp/string_test_file.fur", "w");
    fprintf(f, "test");
    fclose(f);

    f = fopen("/tmp/string_test_file.fur", "r");
    string s = string_new_file(f);
    fclose(f);

    ASSERT_EQ(0, strcmp(s, "test"));
}

TEST(StringTest, CopyString) {
    string s = string_new("test");
    string copy = string_copy(s);
    ASSERT_NE(s, copy);
    ASSERT_EQ(0, strcmp(s, copy));
}

TEST(StringTest, StringLength) {
    string s = string_new("test");
    ASSERT_EQ(4, string_length(s));
}

TEST(StringTest, StringCapacity) { 
    string s = string_new("test");
    ASSERT_EQ(4, string_length(s));
}

TEST(StringTest, StringAvalible) {
    string s = string_new("test");
    ASSERT_EQ(0, string_avalible(s));
}

TEST(StringTest, StringExpand) {
    string s = string_new("test");
    s = string_expand(s, 10);
    ASSERT_EQ(10, string_capacity(s));
    ASSERT_EQ(6, string_avalible(s));
    ASSERT_EQ(4, string_length(s));
    ASSERT_EQ(0, strcmp(s, "test"));
}

TEST(StringTest, StringClear) {
    string s = string_new("test");
    s = string_clear(s);
    ASSERT_EQ(0, strcmp(s, ""));
}

TEST(StringTest, StringAppend){
    string s1 = string_new("hello ");
    string s2 = string_new("world");
    s1 = string_append(s1, s2);
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}

TEST(StringTest, StringAppendLength) {
    string s1 = string_new("hello ");
    string s2 = string_new("world of earth");
    s1 = string_append_length(s1, s2, 5);
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}

TEST(StringTest, StringAppendCString) {
    string s1 = string_new("hello");
    s1 = string_append_cstring(s1, (char *)" world");
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}

TEST(StringTest, StringMultipleAppendWithMalloc) {
    string s1 = string_new("hello");
    s1 = string_append_cstring(s1, (char *)" world");
    malloc(10);
    s1 = string_append_cstring(s1, (char *)" of");
    s1 = string_append_cstring(s1, (char *)" ours");

    ASSERT_EQ(0, strcmp(s1, "hello world of ours"));
}

TEST(StringTest, StringSliceEnd) {
    string s1 = string_new("hello world of ours");
    s1 = string_slice(s1, 0, 11);
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}

TEST(StringTest, StringSlice) {
    string s1 = string_new("hello world of ours");
    s1 = string_slice(s1, 6, 11);
    ASSERT_EQ(0, strcmp(s1, "world"));
}

TEST(StringTest, StringEquals) {
    string s1 = string_new("foo bar");
    string s2 = string_new("foo bar");
    ASSERT_EQ(true, string_equals(s1, s2));
}

TEST(StringTest, StringNotEqualsLength) {
    string s1 = string_new("foo bar");
    string s2 = string_new("foo bar bat");
    ASSERT_EQ(false, string_equals(s1, s2));
}

TEST(StringTest, StringNotEquals) {
    string s1 = string_new("foo bar");
    string s2 = string_new("foo bat");
    ASSERT_EQ(false, string_equals(s1, s2));
}