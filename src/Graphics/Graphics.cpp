#include <irrlicht/irrlicht.h>
#include "irrlicht/driverChoice.h"
#include "GestureCapture/GestureEvent.hpp"
#include "MainController/GestureQueue.hpp"
#include <mutex>
#include "Graphics.hpp"


using namespace irr;



    #ifdef _IRR_WINDOWS_
    #pragma comment(lib, "Irrlicht.lib")
    #pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
    #endif

    video::IVideoDriver* driver;
    scene::ISceneManager* smgr;
    gui::IGUIEnvironment* env;

    const int width = 10;
    const int unit_size = 5;
    const int view_height = width*10;

    class MyEventReceiver : public IEventReceiver
    {
    public:
        // This is the one method that we have to implement
        virtual bool OnEvent(const SEvent& event)
        {
            // Remember whether each key is down or up
            if (event.EventType == irr::EET_KEY_INPUT_EVENT)
            {
                if( event.KeyInput.PressedDown != KeyIsDown[event.KeyInput.Key])
                    GestureQueue::getInstance()->push(new KeyMessage(event.KeyInput.Key,event.KeyInput.Shift,event.KeyInput.Control,event.KeyInput.PressedDown));
                KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
            }

            return false;
        }

        // This is used to check whether a key is being held down
        virtual bool IsKeyDown(EKEY_CODE keyCode) const
        {
            return KeyIsDown[keyCode];
        }

        MyEventReceiver()
        {
            for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
                KeyIsDown[i] = false;
        }

    private:
        // We use this array to store the current state of each key
        bool KeyIsDown[KEY_KEY_CODES_COUNT];
    };

    const size_t maxDirObjects = 100;
    int numDirObjects = 0;
    int numDirNodes = 0;
    std::mutex objLock;
    dirObject *dirObjects;
    scene::ISceneNode* dirNodes[maxDirObjects];
    scene::IBillboardTextSceneNode* dirBillboards[maxDirObjects];

    void newObjects( dirObject* objs, size_t count)
    {
        objLock.lock();
        if( dirObjects )
            delete[] dirObjects;
        numDirObjects = count;
        dirObjects = objs;
        objLock.unlock();
    }

    void emptyNodes()
    {
        for(int i = 0; i < numDirNodes; i++)
        {
            if(dirNodes[i])
                dirNodes[i]->remove();
        }
    }

    void fillNodes()
    {
        objLock.lock();
        emptyNodes();
        for(int i = 0; i < numDirObjects; i++)
        {
            if(dirObjects[i].getType() == 'f')
            {
                dirNodes[i] = smgr->addSphereSceneNode();
            }
            else
            {
                dirNodes[i] = smgr->addCubeSceneNode();
            }

            //std::wcout << dirObjects[i].getName() << std::endl;

            if(dirNodes[i])
            {
                dirNodes[i]->setPosition(core::vector3df
                    (
                        dirObjects[i].getX() * unit_size*width,
                        dirObjects[i].getY() * unit_size*width,
                        50
                    ));


                if(dirObjects[i].isSelected)
                {
                        dirNodes[i]->setMaterialTexture(0, driver->getTexture("media/selected.jpg"));
                        dirNodes[i]->setMaterialFlag(video::EMF_LIGHTING, false);
                        dirBillboards[i] = smgr->addBillboardTextSceneNode
                            (
                                env->getFont("media/bigfont.png"),
                                dirObjects[i].getName(),
                                dirNodes[i],
				core::dimension2d<f32>(10.0f, 4.0f),
                                core::vector3df(0,0,-5),
                                i,
                                video::SColor(100,255,255,255),
                                video::SColor(100,255,255,255)
                            );
    }
                else
                {
                        dirNodes[i]->setMaterialTexture(0, driver->getTexture("media/unselected.jpg"));
                    /*if(dirObjects[i].getType() == 'f')
                    {
                        dirNodes[i]->setMaterialTexture(0, driver->getTexture("media/wall.jpg"));
                    }
                    else
                    {
                        dirNodes[i]->setMaterialTexture(0, driver->getTexture("media/water.jpg"));
                    }*/
                dirNodes[i]->setMaterialFlag(video::EMF_LIGHTING, false);
                dirBillboards[i] = smgr->addBillboardTextSceneNode
                    (
                        env->getFont("media/fontcourier.bmp"),
                        dirObjects[i].getName(),
                        dirNodes[i],core::dimension2d<f32>(8.0f, 5.0f),
                        core::vector3df(0,0,-5),
                        i,
                        video::SColor(100,255,255,255),
                        video::SColor(100,255,255,255)
                    );
                }
            }

        }
        numDirNodes = numDirObjects;
        objLock.unlock();
    }

    /*
    This is the main method. We can now use main() on every platform.
    */
    void initGraphics()
    {
        video::E_DRIVER_TYPE preferedDrivers[] = { video::EDT_DIRECT3D9
                                                 , video::EDT_DIRECT3D8
                                                 , video::EDT_OPENGL
                                                 , video::EDT_BURNINGSVIDEO
                                                 , video::EDT_SOFTWARE
                                                 , video::EDT_NULL
                                                 , video::EDT_COUNT};

        // create device
        MyEventReceiver receiver;

        IrrlichtDevice* device = nullptr;

        for( size_t i = 0; nullptr == device && video::EDT_COUNT != preferedDrivers[i]; i++ )
        {
            device = createDevice(preferedDrivers[i],
                    core::dimension2d<u32>(1366/2, 768), 16, false, false, false, &receiver);
        }

        if( nullptr == device )
            exit(1);

        driver = device->getVideoDriver();
        smgr = device->getSceneManager();
        env = device->getGUIEnvironment();

        /*
        To be able to look at and move around in this scene, we create a first
        person shooter style camera and make the mouse cursor invisible.
        smgr->addCameraSceneNodeFPS();
        device->getCursorControl()->setVisible(false);
        */

        //create camera
	float mid = (width * unit_size) / 2.0;
        smgr->addCameraSceneNode(0,core::vector3df(mid,mid,0),core::vector3df(mid,mid,view_height),-1,true);

        /*
        We have done everything, so lets draw it. We also write the current
        frames per second and the name of the driver to the caption of the
        window.
        */
        int lastFPS = -1;

        // In order to do framerate independent movement, we have to know
        // how long it was since the last frame
        u32 then = device->getTimer()->getTime();



        while(device->run())
        {
            fillNodes();

            if(receiver.IsKeyDown(irr::KEY_ESCAPE))
            {
                exit(0);
            }
            if(receiver.IsKeyDown(irr::KEY_KEY_W))
            {

            }


            driver->beginScene(true, true, video::SColor(255,113,113,133));

            smgr->drawAll(); // draw the 3d scene

            driver->endScene();

            int fps = driver->getFPS();

            if (lastFPS != fps)
            {
                core::stringw tmp(L"Tests are things that are important");
                tmp += driver->getName();
                tmp += L"] fps: ";
                tmp += fps;

                device->setWindowCaption(tmp.c_str());
                lastFPS = fps;
            }
        }

        /*
        In the end, delete the Irrlicht device.
        */
        device->drop();

        exit(0);
    }

/*
That's it. Compile and run.
**/
