#include <stdio.h>

#include "db/config.h"

#include <libpq-fe.h>
#include <vector>

#include "db/proxy.h"

using namespace std;
// list of replicas involved
vector<Database*> replicas;


pthread_mutex_t replicas_lock;

/*
 * thread which is responsible for contacting replicas 
 * and for reporting about their life
*/
pthread_t keep_alive_thread;


int main(int argc, char* argv[]) {
    
    //
    
    
    Program* program = Program::getInstance();
    
    program->setUp(argc, argv);

    char *CONFIG_FILE_PATH = program->getConfigFilePath();

    std::ifstream t;
    t.open(CONFIG_FILE_PATH);
    
    auto json = web::json::value::parse(t);

    replicas = setup_db_replicas_pool(json);
    
    if(replicas.size() == 0) {
        program->log(LOG_WARNING, "Number of databases are zero.\n");
        exit(0);
    } 
    
    if( !connect_to_replicas(replicas) ) {
        program->log(LOG_WARNING, "None of the replicas are alive.\n");
        return 0;
    }
    
    //Proxy* p = new Proxy(55432, replicas[0]);

    vector<const char*> queries;
    for(int i=0;i<1;++i) {
        //queries.push_back("select * from sbtest1 as s1,sbtest2 as s2, sbtest3 as s3 where s1.k = s2.k + s3.k;");
        queries.push_back("select     l_orderkey,     sum(l_extendedprice * (1 - l_discount)) as revenue,     o_orderdate,     o_shippriority from     customer,     orders,     lineitem where     c_mktsegment = 'mike'     and c_custkey = o_custkey     and l_orderkey = o_orderkey     and o_orderdate < 50     and l_shipdate > 50 group by     l_orderkey,     o_orderdate,     o_shippriority order by     revenue desc,     o_orderdate;");
        /*queries.push_back("SELECT * FROM sbtest1;");
        queries.push_back("SELECT * from sbtest1 where k = 2;");
        queries.push_back("SELECT * from sbtest1 where k < 2;");
        queries.push_back("SELECT * from sbtest1 where k = 2 and id = 2;");
        queries.push_back("select * from sbtest1 where k = 3 and c is not null;");*/
    }

    for(const char* query : queries) {
        pair<float, float> res = process_query(replicas[0], query);
        //printf("%f,%f\n", res.first, res.second);
    }
    return 0;
}