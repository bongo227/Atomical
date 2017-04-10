#pragma once

typedef struct {
    int length;
    int capacity;
} string_header;

typedef char *string;

string string_new(const char *str);
string string_new_length(const char *str, int len);
string string_new_file(FILE *f);

void string_free(string s);

string string_copy(string s);

int string_length(string s);
int string_capacity(string s);
int string_avalible(string s);

string string_expand(string s, int capacity);

string string_clear(string s);

string string_append(string s1, string s2);
string string_append_length(string s1, char *s2, int length);
string string_append_cstring(string s, char *str);

string string_slice(string s, int start, int end);

bool string_equals(string s1, string s2);