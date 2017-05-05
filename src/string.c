#include "includes/string.h"

#define STRING_HEADER(s) ((string_header *)s - 1)

string string_new(const char *str) {
    int length = str ? strlen(str) : 0;
    return string_new_length(str, length);
}

string string_new_length(const char *str, int len) {
    // Create header + data
    void *data = malloc(sizeof(string_header) + len + 1);
    if (data == NULL) return NULL;
    
    string s = (char *)data + sizeof(string_header);
    
    // Set the header data
    string_header *header = STRING_HEADER(s);
    header->length = len;
    header->capacity = len;

    // Set the string data
    memcpy(s, str, len);
    s[len] = '\0';

    return s;
}

string string_new_file(FILE *f) {
    fseek (f, 0, SEEK_END);
	int file_length = ftell (f);
	fseek (f, 0, SEEK_SET);

    string s = string_new_length("", file_length);
    fread(s, 1, file_length, f);
    s[file_length] = '\0';
    STRING_HEADER(s)->length = file_length;

    return s;
}

void string_free(string s) {
    free(STRING_HEADER(s));
}

string string_copy(string s) {
    return string_new_length(s, string_length(s));
}

int string_length(string s) {
    return STRING_HEADER(s)->length;
}

int string_capacity(string s) {
    return STRING_HEADER(s)->capacity;
}

int string_avalible(string s) {
    string_header *header = STRING_HEADER(s);
    return header->capacity - header->length;
}

string string_expand(string s, int capacity) {
    string_header *header = STRING_HEADER(s);
    if (header->capacity > capacity) return s;
    header = realloc(header, sizeof(string_header) + capacity);
    header->capacity = capacity;
    return (char *)(header + 1);
}

string string_clear(string s) {
    return string_slice(s, 0, 0);
}

string string_append(string s1, string s2) {
    return string_append_length(s1, s2, string_length(s2));
}

string string_append_length(string s1, char *s2, int length) {
    int current_length = string_length(s1);
    s1 = string_expand(s1, current_length + length + 1);
    memcpy(s1 + current_length, s2, length);
    s1[current_length + length] = '\0';
    STRING_HEADER(s1)->length = current_length + length;
    return s1;
}

string string_append_cstring(string s, char *str) {
    return string_append_length(s, str, strlen(str));
}

string string_slice(string s, int start, int end) {
    string_header *header = STRING_HEADER(s);
    assert(start >= 0);
    assert(end <= header->length);
    int length = end - start;
    if(start > 0) memmove(s, s + start, length);
    s[length] = '\0';
    header->length = length;
    return s;
}

bool string_equals(string s1, string s2) {
    int length = string_length(s1);
    if (length != string_length(s2)) {
        return false;
    }

    for (int i = 0; i < length; i++) {
        if(s1[i] != s2[i]) return false;
    }

    return true;
}