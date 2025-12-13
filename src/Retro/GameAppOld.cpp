#include "stdafx.h"
#include "GameApp.h"
#include "Engine.h"
//=============================================================================
extern int g_frameWidth;
extern int g_frameHeight;
extern unsigned* g_frameBuffer;
extern float* g_depthBuffer;
extern bool keys[256];

const float FOV = 60.0f; // Угол обзора в градусах

std::vector<Texture> textures;
std::vector<SectorO> sectors;
Camera camera(Vector3D(0, 0, 0), 0);
//=============================================================================
void createDefaultTextures()
{
	// Создаем несколько базовых текстур
	for (int i = 0; i < 3; i++) {
		Texture tex(64, 64);

		for (int y = 0; y < 64; y++) {
			for (int x = 0; x < 64; x++) {
				unsigned color;

				if (i == 0) { // Кирпичная текстура
					int brickX = x / 8;
					int brickY = y / 8;
					if ((brickX + brickY) % 2 == 0) {
						color = ColorToUInt(139, 69, 19); // Коричневый
					}
					else {
						color = ColorToUInt(160, 82, 45); // Более светлый коричневый
					}
				}
				else if (i == 1) { // Плитка пола
					if ((x / 16) % 2 == (y / 16) % 2) {
						color = ColorToUInt(100, 100, 100); // Серая
					}
					else {
						color = ColorToUInt(150, 150, 150); // Светло-серая
					}
				}
				else { // Потолок
					color = ColorToUInt(200, 200, 200); // Светло-серый
				}

				tex.SetPixel(x, y, color);
			}
		}

		textures.push_back(tex);
	}
}
//=============================================================================
void createTestScene()
{
	// Создаем тестовый сектор (комната 4x4)
	SectorO sector;

	// Стены комнаты (квадрат 4x4 с центром в (0,0))
	sector.walls.push_back({ Vector2D(-2, -2), Vector2D(2, -2), 0, 3.0f, 0.0f }); // Север
	sector.walls.push_back({ Vector2D(2, -2), Vector2D(2, 2), 0, 3.0f, 0.0f });   // Восток
	sector.walls.push_back({ Vector2D(2, 2), Vector2D(-2, 2), 0, 3.0f, 0.0f });   // Юг
	sector.walls.push_back({ Vector2D(-2, 2), Vector2D(-2, -2), 0, 3.0f, 0.0f }); // Запад

	sector.floorHeight = 0.0f;
	sector.ceilingHeight = 3.0f;
	sector.floorTexture = 1;      // Текстура пола
	sector.ceilingTexture = 2;    // Текстура потолка

	sectors.push_back(sector);

	// Устанавливаем начальную позицию камеры
	camera.position = Vector3D(0, 0, 1.6f); // Высота глаз человека
	camera.angle = 0.0f;
}
//=============================================================================
void renderSector(const SectorO& sector);
void renderWallSlice(int x, int wallTop, int wallBottom, int textureId, float texCoord, float distance);
void renderFloorAndCeiling(const SectorO& sector);
//=============================================================================
bool InitGame()
{
	createDefaultTextures();
	createTestScene();

	return true;
}
//=============================================================================
void CloseGame()
{
}
//=============================================================================
void FrameGame()
{
	if (keys['W']) MoveCamera(camera, 0.1f, 0.0f);   // Вперед
	if (keys['S']) MoveCamera(camera, -0.1f, 0.0f);  // Назад
	if (keys['A']) MoveCamera(camera, 0.0f, -0.1f);  // Влево
	if (keys['D']) MoveCamera(camera, 0.0f, 0.1f);   // Вправо
	if (keys[VK_LEFT]) RotateCamera(camera, -2.0f);  // Влево
	if (keys[VK_RIGHT]) RotateCamera(camera, 2.0f);  // Вправо
	if (keys[VK_UP]) MoveCamera(camera, 0.0f, 0.0f, 0.1f);   // Вверх
	if (keys[VK_DOWN]) MoveCamera(camera, 0.0f, 0.0f, -0.1f); // Вниз

	// Очищаем буферы
	for (int i = 0; i < g_frameWidth * g_frameHeight; i++)
	{
		g_frameBuffer[i] = ColorToUInt(50, 50, 255); // Темно-синий фон
		g_depthBuffer[i] = 10000.0f; // Бесконечность
	}

	// Рендер каждого сектора
	for (const auto& sector : sectors)
	{
		renderSector(sector);
	}
}
//=============================================================================
void renderSector(const SectorO& sector)
{
	float halfFOV = DegToRad(FOV / 2.0f);
	float rayAngleStep = (2.0f * halfFOV) / g_frameWidth;

	for (int x = 0; x < g_frameWidth; x++)
	{
		// Рассчитываем угол луча
		float rayAngle = camera.angle - halfFOV + x * rayAngleStep;

		// Создаем направление луча
		Vector2D rayDir(cos(rayAngle), sin(rayAngle));

		// Находим ближайшую стену
		float minDistance = 1000000.0f;
		const WallSegment* closestWall = nullptr;
		Vector2D hitPoint;

		for (const auto& wall : sector.walls)
		{
			float distance;
			Vector2D intersection;

			if (RayIntersect(Vector2D(camera.position.x, camera.position.y), rayDir,
				wall.start, wall.end, distance, intersection))
			{
				if (distance < minDistance)
				{
					minDistance = distance;
					closestWall = &wall;
					hitPoint = intersection;
				}
			}
		}

		if (closestWall != nullptr)
		{
			// Рассчитываем высоту проекции стены на экран
			float wallHeight = closestWall->heightTop - closestWall->heightBottom;
			float projectedHeight = (wallHeight / minDistance) * (g_frameWidth / 2.0f) / tan(halfFOV);

			// Рассчитываем позицию стены на экране
			int wallTop = (g_frameHeight / 2) - (projectedHeight / 2) - (camera.position.z - closestWall->heightTop) * (g_frameWidth / 2.0f) / (minDistance * tan(halfFOV));
			int wallBottom = wallTop + projectedHeight;

			// Ограничиваем границы
			wallTop = std::max(0, std::min(g_frameHeight, wallTop));
			wallBottom = std::max(0, std::min(g_frameHeight, wallBottom));

			// Рассчитываем координату текстуры
			Vector2D wallVec = closestWall->end - closestWall->start;
			float wallLength = wallVec.Length();
			Vector2D hitVec = hitPoint - closestWall->start;
			float texCoord = hitVec.Length() / wallLength;

			// Рендерим вертикальную полосу стены
			renderWallSlice(x, wallTop, wallBottom, closestWall->textureId, texCoord, minDistance);
		}
	}

	// Рендерим пол и потолок
	renderFloorAndCeiling(sector);
}
//=============================================================================
void renderWallSlice(int x, int wallTop, int wallBottom, int textureId, float texCoord, float distance)
{
	if (textureId >= textures.size()) return;

	const Texture& texture = textures[textureId];

	for (int y = wallTop; y < wallBottom; y++)
	{
		int pixelIndex = y * g_frameWidth + x;

		// Проверяем Z-буфер
		if (g_depthBuffer[pixelIndex] > distance)
		{
			g_depthBuffer[pixelIndex] = distance;

			// Рассчитываем Y-координату текстуры
			float texY = (float)(y - wallTop) / (float)(wallBottom - wallTop);
			int texYInt = (int)(texY * texture.height) % texture.height;

			// Получаем цвет из текстуры и применяем освещение
			COLORREF color = texture.GetPixel((int)(texCoord * texture.width), texYInt);
			color = ApplyLighting(color, distance);

			g_frameBuffer[pixelIndex] = color;
		}
	}
}
//=============================================================================
void renderFloorAndCeiling(const SectorO& sector)
{
	float halfFOV = DegToRad(FOV / 2.0f);
	float rayAngleStep = (2.0f * halfFOV) / g_frameWidth;

	for (int x = 0; x < g_frameWidth; x++)
	{
		for (int y = 0; y < g_frameHeight; y++)
		{
			// Определяем, рисуем ли мы пол или потолок
			bool isFloor = y > g_frameHeight / 2;

			if (isFloor)
			{
				// Рассчитываем точку на плоскости пола
				float screenY = (float)y - (g_frameHeight / 2.0f);
				float depth = (g_frameHeight / 2.0f) / screenY;

				if (depth < 0) continue; // За горизонтом

				// Преобразуем в мировые координаты
				float rayAngle = camera.angle - halfFOV + x * rayAngleStep;
				Vector2D rayDir(cos(rayAngle), sin(rayAngle));

				Vector2D worldPoint(
					camera.position.x + rayDir.x * depth,
					camera.position.y + rayDir.y * depth
				);

				// Рассчитываем цвет пола
				int pixelIndex = y * g_frameWidth + x;
				if (g_depthBuffer[pixelIndex] > depth)
				{
					g_depthBuffer[pixelIndex] = depth;

					// Используем текстуру пола
					int texId = sector.floorTexture;
					if (texId < textures.size())
					{
						const Texture& tex = textures[texId];

						// Простое отображение текстуры пола
						int texX = (int)(worldPoint.x * 4) % tex.width;
						int texY = (int)(worldPoint.y * 4) % tex.height;

						COLORREF color = tex.GetPixel(texX, texY);
						color = ApplyLighting(color, depth);
						g_frameBuffer[pixelIndex] = color;
					}
				}
			}
			else
			{
				// Рендер потолка (аналогично полу, но с другим значением Z)
				float screenY = (g_frameHeight / 2.0f) - (float)y;
				float depth = (g_frameHeight / 2.0f) / screenY;

				if (depth < 0) continue; // За горизонтом

				float rayAngle = camera.angle - halfFOV + x * rayAngleStep;
				Vector2D rayDir(cos(rayAngle), sin(rayAngle));

				Vector2D worldPoint(
					camera.position.x + rayDir.x * depth,
					camera.position.y + rayDir.y * depth
				);

				int pixelIndex = y * g_frameWidth + x;
				if (g_depthBuffer[pixelIndex] > depth)
				{
					g_depthBuffer[pixelIndex] = depth;

					int texId = sector.ceilingTexture;
					if (texId < textures.size()) {
						const Texture& tex = textures[texId];

						int texX = (int)(worldPoint.x * 4) % tex.width;
						int texY = (int)(worldPoint.y * 4) % tex.height;

						COLORREF color = tex.GetPixel(texX, texY);
						color = ApplyLighting(color, depth);
						g_frameBuffer[pixelIndex] = color;
					}
				}
			}
		}
	}
}
//=============================================================================