/******************************************************************************\
* Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include <iostream>
#include <cstring>
#include <thread>
#include "MainController/GestureQueue.hpp"

#include "GestureCapture.hpp"

GestureCapture* GestureCapture::instance = nullptr;

void GestureCapture::onConnect(const Leap::Controller& controller) {
  controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
  controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
}

void GestureCapture::onFrame(const Leap::Controller& controller) {
    // Get the most recent frame and report some basic information
    const Leap::Frame frame = controller.frame();
    const Leap::Frame prevFrame = controller.frame(1);
    //Initialize all gestures to false
    std::bitset<+GestType::INVALID_GESTURE> curGestures{};
    
    Leap::HandList handList = frame.hands();
    handsInFrame.clear();
    for (int i=0; i<handList.count() && i<2; i++) {
        handsInFrame.push_back(Hand(handList.get(i)));
    }

    // TODO don't HandleErrorCommand::IGNORE when the same gesture is made simultaneously by different fingers

    Leap::GestureList gestures = frame.gestures();
    for( auto gl : gestures )
    {
        GestHand hand = (*gl.hands().begin()).isRight() ? GestHand::RIGHT : GestHand::LEFT;
        switch (gl.type()) {
            case Leap::Gesture::TYPE_CIRCLE:
                curGestures[+GestType::CIRCLE] = true;
                if(!activeGestures[+GestType::CIRCLE])
                    GestureQueue::getInstance()->push(std::make_unique<GestureMessage>(GestType::CIRCLE, GestDir::NONE, hand));
                activeGestures[+GestType::CIRCLE] = true;
                break;
            case Leap::Gesture::TYPE_SCREEN_TAP:
                curGestures[+GestType::SCREEN_TAP] = true;
                if(!activeGestures[+GestType::SCREEN_TAP])
                    GestureQueue::getInstance()->push(std::make_unique<GestureMessage>(GestType::SCREEN_TAP, GestDir::NONE, hand));
                activeGestures[+GestType::SCREEN_TAP] = true;
                break;
            case Leap::Gesture::TYPE_SWIPE:
            {
                Leap::SwipeGesture sg = gl;
                Leap::Vector v = sg.direction();
                GestDir swipeType = GestDir::NONE;
                if( std::abs(v.y) > std::abs(v.x) )
                {
                    if(v.y > 0)
                    {
                        swipeType = GestDir::UP;
                    }
                    else
                    {
                        swipeType = GestDir::DOWN;
                    }
                }
                else
                {
                    if(v.x > 0)
                    {
                        swipeType = GestDir::RIGHT;
                    }
                    else
                    {
                        swipeType = GestDir::LEFT;
                    }
                }
                if(swipeType!=GestDir::NONE)
                {
                    curGestures[+GestType::SWIPE] = true;
                    if(!activeGestures[+GestType::SWIPE])
                        GestureQueue::getInstance()->push(std::make_unique<GestureMessage>(GestType::SWIPE, swipeType, hand));
                    activeGestures[+GestType::SWIPE] = true;
                }
                break;
            }
            default:
               std::cout<<"Leap Motion gesture not recognized\n";
               break;
        }
    }

    checkHands(frame, curGestures);

    for(auto i= +GestType::CIRCLE; i<+GestType::INVALID_GESTURE; i++)
    {
        activeGestures[i] = curGestures[i];
    }
}

void GestureCapture::checkHands(Leap::Frame frame, GestFlags& curGestures)
{
  Leap::HandList hands = frame.hands();
  for(Leap::HandList::const_iterator hl = hands.begin(); hl != hands.end(); hl++)
  {
    float pinchStr = (*hl).pinchStrength();
    float grabStr = (*hl).grabStrength();
    if(1==grabStr && 1>pinchStr)
    {
        curGestures[+GestType::GRAB] = true;
        if(!activeGestures[+GestType::GRAB])
            GestureQueue::getInstance()->push(std::make_unique<GestureMessage>(GestType::GRAB, GestDir::NONE, (((*hl).isRight()) ? GestHand::RIGHT : GestHand::LEFT )));
        activeGestures[+GestType::GRAB] = true;
    }
    else if(1==pinchStr && .7>grabStr)
    {
        curGestures[+GestType::PINCH] = true;
        if(!activeGestures[+GestType::PINCH])
            GestureQueue::getInstance()->push(std::make_unique<GestureMessage>(GestType::PINCH, GestDir::NONE, (((*hl).isRight()) ? GestHand::RIGHT : GestHand::LEFT )));
        activeGestures[+GestType::PINCH] = true;
    }
  }
}

GestureCapture::GestureCapture(bool background) : Listener()
{
    instance = this;

    if (background)
        controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

   controller.addListener(*instance);
}

GestureCapture::~GestureCapture()
{
   controller.removeListener(*instance);
   instance = nullptr;
}

GestureCapture* GestureCapture::getInstance()
{
    return instance;
}

static std::vector<Hand> GestureCapture::getHands()
{
    return handsInFrame;
}
