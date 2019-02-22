#ifndef __SCHED_TESTER_H__
#define __SCHED_TESTER_H__

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sched.h>
#include <argp.h>

#include "sched_deadline_wrapper.h"

#define EXEC_TIME 2
#define SCHED_DEFAULT SCHED_DEADLINE

#define DEFAULT_RUNTIME    17000000;
#define DEFAULT_PERIOD    100000000;
#define DEFAULT_DEADLINE  100000000;

#endif // __SCHED_TESTER_H__
