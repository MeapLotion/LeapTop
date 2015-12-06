#include "MainController.hpp"
#include <iostream>

MainController* MainController::curInstance = nullptr;

MainController::MainController()
{
	curInstance = this;
	event_q = new GestureQueue();
	ignore_new = false;
}

MainController::~MainController()
{
}

MainController* MainController::getInstance()
{
	if (curInstance == nullptr)
	{
		curInstance = new MainController();
	}

	return curInstance;
}

void MainController::initThread()
{
	getInstance()->mainLoop();
}

void MainController::mainLoop()
{
	cur_path = fs::path("tests/test_dir");
	while (true)
	{
		std::unique_lock<std::mutex> lk(event_m);
		event_cv.wait(lk, [&]{return event_q->size() != 0;});
		//got something in queue
		GestureEvent* ev = event_q->pop();
		//TODO: ACTUALLY PROCESS THE EVENT
		processEvent(ev);
		while (event_q->size() != 0)
		{
			ev = event_q->pop();
			processEvent(ev);
		}
	}	
}

void MainController::pushEvent(GestureEvent* ge)
{
	if (!ignore_new)
	{
		event_q->push(ge);
		event_cv.notify_one();
	}
}

void MainController::processEvent(GestureEvent* ge)
{
	std::cout << "This is the part where an event gets processed! It was: " << ge->getName() << std::endl;
	switch (ge->getGestureType()) {
		case CIRCLE:
			break;
		case KEY_TAP:
			for(auto& p: fs::directory_iterator(cur_path))
				std::cout << p << std::endl;
			break;
		case SCREEN_TAP:
			break;
		case SWIPE_UP:
			break;
		case SWIPE_DOWN:
			break;
		case SWIPE_RIGHT:
			break;
		case SWIPE_LEFT:
			break;
		default:
			std::cout<<"Gesture" << ge->getName() << " not supported\n";
			break;
	}
}
