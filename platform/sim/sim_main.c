#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "main_app.h"
#include "system_coordinator.h"

extern void MotionTaskStep(void);
extern void CommsTaskStep(void);
extern void IOTaskStep(void);
extern void HealthTaskStep(void);

int main(int argc, char **argv) {
    int max_iterations = 100;
    if (argc > 1) {
        max_iterations = atoi(argv[1]);
    }

    printf("===========================================\n");
    printf(" ToolController - Platform Simulation (SIM)\n");
    printf("===========================================\n");

    // Initialize system coordinator
    system_coordinator_init(&g_sys);

    printf("[SIM MAIN] Running %d simulation loop iterations...\n", max_iterations);
    for (int i = 0; i < max_iterations; i++) {
        MotionTaskStep();
        CommsTaskStep();
        IOTaskStep();
        HealthTaskStep();
    }

    printf("[SIM MAIN] Simulation completed successfully!\n");
    return 0;
}
