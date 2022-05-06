# Overview
This program creates the seating chart for Palatine High School's graduation ceremony. It generates the following:

- Text file for the Arena Ribbon Banner containing the alphabetized list of all students graduating
- Graduate name lists for faculty to read off during the ceremony
- Seating chart generated based on ceremony rules for Palatine High School

### Program Input
The program requires an alphabetized CSV file of graduates to work. It should only consist of three columns (listed from left to right):

1. Last Name
2. First Name
3. Attributes

Note that this file should not have any labels. The first row of the file should contain the first student. The attributes section must adhere to certain keywords. See those attributes in the [Attributes]() section. 

### Program Output
This program will generate 4 files: 
- **arena_ribbon_banner.txt**: File for the Arena Ribbon Banner
- **left_side_speaker_list.txt**: List of names for faculty on the left side of the stage
- **right_side_speaker_list.txt**: List of names for faculty on the right side of the stage
- **seating_chart.csv**: Seating chart that follows the speaker list

# Preparing your Graduate List
There are certain things you must do to prepare your graduate list for the program. The first is to open up your excel file and remove any extra rows/columns. The only columns we should have in our excel file are the following: 

1. Last Name
2. First Name
3. Attributes

Ensure that you have removed the top row of data. This means that you should not have any labels such as "First Name", "Last Name", etc. The first row should be your first graduate.

### Attributes
Once you remove all extra data, you will need to make sure your Attributes match that of the program. Each student can only have one attribute. Here are the supported attributes:

Academic Attributes:
- **Senior Board**
- **Speaker**
- **Academic Scholar**

SPED Attributes
- **SPED Regular Seating**
- **SPED Back Row**
- **SPED With Helper**
- **TA Helper**

Misc Attributes:
- **Twins/Siblings**
- **Not Graduating**

When using the **Twins/Siblings** attribute, students must be listed successively in the list. **Twins/Siblings** supports only up to 2 siblings currently (i.e. Triplets are not supported).

The **SPED With Helper** attribute must be followed by an entry with the attribute **TA Helper** so that they can be seated next to each other. 

### Exporting your Excel file to CSV
The next step is to convert your Excel file to CSV. You simply just need to hit "Save As" and change the file format to be .CSV instead of .XLSX.

Once you have done that, you are ready to run the program.

# Program Configuration
There are some options that you may want to consider before you generate the files. 