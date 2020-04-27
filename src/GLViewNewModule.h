#include "GLView.h"
#include "irrKlang.h"
#include "NetMessengerClient.h"
#include "WO.h"
#include "WOFTGLString.h"
#include "../cwin64/WObanana.h"
//#include "PxPhysicsAPI.h"
//#include "../cwin64/PhysX.h"

using namespace irrklang;
using namespace std;
namespace Aftr
{
   class Camera;
   class IPhysicsManager;

class GLViewNewModule : public GLView
{
public:
   WObanana* banana;
   ISoundSource* collect;
   WO* exit;
   WO* k1;
   WO* k2;
   WO* k3; 
   WO* k4;
   WO* k5;
   WO* k6;
   int key_count = 0;
   WOFTGLString* worldTxt;
   //std::map<PhysX*, int> bananas;
   int numBananas = 0;

   static GLViewNewModule* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewNewModule();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void createNewModuleWayPoints();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );
   float move = 0;
   void adjust(const Vector& x) { banana->moveRelative(x); };
   Vector getLookDirection() { return banana->getLookDirection(); };

   void KeyPress(const SDL_Keycode& key);
   //void Bananer(Vector pos = Vector(0, 0, 0));


protected:
   WOFTGLString* getInitialWorldTxt();
   GLViewNewModule( const std::vector< std::string >& args );
   virtual void onCreate();   


   ISoundEngine* Engine = nullptr;
   char getShift(char c);
   void type(const SDL_Keycode& key);
   bool typing = false;
   bool shift = false;


private:
    bool isMoving();
    NetMessengerClient* net;
    string initTxt;
    //PxPhysicsManager* physics;
};

} //namespace Aftr