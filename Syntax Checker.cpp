#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// ============================================================================
// STRUCTURE DEFINITIONS
// ============================================================================

// Struct to store details of a processed line from the source code file
typedef struct{
	int lineno;         // The actual line number in the source file
	int linelen;        // Total character length of the line text
	char linetext[100]; // String content of the line itself
}fileline;

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================

// Core file processing and analysis routines
void print(fileline str[], int len, char input_filename[]);
int search(char arr[], int len);
void bracket(fileline arr[], int len, FILE *fout);
void keyword(fileline arr[], int length, FILE *fout);
void function_and_prototype_count(fileline arr[], int len, FILE *fout);
void checkkeyword(fileline arr[], int len);
void builtinfunction(fileline arr[], int len, FILE *fout);
void printscan(fileline arr[], int size);
void varcount(fileline arr[], int len, FILE *fout);
void fcheck(fileline arr[], int len, FILE *fout);

// Specific statement validation syntax checkers
int printcheck(char arr[], int size);
int scancheck(char arr[], int size);
int getscheck(char arr[], int size);
int putscheck(char arr[], int size);
int fprintfcheck(char arr[], int size); 
int fscanfcheck(char arr[], int size);  
int forcheck(char arr[], int size);
int whilecheck(char arr[], int size);

// MAIN FUNCTION
int main(){
	FILE *input;        // Pointer to the target source file to read
	FILE *fout;         // Pointer to 'Results.txt' where analysis is written
	fileline str[100];  // Array of structs to hold lines filtered from comments
	char str1[100];     // Temporary buffer to read raw lines from fgets
	char filename[100]; // Buffer to capture user input for the filename
	int totallen, j, i = 0, len, found;
	
//Get the custom filename securely from the terminal
	printf("Enter the filename to check (e.g., input.txt): ");
	if (scanf("%99s", filename) != 1) {
		printf("Error reading filename.\n");
		return 1;
	}
	
//Open the requested file in read-only mode
	input = fopen(filename, "r");
	if (input == NULL) {
		printf("Error: Could not open file '%s'. Make sure it exists in this folder.\n", filename);
		return 1;
	}
	
//Open Results.txt to log the final analysis reports
	fout = fopen("Results.txt", "w");
	if (fout == NULL) {
		printf("Error: Could not create Results.txt\n");
		fclose(input);
		return 1;
	}
//Read the target file line-by-line and clean out single-line comments (//)
	while(fgets(str1, 100, input) != NULL){
		len = strlen(str1);             // Extract line length
		found = search(str1, len);      // Scan for any comment entry point

// Case A: Line has content and contains no double slashes
		if (str1[0] != '\n' && found == -1){
			str[i].lineno = i + 1;
		    str[i].linelen = len;
		    strcpy(str[i].linetext, str1);
		    i++;
		}
// Case B: Line contains a inline comment; truncate anything after '//'
		else if (str1[0] != '\n' && found != -1){
			str[i].lineno = i + 1;
			for(j = 0; j < found; j++) str[i].linetext[j] = str1[j];
			str[i].linetext[found] = '\n';    // Append trailing newline character
			str[i].linetext[found+1] = '\0';  // Safeguard string termination
			str[i].linelen = strlen(str[i].linetext);
			i++;
		}
	}
	fclose(input);     // Finished tracking input data; close target stream
	totallen = i;      // Save total valid, uncommented lines processed
	
//Sequence analysis routines and feed text directly to Results.txt
	fprintf(fout, "******************THE C- PROGRAM***********************\n\n\n\n");
	print(str, totallen, filename); // Generates clean file log copy
	
	fprintf(fout, "-----------BRACKET ,PARANTHESIS Check-----------\n\n\n\n");
	bracket(str, totallen, fout);
	
	fprintf(fout, "\n\n\n\t\t\tKEYWORDS with LINE NUMBER\n\n\n");
	keyword(str, totallen, fout);
	
	fprintf(fout, "\n\n\n\t\t\tTOTAL NUMBER OF BUILTIN FUNCTIONS USED\n\n\n");
	builtinfunction(str, totallen, fout);
	
	fprintf(fout, "\n\n\n\t\t\tTOTAL FUNCTIONS USED INCLUDING MAIN\n\n\n");
	function_and_prototype_count(str, totallen, fout);
	
	fprintf(fout, "\n\n\n\t\t\tVARIABLES WITH COUNTS\n\n\n");
	varcount(str, totallen, fout);
	
	fprintf(fout, "\n\n\n\t\t\tSYNTAX CHECK\n\n");
	fcheck(str, totallen, fout);
	
	fprintf(fout, "\n\n\n\t\t\tTOTAL LINES IN PROGRAM\n\n\n");
	fprintf(fout, "Total line in Program are: %d\n", totallen);
	
	fclose(fout);      // Finished compilation report; safely close out results stream
	
// Clean exit presentation status message for user
	printf("\nProcessing completed successfully!\nAll analysis results have been saved to 'Results.txt'.\n");
	
	return 0;
}

// ============================================================================
// CORE PROCESSING ROUINES DEFINITIONS
// ============================================================================
void print(fileline str[], int len, char input_filename[]){
	FILE * flineout; 
	int i;
	char output_filename[120];
	
// Create uniquely prefixed file tracking format: checked_[filename].txt
	snprintf(output_filename, sizeof(output_filename), "checked_%s", input_filename);
	
	flineout = fopen(output_filename, "w");
	if (flineout != NULL) {
		for(i = 0; i < len; i++){
			fprintf(flineout, "line %d [%d]: %s", str[i].lineno, str[i].linelen, str[i].linetext);
		}
		fclose(flineout);
	}
}

// Helper utility searching line for the start sequence index of comment line '//'
int search(char arr[], int len){
	int found = 0, i;
	for(i = 0; !found && i < len - 1; i++){ 
		if(arr[i] == '/' && arr[i+1] == '/'){
			return i; // Return exact position matching start of comment symbol
		}
	}
	return -1; // Flag matching clean code with no comments present
}

// Scans characters and loops token counts for balancing combinations of {}, (), and []
void bracket(fileline arr[], int len, FILE *fout){
	int *p;
	int i, j;
	p = (int*)calloc(3, sizeof(int)); // Dynamic tracker array for grouping balances
	
// Parallel trackers storing occurrence indices tracking imbalance root locations
	int linea1[20], linea2[20], lineb1[20], lineb2[20], linec1[20], linec2[20];
	int a = 0, a2 = 0, b = 0, b2 = 0, c = 0, c2 = 0;
	
	for(i = 0; i < len; i++){
		for(j = 0; j < arr[i].linelen; j++){
			if (arr[i].linetext[j] == '{')        {*p = *p + 1;       linea1[a++] = arr[i].lineno;}
			else if (arr[i].linetext[j] == '}')   {*p = *p - 1;       linea2[a2++] = arr[i].lineno;}
			else if (arr[i].linetext[j] == '(')   {*(p+1) = *(p+1)+1; lineb1[b++] = arr[i].lineno;}
			else if (arr[i].linetext[j] == ')')   {*(p+1) = *(p+1)-1; lineb2[b2++] = arr[i].lineno;}
			else if (arr[i].linetext[j] == '[')   {*(p+2) = *(p+2)+1; linec1[c++] = arr[i].lineno;}
			else if (arr[i].linetext[j] == ']')   {*(p+2) = *(p+2)-1; linec2[c2++] = arr[i].lineno;}
		}
	}
	
// Exhaust tracking balance flags and spill unclosed/unopened token lines directly into reports
	while(*p > 0)    { fprintf(fout, "line %d: { is greater\n", linea1[--a]); *p -= 1; }
	while(*p < 0)    { fprintf(fout, "line %d: } is greater\n", linea2[--a2]); *p += 1; }
	while(*(p+1) > 0){ fprintf(fout, "line %d: ( is greater\n", lineb1[--b]); *(p+1) -= 1; } 
	while(*(p+1) < 0){ fprintf(fout, "line %d: ) is greater\n", lineb2[--b2]); *(p+1) += 1; }
	while(*(p+2) > 0){ fprintf(fout, "line %d: [ is greater\n", linec1[--c]); *(p+2) -= 1; }
	while(*(p+2) < 0){ fprintf(fout, "line %d: ] is greater\n", linec2[--c2]); *(p+2) += 1; }
	free(p); // Recover block allocation memory footprint safely
}

// Tokenizes lines to discover reserved keywords matching standard dictionary listings
void keyword(fileline arr[], int len, FILE *fout){
	char str[100], a[100];
	char sync[32][10] = {"auto","double","int","struct","break","else","long","switch","case","enum","register","typedef","char","extern","return","union","const","float","short","unsigned","continue","for","signed","void","default","goto","sizeof","volatile","do","if","static","while"};		
	int i, j, l;
	
	for (i = 0; i < len; i++){
		int k = 0;
		strcpy(str, arr[i].linetext);
		for(j = 0; j < arr[i].linelen; j++){
			// Check word boundaries split by delimiter spaces/characters
			if (str[j] == ' ' || str[j] == '(' || str[j] == '\0' || str[j] == '\t' || str[j] == '\n' || str[j] == ';'){
				a[k] = '\0';
				if(k > 0) { 
					for(l = 0; l < 32; l++){
						if (strcmp(a, sync[l]) == 0) fprintf(fout, "Line %d: %s\n", arr[i].lineno, a);
					}
				}
				k = 0; // Reset boundary word builder buffer pointer index
			}
			else { a[k] = str[j]; k++; }
		}
	}
}	

// Identifies structural matches calling internal prebuilt C-runtime keywords
void builtinfunction(fileline arr[], int len, FILE *fout){ 
	char str[100], a[100];
	char sync[30][12] = {"printf","scanf","gets","puts","fscanf","fprintf","fgets","fputs","fputc","fgetc","fopen","fclose","feof","fflush","malloc","calloc","rand","strcmp","strlen","strcpy","strncpy","strncmp","tolower","toupper","strrev","getch","strcat","strncat","sqrt","pow"};		
	int i, j, l, total = 0;
	
	for (i = 0; i < len; i++){
		int k = 0;
		strcpy(str, arr[i].linetext);
		for(j = 0; j < arr[i].linelen; j++){
			if (str[j] == ' ' || str[j] == '(' || str[j] == '\0' || str[j] == '\t' || str[j] == ';'){
				a[k] = '\0';
				if(k > 0) {
					for(l = 0; l < 30; l++){
						if (strcmp(a, sync[l]) == 0) total += 1;
					}
				}
				k = 0;
			}
			else { a[k] = str[j]; k++; }
		}
	}
	fprintf(fout, "Total Builtin Functions: %d\n", total);
}

// Counts functions structural definitions and determines if declarations have matching prototypes
void function_and_prototype_count(fileline arr[], int len, FILE *fout){
	char str[100], a[100];
	char sync[7][10] = {"int","char","void","double","float","long","short"};		
	int i, j, l, total = 0;
	
	// Scan pass 1: Counts runtime function structures instantiated with operational arguments
	for (i = 0; i < len; i++){
		int k = 0;
		strcpy(str, arr[i].linetext);
		int strlength = arr[i].linelen;
		for(j = 0; j < arr[i].linelen; j++){
			if (str[j] == ' ' || str[j] == '(' || str[j] == '\0' || str[j] == '\t'){
				a[k] = '\0';
				if (k > 0) {
					for(l = 0; l < 7; l++){
						if ((strcmp(a, sync[l]) == 0) && strlength >= 2 && str[strlength-2] == ')') total++;
					}
				}
				k = 0;
			}
			else { a[k] = str[j]; k++; }
		}
	}
	fprintf(fout, "Total Functions used including main function: %d\n", total);
	
	// Scan pass 2: Pinpoint entry sequence location of the main execution block
	int fun = 0, found = 0, pos = 0;
	for (i = 0; i < len && !found; i++){
		int k = 0;
		strcpy(str, arr[i].linetext);
		for(j = 0; j < arr[i].linelen; j++){
			if (str[j] == ' ' || str[j] == '(' || str[j] == '\0' || str[j] == '\t'){
				a[k] = '\0';
				if (strcmp(a, "main") == 0) {
					found = 1;
					pos = i;
					break;
				}	
				k = 0;
			}
			else { a[k] = str[j]; k++; }
		}
	}

	// Scan pass 3: Tallies all standard declarations terminated with a semicolon placed before main()
	for(i = 0; i < pos; i++){
		int k = 0;
		strcpy(str, arr[i].linetext);
		int strlength = arr[i].linelen;
	
		for(j = 0; j < arr[i].linelen; j++){
			if (str[j] == ' ' || str[j] == '(' || str[j] == '\0' || str[j] == '\t'){
				a[k] = '\0';
				if(k > 0) {
					for(l = 0; l < 7; l++){
						if ((strcmp(a, sync[l]) == 0) && strlength >= 3 && str[strlength-3] == ')' && str[strlength-2] == ';') fun++;
					}
				}
				k = 0;
			}
			else { a[k] = str[j]; k++; }
		}
	}
	fprintf(fout, "\n\n\n\t\t\tPROTOTYPE CHECK\n\n\n");
	fprintf(fout, "Total prototypes: %d\n", fun);
	
	if(fun < total - 1) fprintf(fout, "Some prototypes are not defined\n");
	else fprintf(fout, "Prototypes are defined\n");
}

// Tallies fundamental localized variables and calculates approximate hardware byte spaces consumed
void varcount(fileline arr[], int len, FILE *fout){
	char str[100], a[100];
	char sync[6][10] = {"int","char","long","double","float","short"};		
	int i, j, total[6] = {0}, memory[6] = {0};
	
	for(i = 0; i < len; i++){
		int k = 0;
		strcpy(str, arr[i].linetext);
		int strlength = arr[i].linelen;
		
		for(j = 0; j < arr[i].linelen; j++){
			if (str[j] == ' ' || str[j] == '(' || str[j] == '\0' || str[j] == '\t'){
				a[k] = '\0';
				if (k > 0) {
					for(int l = 0; l < 6; l++){
						if (strcmp(a, sync[l]) == 0){
							int pos = j;
							// Guard logic ignoring signatures structural function definitions lines
							if((strlength >= 3 && str[strlength-3] != ')') && (strlength >= 2 && str[strlength-2] != ')')){
								for(int x = pos; x < strlength; x++){
									if(str[x] == ',' || str[x] == ';') total[l]++;
								}	
							}
						}
					}		
				}
				k = 0;
			}
			else { a[k] = str[j]; k++; }
		}
	}

	// Output aggregated declaration figures and execute cross-platform math multiplication metrics
	fprintf(fout, "Total int: %d\nTotal Char: %d\nTotal long integer: %d\nTotal Double: %d\nTotal float: %d\nTotal short integer: %d\n", total[0], total[1], total[2], total[3], total[4], total[5]);
	memory[0] = total[0] * sizeof(int);
	memory[1] = total[1] * sizeof(char);
	memory[2] = total[2] * sizeof(long);
	memory[3] = total[3] * sizeof(double);
	memory[4] = total[4] * sizeof(float);
	memory[5] = total[5] * sizeof(short);
	fprintf(fout, "\n\n\n\t\t\tMEMORY CONSUMED BY VARIABLES\n\n\n");
	fprintf(fout, "Memory int: %d BYTES\nMemory Char: %d BYTES\nMemory long integer: %d BYTES\nMemory Double: %d BYTES\nMemory float: %d BYTES\nMemory short integer: %d BYTES\n", memory[0], memory[1], memory[2], memory[3], memory[4], memory[5]);
}

// ============================================================================
// STRUCTURAL SYNTAX MATCH CHECKERS
// ============================================================================
int printcheck(char arr[], int size){
	int pcount = 0, ccount = 0, icount = 0, f = 0;
	for(int i = 0; i < size; i++){
		if(arr[i] == '%') pcount++;
		if(arr[i] == ',') ccount++;
		if(arr[i] == '\"') icount++;
	}
	if(pcount != ccount) f = 1;
	if(icount % 2 != 0) f = 1;
	
	return f; // Returns 1 if faulty structure uncovered
}

// Evaluates scanf syntax: verifies required referencing addresses (&) accompany parameters
int scancheck(char arr[], int size){
	int pcount = 0, ccount = 0, icount = 0, f = 0, acount = 0, scount = 0;
	for(int i = 0; i < size - 1; i++){
		if(arr[i] == '%') pcount++;
		if(arr[i] == ',') ccount++;
		if(arr[i] == '\"') icount++;
		if(arr[i] == '&') acount++;
		if(arr[i] == '%' && arr[i+1] == 's') scount++; // Exceptions skip string array buffers
	}
	if(size > 0 && arr[size-1] == '"') icount++;
	if(size > 0 && arr[size-1] == '&') acount++;
	if(size > 0 && arr[size-1] == '%') pcount++;
	
	if(pcount != ccount) f = 1;
	if(icount % 2 != 0) f = 1;
	if((pcount - scount) != acount) f = 1;
	
	return f;
}

// Evaluates fprintf syntax: aligns formatting percent bounds allowing extra file pointer reference offsets
int fprintfcheck(char arr[], int size){
	int pcount = 0, ccount = 0, icount = 0, f = 0;
	for(int i = 0; i < size; i++){
		if(arr[i] == '%') pcount++;
		if(arr[i] == ',') ccount++;
		if(arr[i] == '\"') icount++;
	}
	if(pcount != ccount - 1) f = 1;
	if(icount % 2 != 0) f = 1;
	
	return f;
}

// Evaluates fscanf syntax: monitors parameter arguments mapping values securely inside file flows
int fscanfcheck(char arr[], int size){
	int pcount = 0, ccount = 0, icount = 0, f = 0, acount = 0, scount = 0;
	for(int i = 0; i < size - 1; i++){
		if(arr[i] == '%') pcount++;
		if(arr[i] == ',') ccount++;
		if(arr[i] == '\"') icount++;
		if(arr[i] == '&') acount++;
		if(arr[i] == '%' && arr[i+1] == 's') scount++; 
	}
	if(size > 0 && arr[size-1] == '"') icount++;
	if(size > 0 && arr[size-1] == '&') acount++;
	if(size > 0 && arr[size-1] == '%') pcount++;
	
	if(pcount != ccount - 1) f = 1;
	if(icount % 2 != 0) f = 1;
	if((pcount - scount) != acount) f = 1;
	
	return f;
}

// Guards gets structures from forbidden formatting markers or inline literals
int getscheck(char arr[], int size){
	int f = 0;
	for(int i = 0; i < size; i++){
		if(arr[i] == '%') f = 1;
		if(arr[i] == ',') f = 1;
		if(arr[i] == '\"') f = 1;
		if(arr[i] == '&') f = 1;
		if(arr[i] == '\'') f = 1;
	}
	return f;
}

// Guards puts structures against processing format variables or invalid quotes
int putscheck(char arr[], int size){
	int f = 0;
	for(int i = 0; i < size; i++){
		if(arr[i] == '%') f = 1;
		if(arr[i] == ',') f = 1;
		if(arr[i] == '\"') f = 1;
		if(arr[i] == '&') f = 1;
		if(arr[i] == '\'') f = 1;
	}
	return f;
}

// Evaluates loop syntax for loops: structural checks validating that exactly 2 conditional semicolons exist inside ()
int forcheck(char arr[], int size){
	int pcount = 0, qcount = 0, f = 0;
	for(int i = 0; i < size; i++){
		if(arr[i] == ';') pcount++;
		if(arr[i] == '\'') qcount++;
	}
	if(pcount != 2) f = 1;
	if(qcount % 2 != 0) f = 1;
	
	return f;
}

// Evaluates loops for while loops: catches trailing structural loop block terminates early
int whilecheck(char arr[], int size){
	int qcount = 0, f = 0, icount = 0;
	for(int i = 0; i < size; i++){
		if(arr[i] == ';') f = 1; // Flag structural bugs if simple standalone while statements end with immediate semicolons
		if(arr[i] == '\'') qcount++;
		if(arr[i] == '\"') icount++;
	}
	if(qcount % 2 != 0) f = 1;
	if(icount % 2 != 0) f = 1;
	
	return f;
}

// Master context syntax parsing loop distributing target strings toward specialized condition checkers
void fcheck(fileline arr[], int len, FILE *fout){
	char str[100], a[100];
	char sync[8][10] = {"printf","scanf","gets","puts","fscanf","fprintf","for","while"};
	int i, j, f;
	
	for (i = 0; i < len; i++){
		int k = 0;
		strcpy(str, arr[i].linetext);
		for(j = 0; j < arr[i].linelen; j++){
			if (str[j] == ' ' || str[j] == '(' || str[j] == '\0' || str[j] == '\t'){
				a[k] = '\0';
				if(k > 0) {
					// Compare string fragments to direct structural rules
					if (strcmp(a, sync[0]) == 0){
						f = printcheck(str, arr[i].linelen);
						if(f) fprintf(fout, "Error in Statement; ---> line no: %d\n", arr[i].lineno);
					} 
					if (strcmp(a, sync[1]) == 0){
						f = scancheck(str, arr[i].linelen);
						if(f) fprintf(fout, "Error in Statement : ---> line no: %d\n", arr[i].lineno);
					} 
					if (strcmp(a, sync[2]) == 0){
						f = getscheck(str, arr[i].linelen);
						if(f) fprintf(fout, "Error in Statement : ---> line no: %d\n", arr[i].lineno);
					} 
					if (strcmp(a, sync[3]) == 0){
						f = putscheck(str, arr[i].linelen);
						if(f) fprintf(fout, "Error in Statement : ---> line no: %d\n", arr[i].lineno);
					} 
					if (strcmp(a, sync[4]) == 0){
						f = fscanfcheck(str, arr[i].linelen);
						if(f) fprintf(fout, "Error in Statement : ---> line no: %d\n", arr[i].lineno);
					} 
					if (strcmp(a, sync[5]) == 0){
						f = fprintfcheck(str, arr[i].linelen);
						if(f) fprintf(fout, "Error in Statement : ---> line no: %d\n", arr[i].lineno);
					} 
					if (strcmp(a, sync[6]) == 0){
						f = forcheck(str, arr[i].linelen);
						if(f) fprintf(fout, "Error in Statement : ---> line no: %d\n", arr[i].lineno);
					} 						
					if (strcmp(a, sync[7]) == 0){
						f = whilecheck(str, arr[i].linelen);
						if(f) fprintf(fout, "Error in Statement : ---> line no: %d\n", arr[i].lineno);
					} 	
				}
				k = 0;
			}
			else { a[k] = str[j]; k++; }
		}
	}
}