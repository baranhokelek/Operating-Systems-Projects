#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <queue>
#include <string>
#include <sys/time.h>
#include <sys/types.h>
#include <set>
#include <fstream>
#include <map>
#define NUM_THREADS 5
// #define __USE_GNU
using namespace std;
int counter = 0;
int tidCounter = 0;
time_t starttime;
time_t simulationtime;
int sleepTime = 1;
double prob;
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_mutex_t mutex3;
pthread_mutex_t countmutex;
int sizes[] = {0, 0, 0, 0};
pthread_cond_t count_threshold_cv;
bool flag = false;
bool breakdown = false;
ofstream cclog, trainlog;

/******************************************************************************
  pthread_sleep takes an integer number of seconds to pause the current thread
  original by Yingwu Zhu
  updated by Muhammed Nufail Farooqi
 *****************************************************************************/
int pthread_sleep(int seconds)
{
    pthread_mutex_t mutex;
    pthread_cond_t conditionvar;
    struct timespec timetoexpire;
    if (pthread_mutex_init(&mutex, NULL))
    {
        return -1;
    }
    if (pthread_cond_init(&conditionvar, NULL))
    {
        return -1;
    }
    struct timeval tp;
    // When to expire is an absolute time, so get the current time and add //it to our delay time
    gettimeofday(&tp, NULL);
    timetoexpire.tv_sec = tp.tv_sec + seconds;
    timetoexpire.tv_nsec = tp.tv_usec * 1000;

    pthread_mutex_lock(&mutex);
    int res = pthread_cond_timedwait(&conditionvar, &mutex, &timetoexpire);
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&conditionvar);

    // Upon successful completion, a value of zero shall be returned
    return res;
}

class Train
{
    // Access specifier
public:
    // Data Members
    int trainid;
    string startpoint;
    string destinationpoint;
    int length;
    time_t startTime;
    time_t departureTime;
    // Member Functions
    void printname()
    {
        cout << "\nTrain id is: " << trainid << "\nStart time: " << ctime(&startTime) << "Start point: " << startpoint << "\nDestination point: " << destinationpoint << "\nLength: " << length << endl;
    }
};

queue<Train> trainsA;
queue<Train> trainsE;
queue<Train> trainsF;
queue<Train> trainsB;
set<int> total;
map<int, Train> mymap;
void *ACLine(void *threadid)
{
    while (time(NULL) < (starttime + simulationtime))
    {
        pthread_mutex_lock(&countmutex);
        if (counter >= 10)
            flag = true;
        pthread_mutex_unlock(&countmutex);
        if (!flag)
        {
            int random = rand() % 100 + 1;
            if (random < prob * 100)
            {
                Train t;
                int random2 = rand() % 100 + 1;
                if (random2 >= 70)
                {
                    t.length = 200;
                }
                else
                    t.length = 100;
                t.startpoint = "A";
                int random3 = rand() % 2;
                if (random3 == 0)
                    t.destinationpoint = "E";
                else
                    t.destinationpoint = "F";
                pthread_mutex_lock(&countmutex);
                counter++;
                tidCounter++;
                pthread_mutex_unlock(&countmutex);
                t.trainid = tidCounter;
                t.startTime = time(NULL);
                pthread_mutex_lock(&mutex2);
                trainsA.push(t);
                total.insert(t.trainid);
                pthread_mutex_unlock(&mutex2);
            }
        }
        else
        {
            pthread_mutex_lock(&countmutex);
            while (counter > 0 && time(NULL) < (starttime + simulationtime))
            {
                pthread_cond_wait(&count_threshold_cv, &countmutex);
            }
            flag = false;
            pthread_mutex_unlock(&countmutex);
        }
        pthread_sleep(sleepTime);
    }
    cout << "ACLine out!" << endl;
    pthread_exit(NULL);
}

void *EDLine(void *threadid)
{

    while (time(NULL) < (starttime + simulationtime))
    {

        pthread_mutex_lock(&countmutex);
        if (counter >= 10)
            flag = true;
        pthread_mutex_unlock(&countmutex);
        if (!flag)
        {
            int random = rand() % 100 + 1;
            if (random < prob * 100)
            {
                Train t;
                int random2 = rand() % 100 + 1;
                if (random2 >= 70)
                {
                    t.length = 200;
                }
                else
                    t.length = 100;
                t.startpoint = "E";
                int random3 = rand() % 2;
                if (random3 == 0)
                    t.destinationpoint = "A";
                else
                    t.destinationpoint = "B";
                pthread_mutex_lock(&countmutex);
                counter++;
                tidCounter++;
                pthread_mutex_unlock(&countmutex);
                t.trainid = tidCounter;
                t.startTime = time(NULL);
                pthread_mutex_lock(&mutex2);
                trainsE.push(t);
                total.insert(t.trainid);
                pthread_mutex_unlock(&mutex2);
            }
        }
        else
        {
            pthread_mutex_lock(&countmutex);
            while (counter > 0 && time(NULL) < (starttime + simulationtime))
            {
                pthread_cond_wait(&count_threshold_cv, &countmutex);
            }
            flag = false;
            pthread_mutex_unlock(&countmutex);
        }

        pthread_sleep(sleepTime);
    }
    cout << "EDLine out!" << endl;
    pthread_exit(NULL);
}

void *FDLine(void *threadid)
{

    while (time(NULL) < (starttime + simulationtime))
    {

        pthread_mutex_lock(&countmutex);
        if (counter >= 10)
            flag = true;
        pthread_mutex_unlock(&countmutex);
        if (!flag)
        {
            int random = rand() % 100 + 1;
            if (random < prob * 100)
            {
                Train t;
                int random2 = rand() % 100 + 1;
                if (random2 >= 70)
                {
                    t.length = 200;
                }
                else
                    t.length = 100;
                t.startpoint = "F";
                int random3 = rand() % 2;
                if (random3 == 0)
                    t.destinationpoint = "A";
                else
                    t.destinationpoint = "B";
                pthread_mutex_lock(&countmutex);
                counter++;
                tidCounter++;
                pthread_mutex_unlock(&countmutex);
                t.trainid = tidCounter;
                t.startTime = time(NULL);
                pthread_mutex_lock(&mutex2);
                trainsF.push(t);
                total.insert(t.trainid);
                pthread_mutex_unlock(&mutex2);
            }
        }
        else
        {
            pthread_mutex_lock(&countmutex);
            while (counter > 0 && time(NULL) < (starttime + simulationtime))
            {
                pthread_cond_wait(&count_threshold_cv, &countmutex);
            }
            flag = false;
            pthread_mutex_unlock(&countmutex);
        }
        pthread_sleep(sleepTime);
    }
    cout << "FDLine out!" << endl;
    pthread_exit(NULL);
}

void *BCLine(void *threadid)
{

    while (time(NULL) < (starttime + simulationtime))
    {
        pthread_mutex_lock(&countmutex);
        if (counter >= 10)
            flag = true;
        pthread_mutex_unlock(&countmutex);

        if (!flag)
        {
            int random = rand() % 100 + 1;
            if (random > prob * 100)
            {
                Train t;
                int random2 = rand() % 100 + 1;
                if (random2 >= 70)
                {
                    t.length = 200;
                }
                else
                    t.length = 100;
                t.startpoint = "B";
                int random3 = rand() % 2;
                if (random3 == 0)
                    t.destinationpoint = "E";
                else
                    t.destinationpoint = "F";
                pthread_mutex_lock(&countmutex);
                counter++;
                tidCounter++;
                pthread_mutex_unlock(&countmutex);
                t.trainid = tidCounter;
                t.startTime = time(NULL);
                pthread_mutex_lock(&mutex2);
                trainsB.push(t);
                total.insert(t.trainid);
                pthread_mutex_unlock(&mutex2);
            }
        }
        else
        {
            pthread_mutex_lock(&countmutex);
            while (counter > 0 && time(NULL) < (starttime + simulationtime))
            {
                pthread_cond_wait(&count_threshold_cv, &countmutex);
            }
            flag = false;
            pthread_mutex_unlock(&countmutex);
        }
        pthread_sleep(sleepTime);
    }
    cout << "BCLine out!" << endl;
    pthread_exit(NULL);
}

void *controlcenter(void *threadid)
{
    while (time(NULL) < (starttime + simulationtime))
    {
        breakdown = false;
        pthread_mutex_lock(&countmutex);
        if (counter >= 10)
            flag = true;
        pthread_mutex_unlock(&countmutex);

        if (counter == 0)
            pthread_cond_broadcast(&count_threshold_cv);

        pthread_mutex_lock(&mutex2);
        sizes[0] = trainsA.size();
        sizes[1] = trainsB.size();
        sizes[2] = trainsE.size();
        sizes[3] = trainsF.size();
        pthread_mutex_unlock(&mutex2);
        if (!(sizes[0] == 0 && sizes[1] == 0 && sizes[2] == 0 && sizes[3] == 0))
            cout << "A: " << sizes[0] << "    B: " << sizes[1] << "    E: " << sizes[2] << "    F: " << sizes[3] << endl;

        if (sizes[0] > sizes[2] && sizes[0] > sizes[3] && sizes[0] > sizes[1])
        {
            if (time(NULL) - trainsA.front().startTime > 1)
            {
                Train topass = trainsA.front();
                topass.printname();
                cout << "Trains Waiting Passage: ";
                for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                    cout << ' ' << *it;
                time_t eventTime = time(NULL);
                cout << endl;
                if (rand() % 10 < 1)
                    breakdown = true;
                if (topass.length == 100)
                    pthread_sleep(1 + ((breakdown) ? 4 : 0));
                else
                    pthread_sleep(2 + ((breakdown) ? 4 : 0));
                int passedID = trainsA.front().trainid;
                pthread_mutex_lock(&mutex2);
                time_t aftertunnel = time(NULL);
                topass.departureTime = (time_t)(aftertunnel + 1);
                cout << "Departure Time of " << topass.trainid << "  " << ctime(&topass.departureTime) << endl;
                mymap.insert(pair<int, Train>(topass.trainid, topass));

                trainsA.pop();
                total.erase(topass.trainid);
                pthread_mutex_unlock(&mutex2);
                cout << "Train " << passedID << " passed the tunnel." << endl;
                pthread_mutex_lock(&countmutex);
                counter--;
                pthread_mutex_unlock(&countmutex);
                cout << "Trains Waiting Passage: ";
                for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                    cout << ' ' << *it;
                cout << endl;
                cclog << ((breakdown) ? "Breakdown       " : "Tunnel Passing  ") << ctime(&eventTime) << "  " << topass.trainid << "  ";
                for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                    cclog << *it << ", ";
                cclog << endl;
                breakdown = false;
                continue;
            }
        }
        else if (sizes[2] > sizes[0] && sizes[2] > sizes[3] && sizes[2] > sizes[1])
        {
            if (time(NULL) - trainsE.front().startTime > 1)
            {
                Train topass = trainsE.front();
                topass.printname();
                cout << "Trains Waiting Passage: ";
                for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                    cout << ' ' << *it;
                time_t eventTime = time(NULL);
                cout << endl;
                if (rand() % 10 < 1)
                    breakdown = true;

                if (topass.length == 100)
                    pthread_sleep(1 + ((breakdown) ? 4 : 0));
                else
                    pthread_sleep(2 + ((breakdown) ? 4 : 0));
                // pthread_sleep(sleepTime);
                int passedID = trainsE.front().trainid;
                pthread_mutex_lock(&mutex2);
                time_t aftertunnel = time(NULL);
                topass.departureTime = (time_t)(aftertunnel + 1);
                cout << "Departure Time of " << topass.trainid << "  " << ctime(&topass.departureTime) << endl;
                mymap.insert(pair<int, Train>(topass.trainid, topass));
                trainsE.pop();
                total.erase(topass.trainid);
                pthread_mutex_unlock(&mutex2);
                cout << "Train " << passedID << " passed the tunnel." << endl;
                pthread_mutex_lock(&countmutex);
                counter--;
                pthread_mutex_unlock(&countmutex);
                cout << "Trains Waiting Passage: ";
                for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                    cout << ' ' << *it;

                cout << endl;
                cclog << ((breakdown) ? "Breakdown       " : "Tunnel Passing  ") << ctime(&eventTime) << "  " << topass.trainid << "  ";
                for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                    cclog << *it << ", ";
                cclog << endl;
                breakdown = false;
                continue;
            }
        }
        else if (sizes[3] > sizes[0] && sizes[3] > sizes[2] && sizes[3] > sizes[1])
        {
            if (time(NULL) - trainsF.front().startTime > 1)
            {
                Train topass = trainsF.front();
                topass.printname();
                cout << "Trains Waiting Passage: ";
                for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                    cout << ' ' << *it;
                time_t eventTime = time(NULL);
                cout << endl;
                if (rand() % 10 < 1)
                    breakdown = true;

                if (topass.length == 100)
                    pthread_sleep(1 + ((breakdown) ? 4 : 0));
                else
                    pthread_sleep(2 + ((breakdown) ? 4 : 0));

                // pthread_sleep(sleepTime);
                int passedID = trainsF.front().trainid;
                pthread_mutex_lock(&mutex2);
                time_t aftertunnel = time(NULL);
                topass.departureTime = (time_t)(aftertunnel + 1);
                cout << "Departure Time of " << topass.trainid << "  " << ctime(&topass.departureTime) << endl;
                mymap.insert(pair<int, Train>(topass.trainid, topass));
                trainsF.pop();
                total.erase(topass.trainid);
                pthread_mutex_unlock(&mutex2);
                cout << "Train " << passedID << " passed the tunnel." << endl;
                pthread_mutex_lock(&countmutex);
                counter--;
                pthread_mutex_unlock(&countmutex);
                cout << "Trains Waiting Passage: ";
                for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                    cout << ' ' << *it;

                cout << endl;
                cclog << ((breakdown) ? "Breakdown       " : "Tunnel Passing  ") << ctime(&eventTime) << "  " << topass.trainid << "  ";
                for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                    cclog << *it << ", ";
                cclog << endl;
                breakdown = false;
                continue;
            }
        }
        else if (sizes[1] > sizes[0] && sizes[1] > sizes[2] && sizes[1] > sizes[3])
        {
            if (time(NULL) - trainsB.front().startTime > 1)
            {
                Train topass = trainsB.front();
                topass.printname();
                cout << "Trains Waiting Passage: ";
                for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                    cout << ' ' << *it;
                time_t eventTime = time(NULL);
                cout << endl;
                if (rand() % 10 < 1)
                    breakdown = true;

                if (topass.length == 100)
                    pthread_sleep(1 + ((breakdown) ? 4 : 0));
                else
                    pthread_sleep(2 + ((breakdown) ? 4 : 0));

                // pthread_sleep(sleepTime);
                int passedID = trainsB.front().trainid;
                pthread_mutex_lock(&mutex2);
                time_t aftertunnel = time(NULL);
                topass.departureTime = (time_t)(aftertunnel + 1);
                cout << "Departure Time of " << topass.trainid << "  " << ctime(&topass.departureTime) << endl;
                mymap.insert(pair<int, Train>(topass.trainid, topass));
                trainsB.pop();
                total.erase(topass.trainid);
                pthread_mutex_unlock(&mutex2);
                cout << "Train " << passedID << " passed the tunnel." << endl;
                pthread_mutex_lock(&countmutex);
                counter--;
                pthread_mutex_unlock(&countmutex);
                cout << "Trains Waiting Passage: ";
                for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                    cout << ' ' << *it;

                cout << endl;
                cclog << ((breakdown) ? "Breakdown       " : "Tunnel Passing  ") << ctime(&eventTime) << "  " << topass.trainid << "  ";
                for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                    cclog << *it << ", ";
                cclog << endl;
                breakdown = false;
                continue;
            }
        }
        else // tie
        {
            if (sizes[0] == 0 && sizes[1] == 0 && sizes[2] == 0 && sizes[3] == 0)
            {
                // cout <<     "No trains. Moving on " << endl;
                continue;
            }
            else if ((sizes[0] == sizes[1] && sizes[2] <= sizes[0]) || (sizes[0] == sizes[2] && sizes[1] <= sizes[0]) || (sizes[0] == sizes[3] && sizes[2] <= sizes[0]))
            {
                // A gets priority

                if (time(NULL) - trainsA.front().startTime > 1)
                {
                    cout << "\nTie occurred. A gets priority      " << sizes[0] << "  " << sizes[1] << "   " << sizes[2] << "   " << sizes[3] << endl;
                    Train topass = trainsA.front();
                    topass.printname();

                    cout << "Trains Waiting Passage: ";
                    for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                        cout << ' ' << *it;
                    time_t eventTime = time(NULL);
                    cout << endl;
                    if (rand() % 10 < 1)
                        breakdown = true;

                    if (topass.length == 100)
                        pthread_sleep(1 + ((breakdown) ? 4 : 0));
                    else
                        pthread_sleep(2 + ((breakdown) ? 4 : 0));

                    int passedID = trainsA.front().trainid;
                    pthread_mutex_lock(&mutex2);
                    time_t aftertunnel = time(NULL);
                    topass.departureTime = (time_t)(aftertunnel + 1);
                    cout << "Departure Time of " << topass.trainid << "  " << ctime(&topass.departureTime) << endl;
                    mymap.insert(pair<int, Train>(topass.trainid, topass));
                    trainsA.pop();
                    total.erase(topass.trainid);
                    cout << "Train " << passedID << " passed the tunnel." << endl;
                    pthread_mutex_unlock(&mutex2);

                    pthread_mutex_lock(&countmutex);
                    counter--;
                    pthread_mutex_unlock(&countmutex);
                    cout << "Trains Waiting Passage: ";
                    for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                        cout << ' ' << *it;
                    cout << endl;
                    cclog << ((breakdown) ? "Breakdown       " : "Tunnel Passing  ") << ctime(&eventTime) << "  " << topass.trainid << "  ";
                    for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                        cclog << *it << ", ";
                    cclog << endl;
                    breakdown = false;
                    continue;
                }
            }
            else if (sizes[1] == sizes[3] || sizes[1] == sizes[2])
            {
                // B gets priority

                if (time(NULL) - trainsB.front().startTime > 1)
                {
                    cout << "\nTie occurred. B gets priority      " << sizes[0] << "  " << sizes[1] << "   " << sizes[2] << "   " << sizes[3] << endl;
                    Train topass = trainsB.front();
                    topass.printname();
                    cout << "Trains Waiting Passage: ";
                    for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                        cout << ' ' << *it;
                    time_t eventTime = time(NULL);
                    cout << endl;
                    if (rand() % 10 < 1)
                        breakdown = true;

                    if (topass.length == 100)
                        pthread_sleep(1 + ((breakdown) ? 4 : 0));
                    else
                        pthread_sleep(2 + ((breakdown) ? 4 : 0));

                    // pthread_sleep(sleepTime);
                    int passedID = trainsB.front().trainid;
                    pthread_mutex_lock(&mutex2);
                    time_t aftertunnel = time(NULL);
                    topass.departureTime = (time_t)(aftertunnel + 1);
                    cout << "Departure Time of " << topass.trainid << "  " << ctime(&topass.departureTime) << endl;
                    mymap.insert(pair<int, Train>(topass.trainid, topass));
                    trainsB.pop();
                    total.erase(topass.trainid);
                    pthread_mutex_unlock(&mutex2);
                    cout << "Train " << passedID << " passed the tunnel." << endl;
                    pthread_mutex_lock(&countmutex);
                    counter--;
                    pthread_mutex_unlock(&countmutex);
                    cout << "Trains Waiting Passage: ";
                    for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                        cout << ' ' << *it;
                    cout << endl;
                    cclog << ((breakdown) ? "Breakdown       " : "Tunnel Passing  ") << ctime(&eventTime) << "  " << topass.trainid << "  ";
                    for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                        cclog << *it << ", ";
                    cclog << endl;
                    breakdown = false;
                    continue;
                }
            }
            else if (sizes[2] == sizes[3])
            {
                // E gets priority

                if (time(NULL) - trainsE.front().startTime > 1)
                {
                    cout << "\nTie occurred. E gets priority      " << sizes[0] << "  " << sizes[1] << "   " << sizes[2] << "   " << sizes[3] << endl;
                    Train topass = trainsE.front();
                    topass.printname();
                    cout << "Trains Waiting Passage: ";
                    for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                        cout << ' ' << *it;
                    time_t eventTime = time(NULL);
                    cout << endl;
                    if (rand() % 10 < 1)
                        breakdown = true;

                    if (topass.length == 100)
                        pthread_sleep(1 + ((breakdown) ? 4 : 0));
                    else
                        pthread_sleep(2 + ((breakdown) ? 4 : 0));

                    // pthread_sleep(sleepTime);
                    int passedID = trainsE.front().trainid;
                    pthread_mutex_lock(&mutex2);
                    time_t aftertunnel = time(NULL);
                    topass.departureTime = (time_t)(aftertunnel + 1);
                    cout << "Departure Time of " << topass.trainid << "  " << ctime(&topass.departureTime) << endl;
                    mymap.insert(pair<int, Train>(topass.trainid, topass));
                    trainsE.pop();
                    total.erase(topass.trainid);
                    pthread_mutex_unlock(&mutex2);
                    cout << "Train " << passedID << " passed the tunnel." << endl;
                    pthread_mutex_lock(&countmutex);
                    counter--;
                    pthread_mutex_unlock(&countmutex);
                    cout << "Trains Waiting Passage: ";
                    for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                        cout << ' ' << *it;
                    cout << endl;
                    cclog << ((breakdown) ? "Breakdown       " : "Tunnel Passing  ") << ctime(&eventTime) << "  " << topass.trainid << "  ";
                    for (set<int>::iterator it = total.begin(); it != total.end(); ++it)
                        cclog << *it << ", ";
                    cclog << endl;
                    breakdown = false;
                    continue;
                }
            }
        }

        pthread_sleep(sleepTime);
    }
    cout << "Control Center out!" << endl;
    pthread_cond_broadcast(&count_threshold_cv);
    for (int i = 1; i < tidCounter; i++)
    {
        Train t = mymap[i];
        if (t.departureTime != 0)
        {
            trainlog << "Train id is: " << t.trainid << "  Start time: " << ctime(&t.startTime) << "  Start point: " << t.startpoint << "  Destination point: " << t.destinationpoint << "  Length: " << t.length << "  Departure time: " << ctime(&t.departureTime) << endl;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    // starttime= chrono::system_clock::to_time_t(chrono::system_clock::now());

    cclog.open("control_center_log.txt");
    trainlog.open("train_log.txt");
    starttime = time(NULL);
    cout << starttime << endl;
    cout << ctime(&starttime) << endl;
    printf("Enter a total simulation time: ");
    cin >> simulationtime;
    cout << endl;
    /* initialize random seed: */
    srand(time(NULL));
    pthread_t threads[NUM_THREADS];
    int rc;

    printf("Enter some probability: ");

    cin >> prob;

    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_mutex_init(&mutex3, NULL);
    pthread_mutex_init(&countmutex, NULL);

    rc = pthread_create(&threads[0], NULL, ACLine, (void *)0);
    if (rc)
    {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        return -1;
    }
    rc = pthread_create(&threads[1], NULL, EDLine, (void *)1);
    if (rc)
    {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        return -1;
    }
    rc = pthread_create(&threads[2], NULL, controlcenter, (void *)2);
    if (rc)
    {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        return -1;
    }
    rc = pthread_create(&threads[3], NULL, FDLine, (void *)3);
    if (rc)
    {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        return -1;
    }
    rc = pthread_create(&threads[4], NULL, BCLine, (void *)4);
    if (rc)
    {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        return -1;
    }
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);
    pthread_join(threads[3], NULL);
    pthread_join(threads[4], NULL);
    /* Last thing that main() should do */

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
    pthread_mutex_destroy(&countmutex);
    // time_t endtime = chrono::system_clock::to_time_t(chrono::system_clock::now());

    time_t endtime = time(NULL);
    cout << endtime << endl;
    cout << ctime(&endtime) << endl;
    pthread_exit(NULL);

    return 0;
}
