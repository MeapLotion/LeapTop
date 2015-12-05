#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include "GestureQueue.hpp"
#include <mutex>
#include <condition_variable>

class MainController {
private:
	static MainController* curInstance;
	MainController();
	~MainController();
	std::mutex event_m;
	std::condition_variable event_cv;
	GestureQueue* event_q;
	bool ignore_new;
	void processEvent(GestureEvent*);
public:
	static MainController* getInstance();
	void initThread();
	void pushEvent(GestureEvent*);
};

#endif
