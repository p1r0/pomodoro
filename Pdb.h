#ifndef __PDB_H__
#define __PDB_H__

#include <sqlite3.h>
#include <vector>
#include <string>

class Activity;

class Pdb
{
    public:
	Pdb();
	~Pdb();
	bool saveActivity(Activity* activity);
	std::vector<Activity> getAllActivities();
        Activity getActivity(std::string id);
        void deleteActivity(std::string id);
        
    private:
        std::vector<Activity> _activityBuffer;
        static int _activity_callback(void *pdb, int argc, char **argv, char **szColName);
        
    protected:
      void _initDb();
      bool _openDb();
      bool _createDb();
      sqlite3 *_db;
};

#endif //__PDB_H__