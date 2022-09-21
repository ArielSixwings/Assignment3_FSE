#ifndef NVS_H
#define NVS_H

#define SAVE_RED 1
#define SAVE_GREEN 2
#define SAVE_BLUE 3
#define SAVE_INTENSITY 40

int32_t readNvsValue(char *option);
void writeNvsValue(int32_t value,char *option);

#endif