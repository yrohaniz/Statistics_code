/* This program calculates the mean and the standard deviation of either
the integrated biomass density file or the integrated carbon concentration
file. It goes through the seed=* directories and picks the similar file names
for either of the integration data and calculates the statistics. When used 
for sensitivity analysis, the values of the parameter under scrutiny are 
included in the params file.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

//****************************************************************FILE SCOPE DECLARATIONS*********************************************************|                                                        |
//**************************************************************************************************************************************************|                                                   
long NUM; // The grid refinement in both x and y direction. It is assumed that these are equal.  
long NTIMES; // The number of time points in the simulation files   
long INIT_SD, FIN_SD; // Values of the initial seed and the final seed      
double INIT_PRM, FIN_PRM, INC_PRM; // Initial, final and increment values of the parameter under study                     
const int NUM_CHARS=100; //Initialize the size of string variables used below.    
//**************************************************************************************************************************************************|
//**************************************************************************************************************************************************|

//This function goes through the white-spaces in a line of params file and grabs the value of the parameter needed for formatting
void param_iteration(long n, char *s, FILE *f) {for (int i=0; i<=n; i++) fscanf(f, "%s", s);}

int main() {
	long i, j, k;
	long n, tot_prm_vals; // The number of white-spaces in the params file and the total number of the studied parameter values
	long numseeds; // The total number of seeds
	char *ptr; // This pointer is used in strtol and is set the next character in the string stored by strtol
	FILE *f_in, *f_out;
	char buff[NUM_CHARS]; // Declaration of a string that stores the input data point
	char SIM_FILE[NUM_CHARS]; //Declaration of a string that stores the input data-file name
	double prm[10]={0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.10,0.20,0.30}; //prm stores the values of the studied parameter
	double *mean, *stddevn; //*mean stores the average and *stddevn the standard deviation for each time point
	char str[NUM_CHARS]; // Declaration of an string to arbitrary length
	char dir[NUM_CHARS];//ditto as above
	char src[NUM_CHARS]; // ditto as above
	char sg[NUM_CHARS];// ditto as above
	struct stat st; // struct variable used for checking whether the folder where the files are saved, exists
	
	f_in = fopen ("statistic_params", "r");
	
	if (fgets(str, NUM_CHARS, f_in) !=NULL) { // Check whether params is opened and have contents
		fgets(str, NUM_CHARS, f_in); // Read the first line which is the number of white spaces
		n = strtol(str, &ptr, 10); //strtol reads the string in the first line and converts it to unsigned long integer
		fgets(str, NUM_CHARS, f_in); // Read the next line which is number of parameters
		printf("%s", str); // Print to the screen the number of parameters
		param_iteration(n,str,f_in);// go through white spaces within the line
		NTIMES = strtol(str, &ptr, 10); // Pick the value of the number of time points
		printf("# of time points: %ld\n", NTIMES);
		param_iteration(n,str,f_in);
		INIT_SD = strtol(str, &ptr, 10); // Pick the value of initial seed
		printf("Initial seed is: %ld\n", INIT_SD);
		param_iteration(n,str,f_in);
		FIN_SD = strtol(str, &ptr, 10); // Pick the value of final seed
		printf("Final seed is: %ld\n", FIN_SD);
		param_iteration(n,str,f_in);
		INIT_PRM = strtod(str, &ptr); // Pick the initial value of the parameter under study
		printf("Initial value of the parameter: %3.2f\n", INIT_PRM);
		param_iteration(n,str,f_in);
		FIN_PRM = strtod(str, &ptr); // Pick the final value of the parameter under study
		printf("Final value of the parameter: %3.2f\n", FIN_PRM);
		param_iteration(n,str,f_in); 
		INC_PRM = strtod(str, &ptr); // Pick the value of the increments by which the parameter changes
		printf("Parameter incremented by: %3.2f\n", INC_PRM);
		param_iteration(n,str,f_in);
		NUM = strtol(str, &ptr, 10); // Pick the value of the grid refinement
		printf("refinement: %ld\n", NUM);
		param_iteration(n,str,f_in);
		strcpy(SIM_FILE, str); // Read the name of the input file
		printf("data file: %s\n", SIM_FILE);
		fscanf(f_in, "%s", str);
		if (feof(f_in)) { // Check whether the end of file is true and if not exit and print warning to screen
			printf("Parameter reading complete.\n");
		}
		else { 
			printf("ERROR in file!CHECK params!\n");
			exit(1);
		}
	}
	
	fclose(f_in); // Close the params file
	
	numseeds = FIN_SD - INIT_SD + 1; // Initialize the total number of seeds
	//tot_prm_vals = (long) ((FIN_PRM - INIT_PRM) / INC_PRM); // Initialize the total number of the studied parameter values
	//prm = INIT_PRM; //Initialize the parameter under investigation
	
	sprintf(dir, "Statistics/"); //Averages for each parameter value are stored in this directory
	
	if (stat(dir, &st) == -1) {
    		mkdir(dir, 0700);
	}// This if-statement checks the existence of the directory '/mean' and creates it if not found
	
	//This for loop goes through the parameter values and calculates the statistics at each time point for each value
	for (i=0; i<=9; i++) {//tot_prm_vals
		sprintf(str, "%s%3.2f", SIM_FILE, prm[i]); // Append to the name of the file the current value of the studied parameter and store the result in str
    	strcpy(sg, dir); // copy dir to sg
    	strcat(sg, str); // String concatenation i.e. append str to scr
    	f_out = fopen (sg, "w"); //Open the file for the current parameter value with the its name given by sg
		if ((mean = (double *) calloc(NTIMES, sizeof(double))) == NULL) {
        	fprintf(stderr,"calloc failed\n");
        	exit(1);
    	}// Allocate clean (=0) memory to *mean
    	if ((stddevn = (double *) calloc(NTIMES, sizeof(double))) == NULL) {
        	fprintf(stderr,"calloc failed\n");
        	exit(1);
    	}// Allocate clean (=0) memory to *stddevn
    	//loop through the existing seeds of the current parameter value, here mean is filled up
		for (j=INIT_SD; j<=FIN_SD; j++) {
			sprintf(src, "seed=%ld/", j);
			strcpy(sg, src);
    		strcat(sg, str); // String concatenation i.e. append str to scr
    		f_in = fopen (sg, "r"); //Open the simulation file corresponding to the current seed value and studied parameter
			// The following for-loop reads and adds the data for each particular time point of the current opened file
			for (k=0; k<NTIMES; k++) {
				fscanf(f_in, "%s", buff); // Read the first data in input file (which is time). It is discarded by updating buff in the following statement
				fscanf(f_in, "%s", buff);
				mean[k] += strtod(buff, &ptr); // Add the value at this time point to the corresponding entry in mean
			}
			fclose(f_in);//Close the input data file before moving on to the next seed
		}
		//loop through the existing seeds of the current parameter value, here stddevn is filled up
		for (j=INIT_SD; j<=FIN_SD; j++) {
			sprintf(src, "seed=%ld/", j);
			strcpy(sg, src);
    		strcat(sg, str); // String concatenation i.e. append str to scr
    		f_in = fopen (sg, "r"); //Open the simulation file corresponding to the current seed value and studied parameter
			// The following for-loop reads and adds the data for each particular time point of the current opened file
			for (k=0; k<NTIMES; k++) {
				fscanf(f_in, "%s", buff); // Read the first data in input file (which is time). It is discarded by updating buff in the following statement
				fscanf(f_in, "%s", buff);
				stddevn[k] += pow((strtod(buff, &ptr)-(mean[k]/(double) numseeds)), 2.0); // Calculate the value minus the corresponding mean at this time point and square it
			}
			fclose(f_in);//Close the input data file before moving on to the next seed
		}
		//This for loop stores the mean value +- standard deviation of each time point for the current parameter value in the output file
		for (k=0; k<NTIMES; k++) {
			fprintf(f_out, "%f %15.14f %15.14f %15.14f\n", (double) k, mean[k]/(double) numseeds, mean[k]/(double) numseeds+sqrt(stddevn[k]/(double) (numseeds-1)), mean[k]/(double) numseeds-sqrt(stddevn[k]/(double) (numseeds-1)));
		}
		fclose(f_out); // Close the output file for the current parameter value
		//prm += INC_PRM; // Increment the value of prm with the increment value read from params file
		free(mean); //Free allocated memory to mean before moving on to the next parameter value
		free(stddevn);// Free allocated memory to stddevn
	}
	
	return 0;
}

