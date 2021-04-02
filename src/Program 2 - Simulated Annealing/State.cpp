#include "State.h"
#include <iostream>
#include <fstream>
using namespace std;

State::State(vector<Course> courseVecOld) {
	// Assumes it will only be set to a vector of size 12.
	for (int i = 0; i < 12; i++) {
		courseVec.push_back(courseVecOld.at(i));
	}
}

void State::operator=(vector<Course> rhs) {
	// Assumes it will only be set to a vector of size 12.
	for (int i = 0; i < 12; i++) {
		courseVec.push_back(rhs.at(i));
	}
}

int State::getFitness() {
	int fitness = 0;

	// Individual course checks:
	for (int i = 0; i < 12; i++) {
		Course* ptrCrs = &courseVec.at(i);
		string c = ptrCrs->getCourse();
		string in = ptrCrs->getInstructor();
		string r = ptrCrs->getRoom();
		string t = ptrCrs->getTime();

		// +3 For each course taught by instructor can teach it
		if (canInstructorTeach(in, c) && in != "Staff") fitness += 3;

		// +1 For each course taught by student staff
		else if (isInstructorStaff(in)) fitness += 1;

		// +5 For each course that is the only course scheduled in a room at a time
		if (!isRoomDoubleBooked(r, t)) fitness += 5;

		// +5 For each course that is in a room large enough to fit it
		// +2 else if room capacity is no more than twice the expected enrollment
		if (canRoomFit(r, c)) {
			fitness += 5;
			if (!isCapacityDouble(r, c)) fitness += 2;
		}

		// +5 For each course that does not have the same instructor teaching another course at the same time
		if (!isInstructorDoubleBooked(in, t)) fitness += 5;
	}

	// -5 For each schedule that has the same instructor teaching more than 4 courses (per course over 4)
	if (hasOverScheduledInstructor()) {
		int spill = 0;

		// Does not consider staff.
		for (int i = 0; i < 6; i++) {
			int tSpill = getOverScheduleSpill(instructors[i]);
			if (tSpill >= 1) spill += tSpill;
		}
		spill *= 5;

		fitness -= spill;
	}

	// -10 For each schedulethat has Rao or Mitchell teaching more courses than Hare or Bingham
	if (isGradTeachingMore()) fitness -= 10;

	// CS101 and CS191 are usually taken the same semester. CS201 and CS291 as well.
	// -15 If either of the pairs are scheduled at the same time.
	// +5 If courses are scheduled for adjacent times.
	// ... if this AND in the same building, +5
	// ... if this AND both on the quad, +0
	// ... if this AND one is in katz || bloch and the oter isn't, -3
	string x[] = {"CS101A", "CS101A", "CS101B", "CS101B", "CS201A", "CS201A", "CS201B", "CS201B"};
	string y[] = {"CS191A", "CS191B", "CS191A", "CS191A", "CS291A", "CS291B", "CS291A", "CS291B"};

	for (int i = 0; i < 8; i++) {
		if (isScheduledSameTime(x[i], y[i])) fitness -= 15;

		if (isScheduledAdjacentTimes(x[i], y[i])) {
			fitness += 5;
			if (isScheduledSameBuilding(x[i], y[i])) fitness += 5;
			if (isOneBlock(x[i], y[i])) fitness -= 3;
			if (isOneKatz(x[i], y[i])) fitness -= 3;
		}
	}

	// +5 If CS101A and CS101B are scheduled 3 hours apart or more
	if (isScheduledThreeHoursApart("CS101A", "CS101B")) fitness += 5;

	// +5 If CS191A and CS191B are scheduled 3 hours apart or more
	if (isScheduledThreeHoursApart("CS191A", "CS191B")) fitness += 5;

	return fitness;
}

void State::fillRandomCourses() {
	for (int i = 0; i < 12; i++) {
		appendRandomCourse();
	}
}

void State::appendRandomCourse() {
	string randCourseStr = "";
	string randInstructorStr = "";
	string randRoomStr = "";
	string randTimeStr = "";
	int randElem;

	bool passed = false;
	while (!passed) {
		randElem = genRandInt(0, 12);
		if (usedCourses[randElem] == "") {
			randCourseStr = courses[randElem];
			usedCourses[randElem] = "NA";
			passed = true;
		}
	}
	
	randElem = genRandInt(0, 6);
	randInstructorStr = instructors[randElem];

	randElem = genRandInt(0, 7);
	randRoomStr = rooms[randElem];

	randElem = genRandInt(0, 7);
	randTimeStr = times[randElem];

	courseVec.push_back(Course(randCourseStr, randInstructorStr, randRoomStr, randTimeStr));
}

void State::makeRandomChange() {
	int option = genRandInt(0, 4);
	int optionSize = optionSizes[option];

	if (option == 0) {
		int newElem = genRandInt(0, 12);
		string newCourse = courseVec.at(newElem).getCourse();

		int oldElem = genRandInt(0, 12);
		string oldCourse = courseVec.at(oldElem).getCourse();

		courseVec.at(newElem).setCourse(oldCourse);
		courseVec.at(oldElem).setCourse(newCourse);
	}

	else if (option == 1) courseVec.at(genRandInt(0, 12)).setInstructor(instructors[genRandInt(0, optionSize)]);

	else if (option == 2) courseVec.at(genRandInt(0, 12)).setRoom(rooms[genRandInt(0, optionSize)]);

	else if (option == 3) courseVec.at(genRandInt(0, 12)).setTime(times[genRandInt(0, optionSize)]);
}

void State::outputCourses() {
	for (int i = 0; i < 12; i++) {
		Course* ptrCrs = &courseVec.at(i);

		cout << i + 1 << ":\n";
		ptrCrs->outputCourse();
		cout << endl;
	}
}

void State::foutputCourses(string output) {
	ofstream fout(output);

	for (int i = 0; i < 12; i++) {
		Course* ptrCrs = &courseVec.at(i);

		fout << " >> " << ptrCrs->getCourse() << ", ";
		fout << ptrCrs->getInstructor() << ", ";
		fout << ptrCrs->getRoom() << ", ";
		fout << ptrCrs->getTime() << endl;
	}
	fout << " Fitness: " << getFitness() << endl;
	fout.close();
}

int State::genRandInt(int min, int max) {
	return rand() % max + min;
}

template <class T>
int State::getIndex(T targArr[], T target, int n) {
	for (int i = 0; i < n; i++) 
		if (targArr[i] == target) return i;
	return -1;
}

bool State::canInstructorTeach(string instructor, string course) {
	if (instructor == "Staff") return false;

	int instructorElem = getIndex(instructors, instructor, 5);
	int courseElem = getIndex(courses, course, 12);

	// Using instructorClasses, navigate to that instructor's row.
	// Search through that row for a matching element of the course's element.
	for (int i = 0; i < 12; i++) 
		if (instructorClasses[instructorElem][i] == courseElem) return true;
	return false;
}

bool State::isInstructorStaff(string instructor) {
	int elem = getIndex(instructors, instructor, 6);
	if (elem == 5) return true;
	return false;
}

bool State::canRoomFit(string room, string course) {
	int roomElem = getIndex(rooms, room, 7);
	int courseElem = getIndex(courses, course, 12);

	// If the room size is greater than or equal to the course size, return true.
	if (roomSizes[roomElem] >= courseSizes[courseElem]) return true;
	return false;
}

bool State::isCapacityDouble(string room, string course) {
	int roomElem = getIndex(rooms, room, 7);
	int courseElem = getIndex(courses, course, 12);

	// If the room's capacity is double the course size, return true
	if (roomSizes[roomElem] >= (courseSizes[courseElem] * 2)) return true;
	return false;
}

bool State::isInstructorDoubleBooked(string instructor, string time) {
	int count = 0;
	// If the instructor is teaching a course at the given time, increment count.
	// If count > 1, return true.
	for (int i = 0; i < 12; i++) {
		Course* ptrCrs = &courseVec.at(i);
		if (ptrCrs->getInstructor() == instructor && ptrCrs->getTime() == time) count++;
		if (count > 1) return true;
	}
	return false;
}

bool State::isRoomDoubleBooked(string room, string time) {
	int count = 0;

	for (int i = 0; i < 12; i++) {
		Course* ptrCrs = &courseVec.at(i);
		if (ptrCrs->getRoom() == room && ptrCrs->getTime() == time) count++;
		if (count > 1) return true;
	}
	return false;
}

bool State::hasOverScheduledInstructor() {
	int raoCount = 0;
	int mitchellCount = 0;
	int binghamCount = 0;
	int hareCount = 0;
	int kuhailCount = 0;
	int staffCount = 0;

	for (int i = 0; i < 12; i++) {
		if (courseVec.at(i).getInstructor() == "Rao") raoCount++;
		else if (courseVec.at(i).getInstructor() == "Mitchell") mitchellCount++;
		else if (courseVec.at(i).getInstructor() == "Bingham") binghamCount++;
		else if (courseVec.at(i).getInstructor() == "Hare") hareCount++;
		else if (courseVec.at(i).getInstructor() == "Kuhail") kuhailCount++;
		else if (courseVec.at(i).getInstructor() == "Staff") staffCount++;
	}

	return
		(raoCount > 4) ||
		(mitchellCount > 4) ||
		(binghamCount > 4) ||
		(hareCount > 4) ||
		(kuhailCount > 4) ||
		(staffCount > 4);
}

int State::getOverScheduleSpill(string instructor) {
	int count = 0;
	for (int i = 0; i < 12; i++)
		if (courseVec.at(i).getInstructor() == instructor) count++;
	return (count - 4);
}

bool State::isInstructorOverScheduled(string instructor) {
	if (instructor == "Staff") return false;

	int count = 0;
	for (int i = 0; i < 12; i++) 
		if (courseVec.at(i).getInstructor() == instructor) count++;
	return count > 4;
}

bool State::isGradTeachingMore() {
	int raoCount = 0;
	int mitchellCount = 0;
	int binghamCount = 0;
	int hareCount = 0;

	for (int i = 0; i < 12; i++) {
		if (courseVec.at(i).getInstructor() == "Rao") raoCount++;
		else if (courseVec.at(i).getInstructor() == "Mitchell") mitchellCount++;
		else if (courseVec.at(i).getInstructor() == "Bingham") binghamCount++;
		else if (courseVec.at(i).getInstructor() == "Hare") hareCount++;
	}

	// If either of the grad students have more than a prof, return true.
	return 
		(raoCount > binghamCount) ||
		(raoCount > hareCount) ||
		(mitchellCount > binghamCount) ||
		(mitchellCount > hareCount);
}

bool State::isScheduledSameTime(string courseOne, string courseTwo) {
	int courseOneElem = -1;
	for (int i = 0; i < 12; i++)
		if (courseOne == courseVec.at(i).getCourse()) courseOneElem = i;

	int courseTwoElem = -1;
	for (int i = 0; i < 12; i++)
		if (courseTwo == courseVec.at(i).getCourse()) courseTwoElem = i;

	return (courseVec.at(courseOneElem).getTime() == courseVec.at(courseTwoElem).getTime());
}

bool State::isScheduledAdjacentTimes(string courseOne, string courseTwo) {
	int courseOneElem = -1;
	for (int i = 0; i < 12; i++)
		if (courseOne == courseVec.at(i).getCourse()) courseOneElem = i;

	int courseTwoElem = -1;
	for (int i = 0; i < 12; i++)
		if (courseTwo == courseVec.at(i).getCourse()) courseTwoElem = i;

	string timeOne = courseVec.at(courseOneElem).getTime();
	string timeTwo = courseVec.at(courseTwoElem).getTime();

	int timeOneElem = getIndex(times, timeOne, 7);
	int timeOneElemLeft = timeOneElem - 1;
	int timeOneElemRight = timeOneElem + 1;

	if (timeOneElemLeft != -1)
		if (times[timeOneElemLeft] == timeTwo) return true;

	if (timeOneElemRight != 12)
		if (times[timeOneElemRight] == timeTwo) return true;

	return false;
}

bool State::isScheduledSameBuilding(string courseOne, string courseTwo) {
	int courseOneElem = -1;
	for (int i = 0; i < 12; i++)
		if (courseOne == courseVec.at(i).getCourse()) courseOneElem = i;

	int courseTwoElem = -1;
	for (int i = 0; i < 12; i++)
		if (courseTwo == courseVec.at(i).getCourse()) courseTwoElem = i;

	return (courseVec.at(courseOneElem).getRoom() == courseVec.at(courseTwoElem).getRoom());
}

bool State::isScheduledFar(string courseOne, string courseTwo) {
	int courseOneElem = -1;
	for (int i = 0; i < 12; i++)
		if (courseOne == courseVec.at(i).getCourse()) courseOneElem = i;

	int courseTwoElem = -1;
	for (int i = 0; i < 12; i++)
		if (courseTwo == courseVec.at(i).getCourse()) courseTwoElem = i;

	courseOne = courseVec.at(courseOneElem).getRoom();
	courseTwo = courseVec.at(courseTwoElem).getRoom();
	return
		((courseOne == "KATS209" || courseOne == "BLOC009") && (courseTwo != "KATS209" && courseTwo != "BLOC009")) ||
		((courseOne != "KATS209" && courseOne != "BLOC009") && (courseTwo == "KATS209" || courseTwo == "BLOC009"));
}

bool State::isOneBlock(string courseOne, string courseTwo) {
	int courseOneElem = -1;
	for (int i = 0; i < 12; i++)
		if (courseOne == courseVec.at(i).getCourse()) courseOneElem = i;

	int courseTwoElem = -1;
	for (int i = 0; i < 12; i++)
		if (courseTwo == courseVec.at(i).getCourse()) courseTwoElem = i;

	return
		(courseVec.at(courseOneElem).getRoom() == "BLOC009" && courseVec.at(courseTwoElem).getRoom() != "BLOC009") ||
		(courseVec.at(courseOneElem).getRoom() != "BLOC009" && courseVec.at(courseTwoElem).getRoom() == "BLOC009");
}

bool State::isOneKatz(string courseOne, string courseTwo) {
	int courseOneElem = -1;
	for (int i = 0; i < 12; i++)
		if (courseOne == courseVec.at(i).getCourse()) courseOneElem = i;

	int courseTwoElem = -1;
	for (int i = 0; i < 12; i++)
		if (courseTwo == courseVec.at(i).getCourse()) courseTwoElem = i;

	return
		(courseVec.at(courseOneElem).getRoom() == "KATZ209" && courseVec.at(courseTwoElem).getRoom() != "KATZ209") ||
		(courseVec.at(courseOneElem).getRoom() != "KATZ209" && courseVec.at(courseTwoElem).getRoom() == "KATZ209");
}

bool State::isScheduledThreeHoursApart(string courseOne, string courseTwo) {
	int courseOneElem = -1;
	for (int i = 0; i < 12; i++)
		if (courseOne == courseVec.at(i).getCourse()) courseOneElem = i;

	int courseTwoElem = -1;
	for (int i = 0; i < 12; i++)
		if (courseTwo == courseVec.at(i).getCourse()) courseTwoElem = i;

	string timeOne = courseVec.at(courseOneElem).getTime();
	string timeTwo = courseVec.at(courseTwoElem).getTime();

	int timeOneElem = getIndex(times, timeOne, 7);

	int timeOneElemLeft = timeOneElem - 3;
	while (timeOneElemLeft > -1) {
		if (times[timeOneElemLeft] == timeTwo) return true;
		timeOneElemLeft--;
	}

	int timeOneElemRight = timeOneElem + 3;
	while (timeOneElemRight < 7) {
		if (times[timeOneElemRight] == timeTwo) return true;
		timeOneElemRight++;
	}

	return false;
}

void State::debugConditions() {
	for (int i = 0; i < 3; i++) {
		Course testCourseOne("CS101A", "Bingham", "ROYL204", "4P");
		Course testCourseTwo("CS101B", "Hare", "FLAR260", "3P");
		Course testCourseThree("CS201A", "Mitchell", "BLOC009", "1P");
		Course testcourseFour("CS201B", "Kuhail", "KATZ209", "12P");

		courseVec.push_back(testCourseOne);
		courseVec.push_back(testCourseTwo);
		courseVec.push_back(testCourseThree);
		courseVec.push_back(testcourseFour);
	}


	bool arrCond[] = {
		canInstructorTeach("Bingham", "CS101A"),
		isInstructorStaff("Staff") && !isInstructorStaff("Hare"),
		!canRoomFit("FLAR260", "CS101A") && canRoomFit("FLAR260", "CS101B"),
		!isCapacityDouble("FLAR260", "CS101B") && isCapacityDouble("HAAG301", "CS101B"),
		isInstructorDoubleBooked("Bingham", "4P") && !isInstructorDoubleBooked("Bingham", "3P"),
		isRoomDoubleBooked("ROYL204", "4P") && !isRoomDoubleBooked("ROYL204", "3P"),
		!hasOverScheduledInstructor(),
		!isInstructorOverScheduled("Bingham"),
		!isGradTeachingMore(),
		!isScheduledSameTime("CS101A", "CS101B"),
		isScheduledAdjacentTimes("CS101A", "CS101B"),
		isScheduledSameBuilding("CS101A", "CS101A") && !isScheduledSameBuilding("CS101A", "CS101B"),
		!isScheduledFar("CS101A", "CS101B"),
		isOneBlock("CS201A", "CS101B"),
		isOneKatz("CS201B", "CS101B"),
		isScheduledThreeHoursApart("CS101A", "CS201A")
	};

	bool isValid = true;
	for (int i = 0; i < sizeof(arrCond); i++) {
		if (arrCond[i] != true) {
			isValid = false;
			cout << "[DEBUG] Conditions do not work as expected to... " << i+1 << endl;
		}
	}

	if (isValid) cout << "[DEBUG] Conditions work as expected to..." << endl;
	courseVec.clear();
}