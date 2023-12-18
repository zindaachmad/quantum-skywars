#include <sl.h>
#include <iostream>
#include <vector>

bool explosionSoundPlayed = false;
double playerScore = 0;

struct Player
{
	double x = 400;
	double y = 360;
	int playerTexture = slLoadTexture("Assets\\player.png");
};

struct Bullet
{
	Player player;

	double x = player.x;
	double y = player.y;
	double damage = 10;
	int bulletTexture = slLoadTexture("Assets\\bullet.png");
};

struct Helicopter
{
	double x = 1000;
	double y = 360;
	double health = 50;
	int helicopterTexture = slLoadTexture("Assets\\helicopter.png");
};

void soundEffects()
{
	int shootSound = slLoadWAV("SFX\\shoot.wav");
	slSoundPlay(shootSound);
}

void updatePlayer(Player& player)
{
	double mouseX = slGetMouseX();
	double mouseY = slGetMouseY();

	double deltaX = mouseX - player.x;
	double deltaY = mouseY - player.y;

	double angle = atan2(deltaY, deltaX);

	double speed = 10.0;

	player.x += cos(angle) * speed;
	player.y += sin(angle) * speed;
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
		soundEffects();

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

void respawnHelicopter(Helicopter& Helicopter)
{
	const int minY = 100;
	Helicopter.x = 1400;
	Helicopter.y = std::rand() % (720 - minY);
	Helicopter.health = 50;
	explosionSoundPlayed = false;
}

void updateHelicopter(Helicopter& helicopter, std::vector<Bullet>& bullets)
{
	double speed = 0.97;
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
			slSoundPlay(explode);
			explosionSoundPlayed = true;
			playerScore += 10;
		}

		respawnHelicopter(helicopter);
	}
}

void showHelicopter(const Helicopter& Helicopter)
{
	slSprite(Helicopter.helicopterTexture, static_cast<int>(round(Helicopter.x)), static_cast<int>(round(Helicopter.y)), 128, 70);
}

int main()
{
	slWindow(1280, 720, "Project S", false);

	Player player;
	Helicopter Helicopter;

	std::vector<Bullet> bullets;
	int mainBackground = slLoadTexture("Assets\\all2.png");

	double lastShootTime = 0.0;
	double shootInterval = 0.1;
	double xbg = 800;

	slShowCursor(false);

	while (!slShouldClose())
	{
		slSprite(mainBackground, xbg, 400, 3000, 840);
		int pixelFont = slLoadFont("Assets\\pixel.ttf");

		updatePlayer(player);
		showPlayer(player);

		updateHelicopter(Helicopter, bullets);
		showHelicopter(Helicopter);

		char pointsString[32];
		snprintf(pointsString, sizeof(pointsString), "Score: %.1f", playerScore);
		slText(50, 650, pointsString);

		slSetFont(pixelFont, 40);

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

		xbg -= 0.2;
	}
}