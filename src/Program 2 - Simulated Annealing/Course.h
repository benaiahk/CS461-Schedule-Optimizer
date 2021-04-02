#include <string>
#include <iostream>
using namespace std;
#pragma once

class Course {
private:
	string instructor;
	string room;
	string time;
	string course;
public:
	Course(string course, string instructor, string room, string time) {
		this->course = course;
		this->instructor = instructor;
		this->room = room;
		this->time = time;
	}

	void outputCourse() {
		cout << course << ", " << instructor << ", " << room << ", " << time << endl;
	}

	string getCourse() { return course; }

	void setCourse(string course) { this->course = course; }

	string getInstructor() { return instructor; }

	void setInstructor(string instructor) { this->instructor = instructor; }

	string getRoom() { return room; }

	void setRoom(string room) { this->room = room; }

	string getTime() { return time; }

	void setTime(string time) { this->time = time; }
};

