#include "Managers/ActivityManager.h"

ActivityManager& ActivityManager::getInstance() {static ActivityManager instance; return instance;}

ActivityManager::ActivityManager() {this->curActivity = nullptr; this->_activitiesCount = 0;}

ActivityManager::~ActivityManager() {}

void ActivityManager::registerActivity(Activity* activity){
    activity->id = this->_activitiesCount;
    this->_activities[this->_activitiesCount] = activity;
    this->_activitiesCount ++;
}

void ActivityManager::setActivity(int id){
    if(id < this->_activitiesCount && id <= MAX_ACTIVITIES){
        if(curActivity) curActivity->stop();
        curActivity = this->_activities[id];
        curActivity->setup();
    }
}

Activity** ActivityManager::getActivities(){
    return this->_activities;
}

Activity* ActivityManager::getActivity(int index){
    return this->_activities[index];
}


int ActivityManager::getActivitiesCount(){
    return this->_activitiesCount;
}

void ActivityManager::loopActivity(){if(curActivity){curActivity->loop();}}
