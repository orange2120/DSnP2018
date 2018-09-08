#include <iostream>
#include <cstdio>

int main()
{
	char key[20];
	int val = 0;
	char val_str[10];
	const char *data = "    \"Mary\" : 70";
	printf("%s\n", data);

	//printf("\"%s\"\n",data);

	sscanf(data, " \"%[^\"]s\"", key);
	sscanf(data, "%*s%*s%[^,]s", val_str);

	val = atoi(val_str);
	printf("key=%s,val=%d\n", key, val);
	//printf("%s\n", val_str);
}
