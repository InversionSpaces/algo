#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma GCC optimize(2)
#pragma GCC optimize(3)
#pragma GCC optimize("Ofast")
#pragma GCC optimize("inline")
#pragma GCC optimize("-fgcse")
#pragma GCC optimize("-fgcse-lm")
#pragma GCC optimize("-fipa-sra")
#pragma GCC optimize("no-stack-protector")
#pragma GCC optimize("unroll-loops")

typedef struct {
    char* str;
    int len;
} str;

int str_cmp(const void* lhs, const void* rhs) {
    const str* lstr = (str*)lhs;
    const str* rstr = (str*)rhs;

    const int len = lstr->len < rstr->len ? lstr->len : rstr->len;
    const int res = strncmp(lstr->str, rstr->str, len);

    if (res) return res;
    if (lstr->len == rstr->len) return 0;
    return lstr->len < rstr->len ? -1 : 1;
}

#define MAX_SIZE (1000000)

str strings[MAX_SIZE];
int dots[MAX_SIZE];
int arr_size = 0;

void parse(char* string) {
    while (*string) {
        int dots_count = strspn(string, ".");
        dots[arr_size] = dots_count;
        string += dots_count;

        int chars_count = strcspn(string, ".");
        strings[arr_size].str = string;
        strings[arr_size].len = chars_count;
        string += chars_count;

        ++arr_size;
    }
}

int main() {
    std::string _string;
    std::getline(std::cin, _string);

    parse(const_cast<char*>(_string.c_str()));

    qsort(strings, arr_size, sizeof(str), str_cmp);

    const size_t len = _string.length();
    char* output = (char*)malloc(len + 1);
    if (output == NULL)
        return 1;

    char* result = output;

    const int skip = strings[0].len == 0;

    for (int i = 0; i < arr_size; ++i) {
        const int dots_count = dots[i];
        if (dots_count > 0)
            memset(output, '.', dots_count);
        output += dots_count;

        const int len = strings[i + skip].len;
        if (len > 0)
            strncpy(output, strings[i + skip].str, len);
        output += len;
    }

    result[len] = 0;
    puts(result);
}
