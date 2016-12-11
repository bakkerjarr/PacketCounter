/*************************************************************************
* Copyright 2016 Jarrod N. Bakker
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Main file for a packet counting program.
*
* FILENAME : pcounter.c
*
* DESCRIPTION :
*       Count packets on an interface and the write the value to a file
*       when the program closes via SIGINT (Ctrl-c) or SIGTERM.
* 
* AUTHOR :  Jarrod N. Bakker    START DATE :    11/12/2016
*
**/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

void sigintHandler(int);
void sigtermHandler(int);
static void writeFile();

static const char *fname = "packet_counts.csv";

/* This is a horrible way of storingi varibles that but it works */
/* well for this scenario. */
static unsigned long numPackets;
static char startTime[64];

/**
 * Write the current value of numPackets to file along with the
 * starting time of the program. Close the program afterwards.
 */
static void writeFile() {
   FILE *fp = fopen(fname, "a");
   fprintf(fp, "%s, %lu\n", startTime, numPackets);
   fclose(fp);
   /* Data has been written, close the program. */
   exit(0);
}

void sigintHandler(int sig) {
    switch (sig) {
    case SIGINT:
        fprintf(stdout, "SIGINT caught...\n");
        writeFile();
    default:
        fprintf(stderr, "wasn't expecting that!\n");
        abort();
    }
}

void sigtermHandler(int sig) {
    switch (sig) {
    case SIGTERM:
        fprintf(stdout, "SIGTERM caught...\n");
        writeFile();
    default:
        fprintf(stderr, "wasn't expecting that!\n");
        abort();
    }
}

int main() {
    /* Initialise handlers */
    signal(SIGINT, sigintHandler);
    signal(SIGTERM, sigtermHandler);
    int *p = NULL;

    /* Initialise shared varibles */
    numPackets = 0;
    time_t t = time(NULL);
    struct tm *localTime = localtime(&t);
    strftime(startTime, sizeof(startTime), "D%Y-%m-%d_h%Hm%Ms%S", localTime);

    while(1);
    return *p;
}
