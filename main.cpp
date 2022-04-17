// Global Imports
#include<bits/stdc++.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int NUM_ROWS = 0;
int ROW_LENGTH = 0;
string FIRST_SIDE = "left";

enum Attribute {
    SeniorBoard,
    Speaker,
    AcademicScholar,
    SPEDRegular,
    SpedBack,
    Excluded
};

struct Student {
    string first;
    string last;
    Attribute attribute;
    vector<Student*> siblings;
};

struct Seat {
    Student* student;
    bool exists;
    Seat() {
        exists = false;
    }
};

// Primary student orderings
vector<Student*> RAW_STUDENTS;
vector<Student*> ORDERED_STUDENTS;

// Secondary student orderings
vector<Student*> SENIOR_BOARD_LIST;
vector<Student*> SPEAKER_LIST;
vector<Student*> ACADEMIC_SCHOLARS_LIST;
vector<Student*> SPED_LIST;
vector<Student*> EVERYONE_ELSE_LIST;

// Inside of row is always front of vector
vector<vector<Seat*>> LEFT_SIDE;
vector<vector<Seat*>> RIGHT_SIDE;

// Create empty candidate objects
void generate_side(vector<vector<Seat*>> side) {
    for (int i = 0; i < NUM_ROWS; i++) {
        vector<Seat*> row;
        for (int j = 0; j < ROW_LENGTH; j++) {
            Seat* new_seat = new Seat();
            row.push_back(new_seat);
        }
        side.push_back(row);
    }
}

bool compareStudents(const Student& a, const Student& b) {
    string aname = a.last;
    string bname = b.last;
    transform(aname.begin(), aname.end(), aname.begin(), ::tolower);
    transform(bname.begin(), bname.end(), bname.begin(), ::tolower);

    int lastname = aname.compare(bname);
    if (lastname < 0) {
        return true;
    } else if (lastname > 0) {
        return false;
    } else {
        aname = a.first;
        bname = b.first;
        transform(aname.begin(), aname.end(), aname.begin(), ::tolower);
        transform(bname.begin(), bname.end(), bname.begin(), ::tolower);

        int firstname = aname.compare(bname);
        if (firstname > 0) {
            return false;
        } else {
            return true;
        }
    }
}

void order_list(vector<Student*>& list) {
    sort(list.begin(), list.end(), compareStudents);
}

void sort_students() {
    for (int i = 0; i < RAW_STUDENTS.size(); i++) {
        Student* current = RAW_STUDENTS.at(i);
        if (current->attribute == SeniorBoard) {
            SENIOR_BOARD_LIST.push_back(current);
        } else if (current->attribute == Speaker) {
            SPEAKER_LIST.push_back(current);
        } else if (current->attribute == AcademicScholar) {
            ACADEMIC_SCHOLARS_LIST.push_back(current);
        } else if (current->attribute == SPEDRegular) {
            EVERYONE_ELSE_LIST.push_back(current);
        } else if (current->attribute == SpedBack) {
            SPED_LIST.push_back(current);
        } else if (current->attribute != Excluded) {
            EVERYONE_ELSE_LIST.push_back(current);
        }
    }
    order_list(SENIOR_BOARD_LIST);
    order_list(SPEAKER_LIST);
    order_list(ACADEMIC_SCHOLARS_LIST);
    order_list(SPED_LIST);
    order_list(EVERYONE_ELSE_LIST);
}

int main(int argc, char *argv[]) {
    // Set up seating
    NUM_ROWS = 10;
    ROW_LENGTH = 5;
    generate_side(LEFT_SIDE);
    generate_side(RIGHT_SIDE);


}