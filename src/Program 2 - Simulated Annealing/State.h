#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <map>
#include <vector>
#include <string>
#include "Course.h"
using namespace std;
#pragma once

class State {
private:
	/*
	One course per room.
	Room must hold the expected amount of students.
	Course must have an instructor.
	Course can be taught by student.
	Classes taken same semester prefer adjacent times.
	Adjacent times should have adjacent rooms. 
	Same courses should be separated by time.
	*/

	/*
	HAAG 301: 70
	HAAG 206: 30
	ROYL 204: 70
	KATZ 209: 50 -> Close, not in quad
	FLAR 310: 80
	FLAR 260: 25
	BLOC 009: 30 -> Really far
	*/

	/*
	CS101-A: 40
	CS101-B: 25
	CS201-A: 30
	CS201-B: 30
	CS191-A: 60
	CS191-B: 20
	CS291-A: 20
	CS291-B: 40
	CS303:   50
	CS341:   40
	CS449:   55
	CS461:   40
	*/

	/*
	10 A
	11 A
	12 P
	1  P
	2  P
	3  P
	4  P
	*/

	/*
	0 - Hare
	1 - Bingham
	2 - Kuhail
	3 - Mitchell -> Graduate
	4 - Rao -> Graduate
	5 - Staff
	*/

	// Each course needs a room, instructor, and time. 
	// Lower temp parameter after 400 successful changes, or 4000 attempts. 
	// Continue until you reach 4000 attempts with no changes.
	// Output final schedule to a file with the fitness score.

	// Array of available courses & their corresponding sizes.
	string courses[12] = { "CS101A", "CS101B", "CS201A", "CS201B", "CS191A", "CS191B",
							"CS291A", "CS291B", "CS303", "CS341", "CS449", "CS461" };
	string usedCourses[12] = { "", "", "", "", "", "", "", "", "", "", "", "" };
	int courseSizes[12] = { 40, 25, 30, 30, 60, 20, 20, 40, 50, 40, 55, 40 };


	// Array of classes & their corresponding sizes.
	string rooms[7] = { "HAAG301",  "HAAG206",  "ROYL204",  "KATZ209",  "FLAR310",  "FLAR260",  "BLOC009" };
	int roomSizes[7] = { 70, 30, 70, 50, 80, 25, 30 };

	// Array of available times.
	string times[7] = { "10A",  "11A",  "12P",  "1P",  "2P",  "3P",  "4P" };
	
	// Array of instructors. Elem 3,4 == students.
	string instructors[6] = { "Hare", "Bingham", "Kuhail", "Mitchell", "Rao", "Staff" };
	int instructorClasses[6][12] = { {0, 1, 2, 3, 6, 7, 8, 10, 11, -1, -1, -1},
									 {0, 1, 2, 3, 4, 5, 6, 7, 10, -1, -1, -1},
									 {8, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1 ,-1},
									 {4, 5, 6, 7, 8, 11, -1, -1, -1, -1, -1, -1},
									 {6, 7, 8, 9, 11, -1, -1, -1, -1, -1, -1, -1},
									 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11} };

	// Array of possible variables to change.
	int optionSizes[4] = { 12, 6, 7, 7 };

public:
	// Vector of courses.
	vector<Course> courseVec;

	State() {
	}

	State(vector<Course> courseVecOld);

	void operator=(vector<Course> rhs);

	int genRandInt(int min, int max);

	void fillRandomCourses();

	void appendRandomCourse();

	void makeRandomChange();

	void outputCourses();

	void foutputCourses(string output);

	int getFitness();

	template <class T>
	int getIndex(T targArr[], T target, int n);

	bool canInstructorTeach(string instructor, string course);

	bool isInstructorStaff(string instructor);

	bool canRoomFit(string room, string course);

	bool isCapacityDouble(string room, string course);

	bool isInstructorDoubleBooked(string instructor, string time);

	bool isRoomDoubleBooked(string room, string time);

	bool hasOverScheduledInstructor();

	int getOverScheduleSpill(string instructor);

	bool isInstructorOverScheduled(string instructor);

	bool isGradTeachingMore();

	bool isScheduledSameTime(string courseOne, string courseTwo);

	bool isScheduledAdjacentTimes(string courseOne, string courseTwo);

	bool isScheduledSameBuilding(string courseOne, string courseTwo);

	bool isScheduledFar(string courseOne, string courseTwo);

	bool isOneBlock(string courseOne, string courseTwo);

	bool isOneKatz(string courseOne, string courseTwo);

	bool isScheduledThreeHoursApart(string courseOne, string courseTwo);

	void debugConditions();
};

