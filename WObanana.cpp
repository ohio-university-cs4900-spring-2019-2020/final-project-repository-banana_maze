#include "WObanana.h"
#ifdef AFTR_CONFIG_USE_BOOST

#include "Camera.h"
#include "Model.h"
#include <complex>
#include "Mat4Fwd.h"
#include <irrKlang.h>

using namespace Aftr;
using namespace irrklang;

WObanana* WObanana::New(const std::string modelFilename, Vector scale, MESH_SHADING_TYPE shadingType, std::string label, Vector pos) {
	return new WObanana(modelFilename, scale, shadingType, label, pos);
}

WObanana::WObanana(const std::string modelFileName, Vector scale, MESH_SHADING_TYPE shadingType, std::string label, Vector pos) {
	this->speed = 0.5;
	this->roll = 0;
	this->driver = nullptr;
	this->banana = WO::New(modelFileName, scale, shadingType);
	this->banana->setLabel(label);
	this->banana->setPosition(pos);
}

WObanana::~WObanana() {}

void WObanana::setDriver(Camera* driver) {
	this->driver = driver;
	if (driver != nullptr) {
		this->driver->setPosition(this->getPosition() + Vector(-10, 0, this->calcVert()));
		this->driver->setParentWorldObject(this->banana);
	}
	else {
		this->keysPressed.clear();
	}
}

void WObanana::onKeyDown(const SDL_KeyboardEvent& key) {
	SDL_Keycode keyDown = key.keysym.sym;
	if (keyDown == SDLK_PLUS || keyDown == SDLK_EQUALS) {
		if (this->speed < 10) {
			this->speed += 0.1;
		}
		else if (this->speed < 0 || this->speed >10) {
			this->speed = 0;
		}
	}
	if (keyDown == SDLK_MINUS) {
		if (this->speed > 0) {
			this->speed -= 0.1; 
		}else if(this->speed < 0 || this->speed > 10) {
			this->speed = 0;
		}
	}
	if (isMovementKey(keyDown)) {
		std::set<SDL_Keycode>::iterator found = keysPressed.find(keyDown);
		if (found == keysPressed.end()) {
			keysPressed.insert(keyDown);
		}
	}
}

void WObanana::onKeyUp(const SDL_KeyboardEvent& key) {
	SDL_Keycode keyUp = key.keysym.sym;
	if (isMovementKey(keyUp)) {
		std::set<SDL_Keycode>::iterator found = keysPressed.find(keyUp);
		if (found != keysPressed.end()) {
			keysPressed.erase(found);
		}
	}
}

void WObanana::onMouseDown(const SDL_MouseButtonEvent& e) {}
void WObanana::onMouseWheelScroll(const SDL_MouseWheelEvent& e) { this->calcZoom(e); }
void WObanana::onMouseUp(const SDL_MouseButtonEvent& e) {}
void WObanana::onMouseMove(const SDL_MouseMotionEvent& e) {}

void WObanana::calcZoom(const SDL_MouseWheelEvent& e) {
	float zoomLVL = e.direction * 0.1f;
	this->distanceFrombanana -= zoomLVL;
}

float WObanana::calcHori() {
	return this->distanceFrombanana * std::cos(this->Rads(this->pitch));
}
float WObanana::calcVert() {
	return this->distanceFrombanana * std::sin(this->Rads(this->pitch));
}

void WObanana::calcCamPos(float hori, float vert) {
	float theta = this->Degs(this->getLookDirection().y) + angleAround;
	float offX = hori * std::sin(this->Rads(theta));
	float offY = hori * std::cos(this->Rads(theta));
	Vector pos = this->getPosition();
	if (this->hasDriver()) {
		this->driver->setPosition(pos.x - 10, pos.y + 0, pos.z + vert);
	}
	this->yaw = 180 - (this->Degs(this->getLookDirection().y) + angleAround);
}

void WObanana::update() {
	for (std::set<SDL_Keycode>::iterator it = this->keysPressed.begin(); it != this->keysPressed.end(); it++) {
		if (*it == SDLK_UP) {
			this->move(this->speed);
		}
		if (*it == SDLK_LEFT) {
			this->banana->rotateAboutRelZ(0.1f);
		}
		if (*it == SDLK_DOWN) {
			this->back(this->speed);
		}
		if (*it == SDLK_RIGHT) {
			this->banana->rotateAboutRelZ(-0.1f);
		}
	}
	this->driver->attachCameraToWO(this->banana, this->banana->getPosition() + Vector(0, 0, 4));
}

void WObanana::move(float distance) {
	for (int i = 0; i < distance; i++) {
		this->moveRelative(this->getLookDirection());
	}
}

void WObanana::back(float distance) {
	for (int i = 0; i < distance; i++ ) {
		this->moveRelative(this->getLookDirection() * -1);
	}
}

bool WObanana::isMovementKey(SDL_Keycode key) {
	return (key == SDLK_UP || key == SDLK_LEFT || key == SDLK_DOWN || key == SDLK_RIGHT
		|| key == SDLK_w || key == SDLK_a || key == SDLK_s || key == SDLK_d);
}

bool WObanana::isMoving() {
	return this->keysPressed.find(SDLK_UP) != this->keysPressed.end()
		|| this->keysPressed.find(SDLK_w) != this->keysPressed.end()
		|| this->keysPressed.find(SDLK_DOWN) != this->keysPressed.end()
		|| this->keysPressed.find(SDLK_s) != this->keysPressed.end();
}

float WObanana::Rads(float deg) {
	return std::tan((deg * Aftr::PI) / 180);
}
float WObanana::Degs(float rad) {
	return rad * (180 / Aftr::PI);
}
#endif