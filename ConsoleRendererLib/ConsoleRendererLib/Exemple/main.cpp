#include "ConsoleRenderer.h"
ConsoleApp ren;

int main()
{
	int i = 0;
	int dir = 1;
	while (true)
	{

		ren.setDrawGlyph(' ');
		ren.drawRect({ {0, 0 }, { 6, 6 } }); //important!! il faut dessiner du noir pour enlever ce qui est deja dans la console
		bool a = ren.held('A');
		ren.setDrawPencil(a ? WHITE : RED, BG_DARK_BLUE, 'a');
		ren.drawPixel(i, 2);

		//std::cout << ren.held('A') << std::endl;

		 //dessine les elements en mémoire du renderer
		i += dir;
		if (i == 5 && dir == 1)
			dir = -1;
		else if(i == 0 && dir == -1)
			dir = 1;

		//ren.present();
	}
}