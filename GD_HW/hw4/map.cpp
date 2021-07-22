#include "Renderer.cpp"


static Renderer *renderer;

static void glutResize(int w, int h) {
	renderer->resize(w, h);
}

static void glutDisplay() {
	renderer->display();
	glutSwapBuffers();
}

static void glutMouse(int button, int state, int x, int y) {
	renderer->mouse(button, state, x, y);
}

static void glutSpecialHandler(int key, int x, int y) {
	renderer->special(key, x, y);
}

static void glutKeyboardHandler(unsigned char key, int x, int y) {
	renderer->keyboard(key, x, y);
}

static void glutMotion(int x, int y) {
	renderer->motion(x, y);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glEnable(GL_MULTISAMPLE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("WindowTitle");

	glutDisplayFunc(glutDisplay);
	glutReshapeFunc(glutResize);

	glutKeyboardFunc(glutKeyboardHandler);
	glutSpecialFunc(glutSpecialHandler);

	glutMouseFunc(glutMouse);

	glutMotionFunc(glutMotion);


	const std::vector<const char *> tiles_path = {
			"../assets/western_bank.png",//0
			"../assets/western_general.png",//1
			"../assets/western_indians.png",//2
			"../assets/western_saloon.png",//3
			"../assets/western_sheriff.png",//4
			"../assets/western_station.png",//5
			"../assets/western_watertower.png",//6
	};

	//std::vector<std::vector<unsigned int>> tiles_map(5, std::vector<unsigned int>(5, 1));

	std::vector<std::vector<unsigned int>> tiles_map(100, std::vector<unsigned int>(100, 0));
	for (auto &row : tiles_map) {
		for (unsigned int &cell : row) {
			cell = random() * 1000 % tiles_path.size();
		}
	}

//	const std::vector<std::vector<unsigned int>> tiles_map{
////			{0},
////			{1},
////			{2},
////			{3},
//			{3, 4, 5},
//			{0, 1, 2, 3, 4, 5, 6},
//			{1, 2},
//	};

	renderer = new Renderer(tiles_path, tiles_map);
	renderer->init();

	glutMainLoop();

	return 0;
}
