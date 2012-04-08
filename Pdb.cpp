#include "Pdb.h"

#include <iostream>
#include "activity.h"

Pdb::Pdb()
{
      if(!_openDb())
      {
	  _createDb();
	  _initDb();
      }
}

Pdb::~Pdb()
{
    //if(_db)
      //sqlite3_close(_db);
}

bool Pdb::saveActivity(Activity* activity)
{
    std::string sql;
    int rc = 0;
    char *szErrMsg = 0;
    std::stringstream ss;
    
    if(activity->getId() == 0)
    {
        std::stringstream pomodoros;
        std::stringstream expected_p;
     
        ss << activity->isCompleted();
        pomodoros << activity->getPomodoros();
        expected_p << activity->getExpectedPomodoros();
        
	sql = "INSERT INTO Activity (title, expected_p, actual_p, date_created, completed)";
        sql += "VALUES ('" + activity->getTitle() + "', " + expected_p.str() + ", " + pomodoros.str() +  ", '" + activity->getDateAsText() + "', " + ss.str() + ")";
    }
    else
    {
	std::stringstream ss;
        std::stringstream ssCompleted;
        std::stringstream pomodoros;
        std::stringstream expected_p;
        
        pomodoros << activity->getPomodoros();
	ss << activity->getId();
        ssCompleted << activity->isCompleted();
        expected_p << activity->getExpectedPomodoros();
	
        sql = "UPDATE Activity set title = '" + activity->getTitle() + "', date_created = '" + activity->getDateAsText() + 
              "', completed = " + ssCompleted.str() + ", expected_p = " + expected_p.str() + ", actual_p = " + pomodoros.str() +  " WHERE id = " + ss.str();
    }
    
    std::cout << sql << std::endl;
    
    rc = sqlite3_exec(_db, sql.c_str(), 0, 0, &szErrMsg);
    if(rc != SQLITE_OK)
    {
	std::cout << szErrMsg << std::endl;
	sqlite3_free(szErrMsg);
	return false;
    }    
    
    if(activity->getId() == 0)
    {
	int id = sqlite3_last_insert_rowid(_db);
	std::cout << "INSERTED with id: " << id << std::endl;
	activity->setId(id);
    }
    
    return true;
}

std::vector<Activity> Pdb::getAllActivities()
{
    char *szErrMsg = 0;
    int rc = 0;
    
    const char* sql = "SELECT id, title, date_created, actual_p, expected_p FROM Activity WHERE completed = 0";
    
    rc = sqlite3_exec(_db, sql, _activity_callback, this, &szErrMsg);
    if(rc != SQLITE_OK)
    {
        std::cout << szErrMsg << std::endl;
        sqlite3_free(szErrMsg);
    }   
    
    std::vector<Activity> activities = _activityBuffer;
    
    _activityBuffer.clear();
    
    return activities;
}

Activity Pdb::getActivity(std::string id)
{
    char *szErrMsg = 0;
    int rc = 0;
    
    std::string sql = "SELECT id, title, date_created, actual_p, expected_p FROM Activity WHERE id = " + id;
    
    rc = sqlite3_exec(_db, sql.c_str(), _activity_callback, this, &szErrMsg);
    if(rc != SQLITE_OK)
    {
        std::cout << szErrMsg << std::endl;
        sqlite3_free(szErrMsg);
    }   
    
    Activity act(this, "");
    
    if(_activityBuffer.size() > 0)
    {
        act = _activityBuffer.at(0);
    }
    
    _activityBuffer.clear();
    
    return act;
}

void Pdb::deleteActivity(std::string id)
{
    char *szErrMsg = 0;
    int rc = 0;
    
    std::string sql = "DELETE FROM Activity WHERE id = " + id;
    
    rc = sqlite3_exec(_db, sql.c_str(), 0, 0, &szErrMsg);
    if(rc != SQLITE_OK)
    {
        std::cout << szErrMsg << std::endl;
        sqlite3_free(szErrMsg);
    }   
}


//PRIVATE

int Pdb::_activity_callback(void *pdb, int argc, char **argv, char **szColName)
{
    Pdb* db = (Pdb*)pdb;
    int id = atoi(argv[0]);
    Activity a(db, argv[1], atoi(argv[4]), id, argv[2]);
    a.setPomodoros(atoi(argv[3]));
    db->_activityBuffer.push_back(a);

    return 0;
}

//PROTECTED

void Pdb::_initDb()
{
    int rc;
    char *szErrMsg = 0;
    // prepare our sql statements
    const char *pSQL[1];
    pSQL[0] = "CREATE TABLE Activity(id INTEGER PRIMARY KEY, title varchar(512), expected_p smallint, actual_p smallint, date_created TEXT, completed smallint)";

    // execute sql
    for(int i = 0; i < 1; i++)
    {
        rc = sqlite3_exec(_db, pSQL[i], 0, 0, &szErrMsg);
        if(rc != SQLITE_OK)
        {
	    std::cout << szErrMsg << std::endl;
            sqlite3_free(szErrMsg);
            break;
        }       
    }
}

bool Pdb::_openDb()
{
    // open database
    int rc = sqlite3_open_v2("pomodoro.db", &_db, SQLITE_OPEN_READWRITE, 0);
    
    if(rc)
    {
        std::cout << "No db found!" << std::endl;
        return false;
    } 
    else 
    {
        std::cout << "Db found!" << std::endl;
        return true;
    }   
}

bool Pdb::_createDb()
{
    // open database
    int rc = sqlite3_open_v2("pomodoro.db", &_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
    
    if(rc)
    {
        std::cout << "Cound't create db!" << std::endl;
        return false;
    } 
    else 
    {
        return true;
    }   
}

