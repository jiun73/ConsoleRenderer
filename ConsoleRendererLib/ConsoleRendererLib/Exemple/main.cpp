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
		if (a)
			i++;
		ren.setDrawPencil(a ? WHITE : RED, BG_DARK_BLUE, 'a');
		ren.drawPixel(i, 2);

		//std::cout << ren.held('A') << std::endl;

		 //dessine les elements en mémoire du renderer

		//ren.update();
		ren.present();
	}
}