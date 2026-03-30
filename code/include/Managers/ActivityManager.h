#ifndef ACTIVITY_MANAGER_H
#define ACTIVITY_MANAGER_H

#include <Arduino.h>
#include "Constants.h"
#include "Activities/Activity.h"


class ActivityManager {
    public:
        static ActivityManager& getInstance();
        void setActivity(int id);
        void loopActivity();
        void registerActivity(Activity* activity);
        Activity** getActivities();
        int getActivitiesCount();
        Activity* curActivity;

    private:
        ActivityManager();
        ~ActivityManager();
        Activity* _activities[MAX_ACTIVITIES];
        int _activitiesCount;
        
};

#endif