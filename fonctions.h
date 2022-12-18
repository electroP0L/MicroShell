#define MAX 99

void getcommand_arguments(char** command, char* argument_list[MAX], char* globalcommand, int* arglen);
void launchprocess_pipe(char* command, char* argument_list[MAX], int arglen, int ispipe, int arrplan);
void launchprocess(char* command, char* argument_list[MAX], int arrplan, int launchout);
void handler(int sig);
void cd(char* command, char* argument_list[MAX]);
