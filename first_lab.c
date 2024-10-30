#include <stdio.h>

int main()
{
	int i;
	char c, n, str[100];
	c = n = getchar();
	for (i = 0; i < 99 && c != '\n'; i++) {
		if (c >= 'a' && c <= 'z') {
			str[i] = c - 32;
		} else if (c >= 'A' && c <= 'Z') {
			str[i] = c + 32;
		} else {
			str[i] = c;
		}
		c = getchar();
	}
	str[i] = '\0';
	printf("%x\n", n);
	printf("%s\n", str);

	return 0;
}
