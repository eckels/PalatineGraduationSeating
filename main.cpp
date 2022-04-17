// Global Imports
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <queue>

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
    string attribute;
    bool sibling;
    Student() {
        sibling = false;
    }
};

struct Seat {
    Student* student;
    bool occupied;
    Seat() {
        occupied = false;
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
vector<vector<Seat*> > LEFT_SIDE;
vector<vector<Seat*> > RIGHT_SIDE;

// Misc
string NEXT_SIDE = FIRST_SIDE;
queue<Student*> SIBLING_QUEUE;

// Create empty candidate objects
void generate_side(vector<vector<Seat*> >& side) {
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

void sort_students() {
    for (int i = 0; i < RAW_STUDENTS.size(); i++) {
        Student* current = RAW_STUDENTS.at(i);
        if (current->attribute.find("Senior Board") != string::npos) {
            SENIOR_BOARD_LIST.push_back(current);
        } else if (current->attribute.find("Speaker") != string::npos) {
            SPEAKER_LIST.push_back(current);
        } else if (current->attribute.find("Academic Scholar") != string::npos) {
            ACADEMIC_SCHOLARS_LIST.push_back(current);
        } else if (current->attribute.find("SPED Regular") != string::npos) {
            EVERYONE_ELSE_LIST.push_back(current);
        } else if (current->attribute.find("SPED Back") != string::npos) {
            SPED_LIST.push_back(current);
        } else if (current->attribute.find("Excluded") == string::npos) {
            EVERYONE_ELSE_LIST.push_back(current);
        }
    }
}

void assign_seat(Seat* seat, Student* student) {
    seat->student = student;
    seat->occupied = true;
}

bool bound_checker(vector<vector<Seat*> >& to_assign, int& row, int& column, string direction) {
    if (column >= ROW_LENGTH) {
        column = 0;
        if (direction == "back") {
            row -= 1;
        } else {
            row += 1;
        }
    }
    if (row < 0 || row >= NUM_ROWS) {
        return false;
    }
    return true;
}

bool seating_helper(int& lrow, int& rrow, int& lcolumn, int& rcolumn, vector<Student*>& students, string direction) {
    bool side = (NEXT_SIDE == "left") ? true : false;
    vector<vector<Seat*> >& to_assign = (side) ? LEFT_SIDE : RIGHT_SIDE;
    int& row = (side) ? lrow : rrow;
    int& column = (side) ? lcolumn : rcolumn;

    if (bound_checker(to_assign, row, column, direction) == false) return false;
    
    Student* curr_student = students.front();
    if (curr_student->sibling == true) {
        SIBLING_QUEUE.push(students.front());
        students.erase(students.begin());
        SIBLING_QUEUE.push(students.front());
        students.erase(students.begin());
        return true;
    }

    if (to_assign[row][column]->occupied == false) {
        if (SIBLING_QUEUE.empty() == false) {
            if (column + 1 < ROW_LENGTH && to_assign[row][column + 1]->occupied == false) {
                assign_seat(to_assign[row][column], SIBLING_QUEUE.front());
                SIBLING_QUEUE.pop();
                assign_seat(to_assign[row][column + 1], SIBLING_QUEUE.front());
                SIBLING_QUEUE.pop();
                column += 2;
                NEXT_SIDE = (NEXT_SIDE == "left") ? "right" : "left";
                return true;
            }
        }

        assign_seat(to_assign[row][column], curr_student);
        students.erase(students.begin());
        column += 1;

        NEXT_SIDE = (NEXT_SIDE == "left") ? "right" : "left";
    } else {
        column += 1;
    }
    return true;
}

void seat_from_back(vector<Student*>& students) {
    int lrow = NUM_ROWS - 1;
    int rrow = NUM_ROWS - 1;
    int lcolumn = 0;
    int rcolumn = 0;
    while (!students.empty() || !SIBLING_QUEUE.empty()) {
        bool res = seating_helper(lrow, rrow, lcolumn, rcolumn, students, "back");
        if (res == false) break;
    }
}

void seat_from_front(vector<Student*>& students) {
    int lrow = 0;
    int rrow = 0;
    int lcolumn = 0;
    int rcolumn = 0;
    while (!students.empty() || !SIBLING_QUEUE.empty()) {
        bool res = seating_helper(lrow, rrow, lcolumn, rcolumn, students, "front");
        if (res == false) break;
    }
}

void populate_seating_chart() {
    seat_from_back(SPED_LIST);
    NEXT_SIDE = FIRST_SIDE;
    seat_from_front(SENIOR_BOARD_LIST);
    seat_from_front(SPEAKER_LIST);
    seat_from_front(ACADEMIC_SCHOLARS_LIST);
    seat_from_front(EVERYONE_ELSE_LIST);
}

string five_printer(string print) {
    string res = print;
    if (print.length() < 5) {
        for (int i = print.length(); i < 5; i++) {
            res += " ";
        }
    }
    return res;
}

void print_chart() {
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < ROW_LENGTH; j++) {
            cout << " ----- ";
        }
        cout << "     ";
        for (int j = 0; j < ROW_LENGTH; j++) {
            cout << " ----- ";
        }
        cout << endl;

        for (int j = ROW_LENGTH - 1; j >= 0; j--) {
            if (LEFT_SIDE[i][j]->occupied) {
                cout << " " << five_printer(LEFT_SIDE[i][j]->student->last.substr(0, 5)) << " ";
            } else {
                cout << "       ";
            }
        }
        cout << "     ";
        for (int j = 0; j < ROW_LENGTH; j++) {
            if (RIGHT_SIDE[i][j]->occupied) {
                cout << " " << five_printer(RIGHT_SIDE[i][j]->student->last.substr(0, 5)) << " ";
            } else {
                cout << "       ";
            }
        }
        cout << endl;

        for (int j = ROW_LENGTH - 1; j >= 0; j--) {
            if (LEFT_SIDE[i][j]->occupied) {
                cout << " " << five_printer(LEFT_SIDE[i][j]->student->first.substr(0, 5)) << " ";
            } else {
                cout << "   -   ";
            }
        }
        cout << "     ";
        for (int j = 0; j < ROW_LENGTH; j++) {
            if (RIGHT_SIDE[i][j]->occupied) {
                cout << " " << five_printer(RIGHT_SIDE[i][j]->student->first.substr(0, 5)) << " ";
            } else {
                cout << "   -   ";
            }
        }
        cout << endl;

        for (int j = ROW_LENGTH - 1; j >= 0; j--) {
            if (LEFT_SIDE[i][j]->occupied) {
                cout << " " << five_printer(LEFT_SIDE[i][j]->student->attribute.substr(0, 5)) << " ";
            } else {
                cout << "       ";
            }
        }
        cout << "     ";
        for (int j = 0; j < ROW_LENGTH; j++) {
            if (RIGHT_SIDE[i][j]->occupied) {
                cout << " " << five_printer(RIGHT_SIDE[i][j]->student->attribute.substr(0, 5)) << " ";
            } else {
                cout << "       ";
            }
        }
        cout << endl;
    }
}

void print_student_list(vector<Student*>& list) {
    for (int i = 0; i < list.size(); i++) {
        cout << list[i]->last << ", " << list[i]->first << " - " << list[i]->attribute << endl;
    }
}

void import_students() {
    vector<vector<string> > content;
	vector<string> row;
    string line;
    string word;

    fstream file ("shortdata.csv", ios::in);

    if(file.is_open()) {
        while (getline(file, line)) {
            row.clear();
            stringstream str(line);
            while(getline(str, word, ',')) {
                row.push_back(word);
                //cout << word << endl;
            }
            content.push_back(row);
        }
    } else {
        cout << "Could not open the file\n";
    }
    for (int i = 0; i < content.size(); i++) {
        Student* student = new Student();
        student->last = content[i][0];
        student->first = content[i][1];
        if (content[i].size() > 2) {
            student->attribute = content[i][2];
            if (content[i][2].find("Sibling") != string::npos) {
                student->sibling = true;
            }
        }
        RAW_STUDENTS.push_back(student);
    }
}

int main(int argc, char *argv[]) {
    import_students();
    //print_student_list(RAW_STUDENTS);
    // Set up seating
    NUM_ROWS = 10;
    ROW_LENGTH = 5;
    generate_side(LEFT_SIDE);
    generate_side(RIGHT_SIDE);

    sort_students();
    //print_student_list(SENIOR_BOARD_LIST);
    print_student_list(RAW_STUDENTS);
    populate_seating_chart();
    print_chart();
    //cout << RIGHT_SIDE[3][4]->student->attribute << endl;
    return 0;
}