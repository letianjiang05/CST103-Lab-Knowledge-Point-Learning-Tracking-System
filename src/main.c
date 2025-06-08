#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define resource "../resource"
// file paths of resource folder
#define logFileLearning "../logs/learning_log.txt"
// file paths of learning logs
#define logFileCumulative "../cumulative_times/cumulative_times.txt"
#define logFileCumulativeToStudent "../cumulative_times/student_cumulative_time.txt"
#define logFileCumulativeToLearningMethod "../cumulative_times/learning_method_cumulative_time.txt"
#define logFileCumulativeToCourse "../cumulative_times/course_cumulative_time.txt"
#define logFileCumulativeToChapter "../cumulative_times/chapter_cumulative_time.txt"
#define logFileCumulativeToSection "../cumulative_times/section_cumulative_time.txt"
#define logFileCumulativeToUnit "../cumulative_times/unit_cumulative_time.txt"
#define logFileCumulativeToDat "../data/cumulative_time.dat"
// File paths of cumulative learning times
#define MAX_LINE_LENGTH 256
struct LearningData {
    char studentID[MAX_LINE_LENGTH];
    char course[MAX_LINE_LENGTH];
    char chapter[MAX_LINE_LENGTH];
    char section[MAX_LINE_LENGTH];
    char unit[MAX_LINE_LENGTH];
    char learningMethod[MAX_LINE_LENGTH];
    char startTime[MAX_LINE_LENGTH];
    char endTime[MAX_LINE_LENGTH];
    double elapsedTime;
};
struct LearningData tempData;
struct LearningData selectedData;

void displayMainMenu();
void displayAllUnit(const char *folderPath, int level);
void displayLog(struct LearningData selectedData);
// display functions
void readTxtFile(const char *folderPath, const char *unit);
void readLog(const char *logFilePath);
void readCumulativeData(const char *logFilePath);
// file reading functions
int compare(const void *a, const void *b);
void getStudentID(struct LearningData *selectedData);
void getLearningMethod(struct LearningData *selectedData);
struct LearningData startLearning(const char *folderPath, int level);
// learning functions
void trackLearningTime(time_t *start, time_t *end, double *elapsed);
// timing functions
void logInfo(
    const char *logFilePath,
    const char *studentID,
    const char *learningMethod,
    const char *startTime,
    const char *endTime,
    const char *course,
    const char *chapter,
    const char *section,
    const char *unit,
    double elapsedTime
);
void updateCumulativeData(const char *filename, const char *name, const double *time);
void writeCumulativeToDat(const char *filename, const char *name, const double *time);
// file writing functions

int main() {
    printf("Welcome to the Knowledge Point Learning Tracking System (KPLTS)!\n");
    int choice;
    do {
        displayMainMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                selectedData = startLearning(resource, 0);
                updateCumulativeData(logFileCumulativeToStudent, selectedData.studentID, &selectedData.elapsedTime);
                updateCumulativeData(logFileCumulativeToLearningMethod, selectedData.learningMethod, &selectedData.elapsedTime);
                updateCumulativeData(logFileCumulativeToCourse, selectedData.course, &selectedData.elapsedTime);
                updateCumulativeData(logFileCumulativeToChapter, selectedData.chapter, &selectedData.elapsedTime);
                updateCumulativeData(logFileCumulativeToSection, selectedData.section, &selectedData.elapsedTime);
                updateCumulativeData(logFileCumulativeToUnit, selectedData.unit, &selectedData.elapsedTime);
                updateCumulativeData(logFileCumulative, "Cumulative Time: ", &selectedData.elapsedTime);
                logInfo(logFileLearning, selectedData.studentID, selectedData.learningMethod, selectedData.startTime, selectedData.endTime, selectedData.course, selectedData.chapter, selectedData.section, selectedData.unit, selectedData.elapsedTime);
                writeCumulativeToDat(logFileCumulativeToDat, selectedData.studentID, &selectedData.elapsedTime);
                displayLog(selectedData);
                // Start Learning
                break;  
            case 2:
                displayAllUnit(resource, 0);
                // Display All Courses
                break;
            case 3:
                readLog(logFileLearning);
                // Read Log
                break;
            case 4:
                readCumulativeData(logFileCumulative);
                // Read Cumulative Learning Times
                break;
            case 5:
                printf("Exiting the program. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please enter a valid option.\n");
        }
    } while (choice != 5);
    return 0;
}

void displayMainMenu() {
    printf("--------------------------------\n");
    printf("## Main Menu ##\n");
    printf("1. Start Learning\n");
    printf("2. View All Units\n");
    printf("3. Read Log\n");
    printf("4. Read Cumulative Learning Times\n");
    printf("5. Exit\n");
}
void displayAllUnit(const char *folderPath, int level) {
    DIR *directory = opendir(folderPath);
    struct dirent *entry;
    struct dirent **sorted_entries;
    int num_entries, i;
    if (directory != NULL) {
        num_entries = scandir(folderPath, &sorted_entries, NULL, alphasort);
        if (num_entries >= 0) {
            qsort(sorted_entries, num_entries, sizeof(struct dirent *), compare);
            for (i = 0; i < num_entries; ++i) {
                entry = sorted_entries[i];
                if (entry->d_type == DT_DIR) {
                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                        printf("%*s%s: %s\n", level * 4, "", (level == 0) ? "Course" : (level == 1) ? "Chapter" : (level == 2) ? "Section" : "Section", entry->d_name);
                        char newPath[256];
                        snprintf(newPath, sizeof(newPath), "%s/%s", folderPath, entry->d_name);
                        displayAllUnit(newPath, level + 1);
                    }
                } else if (entry->d_type == DT_REG) {
                    printf("%*sUnit: %s\n", level * 4, "", entry->d_name);
                }
                free(sorted_entries[i]);
            }
            free(sorted_entries);
        } else {
            printf("Error scanning directory '%s'.\n", folderPath);
        }
        closedir(directory);
    } else {
        printf("Unable to open directory '%s'.\n", folderPath);
    }
}
void displayLog(struct LearningData selectedData) {
    printf("--------------------------------------\n"
           "Selected student ID: %s\n"
           "Selected learning method: %s\n"
           "Start time: %s"
           "End time: %s"
           "Elapsed time: %.2f\n"
           "--------------------------------------\n"
           "Selected course: %s\n"
           "Selected chapter: %s\n"
           "Selected section: %s\n"
           "Selected unit: %s\n",
           selectedData.studentID,
           selectedData.learningMethod,
           selectedData.startTime,
           selectedData.endTime,
           selectedData.elapsedTime,
           selectedData.course,
           selectedData.chapter,
           selectedData.section,
           selectedData.unit);
}

void readTxtFile(const char *folderPath, const char *unit) {
    // Construct the file path using the folderPath and unit
    char filePath[256];
    snprintf(filePath, sizeof(filePath), "%s/%s", folderPath, unit);

    FILE *file = fopen(filePath, "r");
    if (file != NULL) {
        char buffer[256];
        printf("--------------------------------\n");
        printf("Reading file '%s':\n", filePath);
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            printf("%s", buffer); // Printing the content line by line, replace this with your logic
        }
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        fclose(file);
    } else {
        printf("Unable to open file '%s'.\n", filePath);
    }
}
void readLog(const char *logFilePath) {
    FILE *logFile = fopen(logFilePath, "r");
    if (logFile != NULL) {
        char buffer[MAX_LINE_LENGTH];
        printf("--------------------------------\n");
        printf("Reading log file '%s':\n", logFilePath);
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        while (fgets(buffer, sizeof(buffer), logFile) != NULL) {
            printf("%s", buffer); // Display the content line by line
        }
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        fclose(logFile);
    } else {
        printf("Error opening log file '%s' for reading.\n", logFilePath);
    }
}
void readCumulativeData(const char *logFilePath) {
    int choice;
    printf("--------------------------------------\n");
    printf("Select a cumulative learning time to view:\n"
    "1. Student\n"
    "2. Learning Method\n"
    "3. Course\n"
    "4. Chapter\n"
    "5. Section\n"
    "6. Unit\n");
    printf("--------------------------------------\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    switch (choice) {
        case 1:
            readLog(logFileCumulativeToStudent);
            break;
        case 2:
            readLog(logFileCumulativeToLearningMethod);
            break;
        case 3:
            readLog(logFileCumulativeToCourse);
            break;
        case 4:
            readLog(logFileCumulativeToChapter);
            break;
        case 5:
            readLog(logFileCumulativeToSection);
            break;
        case 6:
            readLog(logFileCumulativeToUnit);
            break;
        default:
            printf("Invalid choice. Please enter a valid option.\n");
    }
}

int compare(const void *a, const void *b) {
    const struct dirent **entryA = (const struct dirent **)a;
    const struct dirent **entryB = (const struct dirent **)b;
    return strcasecmp((*entryA)->d_name, (*entryB)->d_name);
}
void getStudentID(struct LearningData *selectedData) {
    printf("--------------------------------------\n");
    printf("Enter your student ID: ");
    scanf("%s", selectedData->studentID);
}
void getLearningMethod(struct LearningData *selectedData) {
    int choice;
    printf("--------------------------------------\n");
    printf("Select a learning method:\n 1. L\n 2. T\n 3. P\n 4. O\n 5. Guided Learning\n 6. Independent Learning\n");
    printf("--------------------------------------\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    switch (choice) {
        case 1:
            strcpy(selectedData->learningMethod, "L");
            break;
        case 2:
            strcpy(selectedData->learningMethod, "T");
            break;
        case 3:
            strcpy(selectedData->learningMethod, "P");
            break;
        case 4:
            strcpy(selectedData->learningMethod, "O");
            break;
        case 5:
            strcpy(selectedData->learningMethod, "Guided Learning");
            break;
        case 6:
            strcpy(selectedData->learningMethod, "Independent Learning");
            break;
        default:
            printf("Invalid choice. Please enter a valid option.\n");
    }
        
}
struct LearningData startLearning(const char *folderPath, int level) {
    DIR *directory = opendir(folderPath);
    struct dirent *entry;
    struct dirent **sorted_entries;
    int num_entries, i;
    time_t startTime, endTime;
    double elapsedTime;
// Update cumulative learning times

    if (directory != NULL) {
        num_entries = scandir(folderPath, &sorted_entries, NULL, alphasort);
        if (num_entries >= 0) {
            qsort(sorted_entries, num_entries, sizeof(struct dirent *), compare);
            int selection = -1;
            while (selection < 0 || selection >= num_entries) {
                printf("--------------------------------------\n");
                printf("Select a %s:\n", (level == 0) ? "course" : (level == 1) ? "chapter" : (level == 2) ? "section" : "unit");
                for (i = 0; i < num_entries; ++i) {
                    entry = sorted_entries[i];
                    printf("%d. %s\n", i, entry->d_name);
                }
                printf("--------------------------------------\n");
                printf("Enter your choice:");
                scanf("%d", &selection);
                if (selection == 0) {
                    printf("Exiting the program. Goodbye!\n");
                    exit(0);
                } else
                if (selection < 0 || selection >= num_entries) {
                    printf("Invalid selection. Please choose a valid option.\n");
                }
            }

            entry = sorted_entries[selection];
            char newPath[256];
            snprintf(newPath, sizeof(newPath), "%s/%s", folderPath, entry->d_name);

            if (level == 0) {
                strcpy(tempData.course, entry->d_name);
                printf("--------------------------------------\n");
                printf("Selected course: %s\n", selectedData.course);
            } else if (level == 1) {
                strcpy(tempData.chapter, entry->d_name);
                printf("--------------------------------------\n");
                printf("Selected chapter: %s\n", selectedData.chapter);
            } else if (level == 2) {
                strcpy(tempData.section, entry->d_name);
                printf("--------------------------------------\n");
                printf("Selected section: %s\n", selectedData.section);
            } else {
                strcpy(selectedData.unit, entry->d_name);
                getStudentID(&selectedData);
                getLearningMethod(&selectedData);
                readTxtFile(folderPath, entry->d_name);
                printf("Start learning timer for unit: %s\n", entry->d_name);
                trackLearningTime(&startTime, &endTime, &elapsedTime);
                // Update LearningData with timing information
                strcpy(selectedData.startTime, ctime(&startTime));
                strcpy(selectedData.endTime, ctime(&endTime));
                selectedData.elapsedTime = elapsedTime;
            }
            if (level < 3) {
                struct LearningData deeperData = startLearning(newPath, level + 1);
                // Update the selectedData with deeper level data
                if (level == 0) {
                    strcpy(tempData.chapter, deeperData.chapter);
                    strcpy(tempData.section, deeperData.section);
                    strcpy(tempData.unit, deeperData.unit);
                }
            }
        } else {
            printf("Error scanning directory '%s'.\n", folderPath);
        }
        closedir(directory);
    } else {
        printf("Unable to open directory '%s'.\n", folderPath);
    }
    strcpy(selectedData.course, tempData.course);
    strcpy(selectedData.chapter, tempData.chapter);
    strcpy(selectedData.section, tempData.section);
    return selectedData;
}

void trackLearningTime(time_t *start, time_t *end, double *elapsed) {
    int timingActive = 1; // Flag to check if timing is active, initially set to start timing
    *start = time(NULL); // Start timing immediately upon function call
    time_t startTime, endTime;
    double elapsedTime;
    startTime = time(NULL);
    while (timingActive) {
        printf("Press '0' to stop timing: ");
        int input;
        scanf("%d", &input);

        if (input == 0) {
            *end = time(NULL); // End timing
            *elapsed = difftime(*end, *start); // Calculate elapsed time
            timingActive = 0; // Stop the loop and timing
        } else {
            printf("Invalid input. Please input '0' to stop timing.\n");
        }
    }
}
void logInfo(
    const char *logFilePath,
    const char *studentID,
    const char *learningMethod,
    const char *startTime,
    const char *endTime,
    const char *course,
    const char *chapter,
    const char *section,
    const char *unit,
    double elapsedTime
) {
    FILE *logFile = fopen(logFilePath, "a"); // Open file in append mode

    if (logFile != NULL) {
        fprintf(logFile, "Student ID: %s\n", studentID);
        fprintf(logFile, "Learning Method: %s\n", learningMethod);
        fprintf(logFile, "Start Time: %s", startTime);
        fprintf(logFile, "End Time: %s", endTime);
        fprintf(logFile, "Elapsed Time: %.2f\n", elapsedTime);
        fprintf(logFile, "\n");
        fprintf(logFile, "Course: %s\n", course);
        fprintf(logFile, "Chapter: %s\n", chapter);
        fprintf(logFile, "Section: %s\n", section);
        fprintf(logFile, "Unit: %s\n", unit);
        fprintf(logFile, "--------------------------------------\n");

        fclose(logFile); // Close the file after writing
    } else {
        printf("Error opening log file '%s' for writing.\n", logFilePath);
    }
}
void updateCumulativeData(const char *filename, const char *name, const double *time) {
    FILE *file = fopen(filename, "r");
    FILE *tempFile = fopen("temp.txt", "w");

    if (file == NULL || tempFile == NULL) {
        printf("Error opening file.\n");
        return;
    }

    char currentName[MAX_LINE_LENGTH];
    double currentTime;
    int found = 0;

    while (fscanf(file, "%[^\n]\n", currentName) == 1) {
        fscanf(file, "%lf\n", &currentTime);

        if (strcmp(currentName, name) == 0 && strcmp(currentName, "") != 0) {
            // Update cumulative time for existing entry
            currentTime += *time;
            fprintf(tempFile, "%s\n%lf\n", currentName, currentTime);
            found = 1;
        } else {
            fprintf(tempFile, "%s\n%lf\n", currentName, currentTime);
        }
    }

    // If not found, add a new entry
    if (!found) {
        fprintf(tempFile, "%s\n%lf\n", name, *time);
    }

    fclose(file);
    fclose(tempFile);

    // Replace the original file with the updated content
    remove(filename);
    rename("temp.txt", filename);
}
void writeCumulativeToDat(const char *filename, const char *name, const double *time) {
    FILE *file = fopen(filename, "ab"); // Open file in append binary mode

    if (file != NULL) {
        fwrite(name, sizeof(char), strlen(name), file); // Write name
        fwrite("\n", sizeof(char), 1, file); // Write newline
        fwrite(time, sizeof(double), 1, file); // Write time
        fwrite("\n", sizeof(char), 1, file); // Write newline

        fclose(file); // Close the file after writing
    } else {
        printf("Error opening .dat file '%s' for writing.\n", filename);
    }
}