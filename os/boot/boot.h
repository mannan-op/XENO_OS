#pragma once

#include<iostream>
#include <vector>
#include <string>
#include "raylib.h"

using namespace std;

class BootScreen {
private:
	vector<string> messages;
	float timer;
	int visibleMessages;

public:
	BootScreen();

	void update(float dt);
	void draw();
	bool isFinished() const;
};