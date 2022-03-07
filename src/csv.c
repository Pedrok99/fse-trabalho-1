#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

void init_csv_file(){
  FILE *csv_pointer;

  csv_pointer = fopen("logs.csv", "w+");
  fseek(csv_pointer, 0L, SEEK_END);
  if(ftell(csv_pointer) != 0){
    return;
  }
  fprintf(csv_pointer,"Date-Time, Internal Temperature, Reference Temperature, External temperature, User Temperature, FAN, RESISTOR, PID\n");
  fclose(csv_pointer);
}

void write_on_csv(float ti, float tr, float te, float ut, int fan_value, int resistor_value, float control_signal){
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  FILE *csv_pointer;
  csv_pointer = fopen("logs.csv", "a");

  fprintf(csv_pointer, "%d-%02d-%02d %02d:%02d:%02d, ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  fprintf(csv_pointer, "%.2f, %.2f, %.2f, %.2f, %d, %d, %.2f\n", ti, tr, te, ut, fan_value, resistor_value, control_signal);
  fclose(csv_pointer);
}

void read_reflow_csv(int *times, int *temperatures, int size){

  char line[200];
  FILE *csv_pointer;
  int time, temp;

  csv_pointer = fopen("curva_reflow.csv", "r");
  
  if(csv_pointer == NULL) printf("Csv não aberto\n");
  fgets(line, 200, csv_pointer); // queimar a primeira linha do csv

  for (int i = 0; i < size; i++)
  {
    if(fscanf(csv_pointer, "%d, %d", &time, &temp) == -1) return;
    times[i] = time;
    temperatures[i] = temp;
  }

}

// int main(){
//   int times[10], temps[10];
//   read_reflow_csv(times, temps, 10);
//   for (int i = 0; i < 10; i++)
//   {
//     printf("%d - %d\n", times[i], temps[i]);
//   }
  
//   return 0;
// }