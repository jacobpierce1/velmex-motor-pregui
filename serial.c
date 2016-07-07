#include <stdio.h>
#include  <libserialport.h>

int xyrange[2] = {0, 5000};
int coords[2] = {0,0};


/* main */
int main(int argc, char **argv) {
  getxy();
  printf("%d %s %d %s", coords[0], ",", coords[1], "\n");
  return 0;
}


/* write x,y into coords array; make sure user picks something in correct range */
void getxy()
{
  char xy[2] = {'x', 'y'};
  int temp = 0;
  
  int i;
  for(i=0; i<=1; i++) {
    printf("%c %s", xy[i], "coord: ");
    while(1) {
      printf("");
      scanf("%d", &temp);
      if (temp >= xyrange[0] && temp  <= xyrange[1]) {
	coords[i] = temp;
	break;
      }
      printf("%s %d %s %d %s", "pick between ",  xyrange[0], ",", xyrange[1], ": ");
    }
  }
}




