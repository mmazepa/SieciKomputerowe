#include <stdio.h>

int main(int argc, char *argv[])
{
	system("clear");
	printf("\n");
	puts("   PODZIAŁ SIECI NA PODSIECI:");
	printf("\n");
	
	int i;
	for(i=0; i<argc; i++){
		if(i == 1)	printf("     %d. SUBNET NAME:        %s\n", i, argv[i]);
		if(i == 2)	printf("     %d. NEEDED SIZE:        %s\n", i, argv[i]);
		if(i == 3)	printf("     %d. ALLOCATED SIZE:     %s\n", i, argv[i]);
		if(i == 4)	printf("     %d. ADDRESS:            %s\n", i, argv[i]);
		if(i == 5)	printf("     %d. MASK:               %s\n", i, argv[i]);
		if(i == 6)	printf("     %d. DEC MASK:           %s\n", i, argv[i]);
		if(i == 7)	printf("     %d. ASSIGNABLE RANGE:   %s %s %s\n", i, argv[i], argv[i+1], argv[i+2]);
		if(i == 10) printf("     %d. BROADCAST:          %s\n", i-2, argv[i]);
	}
   
	printf("\n");
	return 0;
}
