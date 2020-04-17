#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <utility>
#include <syslog.h>
#include "../structures/database.h"

#include "program.h"

Select* process_query(Database* replica, const char* query);
bool connect_to_replicas(std::vector<Database*> replicas);
std::vector<Database*> setup_db_replicas_pool(web::json::value json);