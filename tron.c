/*
 * snake.c
 *
 *  Created on: 12 janv. 2019
 *      Author: Pierre
 */
#include "config.h"

#include "stm32f1xx_hal.h"
#include "stm32f1_uart.h"
#include "stm32f1_ili9341.h"
#include <math.h>
#include <stdbool.h>
#include <tron.h>
#include "menu.h"

#define LARGEUR 5 //largeur du tron
#define TEMPS 4 //temps entre chaque pixel, influe sur la vitesse
#define START 0xFE
#define END 0xFD
#define TAB_SIZE 3 //nombre d'octet contenus dans chaque trame BT
#define MASTER 0 //Indique si le joueur est maitre ou esclave
#define FLAGSTART 0xFA //flags servant à l'initialisation et au lancement du jeu multi
#define FLAGBACK 0xF7
#define FLAGFINISH 0xF9
#define nbXTab 64 //nombre de cases
#define nbYTab 48



bool verifie_col();
void tronAvance (void);

typedef enum Direction
{
	HAUT,
	DROITE,
	BAS,
	GAUCHE
} Direction;

static Direction dirJ1;//direction des joueurs
static Direction dirJ2;

static uint8_t tab[TAB_SIZE]; //tableau contenant les données reçues
static uint8_t indexTab = 0;
static uint8_t looser = 0; //vaut 0 si il n'y a pas de perdant, 1 si le perd 1 gagne et 2 si le joueur 2 perd
static uint8_t on_tron_screen = 0; //permet d'indiquer si le programe s'execute sur cet écran
static uint8_t connect = 0;

uint16_t tron_tete_x; //coordonnées des joueurs
uint16_t tron_tete_y;
uint16_t tron_tete_x2;
uint16_t tron_tete_y2;
uint16_t coulJ1;
uint16_t coulJ2;
uint16_t score;

uint8_t grille[nbXTab][nbYTab] = {{0},{0}}; //Tableau contenant les cases occupées par le Tron, permet de gerer les collisions

bool multi = false;


/*void tronAvance(void)
 * routine d'execution du jeu
 * met à jour les coordonnées du joueur
 * Verifie si il n'y a pas de collision
 * lance l'affichage
 */
void tronAvance (void)
	{
	char str_coor[8];
	char tmpc[3];
	switch (dirJ1)
		{
		case HAUT:
			if(tron_tete_y<=0){ //On verifie si l'on fonce pas dans le mur. Additionnel à la fonction verifie_col()
				if(multi){
					UART_putc(UART2_ID,FLAGFINISH);
				}
				looser = 1;
				break;
			}
			tron_tete_y--;
			if(verifie_col()){
				break;
			}
			grille[tron_tete_x][tron_tete_y] = 1;
			if(multi){
				UART_putc(UART2_ID,START);
				UART_putc(UART2_ID,tron_tete_x);
				UART_putc(UART2_ID,tron_tete_y);
				UART_putc(UART2_ID,HAUT);
				UART_putc(UART2_ID,END);
			}

			print_case(tron_tete_x,tron_tete_y,HAUT,ILI9341_COLOR_RED);
			break;

		case BAS:
			tron_tete_y++;
			if(verifie_col()){
				break;
			}
			grille[tron_tete_x][tron_tete_y] = 1;
			if(multi){
				UART_putc(UART2_ID,START);
				UART_putc(UART2_ID,tron_tete_x);
				UART_putc(UART2_ID,tron_tete_y);
				UART_putc(UART2_ID,BAS);
				UART_putc(UART2_ID,END);
			}
			print_case(tron_tete_x,tron_tete_y,BAS,ILI9341_COLOR_RED);
			break;

		case GAUCHE:
			if(tron_tete_x<=0){
				if(multi){
					UART_putc(UART2_ID,FLAGFINISH);
				}
				looser = 1;
				break;
			}
			tron_tete_x--;
			if(verifie_col()){
				break;
			}
			grille[tron_tete_x][tron_tete_y] = 1;
			if(multi){
				UART_putc(UART2_ID,START);
				UART_putc(UART2_ID,tron_tete_x);
				UART_putc(UART2_ID,tron_tete_y);
				UART_putc(UART2_ID,GAUCHE);
				UART_putc(UART2_ID,END);
			}
			print_case(tron_tete_x,tron_tete_y,GAUCHE,ILI9341_COLOR_RED);
			break;

		case DROITE:
			tron_tete_x++;
			if(verifie_col()){
				break;
			}
			grille[tron_tete_x][tron_tete_y] = 1;
			if(multi){
				UART_putc(UART2_ID,START);
				UART_putc(UART2_ID,tron_tete_x);
				UART_putc(UART2_ID,tron_tete_y);
				UART_putc(UART2_ID,DROITE);
				UART_putc(UART2_ID,END);
			}
			print_case(tron_tete_x,tron_tete_y,DROITE,ILI9341_COLOR_RED);
			break;

		}
}


/*void tron_start(bool multijoueur)
 *param : bool multijoueur, permet de lancer le jeu en mode solo ou en mode multijoueur
 *initialise toutes les variables et contient la boucle principale d'execution du jeu
 *a la fin de la partie, affiche le score ou le gagnant
 */
void tron_start(bool multijoueur){
	on_tron_screen = 1;
	connect = 0; //on indique que la connexion bluetooth n'est pas établie
	for(int k=0; k<nbXTab;k++){    //on réinitialise le tableau à 0
		for(int j=0;j<nbYTab;j++){
			grille[k][j]=0;
		}
	}
	multi = multijoueur;
	ILI9341_Fill(ILI9341_COLOR_BLUE);
	if(multi){
		//Initialisation de l'UART2
		UART_init(UART2_ID,9600);
		if(MASTER){ //le maitre envoie en permanence des données en attente de l'esclave
			tron_tete_x = 1; //coordonnées des joueurs
			tron_tete_y = 1;
			dirJ1 = DROITE;
			coulJ1 = ILI9341_COLOR_RED;
			tron_tete_x2 = nbXTab-2;
			tron_tete_y2 = nbYTab-2;
			dirJ2 = GAUCHE;
			coulJ2 = ILI9341_COLOR_GREEN;
			print_case(tron_tete_x,tron_tete_y,dirJ1,coulJ1);
			grille[tron_tete_x][tron_tete_y] = 1;
			grille[tron_tete_x2][tron_tete_y2] = 1;
			while(1){ //on envoie un flag de début en attente d'une réponse
				UART_putc(UART2_ID,FLAGSTART);
				HAL_Delay(500);
				if(connect==1){ //si la connexion est établie, on quitte la boucle
					break;
				}

			}
		}
		else{ //l'esclave attend de recevoir une donnée du maitre avant de répondre
			tron_tete_x = nbXTab-2; //coordonnées des joueurs
			tron_tete_y = nbYTab-2;
			dirJ1 = GAUCHE;
			coulJ1 = ILI9341_COLOR_GREEN;
			tron_tete_x2 = 1;
			tron_tete_y2 = 1;
			dirJ2 = DROITE;
			coulJ2 = ILI9341_COLOR_RED;
			print_case(tron_tete_x,tron_tete_y,dirJ1,coulJ1);
			grille[tron_tete_x][tron_tete_y] = 1;
			grille[tron_tete_x2][tron_tete_y2] = 1;
			while(1){
				if(connect==1){ //si la connexion est établie, on quitte la boucle
					break;
				}

			}
		}
	}
	else{
		tron_tete_x = 1; //cas du mode solo, un seul joueur
		tron_tete_y = 1;
		coulJ1 = ILI9341_COLOR_RED;
		dirJ1 = DROITE;
		score=0;
		print_case(tron_tete_x,tron_tete_y,dirJ1,coulJ1);
		grille[tron_tete_x][tron_tete_y] = 1;
	}
	HAL_Delay(1000); //delai permettant de stabiliser la connexion bt
	looser = 0;
	while(1){
		tronAvance(); //routine d'execution du jeu tant qu'il n'y a pas de perdant
		if(looser != 0){
			break;
		}
	}
	if(multi){ //si il y a un perdant en mode multijoueur, on affiche le résultat
		if(looser == 1){
			ILI9341_Puts(100,100, "PERDU", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE);
		}
		else{
			ILI9341_Puts(100,100, "GAGNE", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE);
		}
	}
	else{ //en cas du mode solo, on calcule le score
		for(int k=0; k<nbXTab;k++){
				for(int j=0;j<nbYTab;j++){
					if(grille[k][j]==1){
						score++;
					}
				}
		}
		char buffer[5];
		itoa(score,buffer,10);
		ILI9341_Puts(100,100, "Score = " , &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE);
		ILI9341_Puts(180,100, buffer , &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE);
	}

	HAL_Delay(3000);
	connect = 0;
	on_tron_screen = 0;
	if(multi){
		UART_DeInit(UART2_ID);
	}

	menu_start(true); //on renvoie au menu
}


/*void verifie_col()
 *vérifie si la tete arrive sur une case déjà occupée ou hors limites
 */
bool verifie_col(){
	if(grille[tron_tete_x][tron_tete_y]||tron_tete_x<0||tron_tete_x>(nbXTab-1)||tron_tete_y<0||tron_tete_y>(nbYTab-1)){ // si il y a collision
		if(multi){
			UART_putc(UART2_ID,FLAGFINISH); //on indique a l'autre joueur que la partie est finie
		}
		looser = 1;
		return true;
	}
	return false;
}

/*void print_case()
 *affiche une case sur la grille en reproduisant un effet "fluide"
 *optimisée de manière a gérer l'affichage en fonction de la direction du tron
 */
void print_case(uint8_t x, uint8_t y, Direction direct, uint16_t coul){
	uint8_t memDirJ1 = dirJ1; //on enregistre les paramètres des joueurs, indispensables en cas d'interruption et de changement de direction
	uint8_t memDirJ2 = dirJ2;
	uint8_t memXJ2 = tron_tete_x2;//on enregistre les coordonnées du joueur 2, en cas de nouvelle reception entre temps
	uint8_t memYJ2 = tron_tete_y2;
	for (int k = 0; k < LARGEUR ; k++)
		{
			for (int j = 0; j < LARGEUR ; j++)
				{
					switch (memDirJ1){
						case DROITE:
							ILI9341_DrawPixel((tron_tete_x*LARGEUR)+k, (tron_tete_y*LARGEUR)+j, coulJ1);
							break;
						case BAS:
							ILI9341_DrawPixel((tron_tete_x*LARGEUR)+j, (tron_tete_y*LARGEUR)+k, coulJ1);
							break;
						case GAUCHE:
							ILI9341_DrawPixel((tron_tete_x*LARGEUR)+(LARGEUR-(k+1)), (tron_tete_y*LARGEUR)+j, coulJ1);
							break;
						case HAUT:
							ILI9341_DrawPixel((tron_tete_x*LARGEUR)+j, (tron_tete_y*LARGEUR)+(LARGEUR-(k+1)), coulJ1);
							break;
						default:
							ILI9341_DrawPixel((tron_tete_x*LARGEUR)+k, (tron_tete_y*LARGEUR)+j, coulJ1);
							break;
					}
					if(multi){ //on affiche les deux joueurs en même temps, pixels par pixel, evite de sacader
						switch (memDirJ2){
							case DROITE:
								ILI9341_DrawPixel((memXJ2*LARGEUR)+k, (memYJ2*LARGEUR)+j, coulJ2);
								break;
							case BAS:
								ILI9341_DrawPixel((memXJ2*LARGEUR)+j, (memYJ2*LARGEUR)+k, coulJ2);
								break;
							case GAUCHE:
								ILI9341_DrawPixel((memXJ2*LARGEUR)+(LARGEUR-(k+1)), (memYJ2*LARGEUR)+j, coulJ2);
								break;
							case HAUT:
								ILI9341_DrawPixel((memXJ2*LARGEUR)+j, (memYJ2*LARGEUR)+(LARGEUR-(k+1)), coulJ2);
								break;
							default:
								ILI9341_DrawPixel((memXJ2*LARGEUR)+k, (memYJ2*LARGEUR)+j, coulJ2);
								break;
						}
					}
					HAL_Delay(TEMPS);
				}
		}


}


/*void set_direction()
 *permet de changer la direction du joueur. On l'appelle directement lors de l'appui d'un bouton
 */
void set_direction(uint8_t direction){
	if(on_tron_screen==1){
		switch(direction) {
				case 1 :
					if(dirJ1!=BAS)
					{
						dirJ1 = HAUT;
					}
			        break;
			    case 2 :
			    	if(dirJ1!=DROITE)
			    	{
			    		dirJ1 = GAUCHE;
			    	}
			    	break;
			    case 3 :
			    	if(dirJ1!=HAUT)
			    	{
			    		dirJ1 = BAS;
			    	}
			    	break;
			    case 4 :
			    	if(dirJ1!=GAUCHE)
			    	{
						dirJ1 = DROITE;
			    	}
			    	break;
			    default :
			    	break;
			   }
	}
}


/*
 * void tronj2()
 * permet de gerer la connexion bluetooth avec l'autre joueur
 * appel a chaque interruption sur l'UART2 cad à chaque octet reçu
 */

void tronj2(){
	if(on_tron_screen == 1){
		uint8_t c;
		c = UART_getc(UART2_ID);
		if(c==FLAGFINISH){ //en cas de fin du joueur 2
			looser = 2;
		}
		if(MASTER == 0){
			if(c==FLAGSTART){
				UART_putc(UART2_ID,FLAGBACK);
				connect = 1; //on indique que les deux consoles sont connectées
			}
		}
		if(MASTER == 1){
			if(c==FLAGBACK){
				connect = 1; //on indique que les deux consoles sont connectées
			}
		}

		if(c==START){
			indexTab = 0;
		}
		else if (c == END){
			tron_tete_x2 = tab[0]; //on met a jour les coordonnées du joueur 2
			tron_tete_y2 = tab[1];
			grille[tron_tete_x2][tron_tete_y2] = 1;
			dirJ2 = tab[2];
		}
		else if(indexTab<TAB_SIZE){
			tab[indexTab++]=c;
		}
	}
}
