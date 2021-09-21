/**********
  * @file    main.c
  * @author  G. Garcia & A. Riedinger.
  * @version 0.1
  * @date    20.09.21
  * @brief   Se genera una señal sinusoidal de 750 Hz y 2500 Hz a través del DAC
  * 		 y DMA alternada cada un intervalo arbitrario de 10 segundos.
  *
  * SALIDAS:
  	  *	LCD
  	  *	DAC CHN2 PA5
  *
  * ENTRADAS:
  	  * void
**********/

/*------------------------------------------------------------------------------
LIBRERIAS:
------------------------------------------------------------------------------*/
#include "mi_libreria.h"

/*------------------------------------------------------------------------------
DEFINICIONES:
------------------------------------------------------------------------------*/
//Pines del DAC:
#define DAC_Port GPIOA
#define DAC_Pin	 GPIO_Pin_5

//Tiempo de interrupcion por Systick - 50mseg:
#define TimeINT_Systick 0.05

//Cuentas que tarda el Systick en llegar a 10 segundos:
#define TenSeconds	200

//Frecuencias de las senales a generar en Hz:
#define Signal1Freq	750
#define Signal2Freq	2500

//Parámetros de configuración del TIM3 para refresco del LCD:
#define Freq 	 4		//Equivalente a 250mseg
#define TimeBase 200e3
/*------------------------------------------------------------------------------
DECLARACION DE FUNCIONES LOCALES:
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
VARIABLES GLOBALES:
------------------------------------------------------------------------------*/
//Definicion de los pines del LCD:
LCD_2X16_t LCD_2X16[] = {
			// Name  , PORT ,   PIN      ,         CLOCK       ,   Init
			{ TLCD_RS, GPIOC, GPIO_Pin_10, RCC_AHB1Periph_GPIOC, Bit_RESET },
			{ TLCD_E,  GPIOC, GPIO_Pin_11, RCC_AHB1Periph_GPIOC, Bit_RESET },
			{ TLCD_D4, GPIOC, GPIO_Pin_12, RCC_AHB1Periph_GPIOC, Bit_RESET },
			{ TLCD_D5, GPIOD, GPIO_Pin_2,  RCC_AHB1Periph_GPIOD, Bit_RESET },
			{ TLCD_D6, GPIOF, GPIO_Pin_6,  RCC_AHB1Periph_GPIOF, Bit_RESET },
			{ TLCD_D7, GPIOF, GPIO_Pin_7,  RCC_AHB1Periph_GPIOF, Bit_RESET }, };

//Variable para contar el tiempo:
uint32_t TimeIND = 0;

//Varibale para mostrar en el Display la señal que está siendo generada:
uint32_t Signal = 0;
int main(void)
{
	//Valores que conforma la senal sinusoidal a representar:
	const uint16_t SineWave32Bits[Res32Bit] = {
	                      	  	  	  2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
									  3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
									  599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};

/*------------------------------------------------------------------------------
CONFIGURACION DEL MICRO:
------------------------------------------------------------------------------*/
	SystemInit();

	//Inicializacion del DISPLAY LCD:
	INIT_LCD_2x16(LCD_2X16);

	//Inicilizacion del DAC para generar una onda senoidal:
	INIT_DAC_SINE(DAC_Port, DAC_Pin);

	//Inicializacion de interrupcion por tiempo cada 50 mseg:
	INIT_SYSTICK(TimeINT_Systick);

	//Inicialización del TIM3 para refresco del LCD:
	INIT_TIM3();
	SET_TIM3(TimeBase, Freq);

/*------------------------------------------------------------------------------
BUCLE PRINCIPAL:
------------------------------------------------------------------------------*/
    while(1)
    {
    	/*Si pasaron 10 segundos: */
    	if(TimeIND == TenSeconds)
    	{
    		/*Se resetea la variable del Systick: */
    		TimeIND = 0;

    		/*Se prende la primer senal de 750 Hz en la primer pasada:*/
    		if(Signal == 0)
    		{
    			DAC_SINE32BIT(DAC_Port, DAC_Pin, SineWave32Bits, Signal1Freq);
    			Signal = 1;
    		}
    		/*Se prende la segunda senal de 2500 Hz en la segunda pasada y luego se alternan:*/
			else
			{
				DAC_SINE32BIT(DAC_Port, DAC_Pin, SineWave32Bits, Signal2Freq);
				Signal = 0;
			}
    	}
    }

}

/*------------------------------------------------------------------------------
INTERRUPCIONES:
------------------------------------------------------------------------------*/
/*Interrupcion por tiempo - Systick cada 50mseg:*/
void SysTick_Handler()
{
	/*Se actualiza la base de tiempo: */
	TimeIND++;
}

/*Interrupción por agotamiento de cuenta del TIM3 cada 250mseg (4 Hz): */
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

		/*Refresco del LCD: */
		CLEAR_LCD_2x16(LCD_2X16);

		/*Mostrar mensaje generico: */
		PRINT_LCD_2x16(LCD_2X16, 0, 0, "TDII  -  E02 TP5");

		/*Mostrar mensaje señal 1 de 750 Hz */
		if(Signal == 0)
			PRINT_LCD_2x16(LCD_2X16, 0, 1, "SENAL DE 750 Hz");
		else
			PRINT_LCD_2x16(LCD_2X16, 0, 1, "SENAL DE 2.5 kH");
	}
}
