#pragma once

#include "WO.h"
#include "WayPointParameters.h"
#include <vector>


namespace Aftr
{
   class GLView;
   class Camera;
   class WayPointParametersBase;
   enum WayPointActivationType;

   /**
   \author Chad Mourning (Updated by Scott Nykl)

   The WOWayPoint class is used for any event driven interaction.

   There are 3 basic types of WayPoints:  

   Standard WayPoints' "onTrigger" method is called when an activator passes within \see radius distance of it.
   Inverse WayPoints' "onTrigger" method is called when an activator passes outside of the \see radius
   Global WayPoints' "onTrigger method is called whenever all of its prerequisettes have been met and it is enabled.

   All subtypes of way points can be one-use or multi-use.  Mutli-use way points are automatically reenabled based on a given frequency (defauly is 1 second).

   */
   class WOWayPointAbstract : public WO
   {
   public:	
      static WOWayPointAbstract* New( const WayPointParametersBase& params );
     
      /**
      Destructor
      */
      virtual void onUpdateWO();
      virtual void onPhysicsEngineTick();
      virtual ~WOWayPointAbstract();

      /**
      \return true if visable, false otherwise
      */
      bool getVisibility(){return visible;}
      /**
      \param v value to set visibility to
      */
      void setVisibility(bool v){visible = v;}
      /**
      Enables the way point.
      */
      void enable(){able = true;}
      /**
      Disables the way point.
      */
      void disable(){able = false;}
      /**
      \return returns true if way point is enabled, false otherwise
      */
      bool isEnabled(){return able;}
      /**
      \return number of times way point has been triggered
      */
      
      int isTriggered(){return triggered;}
      int reset_trigger(){return triggered = 0;}
      /**
      "Permanently" disables a multi-use way point
      */
      void permDisable();
      /**
      Returns the radius squarred (more efficient for distance checks)
      \return The radius of the waypoint squarred
      */
      
      virtual bool prereqPolicy();
      std::vector<WOWayPointAbstract*>& getPrereqs(); 
      virtual void onTrigger() = 0;

      //returns true if it meets the criteria to activate, returns false otherwise
      virtual bool activate(WO* activator) = 0;

      WayPointActivationType getActivationType(){return type;}
      void setActivationType(WayPointActivationType type){this->type = type;}

      bool isInverse();
      void isInverse(bool x);

      std::vector<WO*>* getActivators() { return &activators; }
      WO* getFirstActivator() { return firstActivator; }

      float getFrequency() { return frequency; }
      void setFrequency(float x) { frequency = x; }

//      std::vector<WOWayPointAbstract*>& getPrereqs() { return prereqs; }

   protected:
      /**
      Protected Constructor
      */
      WOWayPointAbstract( const WayPointParametersBase& params );
      virtual void onCreate();
      /** Parent GLView */
      GLView* view;
	   float frequency;
	   bool visible;
	   std::vector<WO*> activators;	
	   std::vector<WOWayPointAbstract*> prereqs;
	   bool able;
	   int triggered;
	   unsigned int internalTimer;
      WayPointActivationType type;
      WO* firstActivator;
      bool runWhenPaused;
      bool useCamera;
   };

}

