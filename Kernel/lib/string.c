#include <string.h>
#include <stdlib.h>

int strlen(char * string){
	if(string == NULL)
		return -1;
	int i = 0;
	while(string[i] != '\0')
		i++;
	return i;
}
