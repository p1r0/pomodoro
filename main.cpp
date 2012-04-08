#include <stdio.h>
#include <sqlite3.h>
#include <iostream>
#include <getopt.h>
#include <string>
#include <vector>
#include <signal.h>
#include <curses.h>

#include "activity.h"
#include "Pdb.h"

#define PACKAGE		"pomodoro"
#define VERSION		"0.0.1"

#define BREAK           5*60
#define LONG_BREAK      30*60
#define POMODORO        25*60

#define ACTION_ADD      1
#define ACTION_LIST     2
#define ACTION_DELETE   3

void print_help();
void add_activity(std::string, int expectedPomodoros);
void list_all_activities();
void run_timer(int seconds, std::string msg = "", bool quit = true);
std::string secsToTime(int seconds);
void runActivity(std::string id);
void deleteActivity(std::string id);
void completeActivity(std::string id, bool complete);

void notify(std::string);

static void finish_curses_and_quit(int sig);
static void finish_curses(int sig);

// This is the callback function to display the select data in the table
static int callback(void *NotUsed, int argc, char **argv, char **szColName)
{
    for(int i = 0; i < argc; i++)
    {
        std::cout << szColName[i] << " = " << argv[i] << std::endl;
    }

    std::cout << "\n";

    return 0;
}

int main(int argc, char**argv)
{
    int opt;
    int expectedPomodoros = 0;
    int action = 0;
    
    std::string activityTitle;
    /* 
    // no arguments given
    */
    if(argc == 1) 
    {
	fprintf(stderr, "This program needs arguments....\n\n");
	print_help();
	return 1;
    }

    while((opt = getopt(argc, argv, "hVp:a:lbBs:d:c:u:")) != -1) 
    {
	switch(opt) 
	{
	  case 'h':
	    print_help();
	    return 0;
	    break;
	  case 'V':
	    printf("%s %s\n\n", PACKAGE, VERSION); 
	    return 0;
	    break;
          case 'l':
            list_all_activities();
            return 0;
            break;
          case 'p':
            if(!optarg || strlen(optarg) < 1)
            {
                fprintf(stderr, "%s: Error - Option '%c' needs a value\n\n", PACKAGE, opt);
                return 1;
            }
            else
            {
                expectedPomodoros = atoi(optarg);
            }
            break;
	  case 'a':
	    printf("Output: %s\n", optarg);
	    if(!optarg || strlen(optarg) < 1)
	    {
		fprintf(stderr, "%s: Error - Option '%c' needs a value\n\n", PACKAGE, opt);
		return 1;
	    }
	    else
	    {
		action = ACTION_ADD;
                activityTitle = std::string(optarg);
	    }
	    break;
          case 's':
            if(!optarg || strlen(optarg) < 1)
            {
                fprintf(stderr, "%s: Error - Option '%c' needs a value\n\n", PACKAGE, opt);
                return 1;
            }
            else
            {
                runActivity(std::string(optarg));
            }
            break;  
          case 'd':
            if(!optarg || strlen(optarg) < 1)
            {
                fprintf(stderr, "%s: Error - Option '%c' needs a value\n\n", PACKAGE, opt);
                return 1;
            }
            else
            {
                deleteActivity(std::string(optarg));
                return 0;
            }
            break;    
          case 'c':
            if(!optarg || strlen(optarg) < 1)
            {
                fprintf(stderr, "%s: Error - Option '%c' needs a value\n\n", PACKAGE, opt);
                return 1;
            }
            else
            {
                completeActivity(std::string(optarg), true);
                return 0;
            }
            break;
          case 'u':
            if(!optarg || strlen(optarg) < 1)
            {
                fprintf(stderr, "%s: Error - Option '%c' needs a value\n\n", PACKAGE, opt);
                return 1;
            }
            else
            {
                completeActivity(std::string(optarg), false);
                return 0;
            }
            break; 
          case 'b':
            run_timer(BREAK);
            return 0;
            break;
          case 'B':
            run_timer(LONG_BREAK);
            return 0;
            break;
	  case ':':
	    //fprintf(stderr, "%s: Error - Option '%c' needs a value\n\n", PACKAGE, opt);
	    print_help();
	    return 1;
	    break;
	  case '?':
	    //fprintf(stderr, "%s: Error - No such option: '%c'\n\n", PACKAGE, optopt);
	    print_help();
	    return 1;
	}
    }
    
    switch(action)
    {
        case ACTION_ADD:
            add_activity(activityTitle, expectedPomodoros);
            break;
    }
    
    return 0;
}

void print_help()
{
    printf("%s,%s simple pomodoro application\n", PACKAGE, VERSION); 
    printf("%s [-h] [-V] [-a Activity] \n\n", PACKAGE);

    printf("  -h              print this help and exit\n");
    printf("  -V              print version and exit\n\n");

    printf("  -a              adds 'Activity' as a new activity\n");
    printf("  -p NUMBER       If used with -a add an estimated number of pomodoros for the new task\n");
    printf("  -l              Lists all uncompleted activities\n");
    printf("  -b              Starts a short break\n");
    printf("  -B              Starts a long break\n");
    printf("  -s ID           Starts a pomodoro for the task with id: ID\n");
    printf("  -d ID           Deletes the task with id: ID\n");
    printf("  -c ID           Marks the task with id: ID as completed\n");    
    printf("  -u ID           Marks the task with id: ID as uncompleted\n");    
}

void add_activity(std::string title, int expectedPomodoros)
{
    Pdb db;
    Activity act(&db, title, expectedPomodoros);
    act.persist();
    std::cout << "ACTIVITY: " << act.toString() << "added." << std::endl;
}

void list_all_activities()
{
    Pdb db;
    std::vector<Activity> activities = db.getAllActivities();
    
    printf("%-5s\t%-50s\t%-20s\t%-2s\t%-2s\n", "ID", "TITLE", "DATE", "E", "C");
    printf("=====\t=================================================\t====================\t==\t==\n\n");
    
    for(int i = 0; i < activities.size(); i++)
    {   
        Activity act = activities.at(i);
        printf("%5i\t%-50s\t%20s\t%-2i\t%-2i\n", act.getId(), act.getTitle().c_str(), act.getDateAsText().c_str(), act.getExpectedPomodoros(), act.getPomodoros());
        //std::cout << activities.at(i).toString() << std::endl;
    }
}

static void finish_curses_and_quit(int sig)
{
    finish_curses(sig);
    exit(0);
}

static void finish_curses(int sig)
{
    endwin();
}

void run_timer(int seconds, std::string msg, bool quit)
{
    std::string timer;
    timer = secsToTime(seconds);
    
    (void) signal(SIGINT, finish_curses_and_quit);      /* arrange interrupts to terminate */

    (void) initscr();      /* initialize the curses library */
    keypad(stdscr, TRUE);  /* enable keyboard mapping */
    (void) nonl();         /* tell curses not to do NL->CR/NL on output */
    (void) cbreak();       /* take input chars one at a time, no wait for \n */
    (void) noecho();       /* don't echo input */

    if (has_colors())
    {
        start_color();

        /*
         * Simple color assignment, often all we need.
         */
        init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
        init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
        init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    }

    int row,col;                           /* to store the number of rows and *
                                            * the number of colums of the screen */
    initscr();                             /* start the curses mode */
    getmaxyx(stdscr,row,col);              /* get the number of rows and columns */
    mvprintw(row/2,(col-strlen(timer.c_str()))/2,"%s",timer.c_str());
                                            /* print the message at the center of the screen */
    mvprintw(row-1,0, msg.c_str());
    //printw("Try resizing your window(if possible) and then run this program again");
    refresh();
    
    for (;;)
    {
        //int c = getch();     /* refresh, accept single keystroke of input */

        /* process the command keystroke */
        sleep(1);
        
        if(--seconds == 0)
        {
            mvprintw(row/2,(col-strlen("TIME IS UP!"))/2,"%s","TIME IS UP!");   
            refresh();
            system("play ring.wav > /dev/null 2>&1 &");
            if(!quit)
            {
                return finish_curses(0);
            }
            else
            {
                finish_curses_and_quit(0);
            }
        }
           
        mvprintw(row/2,(col-strlen("          "))/2,"%s","          ");   

        timer = secsToTime(seconds);
        mvprintw(row/2,(col-strlen(timer.c_str()))/2,"%s",timer.c_str());   
        refresh();
    }
    
    if(!quit)
    {
        return finish_curses(0);
    }
    else
    {
        finish_curses_and_quit(0);
    }
    
}

std::string secsToTime(int seconds)
{
    std::stringstream ss;
    if(seconds < 60)
    {
        ss << "0:" << seconds;
    }
    else
    {
        int mins = seconds / 60;
        seconds = seconds - mins * 60;
        
        ss << mins << ":" << seconds;
    }
    
    return ss.str();
}

void runActivity(std::string id)
{
    Pdb db;
    Activity act = db.getActivity(id);
    
    if(act.getId() == 0)
    {
        //No activity found
        std::cerr << "No activity found for id: " << id << std::endl;
        exit(1);
    }
    
    run_timer(POMODORO, act.toString(), false);
    act.addPomodoro();
    act.persist();
    
    notify(act.getTitle() + " finished!");
    
}

void deleteActivity(std::string id)
{
    Pdb db;
    db.deleteActivity(id);
    std::cout << "Activity with id: " << id << " deleted." << std::endl;
}

void completeActivity(std::string id, bool complete)
{
    Pdb db;
    Activity act = db.getActivity(id);
    
    if(act.getId() == 0)
    {
        //No activity found
        std::cerr << "No activity found for id: " << id << std::endl;
        exit(1);
    }
    
    act.complete(complete);
    act.persist();
}

void notify(std::string msg)
{
    std::string cmd = "notify-send \"Pomodoro\" \"" +  msg + "\"";
    system(cmd.c_str());
}