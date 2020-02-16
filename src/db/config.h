#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <syslog.h>
#include "../structures/database.h"

#include "program.h"

bool connect_and_listen(char *ip, int port, std::vector<Database*> replicas);
bool connect_to_replicas(std::vector<Database*> replicas);
std::vector<Database*> setup_db_replicas_pool(char *file_path);