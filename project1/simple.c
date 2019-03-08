
#include <stdio.h>
#include <math.h>


int main() {

int num[] = {4, 4, 16, 25, 25, 49, 36, 64, 81, 100};
int i = 0;
double result;
double sum;
for (i; i <= 10; i++){
	int x = num[i];
	result = sqrt(x);
	sum = sum +  result;
	

}

double average;
average  = sum / 10;

printf("Average is equal to: %lf \n", average);

return 0;
} 

