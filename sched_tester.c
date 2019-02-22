#include "sched_tester.h"

const char *argp_program_version = "sched-tester 0.1";
const char *argp_program_bug_address = "<cristian.monticone@gmail.com>";

static char doc[] = "sched-tester -- A simple utility to test Linux schedulers";

static struct argp_option options[] = {
  {"time",       't', "seconds", 0, "Execution time in seconds"},
  {"idle",       'i', 0,         0, "Set scheduler policy to SCHED_IDLE"},
  {"roundrobin", 'r', 0,         0, "The standard round-robin scheduler"},
  {"batch",      'b', 0,         0, "\"batch\" style process execution"},
  {"cpu",        'c', "number",  0, "Set mask affinity to a single cpu"},
  {"deadline",   'd', 0,         0, "Set scheduler to SCHED_DEADLINE", 1},
  {"runtime",    'R', "ns",      0, "Set scheduler runtime value in ns", 1},
  {"period",     'P', "ns",      0, "Set scheduler period value in ns", 1},
  {"deadline",   'D', "ns",      0, "Set scheduler deadline value in ns", 1},
  { 0 }
};

struct arguments {
  time_t execution_time_sec;        /* Execution time in seconds */
  uint32_t cpu_number;              /* CPU number */
  int affinity;                     /* Affinity mask on/off */
  uint32_t sched_policy;            /* Scheduler policy */
  uint64_t sched_runtime;           /* For deadline scheduler */
  uint64_t sched_period;            /* For deadline scheduler */
  uint64_t sched_deadline;          /* For deadline scheduler */
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;

  switch (key) {
    case 't':
      arguments->execution_time_sec = atoi(arg);
      break;
    case 'd':
      arguments->sched_policy = SCHED_DEADLINE;
      break;
    case 'i':
      arguments->sched_policy = SCHED_IDLE;
      break;
    case 'r':
      arguments->sched_policy = SCHED_OTHER;
      break;
    case 'b':
      arguments->sched_policy = SCHED_BATCH;
      break;
    case 'c':
      arguments->cpu_number = atoi(arg);
      arguments->affinity   = 1;
      break;
    case 'R':
      arguments->sched_runtime = atoll(arg);
      break;
    case 'P':
      arguments->sched_period = atoll(arg);
      break;
    case 'D':
      arguments->sched_deadline = atoll(arg);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

static struct argp argp = {options, parse_opt, NULL, doc};

int set_sched_policy(struct sched_attr *attr)
{
  int res;

  res = sched_setattr(0, attr, 0);

  if (res < 0) {
    perror("Sched SetAttr");
    exit(EXIT_FAILURE);
  }

  return res;
}

int set_mask_affinity(int cpu_number) {
  cpu_set_t mask;
  int res;

  CPU_ZERO(&mask);
  CPU_SET(cpu_number, &mask);

  res = sched_setaffinity(0, sizeof(mask), &mask);
  if (res < 0) {
    perror("Sched SetAffinity");
    exit(EXIT_FAILURE);
  }

  return res;
}

int main(int argc, char **argv)
{
  struct arguments arguments;
  struct sched_attr sched_attr;

  // Default args
  arguments.sched_policy       = SCHED_DEFAULT;
  arguments.sched_runtime      = DEFAULT_RUNTIME;
  arguments.sched_period       = DEFAULT_PERIOD;
  arguments.sched_deadline     = DEFAULT_DEADLINE;
  arguments.execution_time_sec = EXEC_TIME;
  arguments.affinity           = 0;

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  // CPU affinity mask
  if (arguments.affinity)
    set_mask_affinity(arguments.cpu_number);

  // Set sched_attr struct
  memset(&sched_attr, 0, sizeof(struct sched_attr));
  sched_attr.size           = sizeof(struct sched_attr);
  sched_attr.sched_policy   = arguments.sched_policy;
  sched_attr.sched_runtime  = arguments.sched_runtime;
  sched_attr.sched_period   = arguments.sched_period;
  sched_attr.sched_deadline = arguments.sched_deadline;
  
  // Change scheduler
  set_sched_policy(&sched_attr);

  alarm(arguments.execution_time_sec);

  printf("sched-tester started successfully.\n");

  while("don't stop me now"); // Cause I having a good time...
                              // Just give me a SIGALRM call!

  exit(EXIT_SUCCESS);
}
