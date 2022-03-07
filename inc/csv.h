#ifndef CSV_H_
#define CSV_H_

void init_csv_file();
void write_on_csv(float ti, float tr, float te, float ut, int fan_value, int resistor_value, float control_signal);
void read_reflow_csv(int *times, int *temperatures, int size);

#endif