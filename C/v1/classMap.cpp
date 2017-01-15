
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
          "Location TEXT, Points INT, Approval TEXT, Instructor TEXT, Type TEXT, "
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
        "Location TEXT, Points INT, Approval TEXT, Instructor TEXT, Type TEXT, "
        "Description TEXT, Subject TEXT, Number TEXT, Section TEXT, Key TEXT"
        ", Note TEXT, Title TEXT, Epithet TEXT);");
    */
    char call[SMALL_BUF]; char day[SMALL_BUF]; int points = 0; 
    char approval[SMALL_BUF]; char instructor[SMALL_BUF]; char type[SMALL_BUF];
    char description[BUF_SIZE]; char subject[SMALL_BUF];
    char number[SMALL_BUF]; char section[SMALL_BUF]; char key[SMALL_BUF];
    char note[BUF_SIZE]; char title[MED_BUF]; char epithet[MED_BUF];
    char loc[SMALL_BUF];

    memset(call, 0, sizeof(call)); memset(day, 0, sizeof(day));
    memset(approval, 0, sizeof(approval));
    memset(instructor, 0, sizeof(instructor)); memset(type, 0, sizeof(type));
    memset(description, 0, sizeof(description));
    memset(subject, 0, sizeof(subject));
    memset(number, 0, sizeof(number)); memset(section, 0, sizeof(section));
    memset(key, 0, sizeof(key)); memset(note, 0, sizeof(note));
    memset(title, 0, sizeof(title)); memset(epithet, 0, sizeof(epithet));
    memset(loc, 0, sizeof(loc));
      
    
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
        char *word_color = (char *)"bgcolor=#99CCFF>";
        char *pch = buf;
      
        if (!strstr(buf, word_color)) {
          word_color = (char *) "bgcolor=#DADADA>";
          
          // first part of cleaning up HTML line to get data
          // Handles when bgcolor is in line (most cases)
          if (strstr(buf, word_color)) {
            pch = strstr(pch, word_color) + strlen(word_color);
          }
          // second part of cleaning up HTML line to get data
          // Handles the end part of the line
          if (strstr(buf, "</td>")) {
            int end = strlen(strstr(buf, "</td>"));
            pch[strlen(pch)-end] = 0;
}

          // Assigning values and specific cleaning for different course fields
          // setting title value
          if (strcmp(word, "b>") == 0 && strstr(buf, "+1")) {
            //fprintf(stdout, "WORD: %s\n", word);
            pch = strstr(pch, ">") + 1;
            int n = strlen(pch) - strlen(strstr(pch, "</"));
            strncpy(title, pch, n);
            //fprintf(stdout, "TITLE: %s\n", title);
          }
          // setting epithet value
          else if (strcmp(word, "b>") == 0 && strstr(buf, "+2")) {
            //fprintf(stdout, "WORD: %s\n", word);
            pch = strstr(pch, ">") + 1;
            int n = strlen(pch) - strlen(strstr(pch, "</"));
            strncpy(epithet, pch, n);
            //fprintf(stdout, "EPITHET: %s\n", epithet);
          }
          // setting call number value
          else if (strcmp(word, "Call Number") == 0) {
            strcpy(call, pch);
            //fprintf(stdout, "CALL: %s\n", call);
          }
          // setting day value
          else if (strcmp(word, "Day &amp; Time") == 0) {
            int lenwhole = strlen(pch);
            char *br = strstr(pch, "<br>");
            int lenloc = strlen(br);
            strncpy(day, pch, lenwhole - lenloc);
            strcpy(loc, br + 4);
            //fprintf(stdout, "DAY: %s\n", day);
            //fprintf(stdout, "LOCATION: %s\n", loc);
          }
          // setting call number value
          else if (strcmp(word, "Points") == 0) {
            points = atoi(pch);
            //fprintf(stdout, "POINTS: %d\n", points);
          }
          // setting approval value
          else if (strcmp(word, "Approvals Required") == 0) {
            strcpy(approval, pch);
            //fprintf(stdout, "APPROVAL: %s\n", approval);
          }
          // TODO: Clean up fringe case where instructor includes email
          //       and webpage
          // setting instructor value
          else if (strcmp(word, "Instructor") == 0) {
            strcpy(instructor, pch);
            //fprintf(stdout, "INSTRUCTOR: %s\n", instructor);
          }
          // setting type value
          else if (strcmp(word, "Type") == 0) {
            strcpy(type, pch);
            //fprintf(stdout, "TYPE: %s\n", type);
          }
          // setting desciption value
          else if (strcmp(word, "Course Description") == 0) {
            strcat(description, pch);
            //fprintf(stdout, "DESCRIPTION: %s\n", description);
          }
          // setting subject value
          else if (strcmp(word, "Subject") == 0) {
            strcpy(subject, pch);
            //fprintf(stdout, "SUBJECT: %s\n", subject);
          }
          // setting number value
          else if (strcmp(word, "Number") == 0) {
            strcpy(number, pch);
            //fprintf(stdout, "NUMBER: %s\n", number);
          }
          // setting section value
          else if (strcmp(word, "Section") == 0) {
            strcpy(section, pch);
            //fprintf(stdout, "SECTION: %s\n", section);
          }
          // setting note value
          else if (strcmp(word, "Note") == 0) {
            strcat(note, pch);
            //fprintf(stdout, "NOTE: %s\n", note);
          }
          // setting key value
          else if (strcmp(word, "Section key") == 0) {
            strcpy(key, pch);
            fprintf(stdout, "SECTION KEY: %s\n", key);

            // Change word so no more processing of lines
            strcpy(word, "");
          }
          
        }
        // Have to update the key word after parsing values, so values are
        // present to be assigned
        // setting keyword
        word_color = (char *) "bgcolor=#99CCFF>";
        if (strstr(buf, word_color)) {
          pch = strstr(buf, word_color) + strlen(word_color);
          pch = strtok(pch, "<");

          strcpy(word, pch);

          // for testing
          //fprintf(stdout, "%s\n", pch);
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
