#include "cstring"
#include <cctype>

extern "C" {

void* memcpy(void* __restrict dest, const void* __restrict src, size_t n) {
    uint8_t* __restrict pdest = static_cast<uint8_t* __restrict>(dest);
    const uint8_t* __restrict psrc = static_cast<const uint8_t* __restrict>(src);

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void* memset(void* s, int c, size_t n) {
    uint8_t* p = static_cast<uint8_t* >(s);

    for (size_t i = 0; i < n; i++) {
        p[i] = static_cast<uint8_t>(c);
    }

    return s;
}

void* memmove(void* dest, const void* src, size_t n) {
    uint8_t* pdest = static_cast<uint8_t* >(dest);
    const uint8_t* psrc = static_cast<const uint8_t* >(src);

    if (reinterpret_cast<uintptr_t>(src) > reinterpret_cast<uintptr_t>(dest)) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (reinterpret_cast<uintptr_t>(src) < reinterpret_cast<uintptr_t>(dest)) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const uint8_t* p1 = static_cast<const uint8_t* >(s1);
    const uint8_t* p2 = static_cast<const uint8_t* >(s2);

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

char* strcpy(char* dest, const char* src) {
    char* ptr = dest;
    while ((*ptr++ = *src++));
    return dest;
}

char* strncpy(char* dest, const char* src, unsigned int n) {
    char* ptr = dest;
    unsigned int i = 0;
    while (i < n && src[i]) {
        ptr[i] = src[i];
        i++;
    }
    while (i < n) {
        ptr[i++] = '\0';
    }
    return dest;
}

unsigned int strlen(const char* str) {
    unsigned int len = 0;
    while (str[len]) len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char* s1, const char* s2, unsigned int n) {
    unsigned int i = 0;
    while (i < n && s1[i] && (s1[i] == s2[i])) {
        i++;
    }
    if (i == n) return 0;
    return (unsigned char)s1[i] - (unsigned char)s2[i];
}

char* strcat(char* dest, const char* src) {
    char* ptr = dest;
    while (*ptr) ptr++;
    while ((*ptr++ = *src++));
    return dest;
}

char* strncat(char* dest, const char* src, unsigned int n) {
    char* ptr = dest;
    unsigned int i = 0;
    while (*ptr) ptr++;
    while (i < n && src[i]) {
        *ptr++ = src[i++];
    }
    *ptr = '\0';
    return dest;
}

char* strchr(const char* str, int c) {
    while (*str) {
        if (*str == (char)c) return (char*)str;
        str++;
    }
    return nullptr;
}

char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;
    while (*haystack) {
        const char* h = haystack;
        const char* n = needle;
        while (*h && *n && (*h == *n)) {
            h++;
            n++;
        }
        if (!*n) return (char*)haystack;
        haystack++;
    }
    return nullptr;
}

char* strtok(char* str, const char* delim) {
    static char* next;
    if (str) next = str;
    if (!next) return nullptr;

    char* start = next;
    while (*start && strchr(delim, *start)) start++;
    if (!*start) {
        next = nullptr;
        return nullptr;
    }

    char* end = start;
    while (*end && !strchr(delim, *end)) end++;

    if (*end) {
        *end = '\0';
        next = end + 1;
    } else {
        next = nullptr;
    }

    return start;
}

char* strtok_r(char* str, const char* delim, char** saveptr) {
    char* token;

    if (str != nullptr)
        *saveptr = str;

    if (*saveptr == nullptr)
        return nullptr;

    char* s = *saveptr;
    while (*s && strchr(delim, *s))
        s++;

    if (*s == '\0') {
        *saveptr = nullptr;
        return nullptr;
    }

    token = s;

    while (*s && !strchr(delim, *s))
        s++;

    if (*s == '\0') {
        *saveptr = nullptr;
    } else {
        *s = '\0';
        *saveptr = s + 1;
    }

    return token;
}

uint64_t strtod(const char *nptr, char **endptr) {
    const char *s = nptr;
    uint64_t result = 0;
    int sign = 1;

    while (*s == ' ' || *s == '\t' || *s == '\n') s++;

    if (*s == '+') s++;
    else if (*s == '-') { sign = -1; s++; }

    while (isdigit((unsigned char)*s)) {
        result = result * 10 + (*s - '0');
        s++;
    }

    if (endptr) *endptr = (char*)s;
    return sign * result;
}

long strtol(const char *nptr, char **endptr, int base) {
    const char *s = nptr;
    long result = 0;
    int neg = 0;

    while (*s == ' ' || *s == '\t' || *s == '\n' ||
           *s == '\r' || *s == '\f' || *s == '\v') {
        s++;
    }

    if (*s == '-') {
        neg = 1;
        s++;
    } else if (*s == '+') {
        s++;
    }

    if (base == 0) {
        if (s[0] == '0') {
            if (s[1] == 'x' || s[1] == 'X') {
                base = 16;
                s += 2;
            } else {
                base = 8;
                s++;
            }
        } else {
            base = 10;
        }
    } else if (base == 16 && s[0] == '0' && (s[1]=='x' || s[1]=='X')) {
        s += 2;
    }

    const char *start = s;

    while (*s) {
        int digit;
        if (*s >= '0' && *s <= '9') digit = *s - '0';
        else if (*s >= 'a' && *s <= 'z') digit = *s - 'a' + 10;
        else if (*s >= 'A' && *s <= 'Z') digit = *s - 'A' + 10;
        else break;

        if (digit >= base) break;

        result = result * base + digit;
        s++;
    }

    if (s == start) {
        if (endptr) *endptr = (char*)nptr;
        return 0;
    }

    if (endptr) *endptr = (char*)s;

    return neg ? -result : result;
}

char* strrchr(const char* s, int c) {
	const char* last = 0;
	while (*s) {
		if (*s == (char)c) last = s;
		s++;
	}
	return (char*)last;
}

}
