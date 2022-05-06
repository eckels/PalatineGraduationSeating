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

/**************************
*
* Configurations
*
**************************/

// Attribute Keywords
const string SENIOR_BOARD_KEYWORD = "Senior Board";
const string SPEAKER_KEYWORD = "Speaker";
const string ACADEMIC_SCHOLARS_KEYWORD = "Academic Scholar";
const string SPED_REGULAR_KEYWORD = "SPED Regular Seating";
const string SPED_BACK_KEYWORD = "SPED Back Row";
const string NOT_GRADUATING_KEYWORD = "Not Graduating";
const string SIBLING_TWIN_KEYWORD = "Twins/Siblings";

// Add-on Keywords
const string SPED_HELPER_KEYWORD = "Helper";
const string SPED_TA_KEYWORD = "TA";

// File Names
string STUDENT_IMPORT_NAME = "student_list.csv";
const string LEFT_SIDE_SPEAKER_FILE_NAME = "left_side_speaker_list.txt";
const string RIGHT_SIDE_SPEAKER_FILE_NAME = "right_side_speaker_list.txt";
const string SEATING_CHART_FILE_NAME = "seating_chart.csv";

// Print seat number alongside name or not (speaker list and seating chart)
const bool SPEAKER_SEAT_NUM_FLAG = true;
const bool CHART_SEAT_NUM_FLAG = true;

// Number of faculty speakers on each side
const int FACULTY_SPEAKER_SIDE_NUM = 2;

// Stage rows and columns
int NUM_ROWS = 38;
int ROW_LENGTH = 9;

// Banner space size for the Arena banner
const int BANNER_SPACER_SIZE = 10;


/**************************
*
* Data Structures
*
**************************/

struct Student {
    string first;
    string last;
    string attribute;
    bool sibling;
    bool helper;
    int seat_number;
    Student() {
        sibling = false;
        helper = false;
    }
};

struct Seat {
    int seat_number;
    Student* student;
    bool occupied;
    Seat() {
        occupied = false;
    }
};

// Primary student orderings
vector<Student*> RAW_STUDENTS;
vector<Student*> STUDENTS_GRADUATING_ALPHABETICAL;
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

queue<Student*> SIBLING_QUEUE;
// Negative if more on left, positive if more on right. If zero, put on left
int SIDE_DIFF = 0;

/**************************
*
* Main Functions
*
**************************/

// Create empty candidate objects
void generate_side(vector<vector<Seat*> >& side, bool first) {
    int counter = (first == true) ? 1 : 2;
    for (int i = 0; i < NUM_ROWS; i++) {
        vector<Seat*> row;
        for (int j = 0; j < ROW_LENGTH; j++) {
            Seat* new_seat = new Seat();
            new_seat->seat_number = counter;
            row.push_back(new_seat);
            counter += 2;
        }
        side.push_back(row);
    }
}

void generate_banner_list() {
    for (int i = 0; i < RAW_STUDENTS.size(); i++) {
        Student* current = RAW_STUDENTS.at(i);
        if (current->attribute.find(NOT_GRADUATING_KEYWORD) == string::npos && current->helper != true) {
            STUDENTS_GRADUATING_ALPHABETICAL.push_back(current);
        }
    }
}

void print_banner_file() {
    string spacer = "";
    for (int i = 0; i < BANNER_SPACER_SIZE; i++) {
        spacer += " ";
    }

    ofstream outfile ("arena_ribbon_banner.txt");

    for (int i = 0; i < STUDENTS_GRADUATING_ALPHABETICAL.size(); i++) {
        Student* current = STUDENTS_GRADUATING_ALPHABETICAL.at(i);
        outfile << current->first << " " << current->last << spacer;
    }

    outfile.close();
}

void sort_students() {
    for (int i = 0; i < RAW_STUDENTS.size(); i++) {
        Student* current = RAW_STUDENTS.at(i);
        if (current->attribute.find(SENIOR_BOARD_KEYWORD) != string::npos) {
            SENIOR_BOARD_LIST.push_back(current);
        } else if (current->attribute.find(SPEAKER_KEYWORD) != string::npos) {
            SPEAKER_LIST.push_back(current);
        } else if (current->attribute.find(ACADEMIC_SCHOLARS_KEYWORD) != string::npos) {
            ACADEMIC_SCHOLARS_LIST.push_back(current);
        } else if (current->attribute.find(SPED_REGULAR_KEYWORD) != string::npos) {
            EVERYONE_ELSE_LIST.push_back(current);
        } else if (current->attribute.find(SPED_BACK_KEYWORD) != string::npos) {
            SPED_LIST.push_back(current);
        } else if (current->attribute.find(NOT_GRADUATING_KEYWORD) == string::npos) {
            EVERYONE_ELSE_LIST.push_back(current);
        }
    }
}

void assign_seat(Seat* seat, Student* student) {
    seat->student = student;
    seat->occupied = true;
    student->seat_number = seat->seat_number;
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
    bool side = true;
    if (SIDE_DIFF >= 0) {
        side = true;
    } else {
        side = false;
    }
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
                if (side) {
                    SIDE_DIFF -= 2;
                } else {
                    SIDE_DIFF += 2;
                }
                return true;
            }
        }

        assign_seat(to_assign[row][column], curr_student);
        students.erase(students.begin());
        column += 1;

        if (side) {
            SIDE_DIFF -= 1;
        } else {
            SIDE_DIFF += 1;
        }
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
    seat_from_front(SENIOR_BOARD_LIST);
    seat_from_front(SPEAKER_LIST);
    seat_from_front(ACADEMIC_SCHOLARS_LIST);
    seat_from_front(EVERYONE_ELSE_LIST);
}

void import_students() {
    vector<vector<string> > content;
	vector<string> row;
    string line;
    string word;

    fstream file (STUDENT_IMPORT_NAME, ios::in);

    if(file.is_open()) {
        while (getline(file, line)) {
            row.clear();
            stringstream str(line);
            while(getline(str, word, ',')) {
                row.push_back(word);
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
            if (content[i][2].find(SIBLING_TWIN_KEYWORD) != string::npos) {
                student->sibling = true;
            }
            if (content[i][2].find(SPED_HELPER_KEYWORD) != string::npos) {
                student->sibling = true;
            }
            if (content[i][2].find(SPED_TA_KEYWORD) != string::npos) {
                student->helper = true;
            }
        }
        RAW_STUDENTS.push_back(student);
    }
}

vector<vector<Student*> > generate_speaker_lists(vector<vector<Seat*> >& seating, int rows, int speakers) {
    vector<vector<Student*> > lists;
    vector<Student*> current;

    // This rounds it up
    int chunk_size = (rows + speakers - 1) / speakers;

    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < ROW_LENGTH; j++) {
            if (seating[i][j]->occupied) {
                if (seating[i][j]->student->helper != true) {
                    current.push_back(seating[i][j]->student);
                }
            }
        }
        if ((i + 1) % chunk_size == 0) {
            lists.push_back(current);
            current.clear();
        }
    }
    if (current.size() > 0) {
        lists.push_back(current);
    }
    return lists;
}

void print_speaker_list(vector<vector<Student*> >& lists, string filename, bool seat_nums) {

    ofstream outfile (filename);

    for (int i = 0; i < lists.size(); i++) {
        outfile << "------ NEW SPEAKER ------" << endl;
        for (int j = 0; j < lists[i].size(); j++) {
            Student* student = lists[i][j];
            if (seat_nums) {
                outfile << student->seat_number << " - ";
            }
            outfile << student->first << " " << student->last << endl;
        }
    }

    outfile.close();
}

void print_seating_chart(string filename, bool seat_nums) {
    ofstream outfile (filename);

    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = ROW_LENGTH - 1; j >= 0; j--) {
            if (seat_nums) {
                outfile << LEFT_SIDE[i][j]->seat_number << " ";
            }
            if (LEFT_SIDE[i][j]->occupied) {
                outfile << LEFT_SIDE[i][j]->student->first << " " << LEFT_SIDE[i][j]->student->last << ",";
            } else {
                outfile << "Empty" << ",";
            }
        }

        outfile << "-,-,";

        for (int j = 0; j < ROW_LENGTH; j++) {
            if (seat_nums) {
                outfile << RIGHT_SIDE[i][j]->seat_number << " ";
            }
            if (RIGHT_SIDE[i][j]->occupied) {
                outfile << RIGHT_SIDE[i][j]->student->first << " " << RIGHT_SIDE[i][j]->student->last << ",";
            } else {
                outfile << "Empty" << ",";
            }
        }

        outfile << "\n";
    }

    outfile.close();
}


/**************************
*
* Debug Print Functions
*
**************************/

string five_printer(string print) {
    string res = print;
    if (print.length() < 5) {
        for (int i = print.length(); i < 5; i++) {
            res += " ";
        }
    }
    return res;
}

void debug_chart() {
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
                //cout << " " << five_printer(to_string(LEFT_SIDE[i][j]->seat_number)) << " ";
            } else {
                cout << "       ";
            }
        }
        cout << "     ";
        for (int j = 0; j < ROW_LENGTH; j++) {
            if (RIGHT_SIDE[i][j]->occupied) {
                cout << " " << five_printer(RIGHT_SIDE[i][j]->student->attribute.substr(0, 5)) << " ";
                //cout << " " << five_printer(to_string(RIGHT_SIDE[i][j]->seat_number)) << " ";
            } else {
                cout << "       ";
            }
        }
        cout << endl;
    }
}

void debug_student_list(vector<Student*>& list) {
    for (int i = 0; i < list.size(); i++) {
        cout << list[i]->last << ", " << list[i]->first << " - " << list[i]->attribute << endl;
    }
}

void debug_speaker_lists(vector<vector<Student*> >& lists) {
    for (int i = 0; i < lists.size(); i++) {
        cout << "------ NEW SPEAKER ------" << endl;
        debug_student_list(lists[i]);
    }
}


/**************************
*
* Program Operations
*
**************************/

int main(int argc, char *argv[]) {
    // Read students from CSV
    import_students();

    // Create the banner
    generate_banner_list();
    print_banner_file();

    // Set up seating
    generate_side(LEFT_SIDE, true);
    generate_side(RIGHT_SIDE, false);

    // Put students in seats
    sort_students();
    populate_seating_chart();

    // Printing debug stuff
    //debug_student_list(RAW_STUDENTS);
    //debug_chart();

    // Print speaker lists
    vector<vector<Student*> > left_lists = generate_speaker_lists(LEFT_SIDE, NUM_ROWS, FACULTY_SPEAKER_SIDE_NUM);
    vector<vector<Student*> > right_lists = generate_speaker_lists(RIGHT_SIDE, NUM_ROWS, FACULTY_SPEAKER_SIDE_NUM);
    print_speaker_list(left_lists, LEFT_SIDE_SPEAKER_FILE_NAME, SPEAKER_SEAT_NUM_FLAG);
    print_speaker_list(right_lists, RIGHT_SIDE_SPEAKER_FILE_NAME, SPEAKER_SEAT_NUM_FLAG);

    // Generate CSV of seating chart
    print_seating_chart(SEATING_CHART_FILE_NAME, CHART_SEAT_NUM_FLAG);
    return 0;
}