#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <utility>
#include <syslog.h>
#include "../structures/database.h"

#include "program.h"

std::pair<float, float> connect_and_listen(char *ip, int port, std::vector<Database*>* replicas, const char* query);
bool connect_to_replicas(std::vector<Database*> replicas);
std::vector<Database*> setup_db_replicas_pool(web::json::value json);