#include <iostream>
#include <vector>
#include <cmath>
#include <windows.h>

const float PI = 3.14159265f;
const int FPS = 30;
float CAMERA_DISTANCE = 5.0f;

const std::vector<float> VERTICES = {
	-1,  1,  1,
	 1,  1,  1,
	-1, -1,  1,
	 1, -1,  1,

	-1,  1, -1,
	 1,  1, -1,
	-1, -1, -1,
	 1, -1, -1,
};

const std::vector<int> FACES = {
	0, 1, 2, 3,
	4, 5, 6, 7,
	0, 1, 4, 5,
	2, 3, 6, 7,
	0, 2, 4, 6,
	1, 3, 5, 7
};
const char FACE_CHAR[] = "~=+#%@";

int cv2DTo1D(float x, float y, int screenWidth, int screenHeight) {
	int px = (int)((x + 1.0f) * 0.5f * (screenWidth - 1));
	int py = (int)((-y + 1.0f) * 0.5f * (screenHeight - 1));
	return py * screenWidth + px;
}

std::vector<float> cv3DTo2D(float x, float y, float z) {
	return { x / z, y / z * 2 };
}

std::vector<float> verticesRotateZ(const std::vector<float>& vertices, float angle) {
	std::vector<float> rotatedVertices(vertices.size());
	angle = angle * PI / 180.0f;
	float cosA = std::cos(angle);
	float sinA = std::sin(angle);
	for (int i = 0; i < vertices.size(); i += 3) {
		float x = vertices[i];
		float y = vertices[i + 1];
		rotatedVertices[i] = x * cosA - y * sinA ;
		rotatedVertices[i + 1] = x * sinA + y * cosA;
		rotatedVertices[i + 2] = vertices[i + 2];
	}
	return rotatedVertices;
}

std::vector<float> verticesRotateY(const std::vector<float>& vertices, float angle) {
	std::vector<float> rotatedVertices(vertices.size());
	angle = angle * PI / 180.0f;
	float cosA = std::cos(angle);
	float sinA = std::sin(angle);
	for (int i = 0; i < vertices.size(); i += 3) {
		float x = vertices[i];
		float z = vertices[i + 2];
		rotatedVertices[i] = x * cosA + z * sinA;
		rotatedVertices[i + 1] = vertices[i + 1];
		rotatedVertices[i + 2] = x * -sinA + z * cosA;
	}
	return rotatedVertices;
}

std::vector<float> verticesRotateX(const std::vector<float>& vertices, float angle) {
	std::vector<float> rotatedVertices(vertices.size());
	angle = angle * PI / 180.0f;
	float cosA = std::cos(angle);
	float sinA = std::sin(angle);
	for (int i = 0; i < vertices.size(); i += 3) {
		float y = vertices[i + 1];
		float z = vertices[i + 2];
		rotatedVertices[i] = vertices[i];
		rotatedVertices[i + 1] = y * cosA - z * sinA;
		rotatedVertices[i + 2] = y * sinA + z * cosA;
	}
	return rotatedVertices;
}

void drawVertices(std::vector<char>& screen, std::vector<float>& vertices, int screenWidth, int screenHeight) {
	char charARray[] = "01234567";
	for (int i = 0; i < vertices.size(); i += 3) {
		float x = vertices[i];
		float y = vertices[i + 1];
		float z = vertices[i + 2] + CAMERA_DISTANCE;
		std::vector<float> point2D = cv3DTo2D(x, y, z);
		int index1D = cv2DTo1D(point2D[0], point2D[1], screenWidth, screenHeight);
		if (index1D >= 0 && index1D < screen.size() - 1) {
			screen[cv2DTo1D(point2D[0], point2D[1], screenWidth, screenHeight)] = charARray[i/3];
		}
	}
}

void drawFaces(std::vector<char>& screen, std::vector<float>& vertices, const std::vector<int>& faces, int screenWidth, int screenHeight) {
	std::vector<float> depth(screenHeight * screenWidth);
	std::fill(depth.begin(), depth.end(), std::numeric_limits<float>::infinity());
	for (int i = 0; i < faces.size(); i += 4) {
		std::vector<float> v1 = {
			vertices[faces[i] * 3], vertices[faces[i] * 3 + 1], vertices[faces[i] * 3 + 2] + CAMERA_DISTANCE
		};
		std::vector<float> v2 = {
			vertices[faces[i + 1] * 3], vertices[faces[i + 1] * 3 + 1], vertices[faces[i + 1] * 3 + 2] + CAMERA_DISTANCE
		};
		std::vector<float> v3 = {
			vertices[faces[i + 2] * 3], vertices[faces[i + 2] * 3 + 1], vertices[faces[i + 2] * 3 + 2] + CAMERA_DISTANCE
		};
		std::vector<float> v4 = {
			vertices[faces[i + 3] * 3], vertices[faces[i + 3] * 3 + 1], vertices[faces[i + 3] * 3 + 2] + CAMERA_DISTANCE
		};

		float stepX1 = std::abs((v1[0] / v1[2] * screenWidth - v2[0] / v2[2] * screenWidth) / 2);
		float stepX2 = std::abs((v3[0] / v3[2] * screenWidth - v4[0] / v4[2] * screenWidth) / 2);
		float stepDiagX1 = std::abs((v1[0] / v1[2] * screenWidth - v4[0] / v4[2] * screenWidth) / 2);
		float stepDiagX2 = std::abs((v2[0] / v2[2] * screenWidth - v3[0] / v3[2] * screenWidth) / 2);

		float stepY1 = std::abs((v1[1] / v1[2] * 2 * screenHeight - v3[1] / v3[2] * 2 * screenHeight) / 2);
		float stepY2 = std::abs((v2[1] / v2[2] * 2 * screenHeight - v4[1] / v4[2] * 2 * screenHeight) / 2);
		float stepDiagY1 = std::abs((v1[1] / v1[2] * 2 * screenHeight - v4[1] / v4[2] * 2 * screenHeight) / 2);
		float stepDiagY2 = std::abs((v2[1] / v2[2] * 2 * screenHeight - v3[1] / v3[2] * 2 * screenHeight) / 2);

		float stepU = 1.0f / (std::fmax(std::fmax(stepX1, stepX2), std::fmax(stepDiagX1, stepDiagX2)) + 1);
		stepU = std::floor(stepU * 100) / 100;

		float stepV = 1.0f / (std::fmax(std::fmax(stepY1, stepY2), std::fmax(stepDiagY1, stepDiagY2)) + 1);
		stepV = std::floor(stepV * 100) / 100;

		float step = std::fmin(stepU, stepV);

		for (float u = 0; u <= 1.0f; u += step) {
			for (float v = 0; v <= 1.0f; v += step) {
				float x = v1[0] * (1 - u) * (1 - v) + v2[0] * u * (1 - v) + v3[0] * (1 - u) * v + v4[0] * u * v;
				float y = v1[1] * (1 - u) * (1 - v) + v2[1] * u * (1 - v) + v3[1] * (1 - u) * v + v4[1] * u * v;
				float z = v1[2] * (1 - u) * (1 - v) + v2[2] * u * (1 - v) + v3[2] * (1 - u) * v + v4[2] * u * v;
				std::vector<float> point2D = cv3DTo2D(x, y, z);
				int index1D = cv2DTo1D(point2D[0], point2D[1], screenWidth, screenHeight);

				if (index1D >= 0 && index1D < screen.size() - 1) {
					if (z < depth[index1D]) {
						depth[index1D] = z;
						screen[index1D] = FACE_CHAR[i / 4];
					}
				}
			}
		}
	}
}

int main() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int screenWidth, screenHeight;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	screenWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	screenHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	std::vector<char> screen(screenHeight * screenWidth + 1);

	float angleX = 0.0f, angleY = 0.0f, angleZ = 0.0f;
	std::vector<float> verticesTmp = VERTICES;
	WORD sleepDelay = (WORD)(1000.0f / FPS);
	
	for (;;) {
		angleX += 1.0f;
		angleY += 1.0f;
		angleZ += 1.0f;

		verticesTmp = verticesRotateX(VERTICES, angleX);
		verticesTmp = verticesRotateY(verticesTmp, angleY);
		verticesTmp = verticesRotateZ(verticesTmp, angleZ);

		std::fill(screen.begin(), screen.end(), ' ');
		
		drawFaces(screen, verticesTmp, FACES, screenWidth, screenHeight);
		//drawVertices(screen, verticesTmp, screenWidth, screenHeight);

		screen[screen.size() - 1] = '\0';
		Sleep(sleepDelay);

		std::cout << "\x1B[H";
		std::cout << screen.data();
	}
	return 0;
}