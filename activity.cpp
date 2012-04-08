#include "activity.h"

#include "Pdb.h"

using namespace boost::posix_time;

Activity::Activity(Pdb* db, std::string title, int expectedPomodoros, int id, std::string date_time)
{
    _id = id;
    _db = db;
    _title = title;
    _expectedPomodoros = expectedPomodoros;
    _actualPomodoros = 0;
    _dateCreated = ptime(second_clock::local_time());
    _completed = 0;
    if(!date_time.empty())
    {
        _dateCreated = time_from_string(date_time);
    }
}

//GETTERS

int Activity::getId()
{
    return _id;
}

boost::posix_time::ptime Activity::getTime()
{
    return _dateCreated;
}

std::string Activity::getTitle()
{
    return _title;
}

std::string Activity::getDateAsText()
{
    std::stringstream ss;
    boost::posix_time::time_facet *df = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S");
 
    ss.imbue(std::locale(ss.getloc(), df)); 
    ss << _dateCreated;
    
    //delete df;
    
    return std::string(ss.str());
}

int Activity::isCompleted()
{
    return _completed;
}

int Activity::getExpectedPomodoros()
{
    return _expectedPomodoros;
}


int Activity::getPomodoros()
{
    return _actualPomodoros;
}



//SETTERS

void Activity::setId(int id)
{
    _id = id;
}

void Activity::addPomodoro()
{
    _actualPomodoros++;
}

void Activity::persist()
{
    _db->saveActivity(this);
}

void Activity::setExpectedPomodoros(int pomodoros)
{
    _expectedPomodoros = pomodoros;
}

void Activity::setPomodoros(int pomodoros)
{
    _actualPomodoros = pomodoros;
}

void Activity::complete(bool complete)
{
    _completed = complete;
}


std::string Activity::toString()
{
    std::stringstream ss;
    boost::posix_time::time_facet *df = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S");
 
    ss.imbue(std::locale(ss.getloc(), df)); 
    ss << _dateCreated;
    
    return _title + " @" + ss.str();
}
