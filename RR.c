// #include "circular_queue.h"
// #include "./src/Includes/defs.h"
// #include "./src/Process.c"

// void RR(CircularQueue *readyQueue, int timeSlice)
// {
//   initClk();

//   int startInterval = getClk();
//   int time = getClk();

//   int totalTurnaround = 0;
//   int totalWeightedTurnaround = 0;
//   int totalWaitingTime = 0;

//   while (1)
//   {

//     if (readyQueue->count == 0)
//       break;

//     PCB *running = getRunningProcess(readyQueue);
//     if (running->state == started) // need when read the process from input file set state = 1
//     {
//       running->startTime = getClk();
//       startInterval = getClk();
//       time = startInterval;
//       printLog(running, getClk());
//       running->state = runningState;
//     }
//     else if (running->state == stopped)
//     {
//       running->state = resumed;
//       printLog(running, getClk());
//       running->state = runningState;
//     }

//     if (getClk() == time + 1)
//     {
//       running->remainingTime--;
//       time++;

//       if (running->remainingTime == 0)
//       {
//         running->state = finished;
//         running->finishTime = getClk();
//         running->turnaround = running->finishTime - running->arrivalTime;
//         running->weightedTurnaround =
//             running->turnaround / (float)running->runningTime;
//         printLog(running, getClk());

//         totalTurnaround += running->turnaround;
//         totalWeightedTurnaround += running->weightedTurnaround;

//         if (terminateProcess(readyQueue) == 0)
//           break;
//         running = getRunningProcess(readyQueue);
//         startInterval = getClk();
//         running->waitingTime += startInterval - running->preemptedAt;
//         time = startInterval;
//       }

//       if (timeSlice == getClk() - startInterval)
//       {
//         running->preemptedAt = getClk();
//         running->state = stopped;
//         printLog(running, getClk());

//         switchRunningProcess(readyQueue);
//         running = getRunningProcess(readyQueue);
//         startInterval = getClk();
//         running->waitingTime += startInterval - running->preemptedAt;
//       }
//     }
//   }
// }
// void main(int argc, char *argv[])
// {

//   CircularQueue readyQueue = createCircularQueue();
//   PCB *pcb1 = initializeProcess((Process){.arrivalTime = 0, .runningTime = 20, .id = 1, .priority = 5});
//   PCB *pcb2 = initializeProcess((Process){.arrivalTime = 0, .runningTime = 10, .id = 2, .priority = 4});
//   PCB *pcb3 = initializeProcess((Process){.arrivalTime = 0, .runningTime = 12, .id = 3, .priority = 2});

//   addProcess(&readyQueue, pcb1);
//   addProcess(&readyQueue, pcb2);
//   addProcess(&readyQueue, pcb3);

//   RR(&readyQueue, 3);
// }