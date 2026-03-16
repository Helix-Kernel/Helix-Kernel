#include "cctype"

bool isspace(char c) {
    return c == ' '  ||
           c == '\t' ||
           c == '\n' ||
           c == '\v' ||
           c == '\f' ||
           c == '\r';
}

bool isdigit(char c) {
    return c >= '0' && c <= '9';
}

bool isalpha(char c) {
	if (c >= 'a' && c <= 'z') return true;
	else if (c >= 'A' && c <= 'Z') return true;
	else return false;
}

bool isalnum(char c) {
	return (isalpha(c) || isdigit(c));
}

char toupper(char c) {
	if (c >= 'a' && c <= 'z') return c + 0x20;
	else return c;
}

char tolower(char c) {
	return toupper(c) - 0x20;
}
