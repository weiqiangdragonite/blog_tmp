

#ifndef BSP_ADC_H
#define BSP_ADC_H



int init_adc(int resolution, int freq, int channel, int interrupt);
void start_adc(void);
int get_adc(int resolution);


#endif  // BSP_ADC_H
