#include "ConsoleRenderer.h"
#include <sstream>
#include <list>
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

V2d_i moveSnake(std::list<V2d_i>& snake, V2d_i dir_vec)
{
	if (dir_vec != 0)
	{
		V2d_i end_pos = snake.front() + dir_vec;
		V2d_i to_remove = snake.back();
		snake.pop_back();
		snake.push_front(end_pos);
		return to_remove;
	}
	return -1;
}

void fillSnake(std::list<V2d_i>& snake, V2d_i pos, int start_size)
{
	for (int i = 0; i < start_size; i++)
	{
		snake.push_back(pos);
	}
}

bool selfCollision(const std::list<V2d_i>& snake)
{
	V2d_i head_pos = snake.front();
	for (auto it = std::next(snake.begin(),1); it != snake.end(); it++)
	{
		if (head_pos == *it)
			return true;
	}
	return false;
}

void renderSnake(const std::list<V2d_i>& snake)
{
	app.pencil(selfCollision(snake) ? RED : WHITE, BG_DARK_BLUE, '*');
	for (auto& c : snake)
	{
		app.pix(c);
	}
}

int main()
{
	V2d_i pos = {0,0}; //vecteur 2D à la position [0,0]
	std::list<V2d_i> snake; fillSnake(snake, 5, 30);
	while (app.run())
	{
		/*
			important!!	il faut dessiner du noir pour enlever ce qui est deja dans la console
			sinon on se retrouve avec des caractères 'fantômes' (on ne les dessine plus, mais il reste parce que rien ne les remplaces)
			c'est d'ailleurs un problème qui est courant chez les renderers, surtout lorsqu'on sort du terrain normal de jeu appellé 'Hall of mirrors'
			voir : https://www.reddit.com/r/gamedev/comments/n6bmh7/what_is_this_rendering_bugeffect_called_when_the/
		*/
		app.glyph(' '); //met le caractère de dessin à ' '
		app.rect({ {0, 0 }, { 2, 2 } }); //dessine un Rectangle à [0,0] de largeur 16 et de longueur 16
		
		V2d_i move_vec = 0;
		if (app.held('a')) //si l'utilisateur pèse 'a'
		{
			move_vec.x -= 1; //bouge le vecteur position vers la droite (composante X - 1)
		}
		else if (app.held('d')) //si l'utilisateur pèse 'd'
		{
			move_vec.x += 1; //bouge le vecteur position vers la gauche
		}
		else if(app.held('w')) //si l'utilisateur pèse 'w'
		{
			move_vec.y -= 1; //haut
		}
		else if (app.held('s'))
		{
			move_vec.y += 1; //bas
		}

		V2d_i last = moveSnake(snake, move_vec);

		app.color(WHITE, BG_BLACK);
		
		if (app.mouse_left_press()) //si on fait un clic gauche
		{
			app.text("Allo!", app.mouse()); //dessine le texte 'Allo!' à la position 'app.mouse()' soit la souris
		}

		app.set_cooldown(100);
		int ran = app.random.range(48, 74); //génère un nombre entre 48 et 74

		app.pencil(WHITE, BG_DARK_BLUE, ran); //on va maintenant dessiner avec 'a', blanc sur fond bleu foncé
		app.pix(pos); //cela met le pixel en mémoire, avec la position 'pos'

		renderSnake(snake);
		app.pencil(WHITE, BG_BLACK, ' ');
		app.pix(last);
	}
}