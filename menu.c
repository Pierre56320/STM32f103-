/*
 * menu.c
 *
 *  Created on: 16 janv. 2019
 *      Author: Victorien
 */

#include "config.h"
#include "menu.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"
#include "stm32f1_sys.h"
#include "stm32f1_ili9341.h"
#include "stm32f1_xpt2046.h"
#include <math.h>
#include <stdbool.h>
#include <tron.h>
//#include "buzzer.h"

#define hauteur 240
#define  longueur 320
#define  marge 10
#define  largeur_case 65

uint16_t widthStr;
uint16_t heightStr;


//Fonction qui gère le tactile pour le menu snake
void check_touch_tron(void){
	while(1){
				int16_t x, y;
				if(XPT2046_getAverageCoordinates(&x, &y, 10, XPT2046_COORDINATE_SCREEN_RELATIVE)){
					//ILI9341_DrawCircle(x,y,1,ILI9341_COLOR_BLUE);

					//Sélection du mode SOLO
					if(x>marge && x<longueur && y>marge && y<marge+largeur_case){
						tron_start(false);
						break;
					}

					//Sélection du mode MULTIJOUEUR
					if(x>marge && x<(longueur-marge) && y>((marge*2)+largeur_case) && y<((marge*3)+largeur_case*2)){
						tron_start(true);
						break;
					}

					//Retour
					if(x>marge && x<(longueur-marge) && y>((marge*2)+largeur_case) && y<((marge+largeur_case)*3)){
						menu_start(false);
						break;
					}
				}
	}
}

//Affichage du menu du snake
void menu_tron(void){

	//Icône du jeu SNAKE
	ILI9341_GetStringSize("SOLO",&Font_11x18,&widthStr ,&heightStr);
	ILI9341_Puts((longueur/2)-(widthStr/2),marge+(largeur_case/3)+3, "SOLO  ", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

	//Icône du jeu BANG
	ILI9341_GetStringSize("MULTIJOUEUR",&Font_11x18,&widthStr ,&heightStr);
	ILI9341_Puts((longueur/2)-(widthStr/2),(marge*2)+(largeur_case/3)+3+largeur_case, "MULTIJOUEUR", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

	//Icône des crédits
	ILI9341_GetStringSize("RETOUR",&Font_11x18,&widthStr ,&heightStr);
	ILI9341_Puts((longueur/2)-(widthStr/2),(marge*3)+(largeur_case/3)+3+largeur_case*2, "RETOUR", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

	check_touch_tron();
}


void check_touch_start(void){
	while(1){
		#if USE_SCREEN_TFT_ILI9341
			#if USE_XPT2046
				int16_t x, y;

				if(XPT2046_getAverageCoordinates(&x, &y, 10, XPT2046_COORDINATE_SCREEN_RELATIVE))
				{
					//ILI9341_DrawCircle(static_x,static_y,15,ILI9341_COLOR_WHITE);
					ILI9341_DrawCircle(x,y,1,ILI9341_COLOR_BLUE);


					//Sélection du jeu TRON
					if(x>marge && x<longueur-marge && y>marge && y<marge+largeur_case){
						//ILI9341_DrawFilledRectangle(marge, marge, longueur-marge, marge+largeur_case, ILI9341_COLOR_RED);
						ILI9341_Puts((longueur/2)-20,marge+(largeur_case/3)+3, "TRON", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
						menu_tron();
						break;
					}

					//Sélection du jeu BANG
					if(x>marge && x<longueur-marge && y>(marge*2)+largeur_case && y<(marge*2)+largeur_case){
						//ILI9341_DrawFilledRectangle(marge, marge, longueur-marge, marge+largeur_case, ILI9341_COLOR_RED);
						menu_start(false);
						break;
					}

					//Sélection des crédits
					if(x>marge && x<longueur-marge && y>(marge*3)+largeur_case*2 && y<(marge+largeur_case)*3){
						//ILI9341_DrawFilledRectangle(marge, marge, longueur-marge, marge+largeur_case, ILI9341_COLOR_RED);
						//buzzer_fever();
						break;
					}


				}

			#endif
		#endif
	}
}

void menu_start(bool firstStart){
	if(firstStart){
		ILI9341_Fill(ILI9341_COLOR_BLACK); //si l'on ne se trouve pas déjà sur un menu on affiche tout, sinon que le nécessaire
	}


	//Icône du jeu TRON
	ILI9341_GetStringSize("TRON",&Font_11x18,&widthStr ,&heightStr);
	if(firstStart){
	ILI9341_DrawFilledRectangle(marge, marge, longueur-marge, marge+largeur_case, ILI9341_COLOR_WHITE);
	}
	ILI9341_Puts((longueur/2)-(widthStr/2),marge+(largeur_case/3)+3, "TRON", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

	//Icône du jeu BANG
	ILI9341_GetStringSize("BANG",&Font_11x18,&widthStr ,&heightStr);
	if(firstStart){
	ILI9341_DrawFilledRectangle(marge, (marge*2)+largeur_case, longueur-marge, (marge+largeur_case)*2, ILI9341_COLOR_WHITE);
	}
	ILI9341_Puts((longueur/2)-(widthStr/2),(marge*2)+(largeur_case/3)+3+largeur_case, "BANG", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

	//Icône des crédits
	ILI9341_GetStringSize("CREDIT",&Font_11x18,&widthStr ,&heightStr);
	if(firstStart){
	ILI9341_DrawFilledRectangle(marge, (marge*3)+largeur_case*2, longueur-marge, (marge+largeur_case)*3, ILI9341_COLOR_WHITE);
	}
	ILI9341_Puts((longueur/2)-(widthStr/2),(marge*3)+(largeur_case/3)+3+largeur_case*2, "CREDIT", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

	//Guette le tactile
	check_touch_start();
}



