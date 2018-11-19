#define INVALID -1
#define HELP 0
#define QUIT 1
#define TYPE 2
#define PRINTER1 3
#define CONVERSION 4
#define PRINTERSS 5
#define JOBS 6
#define PRINT 7
#define CANCEL 8
#define PAUSE 9
#define RESUME 10
#define DISABLE 11
#define ENABLE 12
#define MAXFILENUM 64
#define MAXJOBNUM 64
void print_help();
char* help="help\nquit\ntype file_type\nprinter printer_name file_type\nconversion file_type1 file_type2 conversion_program [arg1 arg2...]\nprinters\njobs\nprint file_name [printer1 printer2 ...\ncancel job_number\npause job_number\nresume job_number\ndisable printer_name\nenable printer_name\n";
int convertable(int start, int target);
int bfs(int start, int target, int find);
int convertToInt(char* first);
int getFileID(char* printer);
char* getFileName(char* printer);
char* getFIleType(char* printer);
int upPrinter(int id, int enabled,int busy);
int creatPrinter(char* token);
int getTypeID(char* type);
int makeMap(int start, int target);
int printP();
int printJ();
int testPrinter(char* name);
int creatJobs(int numOfJobs,char* name);
int testPrinter(char* name);
int creatType(char* token);
int creatConversion(char* token);
int creatPrint(char* token);
int upJobs(int numOfJobs,JOB_STATUS status);
int enablePrinter(char* token);
int my_imp_format_error_message(char* mes);
int my_imp_format_job_status(int jobID);
int my_imp_format_printer_status(int printerID);
char* file_type1; 
char* file_type2;
char* conversion_program;
int initVisited();
void pipeline(char ***cmd,JOB *job);
void getPath(JOB *job);
void printMap();
void printFiles();
int testPipe();
int assignnJob(int printerID);
 typedef struct conversion{
     char* conversion_program;
     char* args[32];
     int numOfArgs;
 }CONVERSIONS;