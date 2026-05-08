#pragma once
#include"raylib.h"
#include<string>

using namespace std;

class LockScreen {
	string inputPassword;
	string correctPassword;
	string errorMessage;
	bool unlocked;
	Texture2D backgroundTexture;
	bool backgroundReady;
	float errorTimer;

	void ensureBackground();

public:
	LockScreen();
	~LockScreen();

	void update(float dt);
	void draw();

	bool isUnlocked();
};