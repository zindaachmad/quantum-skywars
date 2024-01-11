#include <sl.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include <string>
#include <random>

bool explosionSoundPlayed = false;
void updateExplosions();
void showExplosions(int explodeFrame);
bool gameIsOver = false;
bool gameIsRunning = true;
bool gameIsFinished = false;

int bulletShotSound;
int explodeSound;
int collectSound;
int hitSound;
int hurtSound;
int mainSound;

const double minY = 50.0;
const double maxY = 700.0;
const double minX = 75.0;
const double maxX = 1200.0;

double playerScore = 0;

double getRandomX() {
	return static_cast<double>(960);
}

double getRandomY() {
	return static_cast<double>(std::rand() % (500 - 300 + 1) + 500);
}

struct Player
{
	double x = -400;
	double y = 360;
	double health = 100;
	double maxHealth = 100;
	double targetX = 0;
	double targetY = 0;
	double smoothDelay = 0.075;
	int playerTexture = slLoadTexture("Assets\\player.png");
	double shootInterval = 0.3;
};

struct Rapid
{
	double x = rand() % (1800 - 200 + 1) + 200;
	double y = 3000;
	int texture = slLoadTexture("Assets\\rapid.png");
	bool isActive;
	int count = 0;
};

struct Heal
{
	double x = rand() % (1800 - 200 + 1) + 200;
	double y = 3000;
	int texture = slLoadTexture("Assets\\heal.png");
	bool isActive;
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
	double x = 2100;
	double y = std::rand() % (1000 - 400 + 1) + 400;
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
	double x = 2100;
	double y = std::rand() % (1000 - 400 + 1) + 400;
	double health = 60;
	double lastShootTime = 0.0;
	double speed = 2;

	int planeTexture = slLoadTexture("Assets\\plane1.png");
	std::vector<PlaneBullet> bullets;
};

struct TankBullet
{
	double x = 0;
	double y = 0;
	double damage = 50;
	int tankBulletTexture = slLoadTexture("Assets\\tankbullet.png");
	double speed = 1.0;
};

struct Tank
{
	double x = 3000;
	double y = 190;
	double health = 300;
	int tankTexture = slLoadTexture("Assets\\tank.png");

	std::vector<TankBullet> bullets;

	double initialPlayerX;
	double initialPlayerY;
};

struct Explosion
{
	double x, y;
	double animationDelay;
	bool active;
};

void initBulletShotSound() {
	bulletShotSound = slLoadWAV("Assets\\shoot2.wav");
}

void initExplodeSound() {
	explodeSound = slLoadWAV("Assets\\explosion.wav");
}

void initHitSound() {
	hitSound = slLoadWAV("Assets\\hit.wav");
}

void initHurtSound()
{
	hurtSound = slLoadWAV("Assets\\hurt.wav");
}

void initMainSound()
{
	mainSound = slLoadWAV("Assets\\aduh.wav");
}

void initCollectSound()
{
	collectSound = slLoadWAV("Assets\\collect.wav");
}

bool gameOver(Player& player, Helicopter& Helicopter, Plane& plane, Tank& tank, Rapid& rapid, Heal& heal, std::vector<Bullet>& bullets);
bool gameFinished(Player& player, Helicopter& Helicopter, Plane& plane, Tank& tank, Rapid& rapid, Heal& heal, std::vector<Bullet>& bullets);
void resetGame(Player& player, Helicopter& Helicopter, Plane& plane, Tank& tank, Rapid& rapid, Heal& heal, std::vector<Bullet>& bullets);
bool isColliding(const Bullet& bullet, const Tank& tank);
bool isColliding(const Bullet& bullet, const Player& player);
bool isColliding(const Bullet& bullet, const Helicopter& helicopter);
bool isColliding(const Bullet& bullet, const Plane& plane);
bool isColliding(const TankBullet& bullet, const Player& player);
bool isColliding(const PlaneBullet& bullet, const Player& player);
bool isColliding(const Heal& heal, const Player& player);
void showHeal(Heal& heal);
void updateHeal(Heal& heal, Player& player);
bool isColliding(const Rapid& rapid, const Player& player);
void showRapid(Rapid& rapid);
void updateRapid(Rapid& rapid, Player& player);
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

std::vector<Explosion> explode;
std::vector<int> explosions;

int main()
{
	slWindow(1920, 1080, "Project S", false);

	Player player;
	Helicopter Helicopter;
	Plane plane;
	Tank tank;
	Rapid rapid;
	Heal heal;

	initBulletShotSound();
	initExplodeSound();
	initHitSound();
	initMainSound();
	initCollectSound();

	std::vector<Bullet> bullets;

	std::random_device rd;
	explosions;
	{
	explosions.push_back(slLoadTexture("Assets\\explode_0.png"));
	explosions.push_back(slLoadTexture("Assets\\explode_1.png"));
	explosions.push_back(slLoadTexture("Assets\\explode_2.png"));
	explosions.push_back(slLoadTexture("Assets\\explode_3.png"));
	explosions.push_back(slLoadTexture("Assets\\explode_4.png"));
	explosions.push_back(slLoadTexture("Assets\\explode_5.png"));
	explosions.push_back(slLoadTexture("Assets\\explode_6.png"));
	explosions.push_back(slLoadTexture("Assets\\explode_7.png"));
	explosions.push_back(slLoadTexture("Assets\\explode_8.png"));
	explosions.push_back(slLoadTexture("Assets\\explode_9.png"));
	}

	Explosion exp;
	exp.x = 500;
	exp.y = 500;

	int explodeFrame = 0;
	double delay = 0.2;

	double lastExplodeFrameTime = 0.0;
	double explodeFrameDelay = 0.01;

	int mainFont = slLoadFont("Assets\\pixel.ttf");

	int bg1 = slLoadTexture("Assets\\sky.png");
	int bg2 = slLoadTexture("Assets\\1.png");
	int bg3 = slLoadTexture("Assets\\2.png");
	int bg4 = slLoadTexture("Assets\\3.png");
	int bg5 = slLoadTexture("Assets\\4.png");

	int ui = slLoadTexture("Assets\\ui.png");

	double backgroundX1 = 2100;
	double backgroundX2 = 2100;
	double backgroundX3 = 2100;
	double backgroundX4 = 2100;
	double foregroundX = 2100;

	double backgroundMove1 = 0.3;
	double backgroundMove2 = 0.5;
	double backgroundMove3 = 0.7;
	double backgroundMove4 = 0.9;
	double foregroundMove = 1.1;

	double lastShootTime = 0.0;
	double shootInterval = player.shootInterval;

	double tankLastShootTime = 0.0;
	double tankShootInterval = 3;

	slSetFont(mainFont, 25);
	slShowCursor(false);

	slSoundLoop(mainSound);

	while (!slShouldClose())
	{

		if (gameIsRunning)
		{
			slSprite(bg1, backgroundX1 + 4200, 600, 4200, 1120);
			slSprite(bg1, backgroundX1, 600, 4200, 1120);

			slSprite(bg2, backgroundX2 + 4200, 540, 4200, 1120);
			slSprite(bg2, backgroundX2, 540, 4200, 1120);

			slSprite(bg3, backgroundX3 + 4200, 540, 4200, 1120);
			slSprite(bg3, backgroundX3, 540, 4200, 1120);

			slSprite(bg4, backgroundX4 + 4200, 580, 4200, 1120);
			slSprite(bg4, backgroundX4, 580, 4200, 1120);

			if (foregroundX >= 500)
			{
				showRapid(rapid);
				updateRapid(rapid, player);
			}

			if (backgroundX4 >= 600)
			{
				showHeal(heal);
				updateHeal(heal, player);
			}

			updateTank(tank, bullets, player);
			showTank(tank);

			updateTankBullets(tank, player);
			showTankBullets(tank);

			slSprite(bg5, foregroundX + 4200, 540, 4200, 1120);
			slSprite(bg5, foregroundX, 540, 4200, 1120);

			slSprite(ui, 960, 570, 2200, 1105.92);

			double currentTime = slGetTime();

			char pointsString[200];
			snprintf(pointsString, sizeof(pointsString), "Score: %.1f/5000", playerScore);
			slText(50, 1030, pointsString);

			char healthString[200];
			snprintf(healthString, sizeof(healthString), "HP: %.1f/100.0", player.health);
			slText(1550, 1030, healthString);

			slText(700, 1030, "Quantum Skywars 2.0");

			updateExplosions();
			showExplosions(explodeFrame);

			if (currentTime - lastExplodeFrameTime >= explodeFrameDelay)
			{
				explodeFrame = (explodeFrame + 1) % explosions.size();
				lastExplodeFrameTime = currentTime;
			}

			updatePlayer(player, bullets);
			showPlayer(player);

			updateHelicopter(Helicopter, bullets);
			showHelicopter(Helicopter);

			showPlane(plane);
			showPlaneBullets(plane);

			updatePlane(plane, bullets);
			updatePlaneBullets(plane, player);


			if (currentTime - tankLastShootTime >= tankShootInterval)
			{
				shootTankBullet(tank, tankLastShootTime, tankShootInterval, player);
			}

			if (playerScore >= 1500)
				tankShootInterval = 2;

			if (playerScore >= 3000)
				tankShootInterval = 1.5;

			if (slGetMouseButton(SL_MOUSE_BUTTON_1))
			{
				shootBullet(player, bullets, lastShootTime, player.shootInterval);
			}

			updateBullets(bullets);
			for (const auto& bullet : bullets)
			{
				showBullet(bullet);
			}

			slRender();

			backgroundX1 -= backgroundMove1;
			backgroundX2 -= backgroundMove2;
			backgroundX3 -= backgroundMove3;
			backgroundX4 -= backgroundMove4;
			foregroundX -= foregroundMove;

			if (backgroundX1 <= -2100) {
				backgroundX1 += 2100 * 2;
			}
			if (backgroundX2 <= -2100) {
				backgroundX2 += 2100 * 2;
			}
			if (backgroundX3 <= -2100) {
				backgroundX3 += 2100 * 2;
			}
			if (backgroundX4 <= -2100) {
				backgroundX4 += 2100 * 2;
			}
			if (foregroundX <= -2100) {
				foregroundX += 2100 * 2;
			}

			if (slGetKey(SL_KEY_ESCAPE))
			{
				slClose();
			}

			if (playerScore == 100 && player.health > 0) {
				gameFinished(player, Helicopter, plane, tank, rapid, heal, bullets);
				gameIsRunning = false;
			}

		}
		else if (player.health <= 0)
		{
			gameOver(player, Helicopter, plane, tank, rapid, heal, bullets);
			gameIsRunning = false;
		}

	}

	std::cin.get();
}

bool gameFinished(Player& player, Helicopter& Helicopter, Plane& plane, Tank& tank, Rapid& rapid, Heal& heal, std::vector<Bullet>& bullets)
{
	int mainFont = slLoadFont("Assets\\pixel.ttf");
	int bg1 = slLoadTexture("Assets\\sky.png");
	int bg2 = slLoadTexture("Assets\\1.png");
	int bg3 = slLoadTexture("Assets\\2.png");
	int bg4 = slLoadTexture("Assets\\3.png");
	int bg5 = slLoadTexture("Assets\\4.png");

	slSoundStop(mainSound);

	gameIsOver = false;
	gameIsFinished = true;


	slSprite(bg1, 2100, 600, 4200, 1120);
	slSprite(bg2, 2100, 540, 4200, 1120);
	slSprite(bg3, 2100, 540, 4200, 1120);
	slSprite(bg4, 2100, 600, 4200, 1120);
	slSprite(bg5, 2100, 540, 4200, 1120);

	slSetTextAlign(SL_ALIGN_CENTER);
	slSetFontSize(70);
	slText(960, 540, "Congratulations, You Win!");

	slSetTextAlign(0);
	char pointsString[200];
	snprintf(pointsString, sizeof(pointsString), "Your Score: %.1f", playerScore);
	slSetFontSize(30);
	slText(960, 480, pointsString);

	slText(960, 300, "Code, Audio, Graphics: Zinda Achmad Muzaki\nAudio: Tito Alla Khairi\nGraphics: Yazid Akmal Adyatma\nGraphics: Zacky Abdul Muin");

	slRender();

	return false;
}

bool gameOver(Player& player, Helicopter& Helicopter, Plane& plane, Tank& tank, Rapid& rapid, Heal& heal, std::vector<Bullet>& bullets)
{
	int mainFont = slLoadFont("Assets\\pixel.ttf");
	int bg1 = slLoadTexture("Assets\\sky.png");
	int bg2 = slLoadTexture("Assets\\1.png");
	int bg3 = slLoadTexture("Assets\\2.png");
	int bg4 = slLoadTexture("Assets\\3.png");
	int bg5 = slLoadTexture("Assets\\4.png");

	slSoundStop(mainSound);

	gameIsOver = true;


	slSprite(bg1, 2100, 600, 4200, 1120);
	slSprite(bg2, 2100, 540, 4200, 1120);
	slSprite(bg3, 2100, 540, 4200, 1120);
	slSprite(bg4, 2100, 600, 4200, 1120);
	slSprite(bg5, 2100, 540, 4200, 1120);

	slSetTextAlign(SL_ALIGN_CENTER);
	slSetFontSize(70);
	slText(960, 540, "Game Over");

	char pointsString[200];
	snprintf(pointsString, sizeof(pointsString), "Your Score: %.1f", playerScore);
	slSetFontSize(30);
	slText(960, 480, pointsString);

	slRender();

	return true;
}

void resetGame(Player& player, Helicopter& Helicopter, Plane& plane, Tank& tank, Rapid& rapid, Heal& heal, std::vector<Bullet>& bullets) {

	gameIsOver = false;
	gameIsRunning = true;
	playerScore = 0;
	player.health = 100;
	slSetFontSize(25);

	bullets.clear();

}

bool isColliding(const Heal& heal, const Player& player)
{
	return (heal.x < player.x + 32 &&
		heal.x + 32 > player.x &&
		heal.y < player.y + 32 &&
		heal.y + 32 > player.y);
}

void showHeal(Heal& heal)
{
	if (heal.isActive)
	{
		slSprite(heal.texture, heal.x, heal.y, 120, 90);
	}
}

void updateHeal(Heal& heal, Player& player)
{
	heal.y -= 1;

	if (heal.isActive && isColliding(heal, player))
	{
		slSoundPlay(collectSound);
		if (player.health > 70)
		{
			player.health = player.maxHealth;
		}
		else
		{
			player.health += 30;
		}

		heal.isActive = false;
	}

	if (heal.y <= -100)
	{
		heal.y = 2000;
		heal.isActive = true;
	}
}

bool isColliding(const Rapid& rapid, const Player& player)
{
	return (rapid.x < player.x + 32 &&
		rapid.x + 32 > player.x &&
		rapid.y < player.y + 32 &&
		rapid.y + 32 > player.y);
}

void showRapid(Rapid& rapid)
{
	if (rapid.isActive)
	{
		slSprite(rapid.texture, rapid.x, rapid.y, 120, 90);
	}
}

void updateRapid(Rapid& rapid, Player& player) 
{
	rapid.y -= 1;

	if (rapid.count <= 3) {

		if (rapid.isActive && isColliding(rapid, player))
		{
			slSoundPlay(collectSound);
			player.shootInterval -= 0.05;
			rapid.isActive = false;
			rapid.count += 1;
		}

		if (rapid.y <= -100)
		{
			rapid.y = 4000;
			rapid.isActive = true;
		}
	}
}

void updateExplosions()
{
	for (auto& exp : explode)
	{
		exp.animationDelay -= slGetDeltaTime();

		if (exp.animationDelay <= 0)
		{
			exp.active = false;
		}
	}
	explode.erase(std::remove_if(explode.begin(), explode.end(), [](const Explosion& exp) { return !exp.active; }), explode.end());
}

void showExplosions(int explodeFrame)
{
	for (const auto& exp : explode)
	{
		slSprite(explosions[explodeFrame], exp.x, exp.y, 400, 400);
	}
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
			it->x += tan(angle) * it->speed;
			it->y += gravity;

			if (it->x < 0 || it->y < 0 || it->x > 1920 || it->y > 1080)
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
	tank.x = 2500;
	tank.y = 190;
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
			slSoundPlay(hitSound);
		}
		else
		{
			++it;
		}
	}

	updateTankBullets(tank, player);

	if (tank.health <= 0) {
		if (!explosionSoundPlayed) {
			slSoundPlay(explodeSound);
			playerScore += 100;

			Explosion explosion;
			explosion.x = tank.x;
			explosion.y = tank.y;
			explosion.animationDelay = 0.1;
			explosion.active = true;
			explode.push_back(explosion);
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
	const double minY = 100.0;
	const double maxY = 980.0;

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
	slSprite(player.playerTexture, static_cast<int>(round(player.x)), static_cast<int>(round(player.y)), 256, 256);
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

		slSoundPlay(bulletShotSound);

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
	Helicopter.x = 2100;
	Helicopter.y = std::rand() % (900 - 400 + 1) + 400;
	Helicopter.health = 100;

	explosionSoundPlayed = false;
}

void updateHelicopter(Helicopter& helicopter, std::vector<Bullet>& bullets)
{
	helicopter.x -= 4;

	for (auto it = bullets.begin(); it != bullets.end();)
	{
		if (isColliding(*it, helicopter))
		{
			helicopter.health -= it->damage;
			it = bullets.erase(it);
			slSoundPlay(hitSound);
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
			slSoundPlay(explodeSound);
		}

		Explosion explosion;
		explosion.x = helicopter.x;
		explosion.y = helicopter.y;
		explosion.animationDelay = 0.1;
		explosion.active = true;
		explode.push_back(explosion);

		respawnHelicopter(helicopter);
	}

	if (helicopter.x <= -100)
	{
		respawnHelicopter(helicopter);
	}
}

void showHelicopter(const Helicopter& Helicopter)
{
	slSprite(Helicopter.helicopterTexture, static_cast<int>(round(Helicopter.x)), static_cast<int>(round(Helicopter.y)), 150, 75);
}

void respawnPlane(Plane& plane) {

	plane.x = 2100;
	plane.y = std::rand() % (900 - 400 + 1) + 400;
	plane.health = 60;

	explosionSoundPlayed = false;
}

void updatePlane(Plane& plane, std::vector<Bullet>& bullets) {

	plane.x -= plane.speed;

	for (auto it = bullets.begin(); it != bullets.end();) {
		if (isColliding(*it, plane)) {
			plane.health -= it->damage;
			it = bullets.erase(it);
			slSoundPlay(hitSound);
		}
		else {
			++it;
		}
	}
	if (plane.health <= 0) {
		Explosion explosion;
		explosion.x = plane.x;
		explosion.y = plane.y;
		explosion.animationDelay = 0.1;
		explosion.active = true;
		explode.push_back(explosion);

		if (!explosionSoundPlayed) {
			slSoundPlay(explodeSound);
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
	slSprite(plane.planeTexture, static_cast<int>(round(plane.x)), static_cast<int>(round(plane.y)), 140, 60);
}

void updatePlaneBullets(Plane& plane, Player& player) {

	double currentTime = slGetTime();
	double shootInterval = 2.5;

	if (currentTime - plane.lastShootTime >= shootInterval) 
	{
		PlaneBullet bullet;
		bullet.x = plane.x;
		bullet.y = plane.y;
		plane.bullets.push_back(bullet);

		plane.lastShootTime = currentTime;
	}

	for (auto it = plane.bullets.begin(); it != plane.bullets.end();) {
		it->x -= 7;

		if (isColliding(*it, player)) {
			player.health -= it->damage;
			it = plane.bullets.erase(it);
			slSoundPlay(hurtSound);
		}
		else {
			++it;
		}
	}

	if (player.health == 0)
	{
		gameIsRunning = false;
	}

	if (playerScore >= 500)
	{
		shootInterval = 1.5;
	}

	if (playerScore >= 1500) 
	{
		shootInterval = 1;
	}
}

void showPlaneBullets(const Plane& plane) {

	for (const auto& bullet : plane.bullets) {
		slSprite(bullet.bulletTexture, bullet.x, bullet.y - 10, 32, 32);
	}
}