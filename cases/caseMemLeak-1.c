#include <stdio.h>
#include <stdlib.h>
const char *env = "DVROOT";
int main()
{
	char *p;
	p = malloc(100);

	p[1] = 36;
	
	const char* s = getenv(env);
	printf("%s : %s\n",env, (s!=NULL)? s : "getenv returned NULL");
	p[101] = 78;
}
