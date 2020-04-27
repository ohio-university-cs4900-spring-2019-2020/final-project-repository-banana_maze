#include "GLViewNewModule.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"
#include "AftrGLRendererBase.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "AftrGLRendererBase.h"
#include "../cwin64/WObanana.h"
#include "../cwin64/NetMsgBanana.h"

//If we want to use way points, we need to include this.
#include "NewModuleWayPoints.h"
#include <irrKlang.h>
#include <Windows.h>
#include <conio.h>
#include "../cwin64/NetMsgTxt.h"
#include "../cwin64/NetMsgBananer.h"

using namespace irrklang;
using namespace Aftr;
WOWayPointAbstract* key1;
WOWayPointAbstract* key2;
WOWayPointAbstract* key3; 
WOWayPointAbstract* key4; 
WOWayPointAbstract* key5;
WOWayPointAbstract* key6;
//using namespace physx;

const std::string sharedMM = ManagerEnvironmentConfiguration::getSMM();

GLViewNewModule* GLViewNewModule::New( const std::vector< std::string >& args )
{
   GLViewNewModule* glv = new GLViewNewModule( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}


GLViewNewModule::GLViewNewModule( const std::vector< std::string >& args ) : GLView( args )
{
   //ISoundEngine* Engine = createIrrKlangDevice();
   //Initialize any member variables that need to be used inside of LoadMap() here.
   //Note: At this point, the Managers are not yet initialized. The Engine initialization
   //occurs immediately after this method returns (see GLViewNewModule::New() for
   //reference). Then the engine invoke's GLView::loadMap() for this module.
   //After loadMap() returns, GLView::onCreate is finally invoked.

   //The order of execution of a module startup:
   //GLView::New() is invoked:
   //    calls GLView::init()
   //       calls GLView::loadMap() (as well as initializing the engine's Managers)
   //    calls GLView::onCreate()

   //GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
}


void GLViewNewModule::onCreate()
{
   //GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
   //At this point, all the managers are initialized. That is, the engine is fully initialized.
   
    this->Engine = createIrrKlangDevice();
    if (this->pe != NULL)
    {
        //optionally, change gravity direction and magnitude here
        //The user could load these values from the module's aftr.conf
        this->pe->setGravityNormalizedVector(Vector(0, 0, -1.0f));
        this->pe->setGravityScalar(Aftr::GRAVITY);
    }
    //this->setActorChaseType(STANDARDEZNAV);
    //this->physics = PxPhysicsManager::New();
    this->banana = WObanana::New();
    worldLst->push_back(this->banana->getbananaWO());
    /*
    //Play the 2D music (background) - Banana Phone song - from youtube - link in assignment video
                    and 3D music (on location) - Given Soundtrack
    ISoundSource* noise1 = Engine->addSoundSourceFromFile("../irrKlang/irrKlang-64bit-1.6.0/media/getout.ogg");
    noise1->setDefaultVolume(0.9f);
    //call the pointer to play the 3D music
    ISound* music2 = Engine->play3D(noise1, vec3df(50, 50, 50), true);
    //music->setIsPaused(false);
    //music->setMinDistance(5.0f);
    */
    //************************************************
    collect = Engine->addSoundSourceFromFile("../irrKlang/irrKlang-64bit-1.6.0/media/keys.mp3");
    collect->setDefaultVolume(10.0f);

    ISoundSource* banana1 = Engine->addSoundSourceFromFile("../irrKlang/irrKlang-64bit-1.6.0/media/Riffi_Bananaphone.mp3");
    banana1->setDefaultVolume(0.3f);
    ISound* music2 = Engine->play2D(banana1, true);

    this->initTxt = "Hit 't' to start driving the car";
    this->worldTxt = this->getInitialWorldTxt();
    worldLst->push_back(this->worldTxt);

    /*
    ISoundSource* victory1 = Engine->addSoundSourceFromFile("../irrKlang/irrKlang-64bit-1.6.0/media/victory.mp3");
    victory1->setDefaultVolume(1.0f);
    ISound* music1 = nullptr;
    music1 = Engine->play3D(victory1, vec3df(410, -250, 8), true);
    */

    //This is where I initialize the multiple ports that is able to run the multiple instances
    if (ManagerEnvironmentConfiguration::getVariableValue("NetServerLisenPort") == "12683") {
        this->net = NetMessengerClient::New("127.0.0.1", "12682");
    }
    else {
        this->net = NetMessengerClient::New("127.0.0.1", "12683");
    }

    //Default is STANDARDEZNAV mode
   //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
}


GLViewNewModule::~GLViewNewModule()
{
   //Implicitly calls GLView::~GLView()
}


void GLViewNewModule::updateWorld()
{
    //set listener position
    Engine->setListenerPosition(
        /*Position*/vec3df(cam->getPosition().x, cam->getPosition().y, cam->getPosition().z),
        /*Look Direction*/vec3df(cam->getLookDirection().x, cam->getLookDirection().y, cam->getLookDirection().z),
        /*Velocity*/vec3df(0, 0, 0),
        /*Normal Direction\upVector*/ vec3df(cam->getNormalDirection().x, cam->getNormalDirection().y, cam->getNormalDirection().z)
    );
    if (key_count == 6) {
        worldLst->eraseViaWOptr(exit);
    }

    if (key1->isTriggered() == 1) {
        ISound* sound = Engine->play2D(collect);
        worldLst->eraseViaWOptr(k1);
        worldLst->eraseViaWOptr(key1);
        key_count++;
    }
    if (key2->isTriggered() == 1) {
        ISound* sound = Engine->play2D(collect);
        worldLst->eraseViaWOptr(k2);
        worldLst->eraseViaWOptr(key2);
        key_count++;
    }
    if (key3->isTriggered() == 1) {
        ISound* sound = Engine->play2D(collect);
        worldLst->eraseViaWOptr(k3);
        worldLst->eraseViaWOptr(key3);
        key_count++;
    }
    if (key4->isTriggered() == 1) {
        ISound* sound = Engine->play2D(collect);
        worldLst->eraseViaWOptr(k4);
        worldLst->eraseViaWOptr(key4);
        key_count++;
    }
    if (key5->isTriggered() == 1) {
        ISound* sound = Engine->play2D(collect);
        worldLst->eraseViaWOptr(k5);
        worldLst->eraseViaWOptr(key5);
        key_count++;
    }
    if (key6->isTriggered() == 1) {
        ISound* sound = Engine->play2D(collect);
        worldLst->eraseViaWOptr(k6);
        worldLst->eraseViaWOptr(key6);
        key_count++;
    }

    Engine->setRolloffFactor(0.1f);
    if (this->isMoving()) {
        this->banana->update();
    }
    if (this->net->isTCPSocketOpen()) {
        if (this->worldTxt->getText() != this->initTxt) {
            this->net->sendNetMsgSynchronousTCP(NetMsgTxt(this->worldTxt->getText()));
        }
        if (this->isMoving()) {
            this->net->sendNetMsgSynchronousTCP(NetMsgBanana(this->banana->getPosition(), this->banana->getLookDirection()));
        }
    }
    //this->physics->simulate(this->net, this->bananas);
    GLView::updateWorld(); //Just call the parent's update world first.
                          //If you want to add additional functionality, do it after
                          //this call.   
}


void GLViewNewModule::onResizeWindow( GLsizei width, GLsizei height )
{
   GLView::onResizeWindow( width, height ); //call parent's resize method.
}


void GLViewNewModule::onMouseDown( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseDown( e );
}


void GLViewNewModule::onMouseUp( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseUp( e );
}


void GLViewNewModule::onMouseMove( const SDL_MouseMotionEvent& e )
{
   GLView::onMouseMove( e );
}


void GLViewNewModule::onKeyDown( const SDL_KeyboardEvent& key )
{
    if (!this->typing) {
        GLView::onKeyDown(key);
            if (key.keysym.sym == SDLK_0)
        this->setNumPhysicsStepsPerRender(1);

        //slide whistle controls
        if (key.keysym.sym == SDLK_COMMA)
        {
            Engine->play2D("../irrKlang/irrKlang-64bit-1.6.0/media/whistle_up.mp3");
        }
        else if (key.keysym.sym == SDLK_PERIOD)
        {
            Engine->play2D("../irrKlang/irrKlang-64bit-1.6.0/media/whistle_down.mp3");
        }
        else if (key.keysym.sym == SDLK_SLASH) {
            Engine->play2D("../irrKlang/irrKlang-64bit-1.6.0/media/keys.mp3");
        }
        //recognizes moving banana
        if (this->isMoving()) { this->banana->onKeyDown(key); }

        //camera controls
        SDL_Keycode pressed = key.keysym.sym;
        if (pressed == SDLK_w && this->cam != nullptr) {
            this->cam->moveInLookDirection(10);
        }
        if (pressed == SDLK_s && this->cam != nullptr) {
            this->cam->moveOppositeLookDirection(10);
        }
        if (pressed == SDLK_a && this->cam != nullptr) {
            this->cam->moveLeft();
            this->cam->moveLeft();
            this->cam->moveLeft();
            this->cam->moveLeft();
            this->cam->moveLeft();
            this->cam->moveLeft();
        }
        if (pressed == SDLK_d && this->cam != nullptr) {
            this->cam->moveRight();
            this->cam->moveRight();
            this->cam->moveRight();
            this->cam->moveRight();
            this->cam->moveRight();
            this->cam->moveRight();
        }
    }
    this->KeyPress(key.keysym.sym);
}


void GLViewNewModule::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );
   if (this->isMoving()) { this->banana->onKeyUp(key); }
}


void Aftr::GLViewNewModule::loadMap()
{
   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   std::vector<std::string> skyBoxImageNames;

   skyBoxImageNames.push_back(sharedMM + "/images/skyboxes/space_Hubble_Nebula+6.jpg");
   {
       ManagerLight::setGlobalAmbientLight(aftrColor4f(4.0f, 3.0f, 2.0f, 1.0f));
       WOLight* light = WOLight::New();
       light->isDirectionalLight(true);
       light->setPosition(Vector(0, 0, 100));
       light->getModel()->setDisplayMatrix(Mat4::rotateIdentityMat({ 0,1,0 }, 90.0f * Aftr::DEGtoRAD));
       light->setLabel("Light");
       worldLst->push_back(light);
   }

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition( 0, 0, 20);

   std::string shinyRedPlasticCube( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );
   std::string wheeledCar( ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl" );
   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   std::string human( ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl" );
      
   //SkyBox Textures readily available
   //skyBoxImageNames.push_back(ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg");
   
   float ga = 0.1f; //Global Ambient Light level for this module
   ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 1.0f ) );
   WOLight* light = WOLight::New();
   light->isDirectionalLight( true );
   light->setPosition( Vector( 100, 100, 100 ) );
   //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
   //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
   light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
   light->setLabel( "Light" );
   worldLst->push_back( light );

   //Create the SkyBox
   WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
   wo->setPosition( Vector( 0,0,0 ) );
   wo->setLabel( "Sky Box" );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   worldLst->push_back( wo );

   ////Create the infinite grass plane (the floor)
   wo = WO::New( grass, Vector( 4, 4, 4 ), MESH_SHADING_TYPE::mstFLAT );
   wo->setPosition( Vector( 0, 0, 0 ) );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
   grassSkin.getMultiTextureSet().at( 0 )->setTextureRepeats( 5.0f );
   grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
   grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
   grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiney" it is)
   grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
   wo->setLabel( "Grass" );
   worldLst->push_back( wo );

   createNewModuleWayPoints();

   k1 = WO::New("../mm/models/key.dae", Vector(0.02f, 0.02f, 0.02f));
   k1->setPosition(Vector(410, -176, 11));
   worldLst->push_back(k1);
   k2 = WO::New("../mm/models/key.dae", Vector(0.02f, 0.02f, 0.02f));
   k2->setPosition(Vector(-25, 410, 11));
   worldLst->push_back(k2);
   k3 = WO::New("../mm/models/key.dae", Vector(0.02f, 0.02f, 0.02f));
   k3->setPosition(Vector(-410, 410, 11));
   worldLst->push_back(k3);
   k4 = WO::New("../mm/models/key.dae", Vector(0.02f, 0.02f, 0.02f));
   k4->setPosition(Vector(-325, -330, 11));
   worldLst->push_back(k4);
   k5 = WO::New("../mm/models/key.dae", Vector(0.02f, 0.02f, 0.02f));
   k5->setPosition(Vector(410, -410, 11));
   worldLst->push_back(k5);
   k6 = WO::New("../mm/models/key.dae", Vector(0.02f, 0.02f, 0.02f));
   k6->setPosition(Vector(30, -300, 11));
   worldLst->push_back(k6);

   //exit area reward :)
   WO* t = WO::New("../mm/models/trophy.dae", Vector(10, 10, 10));
   t->setPosition(Vector(410, -250, 8));
   t->rotateAboutRelZ(-1.575);
   worldLst->push_back(t);
   exit = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   exit->setPosition(Vector(375, -245, 0));
   exit->rotateAboutRelZ(1.575);
   worldLst->push_back(exit);
   //x is red, y is green, z is blue

   //starting area directional walls
   //based off of +x direction
   //north wall
   WO* wall1 = WO::New("../mm/models/wall.dae", Vector(45, 4, 4));
   wall1->setPosition(Vector(450, 0, 0));
   wall1->rotateAboutRelZ(1.575);
   worldLst->push_back(wall1);
   //south wall
   WO* wall2 = WO::New("../mm/models/wall.dae", Vector(45, 4, 4));
   wall2->setPosition(Vector(-450, 0, 0));
   wall2->rotateAboutRelZ(1.575);
   worldLst->push_back(wall2);
   //west wall
   WO* wall3 = WO::New("../mm/models/wall.dae", Vector(45, 4, 4));
   wall3->setPosition(Vector(0, 450, 0));
   wall3->rotateAboutRelZ(0);
   worldLst->push_back(wall3);
   //east wall
   WO* wall4 = WO::New("../mm/models/wall.dae", Vector(45, 4, 4));
   wall4->setPosition(Vector(0, -450, 0));
   wall4->rotateAboutRelZ(0);
   worldLst->push_back(wall4);

   //latitude wall - bottom right angle
   WO* box = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box->setPosition(Vector(20, 60, 0));
   box->rotateAboutRelZ(1.575);
   worldLst->push_back(box);
   //longitude wall
   WO* box1 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box1->setPosition(Vector(59, 24, 0));
   box1->rotateAboutRelZ(0);
   worldLst->push_back(box1);
   //top left
   WO* box2 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box2->setPosition(Vector(-20, -60, 0));
   box2->rotateAboutRelZ(1.575);
   worldLst->push_back(box2);
   WO* box3 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box3->setPosition(Vector(-59, -24, 0));
   box3->rotateAboutRelZ(0);
   worldLst->push_back(box3);
   //top right
   WO* box4 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box4->setPosition(Vector(-20, 60, 0));
   box4->rotateAboutRelZ(1.575);
   worldLst->push_back(box4);
   WO* box5 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box5->setPosition(Vector(-59, 24, 0));
   box->rotateAboutRelZ(0);
   worldLst->push_back(box5);
   //bottom left
   WO* box6 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box6->setPosition(Vector(20, -60, 0));
   box6->rotateAboutRelZ(1.575);
   worldLst->push_back(box6);
   WO* box7 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box7->setPosition(Vector(59, -24, 0));
   box7->rotateAboutRelZ(0);
   worldLst->push_back(box7);
   //end of start area

   WO* p = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   p->setPosition(Vector(145, 0, 0));
   p->rotateAboutRelZ(1.575);
   worldLst->push_back(p);
   WO* p1 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   p1->setPosition(Vector(145, 78, 0));
   p1->rotateAboutRelZ(1.575);
   worldLst->push_back(p1);
   WO* p2 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   p2->setPosition(Vector(145, -114, 0));
   p2->rotateAboutRelZ(1.575);
   worldLst->push_back(p2);
   WO* p3 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   p3->setPosition(Vector(67, -114, 0));
   p3->rotateAboutRelZ(1.575);
   worldLst->push_back(p3);
   WO* p4 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   p4->setPosition(Vector(20, -125, 0));
   p4->rotateAboutRelZ(1.575);
   worldLst->push_back(p4); 
   WO* a = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   a->setPosition(Vector(103, -78, 0));
   a->rotateAboutRelZ(0);
   worldLst->push_back(a);
   WO* a1 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   a1->setPosition(Vector(64, -158, 0));
   a1->rotateAboutRelZ(0);
   worldLst->push_back(a1);
   //********************
   WO* box8 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box8->setPosition(Vector(20, 140, 0));
   box8->rotateAboutRelZ(1.575);
   worldLst->push_back(box8);
   WO* box9 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box9->setPosition(Vector(-24, 176, 0));
   box9->rotateAboutRelZ(0); 
   worldLst->push_back(box9);
   WO* box10 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box10->setPosition(Vector(95, 60, 0)); 
   box10->rotateAboutRelZ(1.575);
   worldLst->push_back(box10);
   WO* box11 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box11->setPosition(Vector(180.5, -36, 0));
   box11->rotateAboutRelZ(0);
   worldLst->push_back(box11);
   WO* box12 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box12->setPosition(Vector(-68.25, -125, 0));
   box12->rotateAboutRelZ(1.575);
   worldLst->push_back(box12);
   WO* box13 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box13->setPosition(Vector(-104.5, -81, 0));
   box13->rotateAboutRelZ(0);
   worldLst->push_back(box13);
   WO* box14 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box14->setPosition(Vector(-24, -161, 0));
   box14->rotateAboutRelZ(0);
   worldLst->push_back(box14);
   WO* box15 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box15->setPosition(Vector(60, 100, 0));
   box15->rotateAboutRelZ(0);
   worldLst->push_back(box15);
   WO* box16 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box16->setPosition(Vector(140, -158, 0));
   box16->rotateAboutRelZ(0);
   worldLst->push_back(box16);
   WO* box17 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box17->setPosition(Vector(224.25, -72, 0));
   box17->rotateAboutRelZ(1.575);
   worldLst->push_back(box17);
   WO* box18 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box18->setPosition(Vector(260.5, -116, 0));
   box18->rotateAboutRelZ(0);
   worldLst->push_back(box18);
   WO* box19 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box19->setPosition(Vector(339, -116, 0));
   box19->rotateAboutRelZ(0);
   worldLst->push_back(box19);
   WO* box20 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box20->setPosition(Vector(375, -158, 0));
   box20->rotateAboutRelZ(1.575);
   worldLst->push_back(box20);
   WO* box21 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box21->setPosition(Vector(339, -202, 0));
   box21->rotateAboutRelZ(0);
   worldLst->push_back(box21);
   WO* box22 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box22->setPosition(Vector(295, -238, 0));
   box22->rotateAboutRelZ(1.575);
   worldLst->push_back(box22);
   WO* box23 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box23->setPosition(Vector(250, -202, 0));
   box23->rotateAboutRelZ(0);
   worldLst->push_back(box23);
   WO* box24 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box24->setPosition(Vector(251.5, -273.5, 0));
   box24->rotateAboutRelZ(0);
   worldLst->push_back(box24);
   WO* box25 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box25->setPosition(Vector(251.5, -350, 0));
   box25->rotateAboutRelZ(0);
   worldLst->push_back(box25);
   WO* box26 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box26->setPosition(Vector(331.5, -350, 0));
   box26->rotateAboutRelZ(0);
   worldLst->push_back(box26);
   WO* box27 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box27->setPosition(Vector(411.5, -350, 0));
   box27->rotateAboutRelZ(0);
   worldLst->push_back(box27);
   WO* box28 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box28->setPosition(Vector(411, -283.5, 0));
   box28->rotateAboutRelZ(0);
   worldLst->push_back(box28);
   WO* box29 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box29->setPosition(Vector(181.5, -273.5, 0));
   box29->rotateAboutRelZ(0);
   worldLst->push_back(box29);
   WO* box30 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box30->setPosition(Vector(146, -312, 0));
   box30->rotateAboutRelZ(1.575);
   worldLst->push_back(box30);
   WO* box31 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box31->setPosition(Vector(68, -200, 0));
   box31->rotateAboutRelZ(1.575);
   worldLst->push_back(box31);
   WO* box32 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box32->setPosition(Vector(68, -280, 0));
   box32->rotateAboutRelZ(1.575);
   worldLst->push_back(box32);
   WO* box33 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box33->setPosition(Vector(68, -406, 0));
   box33->rotateAboutRelZ(1.575);
   worldLst->push_back(box33);
   WO* box34 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box34->setPosition(Vector(30, -370, 0));
   box34->rotateAboutRelZ(0);
   worldLst->push_back(box34);
   WO* box35 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box35->setPosition(Vector(32, -324, 0));
   box35->rotateAboutRelZ(0);
   worldLst->push_back(box35);
   WO* box36 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box36->setPosition(Vector(-12, -288, 0));
   box36->rotateAboutRelZ(1.575);
   worldLst->push_back(box36);
   WO* box37 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box37->setPosition(Vector(-48, -244, 0));
   box37->rotateAboutRelZ(0);
   worldLst->push_back(box37);
   WO* box38 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box38->setPosition(Vector(-87, -334, 0));
   box38->rotateAboutRelZ(1.575);
   worldLst->push_back(box38);
   WO* box39 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box39->setPosition(Vector(-130, -370, 0));
   box39->rotateAboutRelZ(0);
   worldLst->push_back(box39);
   WO* box40 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box40->setPosition(Vector(-51, -370, 0));
   box40->rotateAboutRelZ(0);
   worldLst->push_back(box40);
   WO* box41 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box41->setPosition(Vector(-243, -407, 0));
   box41->rotateAboutRelZ(1.575);
   worldLst->push_back(box41);
   WO* box42 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box42->setPosition(Vector(-330, -375, 0));
   box42->rotateAboutRelZ(0);
   worldLst->push_back(box42);
   WO* box43 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box43->setPosition(Vector(-405, -375, 0));
   box43->rotateAboutRelZ(0);
   worldLst->push_back(box43);
   WO* box44 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box44->setPosition(Vector(-294, -331, 0));
   box44->rotateAboutRelZ(1.575);
   worldLst->push_back(box44);
   WO* box45 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box45->setPosition(Vector(-330, -295, 0));
   box45->rotateAboutRelZ(0);
   worldLst->push_back(box45);
   WO* box46 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box46->setPosition(Vector(-373, -259, 0));
   box46->rotateAboutRelZ(1.575);
   worldLst->push_back(box46);
   WO* box47 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box47->setPosition(Vector(-337, -215, 0));
   box47->rotateAboutRelZ(0);
   worldLst->push_back(box47);
   WO* box48 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box48->setPosition(Vector(-260, -215, 0));
   box48->rotateAboutRelZ(0);
   worldLst->push_back(box48);
   WO* box49 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box49->setPosition(Vector(-216, -251, 0));
   box49->rotateAboutRelZ(1.575);
   worldLst->push_back(box49);
   WO* box50 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box50->setPosition(Vector(-180, -294.75, 0));
   box50->rotateAboutRelZ(0);
   worldLst->push_back(box50);
   WO* box51 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box51->setPosition(Vector(-163, -200, 0));
   box51->rotateAboutRelZ(1.575);
   worldLst->push_back(box51);
   WO* box52 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box52->setPosition(Vector(-127, -244, 0));
   box52->rotateAboutRelZ(0);
   worldLst->push_back(box52);
   WO* box53 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box53->setPosition(Vector(-207, -164, 0));
   box53->rotateAboutRelZ(0);
   worldLst->push_back(box53);
   WO* box54 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box54->setPosition(Vector(-220, -43, 0));
   box54->rotateAboutRelZ(1.575);
   worldLst->push_back(box54);
   WO* box55 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box55->setPosition(Vector(-140.75, 10, 0));
   box55->rotateAboutRelZ(1.575);
   worldLst->push_back(box55);
   WO* box56 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box56->setPosition(Vector(-140.75, -37, 0));
   box56->rotateAboutRelZ(1.575);
   worldLst->push_back(box56);
   WO* box57 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box57->setPosition(Vector(-178, 46, 0));
   box57->rotateAboutRelZ(0);
   worldLst->push_back(box57);
   WO* box58 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box58->setPosition(Vector(-263, -80, 0));
   box58->rotateAboutRelZ(0);
   worldLst->push_back(box58);
   WO* box59 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box59->setPosition(Vector(-307, -116, 0));
   box59->rotateAboutRelZ(1.575);
   worldLst->push_back(box59);
   WO* box60 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box60->setPosition(Vector(-307, -36, 0));
   box60->rotateAboutRelZ(1.575);
   worldLst->push_back(box60);
   WO* box61 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box61->setPosition(Vector(-307, 42, 0));
   box61->rotateAboutRelZ(1.575);
   worldLst->push_back(box61);
   WO* box62 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box62->setPosition(Vector(-350, -153, 0));
   box62->rotateAboutRelZ(0);
   worldLst->push_back(box62);
   WO* box63 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box63->setPosition(Vector(-413, -153, 0));
   box63->rotateAboutRelZ(0);
   worldLst->push_back(box63);
   WO* box64 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box64->setPosition(Vector(-350, -80, 0));
   box64->rotateAboutRelZ(0);
   worldLst->push_back(box64);
   WO* box65 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box65->setPosition(Vector(-385.75, -116, 0));
   box65->rotateAboutRelZ(1.575);
   worldLst->push_back(box65);
   WO* box66 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box66->setPosition(Vector(-350, -8, 0));
   box66->rotateAboutRelZ(0);
   worldLst->push_back(box66);
   WO* box67 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box67->setPosition(Vector(-385.75, 169, 0));
   box67->rotateAboutRelZ(1.575);
   worldLst->push_back(box67);
   WO* box68 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box68->setPosition(Vector(-345, 133, 0));
   box68->rotateAboutRelZ(0);
   worldLst->push_back(box68);
   WO* box69 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box69->setPosition(Vector(-385.75, 247, 0));
   box69->rotateAboutRelZ(1.575);
   worldLst->push_back(box69);
   WO* box70 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box70->setPosition(Vector(-422, 290, 0));
   box70->rotateAboutRelZ(0);
   worldLst->push_back(box70);
   WO* box71 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box71->setPosition(Vector(-422, 370, 0));
   box71->rotateAboutRelZ(0);
   worldLst->push_back(box71);
   WO* box72 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box72->setPosition(Vector(-322, 406, 0));
   box72->rotateAboutRelZ(1.575);
   worldLst->push_back(box72);
   WO* box73 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box73->setPosition(Vector(-286, 362, 0));
   box73->rotateAboutRelZ(0);
   worldLst->push_back(box73);
   WO* box74 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box74->setPosition(Vector(-242, 326, 0));
   box74->rotateAboutRelZ(1.575);
   worldLst->push_back(box74);
   WO* box75 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box75->setPosition(Vector(-286, 214, 0));
   box75->rotateAboutRelZ(0);
   worldLst->push_back(box75);
   WO* box76 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box76->setPosition(Vector(-210, 214, 0));
   box76->rotateAboutRelZ(0);
   worldLst->push_back(box76);
   WO* box77 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box77->setPosition(Vector(-170, 326, 0));
   box77->rotateAboutRelZ(1.575);
   worldLst->push_back(box77);
   WO* box78 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box78->setPosition(Vector(-170, 406, 0));
   box78->rotateAboutRelZ(1.575);
   worldLst->push_back(box78);
   WO* box79 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box79->setPosition(Vector(-170, 250, 0));
   box79->rotateAboutRelZ(1.575);
   worldLst->push_back(box79);
   WO* box80 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box80->setPosition(Vector(-245, 170, 0));
   box80->rotateAboutRelZ(1.575);
   worldLst->push_back(box80);
   WO* box81 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box81->setPosition(Vector(-180, 126, 0));
   box81->rotateAboutRelZ(0);
   worldLst->push_back(box81);
   WO* box82 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box82->setPosition(Vector(-209, 126, 0));
   box82->rotateAboutRelZ(0);
   worldLst->push_back(box82);
   WO* box83 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box83->setPosition(Vector(-67, 212, 0));
   box83->rotateAboutRelZ(1.575);
   worldLst->push_back(box83);
   WO* box84 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box84->setPosition(Vector(-67, 290, 0));
   box84->rotateAboutRelZ(1.575);
   worldLst->push_back(box84);
   WO* box85 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box85->setPosition(Vector(-67, 409, 0));
   box85->rotateAboutRelZ(1.575);
   worldLst->push_back(box85);
   WO* box86 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box86->setPosition(Vector(-31, 326, 0));
   box86->rotateAboutRelZ(0);
   worldLst->push_back(box86);
   WO* box87 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box87->setPosition(Vector(10, 290, 0));
   box87->rotateAboutRelZ(1.575);
   worldLst->push_back(box87);
   WO* box88 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box88->setPosition(Vector(-64, 96, 0));
   box88->rotateAboutRelZ(0);
   worldLst->push_back(box88);
   WO* box89 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box89->setPosition(Vector(67, 409, 0));
   box89->rotateAboutRelZ(1.575);
   worldLst->push_back(box89);
   WO* box90 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box90->setPosition(Vector(67, 330, 0));
   box90->rotateAboutRelZ(1.575);
   worldLst->push_back(box90);
   WO* box91 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box91->setPosition(Vector(103.5, 286, 0));
   box91->rotateAboutRelZ(0);
   worldLst->push_back(box91);
   WO* box92 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box92->setPosition(Vector(147.5, 322, 0));
   box92->rotateAboutRelZ(1.575);
   worldLst->push_back(box92);
   WO* box93 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box93->setPosition(Vector(-23, 373, 0));
   box93->rotateAboutRelZ(0);
   worldLst->push_back(box93);
   WO* box94 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box94->setPosition(Vector(210, 322, 0));
   box94->rotateAboutRelZ(1.575);
   worldLst->push_back(box94);
   WO* box95 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box95->setPosition(Vector(210, 260, 0));
   box95->rotateAboutRelZ(1.575);
   worldLst->push_back(box95);
   WO* box97 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box97->setPosition(Vector(170, 210, 0));
   box97->rotateAboutRelZ(0);
   worldLst->push_back(box97);
   WO* box98 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box98->setPosition(Vector(100, 210, 0));
   box98->rotateAboutRelZ(0);
   worldLst->push_back(box98);
   WO* box99 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box99->setPosition(Vector(246, 365, 0));
   box99->rotateAboutRelZ(0);
   worldLst->push_back(box99);
   WO* box100 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box100->setPosition(Vector(210, 180, 0));
   box100->rotateAboutRelZ(1.575);
   worldLst->push_back(box100);
   WO* box101 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box101->setPosition(Vector(325, 365, 0));
   box101->rotateAboutRelZ(0);
   worldLst->push_back(box101);
   WO* box102 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box102->setPosition(Vector(325, 292, 0));
   box102->rotateAboutRelZ(0);
   worldLst->push_back(box102);
   WO* box103 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box103->setPosition(Vector(369, 329, 0));
   box103->rotateAboutRelZ(1.575);
   worldLst->push_back(box103);
   WO* box104 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box104->setPosition(Vector(405, 230, 0));
   box104->rotateAboutRelZ(0);
   worldLst->push_back(box104);
   WO* box105 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box105->setPosition(Vector(325, 230, 0));
   box105->rotateAboutRelZ(0);
   worldLst->push_back(box105);
   WO* box106 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box106->setPosition(Vector(320, 68, 0));
   box106->rotateAboutRelZ(0);
   worldLst->push_back(box106);
   WO* box107 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box107->setPosition(Vector(320, 3, 0));
   box107->rotateAboutRelZ(0);
   worldLst->push_back(box107);
   WO* box108 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box108->setPosition(Vector(254, 3, 0));
   box108->rotateAboutRelZ(0);
   worldLst->push_back(box108);
   WO* box109 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box109->setPosition(Vector(320, 152, 0));
   box109->rotateAboutRelZ(0);
   worldLst->push_back(box109);
   WO* box110 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box110->setPosition(Vector(254, 152, 0));
   box110->rotateAboutRelZ(0);
   worldLst->push_back(box110);
   WO* box111 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box111->setPosition(Vector(364, 116, 0));
   box111->rotateAboutRelZ(1.575);
   worldLst->push_back(box111);
   WO* box112 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box112->setPosition(Vector(364, 40, 0));
   box112->rotateAboutRelZ(1.575);
   worldLst->push_back(box112);
   WO* box113 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box113->setPosition(Vector(211, 103, 0));
   box113->rotateAboutRelZ(1.575);
   worldLst->push_back(box113);

   WO* box114 = WO::New("../mm/models/wall.dae", Vector(4, 4, 4));
   box114->setPosition(Vector(409, -202, 0));
   box114->rotateAboutRelZ(0);
   worldLst->push_back(box114);  


}

bool GLViewNewModule::isMoving() {
    return this->banana != nullptr && this->banana->hasDriver();
}


void GLViewNewModule::createNewModuleWayPoints()
{
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = true;
   WOWayPointSpherical* wayPt = WOWP1::New( params, 6 );
   //x is red, y is green, z is blue
   wayPt->setPosition( Vector( 0, 0, 100 ) );
   worldLst->push_back( wayPt );
   worldLst->eraseViaWOptr(wayPt);

   WayPointParametersBase keys(this);
   keys.frequency = 5000;
   keys.useCamera = true;
   keys.visible = true;

   key1 = WOWP1::New(keys, 30);
   key1->setPosition(Vector(410, -175, 0));
   //key1->getActivators()->push_back(banana);
   worldLst->push_back(key1);
   key2 = WOWP1::New(keys, 30);
   key2->setPosition(Vector(-25, 410, 0));
   worldLst->push_back(key2);
   key3 = WOWP1::New(keys, 30);
   key3->setPosition(Vector(-410, 410, 0));
   worldLst->push_back(key3);
   key4 = WOWP1::New(keys, 30);
   key4->setPosition(Vector(-325, -330, 0));
   worldLst->push_back(key4);
    key5= WOWP1::New(keys, 30);
   key5->setPosition(Vector(410, -410, 0));
   worldLst->push_back(key5);
   key6 = WOWP1::New(keys, 30);
   key6->setPosition(Vector(30, -300, 0));
   worldLst->push_back(key6);
}



WOFTGLString* GLViewNewModule::getInitialWorldTxt() {
    WOFTGLString* worldString = WOFTGLString::New(sharedMM + "/fonts/primemin.ttf", 72);
    worldString->setPosition(500, 0, 500);
    worldString->setLabel("World Text");
    worldString->setText(this->initTxt);
    worldString->getModel()->setLookDirection(Vector(0, -1, 0));
    worldString->getModel()->setNormalDirection(Vector(-1, 0, 0));
    worldString->getModel()->setScale(Vector(1000, 1000, 1000));
    return worldString;
}

void GLViewNewModule::KeyPress(const SDL_Keycode& key) {
    if (this->typing) {
        switch (key) {
            case SDLK_RETURN:
                this->typing = false;
                break;
            case SDLK_LSHIFT:
            case SDLK_RSHIFT:
                if (!this->shift) {
                    this->shift = true;
                }
                break;
            case SDLK_TAB:
                net->sendNetMsgSynchronousTCP(NetMsgTxt(this->worldTxt->getText()));
                break;
            case SDLK_ESCAPE:
                this->typing = false;
                break;
            default:
                this->type(key);
        }
        return;
    }
    switch (key) {
        case SDLK_RETURN:
            this->typing = true;
            if (this->worldTxt->getText() == this->initTxt) {
                this->worldTxt->setText("");
            }
            return;
        case SDLK_t:
            if (this->banana != nullptr) {
                if (this->cam != nullptr && !this->banana->hasDriver()) {
                    this->banana->setDriver(this->cam);
                    this->cam->attachCameraToWO(this->banana->getbananaWO(), this->banana->getPosition());
                }
                else {
                    this->banana->setDriver(nullptr);
                    this->cam->detachCameraFromWO();
                }
            }
            break;
        /*case SDLK_b:
            this->Bananer();
            break;*/
        default: break;
    }
}

void GLViewNewModule::type(const SDL_Keycode& key) {
    std::string text = this->worldTxt->getText();
    if (key == SDLK_BACKSPACE) {
        if (text.size() < 2) {
            text = "";
        }
        else {
            text.pop_back();
        }
    }
    else if (this->shift) {
        text += this->getShift(key);
    }
    else {
        text += key;
    }
    this->worldTxt->setText(text);
}

char GLViewNewModule::getShift(char c) {
    if (c < 123 && c > 96) {
        return (c - 32);
    }
    switch (c) {
    case 39: return 34;// ' = "
    case 44: return 60;// , = <
    case 45: return 95;// - = _
    case 46: return 62;// . = >
    case 47: return 63;// / = ?
    case 48: return 41;// 0 = )
    case 49: return 33;// 1 = !
    case 50: return 64;// 2 = @
    case 51: return 35;// 3 = #
    case 52: return 36;// 4 = $
    case 53: return 37;// 5 = %
    case 54: return 94;// 6 = ^
    case 55: return 38;// 7 = &
    case 56: return 42;// 8 = *
    case 57: return 40;// 9 = (
    case 59: return 58;// ; = :
    case 61: return 43;// = = +
    case 91: return 123;// [ = {
    case 92: return 124;// \ = |
    case 93: return 125;// ] = }
    case 96: return 126;// ` = ~
    default: return c;

    }
}


/*
//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg" ); - current using this one
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );
*/
