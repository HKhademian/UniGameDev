#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <SOIL/SOIL.h>
#include <utility>
#include <vector>

class Renderer {
public:
	explicit Renderer(std::vector<const char *> tiles_path, std::vector<std::vector<unsigned int>> tiles_map)
			: tiles_path(std::move(tiles_path)),
				tiles_map(std::move(tiles_map)) {}

	void init() {
		initTextures();
		glEnable(GL_DEPTH_TEST);
	}

	void resize(int w, int h) {
		glViewport(0, 0, w, h);
	}

	void initTextures() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		tiles_texture.clear();
		for (const auto &path : tiles_path) {
			unsigned int terrain = SOIL_load_OGL_texture(
					path,
					SOIL_LOAD_RGBA,
					SOIL_CREATE_NEW_ID,
					0//SOIL_FLAG_INVERT_Y
			);
			tiles_texture.push_back(terrain);
		}

		midRow = 1.0f * (float) (tiles_map.size() / 2);
		midCol = 1.0f * (float) (tiles_map.size() / 2);
	}

	void display() {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		float h = glutGet(GLUT_WINDOW_HEIGHT);
		float w = glutGet(GLUT_WINDOW_WIDTH);
		float s = 1.0f / zoom;
		float r = w / h;

		glOrtho(-s * r, s * r, -s, s, -1000.0f, 1000.0f);
		gluLookAt(+0, +0, +10, 0, 0, 0, 1, 0, 0);
		glTranslatef(0, 0, 0);

		drawMap();
	}

	void drawMap() {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		for (int row = 0; row < tiles_map.size(); row++) {
			for (int col = 0; col < tiles_map[row].size(); col++) {
				unsigned int tile = tiles_map[row][col];
				// std::cout << tile << " ";
				drawTileAt(col, row, tile);
			}
			// std::cout << std::endl;
		}
		// std::cout << std::endl;
	}

	void drawTileAt(float col, float row, unsigned int tile) {
		float oddPad = (float) ((int) row & 1) * 0.5f;
		float xPos = col * -1.0f + oddPad;
		float yPos = row * -0.75f;
		float zPos = (1000 * row + col) * 0.0001f;

		glPushMatrix();
		glRotatef(90, 0, 0, 1);
		glTranslatef(midCol + xPos, midRow + yPos, zPos);
		if (selectCol == col && selectRow == row)
			glScalef(0.9, 0.9, 0.9);
		drawTerrian(tile);
		glPopMatrix();

	}

	void drawTerrian(unsigned int tileType) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tiles_texture[tileType]);

		glBegin(GL_QUADS);

		glColor4f(1.0f, 1.0f, 1.0, 1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(-0.5f, -0.5f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(0.5f, -0.5f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(0.5f, 0.5f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(-0.5f, 0.5f);

		glEnd();
		glDisable(GL_TEXTURE_2D);
	}


	void mouse(int button, int state, int x, int y) {
		if (button == GLUT_LEFT_BUTTON) {
			if (state == GLUT_DOWN) {
				motion(x, y);
			} else if (state == GLUT_UP) {
				selectCol = -1, selectRow = -1;
			}
		} else if (button == 3) {
			zoom += 0.1;
		} else if (button == 4) {
			zoom -= 0.1;
		}
		zoom = zoom < 0.1f ? 0.1f : zoom > 1.0f ? 1.0f : zoom;
		glutPostRedisplay();
	}

	void motion(int x, int y) {
		// https://community.khronos.org/t/converting-window-coordinates-to-world-coordinates/16029/8
		GLint viewport[4]; //var to hold the viewport info
		GLdouble modelview[16]; //var to hold the modelview info
		GLdouble projection[16]; //var to hold the projection matrix info
		GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
		GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates

		glGetDoublev(GL_MODELVIEW_MATRIX, modelview); //get the modelview info
		glGetDoublev(GL_PROJECTION_MATRIX, projection); //get the projection matrix info
		glGetIntegerv(GL_VIEWPORT, viewport); //get the viewport info

		winX = (float) x;
		winY = (float) y;
		winZ = 0;

		//get the world coordinates from the screen coordinates
		gluUnProject(winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);
		selectRow = (round(-worldX * 2) / 2 + midRow) / 0.75;
		if (selectRow % 2)worldY -= 0.5f;
		selectCol = (round(-worldY * 2) / 2 + midCol) / 1;

		glutPostRedisplay();
	}

	void special(int key, int x, int y) {
		switch (key) {
			case GLUT_KEY_UP:
				midRow += speed;
				break;
			case GLUT_KEY_DOWN:
				midRow -= speed;
				break;
			case GLUT_KEY_RIGHT:
				midCol += speed;
				break;
			case GLUT_KEY_LEFT:
				midCol -= speed;
				break;
			default:
				return;
		}
		glutPostRedisplay();
	}

	void keyboard(unsigned char key, int x, int y) {
		switch (key) {
			case 'w':
				midRow += speed;
				break;
			case 's':
				midRow -= speed;
				break;
			case 'd':
				midCol += speed;
				break;
			case 'a':
				midCol -= speed;
				break;
			case '+':
				zoom -= 0.1;
				break;
			case '-':
				zoom += 0.1;
				break;
			default:
				return;
		}
		zoom = zoom < 0.1f ? 0.1f : zoom > 1.0f ? 1.0f : zoom;
		glutPostRedisplay();
	}

private:

	const std::vector<const char *> tiles_path;
	const std::vector<std::vector<unsigned int>> tiles_map;
	std::vector<unsigned int> tiles_texture;

	int selectCol = 2, selectRow = 2;
	float midRow = 0, midCol = 0, zoom = 0.5, speed = 0.3;
};
