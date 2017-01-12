
/*
 * classMap.cpp
 * This is the class that will build up the map of the classes
 */

#include <string>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <iostream>

extern "C" {
#include "pullcourses.h"
#include <sqlite3.h>
}

using namespace std;

#define BUF_SIZE 4096
#define SMALL_BUF 50
#define MED_BUF 150


int main(int argc, char **argv)
{
  // Set up database 
  sqlite3 *db;
  char *err_msg = 0;

  int rc = sqlite3_open("directory.db", &db);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);

    return 1;
  }

  char sql[BUF_SIZE];
  memset(sql, 0, sizeof(sql));
  strcpy (sql, "DROP TABLE IF EXISTS Courses;"
          "CREATE TABLE Courses(Id INTEGER PRIMARY KEY, Call TEXT, Day TEXT"
          "Points INT, Approval TEXT, Instructor TEXT, Type TEXT, "
          "Description TEXT, Subject TEXT, Number TEXT, Section TEXT, Key TEXT"
          ", Note TEXT, Title TEXT, Epithet TEXT);");

  rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Error encountered while creating table\n"
            "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);
  }
  
  // Starting at home page of bulletin
  string serverName("www.columbia.edu");
  string serverPort("80");
  //  string filePath("/cu/bulletin/uwb/home.html");
  string filePath("/cu/bulletin/uwb/subj/COMS/_Spring2017.html");


    
  deque<string> pages;
  pages.push_back(filePath);

  char buf[BUF_SIZE];
  memset(buf, 0, sizeof(buf));

  while (pages.begin() != pages.end()) {
    // not sure need fopen with actual file
    FILE *outputFile = fopen("temp.txt", "w+b");
    if (outputFile == NULL) {
      perror("can't open output file");
      // close database file
      sqlite3_close(db);
      exit(1);
    }
    
    char *cfilePath = (char *)(*pages.begin()).data();
    fprintf(stdout, "%s\n", cfilePath);

    pullcourse((char *)serverName.data(), (char *)serverPort.data(),
               cfilePath, outputFile);

    // cout << "POPPING" << endl;
    pages.pop_front();
    rewind(outputFile);

    // When 1 adds pages to deque, when 0 parses course
    int pagesAdd = 1;

    // word and data used to store values of course
   
    char word[100];
    memset(word, 0, sizeof(word));
    char data[BUF_SIZE];
    memset(data, 0, sizeof(data));

    /*  "CREATE TABLE Courses(Id INTEGER PRIMARY KEY, Call TEXT, Day TEXT"
        "Points INT, Approval TEXT, Instructor TEXT, Type TEXT, "
        "Description TEXT, Subject TEXT, Number TEXT, Section TEXT, Key TEXT"
        ", Note TEXT, Title TEXT, Epithet TEXT);");
    */
    char call[SMALL_BUF]; char day[SMALL_BUF]; int points;
    char approval[SMALL_BUF]; char instructor[SMALL_BUF]; char type[SMALL_BUF];
    char description[BUF_SIZE]; char subject[SMALL_BUF];
    char number[SMALL_BUF]; char section[SMALL_BUF]; char key[SMALL_BUF];
    char note[BUF_SIZE]; char title[MED_BUF]; char epithet[MED_BUF];

    memset(call, 0, sizeof(call)); memset(day, 0, sizeof(day));
    memset(approval, 0, sizeof(approval));
    memset(instructor, 0, sizeof(instructor)); memset(type, 0, sizeof(type));
    memset(description, 0, sizeof(description));
    memset(subject, 0, sizeof(subject));
    memset(number, 0, sizeof(number)); memset(section, 0, sizeof(section));
    memset(key, 0, sizeof(key)); memset(note, 0, sizeof(note));
    memset(title, 0, sizeof(title)); memset(epithet, 0, sizeof(epithet));
      
    char *word_color = (char *)"bgcolor=#99CCFF>";
    
    while (fgets(buf, sizeof(buf), outputFile)) {
     
      // To add on courses to the the pages deque
      if (pagesAdd == 1) {
        if (strstr(buf, "subj") && !strstr(buf, "_text.html")) {
          //for testing
          //fprintf(stdout, "%s\n", buf);
        
          char *pch;
          pch = strtok(buf, "\"");
          pch = strtok(NULL, "\"");

          while (pch != NULL) {
         
            char str[100];
            memset(str, 0, sizeof(str));
        
            if (strstr(pch, "sel")) {
              strcpy(str, "/cu/bulletin/uwb/");
            }
        
            strcat(str, pch);

            // for testing
            //fprintf(stdout, "%s\n", pch);
        
            string s(str);
            //cout << "ADDING: " << s << "\n" << endl;
            pages.push_back(s);

            // Should document what these two lines are doing
            pch = strtok(NULL, "\"");
            pch = strtok(NULL, "\"");
          }
        }
        // If we have reached first course, then all the rest of the pages in
        // deque will be courses, so can switch pagesAdd to 0
        if (strstr(buf, "title") && strstr(buf, "section")) {
          pagesAdd = 0;
        }      
      }


      // Add courses to database //
      // Get course field
      if (pagesAdd == 0) {
        // setting keyword
        if (strstr(buf, word_color)) {
          char *pch = strstr(buf, word_color) + strlen(word_color);
          pch = strtok(pch, "<");

          strcpy(word, pch);

          // for testing
          fprintf(stdout, "%s\n", pch);
        }
        // setting title value
        else if (strcmp(word, "b>") ){ //&& strstr(buf, "+1")) {
        }
      }
    }
    fclose(outputFile);
  }

  // for testing
  /*

  deque<string>::iterator it = pages.begin();
  while (it != pages.end()) {
    cout << *it++ <<  "\n" << endl;
  }
  */

  sqlite3_close(db);  
  
  return 0;
}
