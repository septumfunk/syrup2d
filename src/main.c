//? septumfunk 2024
#include "engine/engine.h"
#include "util/log.h"
#include <string.h>

int main(int argv, char **argc) {
    char *folder = DEFAULT_RESOURCE_FOLDER;

    for (int i = 1; i < argv; ++i) {
        char *arg = argc[i];

        if (strcmp(arg, "-new")) {
            if (argv < i + 1) {
                log_error("Flag '-new' missing values: width, height,");
                return -1;
            }
            folder = argc[++i];
            continue;
        }
        if (strcmp(arg, "-open")) {
            if (argv < i + 1) {
                log_error("Flag '-open' missing value.");
                return -1;
            }
            folder = argc[++i];
            continue;
        }
        if (strcmp(arg, "-edit")) {
            // Enable edit mode
            continue;
        }
        log_error("Unknown argument '%s'.", arg);
    }

    engine_start(folder);
}
