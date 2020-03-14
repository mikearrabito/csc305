// Michael Arrabito
// CSC 305 Lab 2

#include <iostream>
#include <string>
#include <map>

using namespace std;

struct process{
    string name;
    string status;
    int part;
    int memNeeded;
    int waste;

    process() {
        name = "";
        status = "";
        part = -1;
        memNeeded = 0;
        waste = 0;
    }

    process(string n, string s, int p, int m) {
        name = n;
        status = s;
        part = p;
        memNeeded = m;
        waste = -1;
    }
};

struct partition {
    string name;
    bool inUse;
    int size;

    partition() {
        name = "";
        size = 0;
        inUse = false;
    }
    partition(string n, int s) {
        name = n;
        size = s;
        inUse = false;
    }
};

int calcWaste(process* p[]) {
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        if (p[i]->waste > 0) {
            sum += p[i]->waste;
        }
    }
    return sum;
}

void printResults(process* procs[], partition* parts[]) {
    for (int i = 0; i < 5; i++) {
        string w = to_string(procs[i]->waste);
        if (procs[i]->waste == -1) {
            w = "N\\A";
        }
        string p = "P" + to_string(procs[i]->part);
        if (procs[i]->part == -1) {
            p = "N\\A";
        }
        cout <<"ID: "<< procs[i]->name << " " << "Partition :" << p << " " << "Waste: " << w
            << " State: " << procs[i]->status << endl;
    }
    cout << "Total waste: " << calcWaste(procs) << endl;
    return;
}

void firstfit(process* job[], partition* part[]){
    // For each job search for first partition with enough space
    for (int i = 0; i < 5; i++) {
        // For each job
        for (int j = 0; j < 4; j++) {
            // for each partition
            if (job[i]->memNeeded <= part[j]->size && part[j]->inUse!=true) {
                // If first partition encountered has enough space, give partition that job
                job[i]->part = j+1;
                job[i]->waste = part[j]->size - job[i]->memNeeded;
                job[i]->status = "Running";
                part[j]->inUse = true;
                break; // This job is done, go to next
            }
        }
    }

    cout << "First Fit:" << endl;
    printResults(job, part);

    return;
}

void nextfit(process* job[], partition* part[]) {
    //similar to firstfit but begins from where it left off after allocating previous job

    int j = 0;
    int count = 0;
    for (int i = 0; i < 5; i++) {
        while(j < 4 && count < 4) {
            // will go through the 4 total partitions we have
            // count variable added to stop loop once we have checked all 4 partitions
            if (job[i]->memNeeded <= part[j]->size && part[j]->inUse != true) {
                job[i]->part = j + 1;
                job[i]->waste = part[j]->size - job[i]->memNeeded;
                job[i]->status = "Running";
                part[j]->inUse = true;
                break; // goes to next i (job) and maintains the current j(partition)
            }
            count++; // to stop when we have checked all partitions
            j = (j + 1) % 4; // using mod to go back to start once we go over number of partitions
            // (from j=3 we get j+1 = 4 % 4 = 0)
        }
    }

    cout <<endl<< "Next Fit:" << endl;
    printResults(job, part);

    return;
}

void bestfit(process* job[], partition* part[]) {
    // assign to first possible candidate
    // update when finding another part that is < current num but still > memneeded
    for (int i = 0; i < 5; i++) {
        int current = 99999;
        int bestfit = -1;
        for (int j = 0; j < 4; j++) {
            if (job[i]->memNeeded <= part[j]->size && part[j]->inUse != true && part[j]->size < current) {
                
                bestfit = j;
                current = part[j]->size;
            }
   
        }
        if (bestfit >= 0) {
            // only execute if we found a part with enough space
            job[i]->part = bestfit + 1;
            job[i]->waste = part[bestfit]->size - job[i]->memNeeded;
            job[i]->status = "Running";
            part[bestfit]->inUse = true;
        }
    }
    cout << endl<< "Best Fit:" << endl;
    printResults(job, part);

    return;
}

void worstfitfixed(process* job[], partition* part[]) {
    // similar to bestfit, but select largest instead of smallest available partition
    // still checking if partition is large enough for job, etc

    for (int i = 0; i < 5; i++) {
        int current = 0;
        int worstfit = -1;
        for (int j = 0; j < 4; j++) {
            if (job[i]->memNeeded <= part[j]->size && part[j]->inUse != true && part[j]->size > current) {

                worstfit = j;
                current = part[j]->size;
            }

        }
        if (worstfit >= 0) {
            // only execute if we found a part with enough space
            job[i]->part = worstfit + 1;
            job[i]->waste = part[worstfit]->size - job[i]->memNeeded;
            job[i]->status = "Running";
            part[worstfit]->inUse = true;
        }
    }

    cout << endl << "Worst Fit (fixed):" << endl;
    printResults(job, part);

    return;
}


void worstfitdynamic(process* job[], partition* part[]) {

    int extra = 0;
    for (int i = 0; i < 5; i++) {
        int current = 0;
        int worstfit = -1;
        for (int j = 0; j < 4; j++) {
            if (job[i]->memNeeded <= part[j]->size && part[j]->inUse != true && part[j]->size > current) {

                worstfit = j;
                current = part[j]->size;
            }

        }
        if (worstfit >= 0) {
            // only execute if we found a part with enough space
            job[i]->part = worstfit + 1;

            if (job[i]->memNeeded > extra) {
                // If extra waste is greater than memneeded, use extra wasted memory first
                extra -= job[i]->memNeeded;
                job[i]->waste = 0;
            }
            else {
                extra += job[i]->waste = part[worstfit]->size - job[i]->memNeeded;
            }
            
            job[i]->status = "Running";
            part[worstfit]->inUse = true;
        }
    }

    cout << endl << "Worst Fit (dynamic):" << endl;
    printResults(job, part);

    return;
}


void reset(process* procs[], partition* parts[]) {
    for (int i = 0; i < 5; i++) {
        procs[i]->status = "Waiting";
        procs[i]->waste = -1;
        procs[i]->part = -1;
    }
    for (int i = 0; i < 4; i++) {
        parts[i]->inUse = false;
    }
    return;
}

int main(){
    partition* parts[4];

    for (int i = 0; i < 4; i++) {
        parts[i] = new partition("P" + to_string(i + 1), 0);
    }
    parts[0]->size = 100;
    parts[1]->size = 300;
    parts[2]->size = 200;
    parts[3]->size = 450;

    process* procs[5]; // Holds job number, status, partition used
    for (int i = 0; i < 5; i++) {
        procs[i] = new process("J" + to_string(i + 1), "Waiting", -1, 100);
    }
    // Values given from assignment
    procs[0]->memNeeded = 200;
    procs[1]->memNeeded = 100;
    procs[2]->memNeeded = 300;
    procs[3]->memNeeded = 300;
    procs[4]->memNeeded = 200;

    firstfit(procs, parts);

    reset(procs, parts);
    nextfit(procs, parts);

    reset(procs, parts);
    bestfit(procs, parts);

    reset(procs, parts);
    worstfitfixed(procs, parts);

    reset(procs, parts);
    worstfitdynamic(procs, parts);

    return 0;
}

