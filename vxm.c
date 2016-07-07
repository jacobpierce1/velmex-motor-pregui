#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libserialport.h>
#include <stddef.h>
#include <unistd.h>

void setup();
void move_motor(char *vxm_command);
void getports();
void checkerror(int error);
void make_command(int mode);
void getxy(int mode);
int getmode();
void recalibrate();
void convert_to_steps();
void hysteresis();

/* CUSTOMIZEABLE PRESETS */
int xyrange[2] = {-100000, 100000};
int start_speed = 2500;

/* GLOBAL VARIABLES, DO NOT TOUCH */
int coords[2] = {0,0};
int scroll = 0;
char vxm_command[200];

/* CONSTANTS */
const float d_per_step = .0002500;  /* inches */
const float in_per_cm = .3937;


/* -----------------------------------------------------------
--------------------------------------------------------------
-------------------------------------------------------------- */
int main(int argc, char **argv)
{
  setup();
  while(1)
  {
    int mode = getmode();
    if(mode == -1) recalibrate();
    else if(mode == 9) {convert_to_steps(); continue;}
    make_command(mode);
    move_motor(vxm_command);
    
    int temp;
    printf("enter number to continue ");
    scanf("%d", &temp);

    hysteresis();
  }
  return 0;
}

void setup() {
  strcpy(vxm_command, "E,C");
  
  char start_speed_str[6];
  sprintf(start_speed_str, "%d", start_speed);
  strcat(vxm_command, ",S1M");
  strcat(vxm_command, start_speed_str);
  strcat(vxm_command, ",S2M");
  strcat(vxm_command, start_speed_str);
  strcat(vxm_command, ",R");

  move_motor(vxm_command);
}
	

/* Construct the command, depending on the mode */
void make_command(int mode) {
  char x_move[15];
  char y_move[15];
  char x_coord_str[5];
  char y_coord_str[5]; 
  int velocity = 0;
  char velocity_str[5];
  int sweep_dist = 0;
  char sweep_dist_str[5];

  switch(mode) {
  case 1:
    puts("abs move mode. enter (x,y). type -1 to quit."); 
    strcpy(x_move, ", IA1M");
    strcpy(y_move, ", IA2M");
    break;
  case 2:
    puts("rel move mode. enter (x,y). type -1 to quit."); 
    strcpy(x_move, ", I1M");
    strcpy(y_move, ", I2M");
    break;
  case 3:
    puts("scrollx move mode. enter fixed y and scroll direction (+ or - ==> 1,2)");
    printf("scroll direction: ");
    scanf("%d", &scroll);
    strcpy(y_move, ", IA2M");
    break;
  case 4:
    puts("scrolly move mode. enter fixed x and scroll direction (+ or - ==> 1,2)");
    printf("scroll direction: ");
    scanf("%d", &scroll);
    strcpy(x_move, ", IA1M");
    break;
  case 5:
    puts("custom command mode. enter a command (no spaces).");
    break;
  case 6:
  case 7:
    printf("sweep mode. enter sweep velocity and distance.\n");
    printf("sweep velocity: ");
    scanf("%d", &velocity);
    printf("sweep distance: ");
    scanf("%d", &sweep_dist);
    break;
  case 8:
    break;
  default: puts("ERROR");
  }
  
  if(mode < 5) getxy(mode);

  strcpy(vxm_command, "");
  strcpy(vxm_command, "E,C");

  if (mode==1 || mode==2) {
    sprintf(x_coord_str, "%d", coords[0]);
    sprintf(y_coord_str, "%d", coords[1]);
    
    strcat(x_move, x_coord_str);  
    strcat(y_move, y_coord_str);
    
    strcat(vxm_command, x_move);
    strcat(vxm_command, y_move);
  }
  if (mode==3) {
    sprintf(y_coord_str, "%d", coords[1]);
    strcat(y_move, y_coord_str);
    strcat(vxm_command, y_move);
    strcat(vxm_command, ", ");
    
    strcat(vxm_command, "IA1M");
    if(scroll==1) strcat(vxm_command, "-10000, P30, IA1M10000");
    else strcat(vxm_command, "10000, P20, IA1M-10000");
  }
  if (mode==4) {
    sprintf(x_coord_str, "%d", coords[1]);
    strcat(x_move, x_coord_str);
    strcat(vxm_command, x_move);
    strcat(vxm_command, ", ");
    
    strcat(vxm_command, "IA2M");
    if(scroll==1) strcat(vxm_command, "-10000, P30, IA2M10000");
    else strcat(vxm_command, "10000, P20, IA2M-10000");
  }
  if (mode==5) {
    printf("Enter a command: ");
    char *temp = calloc(30, sizeof(char));
    scanf("%s", temp);
    strcat(vxm_command, ", ");
    strcat(vxm_command, temp);
    free(temp);
  }
  if (mode==6 || mode==7) {
    sprintf(velocity_str, "%d", velocity);
    sprintf(sweep_dist_str, "%d", sweep_dist);
    strcat(vxm_command, ",S1M");
    strcat(vxm_command, velocity_str);
    strcat(vxm_command, ",S2M");
    strcat(vxm_command, velocity_str);
    
    int a;
    if(mode ==6) {
      for(a=1;a*sweep_dist<=3500;a++) {
	strcat(vxm_command, ",I1M0,I2M");
	strcat(vxm_command, sweep_dist_str);
	strcat(vxm_command, ",I1M-0,I2M");
	strcat(vxm_command, sweep_dist_str);
      }
      strcat(vxm_command, ",I1M0,I2M");
      strcat(vxm_command, sweep_dist_str);
      strcat(vxm_command, ",I1M-0");
    }

    if (mode==7) {
      for(a=1;a*sweep_dist<=3500;a++) {
	strcat(vxm_command, ",I2M0,I1M");
	strcat(vxm_command, sweep_dist_str);
	strcat(vxm_command, ",I2M-0,I1M");
	strcat(vxm_command, sweep_dist_str);
      }
      strcat(vxm_command, ",I2M0,I1M");
      strcat(vxm_command, sweep_dist_str);
      strcat(vxm_command, ",I2M-0");
    }
  }
  if (mode==8) strcat(vxm_command, ",I1M-0,I2M-0");

  
  strcat(vxm_command, ", R");
  printf("command entered: %s \n\n", vxm_command);
  return;
}


/* make user select a mode */
int getmode()
{
  int temp = 0;
  printf("select mode: abs (1), rel (2), scrollx (3), scrolly (4), custom (5), sweepx (6), sweepy(7), go to 0,0 (8), step conversion (9), quit (-1): ");
  
  while (1) {
    scanf("%d", &temp);
    if(temp > 9 || temp == 0 || temp < -1) printf("Not an option. ");
    else return temp;
  }
}


/* move the motor according to command */
void move_motor(char *vxm_command)
{
  struct sp_port **ports;

  sp_list_ports(&ports); 
  sp_open(ports[0], SP_MODE_WRITE);
 
  sp_blocking_write(ports[0], vxm_command, strlen(vxm_command)+1, 500);
  sp_drain(ports[0]);

  sp_close(ports[0]);
  sp_free_port_list(ports);
  return;
} 


/* write x,y into coords array; make sure user picks something in correct range */
void getxy(int mode)
{
  char xy[2] = {'x', 'y'};
  int temp = 0;
  
  int i;
  for(i=0; i<=1; i++) {
    if(mode==3 && i==0) continue;
    if(mode==4 && i==1) continue;
    printf("%c %s", xy[i], "coord: ");
    while(1) {
      scanf("%d", &temp);
      if (temp == -1) recalibrate();
      else if (temp >= xyrange[0] && temp  <= xyrange[1]) {
	coords[i] = temp;
	break;
      }
      printf("%s %d %s %d %s", "pick between ",  xyrange[0], ",", xyrange[1], ": ");
    }
  }
}


/* recalibrate. when turned off and back on, it thinks it is at 0,0 */
void recalibrate() {
  move_motor("E,C,I1M-0,I2M-0,R");
  exit(0);
}

/* cm, cm/s to step converter */
void convert_to_steps() {
  float dist;
  float velocity;

  printf("distance (cm): ");
  scanf("%f", &dist);
  printf("velocity (cm/s): ");
  scanf("%f", &velocity);

  printf("num steps: %f \n", dist * in_per_cm / d_per_step);
  printf("velocity (steps/s): %f \n\n", velocity * in_per_cm / d_per_step);
}

void hysteresis() {
  int i;
  for (i=0;i<100;i++) {
    move_motor("E,C,I1M2500,I1M-2500,R");
    sleep(2.1);
  }
}


/* -----------------------------------------
   PORT INFORMATION AND DEBUGGING
   ----------------------------------------- */

/* view available ports */
void getports()
{   
  int i;
  struct sp_port **ports;

  sp_list_ports(&ports);

  for (i = 0; ports[i] != NULL; i++) {
    printf("Found port[%d]: '%s'.\n", i, sp_get_port_name(ports[i]));
    printf("Description:'%s' .\n", sp_get_port_description(ports[i]));
  }
  
  sp_free_port_list(ports);
  return;
}


/* for debugging, e.g. checkerror(sp_blocking_write( --- ) */
void checkerror(int error) {
  if(error == SP_OK) printf("no error \n");
  else if(error == SP_ERR_ARG) printf("invalid arugment\n");
  else if(error == SP_ERR_FAIL) printf("system error\n");
  else if(error == SP_ERR_MEM) printf("mem allocation failure\n");
  else if(error == SP_ERR_SUPP) printf("requested operation not supported\n");
  else printf("unknown error\n");
  return;
}
