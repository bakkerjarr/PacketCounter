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

#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

void countPacket(u_char *user, const struct pcap_pkthdr *header, const u_char *packet);
void sigintHandler(int sig);
void sigtermHandler(int sig);
static void writeFile();

static const char *ARGS = "<interface name>";
static const char *FNAME = "packet_counts.csv";
static const int NUM_ARGS = 1; // No. of extra args that are expected

/* This is a horrible way of storing varibles but it works well for */
/* this scenario. */
static unsigned long numPackets;
static char startTime[64];
static pcap_t *pcapture;

/**
 * Write the current value of numPackets to file along with the
 * starting time of the program. Close the program afterwards.
 */
static void writeFile() {
   FILE *fp = fopen(FNAME, "a");
   fprintf(stdout, "Captured %lu packets. Writing result to file %s\n", numPackets, FNAME);
   fprintf(fp, "%s, %lu\n", startTime, numPackets);
   fclose(fp);
   /* Data has been written, close the program. */
   pcap_close(pcapture);
   fprintf(stdout, "Closing program.\n");
   exit(0);
}

void sigintHandler(int sig) {
    switch (sig) {
    case SIGINT:
        fprintf(stdout, "Caught SIGINT signal.\n");
        writeFile();
    default:
        fprintf(stderr, "Caught signal %d. Aborting program.\n", sig);
        abort();
    }
}

void sigtermHandler(int sig) {
    switch (sig) {
    case SIGTERM:
        fprintf(stdout, "Caught SIGTERM signal.\n");
        writeFile();
    default:
        fprintf(stderr, "Caught signal %d. Aborting program.\n", sig);
        abort();
    }
}

/**
 * Increment the packet counter variable every time we receive a packet.
 * Nice and simple. The parameters are not explained as they are not
 * used by this function.
 */
void countPacket(u_char *user, const struct pcap_pkthdr *header, const u_char *packet){
    ++numPackets;
}

int main(int argc, char * argv[]) {
    /* Check arguments */
    if (argc != NUM_ARGS+1) {
        fprintf(stderr, "Error: Expected %d arguments, passed %d.\n", NUM_ARGS, argc-1);
        fprintf(stdout, "Usage: %s %s\n", argv[0], ARGS);
        return 1;
    }

    /* Initialise handlers */
    signal(SIGINT, sigintHandler);
    signal(SIGTERM, sigtermHandler);
    int *p = NULL;

    /* Initialise shared varibles */
    numPackets = 0;
    time_t t = time(NULL);
    struct tm *localTime = localtime(&t);
    strftime(startTime, sizeof(startTime), "D%Y-%m-%d_h%Hm%Ms%S", localTime);

    /* Prepare an interface for packet capture in promiscuous mode */
    char errbuf[PCAP_ERRBUF_SIZE];
    pcapture = pcap_open_live(argv[1], BUFSIZ, 1, 0, errbuf);
    if (pcapture == NULL) {
        fprintf(stderr, "Unable to start packet capture on interface %s\n.", argv[1]);
        return 1;
    }

    /* Capture packet indefinitely */
    pcap_loop(pcapture, 0, countPacket, NULL);

    while(1);
    return *p;
}
