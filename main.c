/*
 * main.c
 *
 *  Created on: May 1, 2015
 *      Author: Juan Pablo VEGA - Laboratorio de Microprocesadores
 */

#include "LDM.h"
#include "../App.h"

int main (void)
{
	__LDM_Init();
	
	__LDM_DisableInterrupts();
	
	/**************************/
	/* Program-specific setup */
	/**************************/
	App_Init();               //
	/**************************/
	
	__LDM_EnableInterrupts();
	
	/**************************/
	/* Program-specific loop  */
	/**************************/
	__FOREVER__				  //
		App_Run();   		  //
	/**************************/
}
