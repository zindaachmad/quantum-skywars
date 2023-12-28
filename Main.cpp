#include <sl.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include <string>

bool explosionSoundPlayed = false;
bool gameOver();
bool gameIsOver = false;

const double minY = 50.0;
const double maxY = 700.0;
const double minX = 75.0;
const double maxX = 1200.0;

double playerScore = 0;

struct Player
{
	double x = 400;
	double y = 360;
	double health = 100;
	double maxHealth = 100;
	double targetX = 0;
	double targetY = 0;
	double smoothDelay = 0.075;
	int playerTexture = slLoadTexture("Assets\\player.png");
};

struct Bullet
{
	Player player;

	double x = player.x;
	double y = player.y;
	double damage = 20;
	int bulletTexture = slLoadTexture("Assets\\bullet2.png");
};

struct Helicopter
{
	double x = 1500;
	double y = std::rand() % (600 - 200 + 1) + 100;
	double health = 30;
	int helicopterTexture = slLoadTexture("Assets\\helicopter.png");
};

struct PlaneBullet {
	double x = 0;
	double y = 0;
	double damage = 20;
	int bulletTexture = slLoadTexture("Assets\\bullet2.png");
};

struct Plane 
{
	double x = 1500;
	double y = std::rand() % (700 - 100 + 1) + 125;
	double health = 60;
	int planeTexture = slLoadTexture("Assets\\plane1.png");
	std::vector<PlaneBullet> bullets;
	double lastShootTime = 0.0;
};

struct TankBullet
{
	double x = 0;
	double y = 0;
	double damage = 100;
	int tankBulletTexture = slLoadTexture("Assets\\tankbullet.png");
	double speed = 1.0;
};

struct Tank
{
	double x = 1500;
	double y = 90;
	double health = 300;
	int tankTexture = slLoadTexture("Assets\\tank.png");

	std::vector<TankBullet> bullets;

	double initialPlayerX;
	double initialPlayerY;
};

const std::string basePath = "Assets\\explode_";
const std::string extension = ".png";
const int numImages = 10;
int explosionTextures[numImages];

void initExplosionTextures()
{
	for (int i = 0; i < numImages; ++i) {
		std::string filename = basePath + std::to_string(i) + extension;
		explosionTextures[i] = slLoadTexture(filename.c_str());
	}
}

void drawExplosion(int x, int y)
{
	static double explosionX = 100.0;
	static double explosionY = 100.0;
	static int currentFrame = 0;
	static double frameDuration = 0.1;

	double deltaTime = slGetDeltaTime();

	if (currentFrame < 9) {
		if ((currentFrame + 1) * frameDuration < deltaTime) {
			currentFrame++;
		}
	}

	std::cout << "Explosion Coordinates: (" << x << ", " << y << ")" << std::endl;
	std::cout << "Current Frame: " << currentFrame << std::endl;

	slSprite(explosionTextures[currentFrame], x, y, 400, 400);
}


bool gameOver();
bool isColliding(const Bullet& bullet, const Tank& tank);
bool isColliding(const Bullet& bullet, const Player& player);
bool isColliding(const Bullet& bullet, const Helicopter& helicopter);
bool isColliding(const Bullet& bullet, const Plane& plane);
bool isColliding(const TankBullet& bullet, const Player& player);
bool isColliding(const PlaneBullet& bullet, const Player& player);
void shootBullet(Player& player, std::vector<Bullet>& bullets, double& lastShootTime, double shootInterval);
void updateBullets(std::vector<Bullet>& bullets);
void showBullet(const Bullet& bullet);
void showPlayer(Player& player);
void updatePlayer(Player& player, std::vector<Bullet>& bullets);
void showHelicopter(const Helicopter& helicopter);
void updateHelicopter(Helicopter& helicopter, std::vector<Bullet>& bullets);
void showPlane(const Plane& plane);
void updatePlane(Plane& plane, std::vector<Bullet>& bullets);
void showPlaneBullets(const Plane& plane);
void updatePlaneBullets(Plane& plane, Player& player);
void showTank(Tank& tank);
void updateTank(Tank& tank, std::vector<Bullet>& bullets, Player& player);
void showTankBullets(Tank& tank);
void updateTankBullets(Tank& tank, Player& player);
void respawnTank(Tank& tank);
void shootTankBullet(Tank& tank, double& lastShootTime, double shootInterval, Player& player);

int main()
{
	slWindow(1280, 720, "Project S", false);

	Player player;
	Helicopter Helicopter;
	Plane plane;
	Tank tank;

	std::vector<Bullet> bullets;
	int mainBackground = slLoadTexture("Assets\\main.png");
	int mainFont = slLoadFont("Assets\\pixel.ttf");

	double lastShootTime = 0.0;
	double shootInterval = 0.2;

	double tankLastShootTime = 0.0;
	double tankShootInterval = 2.5;

	double bgSpeed = 800;

	slSetFont(mainFont, 30);
	slShowCursor(false);

	initExplosionTextures();

	while (!slShouldClose())
	{
		if (!gameIsOver)
		{
			slSprite(mainBackground, bgSpeed, 400, 6000, 840);

			char pointsString[200];
			snprintf(pointsString, sizeof(pointsString), "Score: %.1f", playerScore);
			slText(50, 650, pointsString);

			char healthString[200];
			snprintf(healthString, sizeof(healthString), "HP: %.1f", player.health);
			slText(1000, 650, healthString);

			updatePlane(plane, bullets);
			showPlane(plane);

			updatePlayer(player, bullets);
			showPlayer(player);

			updateHelicopter(Helicopter, bullets);
			showHelicopter(Helicopter);

			updatePlane(plane, bullets);
			updatePlaneBullets(plane, player);

			showPlane(plane);
			showPlaneBullets(plane);

			updateTank(tank, bullets, player);
			showTank(tank);

			updateTankBullets(tank, player);
			showTankBullets(tank);


			double currentTime = slGetTime();
			if (currentTime - tankLastShootTime >= tankShootInterval)
			{
				shootTankBullet(tank, tankLastShootTime, tankShootInterval, player);
			}

			if (slGetMouseButton(SL_MOUSE_BUTTON_1))
			{
			shootBullet(player, bullets, lastShootTime, shootInterval);
			}

			updateBullets(bullets);
			for (const auto& bullet : bullets)
			{
			showBullet(bullet);
			}

			slRender();

			bgSpeed -= 0.3;

			if (player.health <= 0)
			{
				gameIsOver = true;
			}

			if (slGetKey(SL_KEY_ESCAPE))
			{
				slClose();
			}

			if (slGetMouseButton(SL_MOUSE_BUTTON_2))
			{
				drawExplosion(600, 600);
			}

		}
		else
		{
			gameOver();
			break;
		}

	}

	std::cin.get();
}

bool gameOver()
{
	int mainBackground = slLoadTexture("Assets\\main.png");
	int mainFont = slLoadFont("Assets\\pixel.ttf");

	gameIsOver = true;
 
	slSprite(mainBackground, 640, 360, 6000, 840);

	slSetTextAlign(SL_ALIGN_CENTER);
	slSetFontSize(40);
	slText(640, 360, "Game Over");

	char pointsString[200];
	snprintf(pointsString, sizeof(pointsString), "Your Score: %.1f", playerScore);
	slSetFontSize(30);
	slText(640, 320, pointsString);

	slRender(); 

	return true;
}

bool isColliding(const TankBullet& bullet, const Player& player)
{
	return (bullet.x < player.x + 32 &&
		bullet.x + 32 > player.x &&
		bullet.y < player.y + 32 &&
		bullet.y + 32 > player.y);
}

void shootTankBullet(Tank& tank, double& lastShootTime, double shootInterval, Player& player)
{
	double currentTime = slGetTime();

	if (currentTime - lastShootTime >= shootInterval)
	{
		TankBullet bullet;
		bullet.x = tank.x;  
		bullet.y = tank.y;
		tank.initialPlayerX = player.x;
		tank.initialPlayerY = player.y;
		tank.bullets.push_back(bullet);

		lastShootTime = currentTime;
	}
}

void showTankBullets(Tank& tank)
{
	for (const auto& bullet : tank.bullets)
	{
		slSprite(bullet.tankBulletTexture, bullet.x - 10, bullet.y - 10, 32, 32);
	}
}

void updateTankBullets(Tank& tank, Player& player)
{
	double gravity = 0.980665;

	for (auto it = tank.bullets.begin(); it != tank.bullets.end();)
	{
		it->x -= it->speed;

		if (isColliding(*it, player))
		{
			it = tank.bullets.erase(it);
			player.health -= 80;
		}
		else
		{
			double angle = 90;

			// Update horizontal position
			it->x += tan(angle) * it->speed;

			// Update vertical position with gravity
			it->y += gravity;  // Adjust this line

			if (it->x < 0 || it->y < 0 || it->x > 1280 || it->y > 700)
			{
				it = tank.bullets.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

bool isColliding(PlaneBullet& bullet, Player& player) {
	return (bullet.x < player.x + 64 &&
		bullet.x + 64 > player.x &&
		bullet.y < player.y + 64 &&
		bullet.y + 64 > player.y);
}

void showTank(Tank& tank)
{
	slSprite(tank.tankTexture, static_cast<int>(round(tank.x)), static_cast<int>(round(tank.y)), 200, 200);
}

void respawnTank(Tank& tank) {
	tank.x = 1400;
	tank.y = 90;
	tank.health = 300;
}

bool isColliding(const PlaneBullet& bullet, const Player& player) {
	return (bullet.x < player.x + 64 &&
		bullet.x + 64 > player.x &&
		bullet.y < player.y + 64 &&
		bullet.y + 64 > player.y);
}

bool isColliding(const Bullet& bullet, const Tank& tank) {

	return (bullet.x < tank.x + 64 &&
		bullet.x + 64 > tank.x &&
		bullet.y < tank.y + 64 &&
		bullet.y + 64 > tank.y);
}

void updateTank(Tank& tank, std::vector<Bullet>& bullets, Player& player) {
	double speed = 0.5;
	tank.x -= speed;

	for (auto it = bullets.begin(); it != bullets.end();)
	{
		if (isColliding(*it, tank))
		{
			tank.health -= it->damage;
			it = bullets.erase(it);
		}
		else
		{
			++it;
		}
	}

	updateTankBullets(tank, player);

	if (tank.health <= 0) {
		if (!explosionSoundPlayed) {
			int explode = slLoadWAV("SFX\\explosion.wav");
			playerScore += 100;
		}
		respawnTank(tank);
	}

	if (tank.x <= -100)
	{
		respawnTank(tank);
	}
}

void updatePlayer(Player& player, std::vector<Bullet>& bullets) {

	double mouseX = slGetMouseX();
	double mouseY = slGetMouseY();

	player.x = player.x + (player.targetX - player.x) * player.smoothDelay;
	player.y = player.y + (player.targetY - player.y) * player.smoothDelay;

	player.targetX = mouseX;
	player.targetY = mouseY;

	const double minX = 50.0;
	const double maxX = 1880.0;
	const double minY = 20.0;
	const double maxY = 1050.0;

	player.targetX = std::max(minX, std::min(player.targetX, maxX));
	player.targetY = std::max(minY, std::min(player.targetY, maxY));
}

bool isColliding(const Bullet& bullet, const Player& player) 
{

	return (bullet.x < player.x + 128 &&
		bullet.x + 128 > player.x &&
		bullet.y < player.y + 128 &&
		bullet.y + 128 > player.y);
}

void showPlayer(Player& player)
{
	slSprite(player.playerTexture, static_cast<int>(round(player.x)) , static_cast<int>(round(player.y)), 256, 256);
}

void showBullet(const Bullet& bullet)
{
	slSprite(bullet.bulletTexture, bullet.x, bullet.y - 10, 32, 32);
}

void shootBullet(Player& player, std::vector<Bullet>& bullets, double& lastShootTime, double shootInterval)
{
	double currentTime = slGetTime();

	if (currentTime - lastShootTime >= shootInterval)
	{
		Bullet bullet;
		bullet.x = player.x;
		bullet.y = player.y;
		bullets.push_back(bullet);

		lastShootTime = currentTime;
	}
}

void updateBullets(std::vector<Bullet>& bullets)
{
	for (auto& bullet : bullets)
	{
		bullet.x += 25.0;
	}
}

bool isColliding(const Bullet& bullet, const Helicopter& Helicopter)
{
	return (bullet.x < Helicopter.x + 128 &&
		bullet.x + 128 > Helicopter.x &&
		bullet.y < Helicopter.y + 128 &&
		bullet.y + 128 > Helicopter.y);
}

bool isColliding(const Bullet& bullet, const Plane& plane) {

	return (bullet.x < plane.x + 128 &&
		bullet.x + 128 > plane.x &&
		bullet.y < plane.y + 128 &&
		bullet.y + 128 > plane.y);
}

void respawnHelicopter(Helicopter& Helicopter)
{
	const int minY = 100;
	Helicopter.x = 1400;
	Helicopter.y = std::rand() % (720 - minY);
	Helicopter.health = 30;
	explosionSoundPlayed = false;
}

void updateHelicopter(Helicopter& helicopter, std::vector<Bullet>& bullets)
{
	double speed = 1.25;
	helicopter.x -= speed;

	for (auto it = bullets.begin(); it != bullets.end();)
	{
		if (isColliding(*it, helicopter))
		{
			helicopter.health -= it->damage;
			it = bullets.erase(it);
		}
		else
		{
			++it;
		}
	}

	if (helicopter.health <= 0)
	{
		playerScore += 25;
		if (!explosionSoundPlayed)
		{
			int explode = slLoadWAV("SFX\\explosion.wav");
		}

		respawnHelicopter(helicopter);
	}

	if (helicopter.x <= -100)
	{
		respawnHelicopter(helicopter);
	}
}

void showHelicopter(const Helicopter& Helicopter)
{
	slSprite(Helicopter.helicopterTexture, static_cast<int>(round(Helicopter.x)), static_cast<int>(round(Helicopter.y)), 128, 70);
}

void respawnPlane(Plane& plane) {
	const int minY = 100;
	plane.x = 1400;
	plane.y = std::rand() % (720 - minY);
	plane.health = 60;
	explosionSoundPlayed = false;
}

void updatePlane(Plane& plane, std::vector<Bullet>& bullets) {
	double speed = 1.5;
	plane.x -= speed;

	for (auto it = bullets.begin(); it != bullets.end();) {
		if (isColliding(*it, plane)) {
			plane.health -= it->damage;
			it = bullets.erase(it);
		}
		else {
			++it;
		}
	}
	if (plane.health <= 0) {
		if (!explosionSoundPlayed) {
			int explode = slLoadWAV("SFX\\explosion.wav");
			playerScore += 25;
		}
		respawnPlane(plane); 
	}

	if (plane.x <= -100)
	{
		respawnPlane(plane);
	}
}

void showPlane(const Plane& plane) {
	slSprite(plane.planeTexture, static_cast<int>(round(plane.x)), static_cast<int>(round(plane.y)), 128, 50);
}

void updatePlaneBullets(Plane& plane, Player& player) {
	double currentTime = slGetTime();
	double cihuy = 2.5;
	double shootInterval = cihuy;

	if (currentTime - plane.lastShootTime >= shootInterval) {
		PlaneBullet bullet;
		bullet.x = plane.x;
		bullet.y = plane.y;
		plane.bullets.push_back(bullet);

		plane.lastShootTime = currentTime;
	}

	for (auto it = plane.bullets.begin(); it != plane.bullets.end();) {
		it->x -= 5.5;

		if (isColliding(*it, player)) {
			player.health -= it->damage;
			it = plane.bullets.erase(it);
		}
		else {
			++it;
		}
	}
	
	if (player.health == 0)
	{
		gameOver();
	}

	if (playerScore >= 500)
	{
		cihuy -= 1;
	}
}

void showPlaneBullets(const Plane& plane) {
	for (const auto& bullet : plane.bullets) {
		slSprite(bullet.bulletTexture, bullet.x, bullet.y - 10, 32, 32);
	}
}