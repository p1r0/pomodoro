#ifndef __ACTIVITY_H__
#define __ACTIVITY_H__

#include <string>
#include "boost/date_time/posix_time/posix_time.hpp"

class Pdb;

class Activity
{
    public:
	Activity(Pdb* db, std::string title, int expectedPomodoros = 0, int id = 0, std::string date_time = "");
	
	std::string toString();
	
	//Getters
	int getId();
	std::string getTitle();
	boost::posix_time::ptime getTime();
	std::string getDateAsText();
        int isCompleted();
        int getPomodoros();
        int getExpectedPomodoros();

	//Setters
	void setId(int id);
	//void setTime(boost::posix_time::ptime date);
	//void setTitle(std::string title);
        void setExpectedPomodoros(int pomodoros);
        void setPomodoros(int pomodoros);
	void complete(bool complete);
        
        void addPomodoro();
        
        
	void persist();
    private:
	std::string _title;
	int _id;
	int _expectedPomodoros;
	int _actualPomodoros;
        int _completed;
	boost::posix_time::ptime _dateCreated;
	Pdb* _db;
};

#endif //__ACTIVITY_H__