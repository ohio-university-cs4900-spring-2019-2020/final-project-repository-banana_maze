#pragma once

#include "WO.h"
#include "Model.h"
#include <irrKlang.h>

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class WObanana : public WO {
	public:
		static WObanana* New(
			//default looking in +x direction
			const std::string modelFileName = "../mm/models/banana.dae",
			Vector scale = Vector(1, 1, 1),
			MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstSMOOTH,
			std::string label = "banana",
			Vector pos = Vector (0,0,5)
			);
		~WObanana();

		//keyboard inputs
		void onKeyDown(const SDL_KeyboardEvent& key);
		void onKeyUp(const SDL_KeyboardEvent& key);
		void onMouseWheelScroll(const SDL_MouseWheelEvent& e);
		void onMouseDown(const SDL_MouseButtonEvent& e);
		void onMouseUp(const SDL_MouseButtonEvent& e);
		void onMouseMove(const SDL_MouseMotionEvent& e);

		void setSpeed(float newSpeed) { this->speed = newSpeed;}
		float getSpeed() { return this->speed; }

		Vector getPosition() { return banana->getPosition(); }
		Vector getLookDirection() { return banana->getLookDirection(); }
		Vector getNormalDirection() { return banana->getNormalDirection(); }
		void setPosition(const Vector& newXYZ) { banana->setPosition(newXYZ); }
		void setPosition(float x, float y, float z) { banana->setPosition(Vector(x, y, z)); }
		void setLookDirection(const Vector& lookDirection) { banana->getModel()->setLookDirection(lookDirection); }
		void setNormalDirection(const Vector& normalDirection) { banana->getModel()->setNormalDirection(normalDirection); }
		void moveRelative(const Vector& dxdydz) { banana->moveRelative(dxdydz); }

		//move forward
		void move(float distance = 0.1f);
		//move backward
		void back(float distance = 0.1f);

		void update();
		bool hasDriver() { return driver != nullptr;}
		void setDriver(Camera* newDriver);
		Camera* getDriver() { return driver; }
		WO* getbananaWO() { return banana; }

		bool isMoving();
		float lookAngle = 0;
		float yaw = 0;

		float Rads(float deg);
		float Degs(float rad);


	protected:
		WO* banana;
		Camera* driver;
		std::set<SDL_Keycode> keysPressed;
		float speed, roll;
		float distanceFrombanana = 32.28f;
		float angleAround = 0;
		float pitch = 16.1892f;
		void calcZoom(const SDL_MouseWheelEvent& e);
		float calcHori();
		float calcVert();
		void calcCamPos(float hori, float vert);

		WObanana(
			const std::string modelFileName = "../mm/models/banana.dae",
			Vector scale = Vector(1, 1, 1),
			MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstSMOOTH,
			std::string label = "banana",
			Vector pos = Vector(-50,0,5)
		);
	private:
		bool isMovementKey(SDL_Keycode key);
	};
}
#endif