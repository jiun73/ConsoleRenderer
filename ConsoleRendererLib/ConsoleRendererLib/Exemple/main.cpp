#include "ConsoleRenderer.h"
#include <sstream>
ConsoleApp app;

/*
* Note: en plus des caractères normals (a, b, c, d... etc)
* il existe d'autres caractère fesant parti de ASCII qui peuvent être très utile
* voir la page: https://theasciicode.com.ar/
* 
* pour utiliser ces caractères, il faut ecrire directement le code
* ex: à la place d'écrire 'A' on peut écrire 65
* 
* par exemple, 219 dessine un bloc plein
*/

int main()
{
	V2d_i pos = {0,0}; //vecteur 2D à la position [0,0]
	while (app.run())
	{
		/*
			important!!	il faut dessiner du noir pour enlever ce qui est deja dans la console
			sinon on se retrouve avec des caractères 'fantômes' (on ne les dessine plus, mais il reste parce que rien ne les remplaces)
			c'est d'ailleurs un problème qui est courant chez les renderers, surtout lorsqu'on sort du terrain normal de jeu appellé 'Hall of mirrors'
			voir : https://www.reddit.com/r/gamedev/comments/n6bmh7/what_is_this_rendering_bugeffect_called_when_the/
		*/
		app.glyph(' '); //met le caractère de dessin à ' '
		app.rect({ {0, 0 }, { 16, 16 } }); //dessine un Rectangle à [0,0] de largeur 16 et de longueur 16
		
		if (app.held('a')) //si l'utilisateur pèse 'a'
		{
			pos.x -= 1; //bouge le vecteur position vers la droite (composante X - 1)
		}

		if (app.held('d')) //si l'utilisateur pèse 'd'
		{
			pos.x += 1; //bouge le vecteur position vers la gauche
		}

		if(app.held('w')) //si l'utilisateur pèse 'w'
		{
			pos.y -= 1; //haut
		}

		if (app.held('s'))
		{
			pos.y += 1; //bas
		}
		app.color(WHITE, BG_BLACK);
		
		if (app.mouse_left_press()) //si on fait un clic gauche
		{
			app.text("Allo!", app.mouse()); //dessine le texte 'Allo!' à la position 'app.mouse()' soit la souris
		}

		int ran = app.random.range(48, 74); //génère un nombre entre 48 et 74

		app.pencil(WHITE, BG_DARK_BLUE, ran); //on va maintenant dessiner avec 'a', blanc sur fond bleu foncé
		app.pix(pos); //cela met le pixel en mémoire, avec la position 'pos'
	}
}