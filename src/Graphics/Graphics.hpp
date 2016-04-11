#include "DirObject.hpp"
#include <vector>
#include <array>
#include <atomic>
#include <mutex>
#include "EventListener.hpp"
#include "irrlicht/ISceneNode.h"
#include "irrlicht/ICameraSceneNode.h"
#include "irrlicht/IVideoDriver.h"
#include "irrlicht/ISceneManager.h"
#include "irrlicht/IGUIEnvironment.h"
#include "irrlicht/IrrlichtDevice.h"
#include "GHand.hpp"

#ifndef GRAPHICS_H
#define GRAPHICS_H

using namespace irr;

class Graphics
{

public:
    Graphics();
    ~Graphics();
    void mainLoop();
    static Graphics* getInstance();

    void newObjects( std::vector<DirObject> );

    static void initGraphics();
    static void killGraphics();


    static const int CELL_WIDTH;
    static const int OBJ_WIDTH;
    static const int GRID_WIDTH;
    static const int GRID_HEIGHT;
    static const int GRID_DEPTH;
    static const int CAM_HEIGHT;

private:

    enum
    {
        CAM_PERSP = 0,
        CAM_ORTH,
        NUM_CAMS
    };

    static Graphics* instance;
    static const int VIEW_WIDTH;
    static const int VIEW_HEIGHT;
    static const int VIEW_DEPTH;
    static const int max_text_length;
    static const std::array<video::E_DRIVER_TYPE, 6> preferedDrivers;

    IrrlichtDevice*       device;
    video::IVideoDriver*  driver;
    scene::ISceneManager* smgr;
    gui::IGUIEnvironment* env;
    EventListener         receiver;
    GHand* leftHand;
    GHand* rightHand;

    bool run;

    std::atomic_bool need_node_update;

    bool tiltingR;
    bool tiltingL;
    bool tiltingU;
    bool tiltingD;

    std::mutex objLock;
    std::vector<DirObject> dirObjects;
    std::vector<scene::ISceneNode*> dirNodes;
    std::array<scene::ICameraSceneNode*, NUM_CAMS> cams;

    void createCameras();
    void emptyNodes();
    void fillNodes();
    void checkScroll(scene::ICameraSceneNode*, EventListener&);
    void checkTilt(scene::ICameraSceneNode* cam, EventListener& receiver);
    void drawHands();

};

#endif
