#include <sl.h>
#include <iostream>
#include <vector>

bool explosionSoundPlayed = false;
double playerScore = 0;

struct Player
{
	double x = 400;
	double y = 360;
	double health = 100;
	int playerTexture = slLoadTexture("Assets\\player.png");
};

struct Bullet
{
	Player player;

	double x = player.x;
	double y = player.y;
	double damage = 15;
	int bulletTexture = slLoadTexture("Assets\\bullet.png");
};

struct Helicopter
{
	double x = 1500;
	double y = std::rand() % (600 - 200 + 1) + 100;
	double health = 30;
	int helicopterTexture = slLoadTexture("Assets\\helicopter.png");
};

struct PlaneBullet {
	double x;
	double y;
	double damage = 10;
	int bulletTexture = slLoadTexture("Assets\\bullet.png");
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

void soundEffects()
{
	int shootSound = slLoadWAV("SFX\\shoot.wav");
	slSoundPlay(shootSound);
}

void updatePlayer(Player& player, std::vector<Bullet>& bullets) {
	double mouseX = slGetMouseX();
	double mouseY = slGetMouseY();

	double deltaX = mouseX - player.x;
	double deltaY = mouseY - player.y;

	double angle = atan2(deltaY, deltaX);

	double speed = 10.0;

	player.x += cos(angle) * speed;
	player.y += sin(angle) * speed;
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
	slSprite(bullet.bulletTexture, bullet.x, bullet.y, 128, 128);
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
	double speed = 1.9;
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
		if (!explosionSoundPlayed)
		{
			int explode = slLoadWAV("SFX\\explosion.wav");
			playerScore += 10;
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
	double speed = 2.5;
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
			playerScore += 10;
		}
		respawnPlane(plane); 
	}

	if (plane.x <= -100)
	{
		respawnPlane(plane);
	}
}

void showPlane(const Plane& plane) {
	slSprite(plane.planeTexture, static_cast<int>(round(plane.x)), static_cast<int>(round(plane.y)), 180, 80);
}

void updatePlaneBullets(Plane& plane) {
	double currentTime = slGetTime();
	double shootInterval = 2.5; // Adjust the interval as needed

	if (currentTime - plane.lastShootTime >= shootInterval) {
		PlaneBullet bullet;
		bullet.x = plane.x;
		bullet.y = plane.y;
		plane.bullets.push_back(bullet);

		plane.lastShootTime = currentTime;
	}

	for (auto& bullet : plane.bullets) {
		bullet.x -= 7.5;
	}
}

void showPlaneBullets(const Plane& plane) {
	for (const auto& bullet : plane.bullets) {
		slSprite(bullet.bulletTexture, bullet.x, bullet.y, 128, 128);
	}
}

int main()
{
	slWindow(1280, 720, "Project S", false);

	Player player;
	Helicopter Helicopter;
	Plane plane;

	std::vector<Bullet> bullets;
	int mainBackground = slLoadTexture("Assets\\all2.png");

	double lastShootTime = 0.0;
	double shootInterval = 0.1;
	double xbg1 = 800;

	slShowCursor(false);

	while (!slShouldClose())
	{
		slSprite(mainBackground, xbg1, 400, 3000, 840);

		updatePlane(plane, bullets);
		showPlane(plane);

		updatePlayer(player, bullets);
		showPlayer(player);

		updateHelicopter(Helicopter, bullets);
		showHelicopter(Helicopter);

		updatePlane(plane, bullets);
		updatePlaneBullets(plane);
		showPlane(plane);
		showPlaneBullets(plane);

		if (slGetKey('X'))
		{
			shootBullet(player, bullets, lastShootTime, shootInterval);
		}

		updateBullets(bullets);
		for (const auto& bullet : bullets)
		{
			showBullet(bullet);
		}


		slRender();

		xbg1 -= 0.7;
	}

	std::cin.get();
}