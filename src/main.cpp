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
    
    
    for(int i=0;i<3;++i) {
        //queries.push_back("select * from sbtest1 as s1,sbtest2 as s2, sbtest3 as s3 where s1.k = s2.k + s3.k;");
        //query 2
        
        
        queries.push_back(" 		select  			min(ps_supplycost)  		from  			partsupp,  			supplier,  			nation,  			region  		where  50 = ps_partkey  			and s_suppkey = ps_suppkey  			and s_nationkey = n_nationkey  			and n_regionkey = r_regionkey  			and r_name = 'EUROPE';");
        
        queries.push_back("select 	s_acctbal, 	s_name, 	n_name, 	p_partkey, 	p_mfgr, 	s_address, 	s_phone, 	s_comment from 	part, 	supplier, 	partsupp, 	nation, 	region where         p_partkey = ps_partkey 	and s_suppkey = ps_suppkey 	and p_size = 15 	and p_type like '%BRASS' 	and s_nationkey = n_nationkey 	and n_regionkey = r_regionkey 	and r_name = 'EUROPE' 	 order by 	s_acctbal desc, 	n_name, 	s_name, 	p_partkey; ");
        queries.push_back("     select  	s_acctbal,  	s_name,  	n_name,  	p_partkey,  	p_mfgr,  	s_address,  	s_phone,  	s_comment  from  	part,  	supplier,  	partsupp,  	nation,  	region  where     p_partkey = ps_partkey  	and s_suppkey = ps_suppkey  	and p_size = 15  	and p_type like '%BRASS'  	and s_nationkey = n_nationkey  	and n_regionkey = r_regionkey  	and r_name = 'EUROPE'  	and ps_supplycost = (  		select  			min(ps_supplycost)  		from  			partsupp,  			supplier,  			nation,  			region  		where  p_partkey = ps_partkey  			and s_suppkey = ps_suppkey  			and s_nationkey = n_nationkey  			and n_regionkey = r_regionkey  			and r_name = 'EUROPE'  	)  order by  	s_acctbal desc,  	n_name,  	s_name,  	p_partkey;  ");
        
        //query 3 
        queries.push_back("select 	l_orderkey, 	sum(l_extendedprice * (1 - l_discount)) as revenue, 	o_orderdate, 	o_shippriority from 	customer, 	orders, 	lineitem where 	c_mktsegment = 'BUILDING' 	and c_custkey = o_custkey 	and l_orderkey = o_orderkey 	and o_orderdate < date '1995-03-15' 	and l_shipdate > date '1995-03-15' group by 	l_orderkey, 	o_orderdate, 	o_shippriority order by 	revenue desc, 	o_orderdate;");
        //query 5
        queries.push_back("select  	n_name,  	sum(l_extendedprice * (1 - l_discount)) as revenue  from  	customer,  	orders,  	lineitem,  	supplier,  	nation,  	region  where        c_custkey = o_custkey  	and l_orderkey = o_orderkey  	and l_suppkey = s_suppkey  	and c_nationkey = s_nationkey  	and s_nationkey = n_nationkey  	and n_regionkey = r_regionkey  	and r_name = 'ASIA'  	and o_orderdate >= date '1994-01-01'  	and o_orderdate < date '1994-01-01' + interval '1' year  group by  	n_name  order by  	revenue desc;");
        //6
        queries.push_back("select 	sum(l_extendedprice * l_discount) as revenue from 	lineitem where 	l_shipdate >= date '1994-01-01' 	and l_shipdate < date '1994-01-01' + interval '1' year 	and l_discount between .06 - 0.01 and .06 + 0.01 	and l_quantity < 24; ");
        
        //7
        queries.push_back("select 	supp_nation, 	cust_nation, 	l_year, 	sum(volume) as revenue from 	( 		select 			n1.n_name , 			n2.n_name, 			extract(year from l_shipdate) , 			l_extendedprice * (1 - l_discount) 		from 			supplier, 			lineitem, 			orders, 			customer, 			nation n1, 			nation n2 		where 			s_suppkey = l_suppkey 			and o_orderkey = l_orderkey 			and c_custkey = o_custkey 			and s_nationkey = n1.n_nationkey 			and c_nationkey = n2.n_nationkey 			and ( 				(n1.n_name = 'FRANCE' and n2.n_name = 'GERMANY') 				or (n1.n_name = 'GERMANY' and n2.n_name = 'FRANCE') 			) 			and l_shipdate between date '1995-01-01' and date '1996-12-31' 	)  group by 	supp_nation, 	cust_nation, 	l_year order by 	supp_nation, 	cust_nation, 	l_year;");

        //8
        queries.push_back("select 	o_year, 	sum(case 		when nation = 'BRAZIL' then volume 		else 0 	end) / sum(volume) as mkt_share from 	( 		select 			extract(year from o_orderdate) as o_year, 			l_extendedprice * (1 - l_discount) as volume, 			n2.n_name as nation 		from 			part, 			supplier, 			lineitem, 			orders, 			customer, 			nation n1, 			nation n2, 			region 		where 			p_partkey = l_partkey 			and s_suppkey = l_suppkey 			and l_orderkey = o_orderkey 			and o_custkey = c_custkey 			and c_nationkey = n1.n_nationkey 			and n1.n_regionkey = r_regionkey 			and r_name = 'AMERICA' 			and s_nationkey = n2.n_nationkey 			and o_orderdate between date '1995-01-01' and date '1996-12-31' 			and p_type = 'ECONOMY ANODIZED STEEL' 	)  group by 	o_year order by 	o_year;");

        //9
        queries.push_back("select 	nation, 	o_year, 	sum(amount) as sum_profit from 	( 		select 			n_name as nation, 			extract(year from o_orderdate) as o_year, 			l_extendedprice * (1 - l_discount) - ps_supplycost * l_quantity as amount 		from 			part, 			supplier, 			lineitem, 			partsupp, 			orders, 			nation 		where 			s_suppkey = l_suppkey 			and ps_suppkey = l_suppkey 			and ps_partkey = l_partkey 			and p_partkey = l_partkey 			and o_orderkey = l_orderkey 			and s_nationkey = n_nationkey 			and p_name like '%green%' 	) group by 	nation, 	o_year order by 	nation, 	o_year desc;");

        //10
        queries.push_back("select 	c_custkey, 	c_name, 	sum(l_extendedprice * (1 - l_discount)) as revenue, 	c_acctbal, 	n_name, 	c_address, 	c_phone, 	c_comment from 	customer, 	orders, 	lineitem, 	nation where    c_custkey = o_custkey 	and l_orderkey = o_orderkey 	and o_orderdate >= date '1993-10-01' 	and o_orderdate < date '1993-10-01' + interval '3' month 	and l_returnflag = 'R' 	and c_nationkey = n_nationkey group by 	c_custkey, 	c_name, 	c_acctbal, 	c_phone, 	n_name, 	c_address, 	c_comment order by 	revenue desc;");
        
        //11
        queries.push_back("select 	ps_partkey, 	sum(ps_supplycost * ps_availqty) as value from 	partsupp, 	supplier, 	nation where 	ps_suppkey = s_suppkey 	and s_nationkey = n_nationkey 	and n_name = 'GERMANY' group by 	ps_partkey having 		sum(ps_supplycost * ps_availqty) > ( 			select 				sum(ps_supplycost * ps_availqty) * 0.0001000000 			from 				partsupp, 				supplier, 				nation 			where 				ps_suppkey = s_suppkey 				and s_nationkey = n_nationkey 				and n_name = 'GERMANY' 		) order by 	value desc;");
        queries.push_back("select sum(ps_supplycost * ps_availqty) * 0.0001000000 from partsupp, supplier, nation where ps_suppkey = s_suppkey and s_nationkey = n_nationkey and n_name = 'GERMANY';");
        

        //12
        queries.push_back("select 	l_shipmode, 	sum(case 		when o_orderpriority = '1-URGENT' 			or o_orderpriority = '2-HIGH' 			then 1 		else 0 	end) as high_line_count, 	sum(case 		when o_orderpriority <> '1-URGENT' 			and o_orderpriority <> '2-HIGH' 			then 1 		else 0 	end) as low_line_count from 	orders, 	lineitem where 	o_orderkey = l_orderkey 	and l_shipmode in ('MAIL', 'SHIP') 	and l_commitdate < l_receiptdate 	and l_shipdate < l_commitdate 	and l_receiptdate >= date '1994-01-01' 	and l_receiptdate < date '1994-01-01' + interval '1' year group by 	l_shipmode order by 	l_shipmode;");

        //13
        //queries.push_back("select  	c_count,  	count(*) as custdist  from  	(  		select  			c_custkey,  			count(o_orderkey)  		from  			customer left outer join orders on  				c_custkey = o_custkey  				and o_comment not like '%special%requests%'  		group by  			c_custkey  	) as c_orders (c_custkey, c_count)  group by  	c_count  order by  	custdist desc,  	c_count desc;");
        
        //14
        queries.push_back("select 	100.00 * sum(case 		when p_type like 'PROMO%' 			then l_extendedprice * (1 - l_discount) 		else 0 	end) / sum(l_extendedprice * (1 - l_discount)) as promo_revenue from 	lineitem, 	part where 	l_partkey = p_partkey 	and l_shipdate >= date '1995-09-01' 	and l_shipdate < date '1995-09-01' + interval '1' month;");
        
        //15
        // view queries.push_back("");

        //16
        // in condition
        //queries.push_back("  select 	p_brand, 	p_type, 	p_size, 	count(distinct ps_suppkey) as supplier_cnt from 	partsupp, 	part where 	p_partkey = ps_partkey 	and p_brand <> 'Brand#45' 	and p_type not like 'MEDIUM POLISHED%' 	and p_size in (49, 14, 23, 45, 19, 3, 36, 9) 	and ps_suppkey not in ( 		select 			s_suppkey 		from 			supplier 		where 			s_comment like '%Customer%Complaints%' 	) group by 	p_brand, 	p_type, 	p_size order by 	supplier_cnt desc, 	p_brand, 	p_type, 	p_size;");

        //17
        //root is null
        queries.push_back("  select 	sum(l_extendedprice) / 7.0 as avg_yearly from 	lineitem, 	part where 	p_partkey = l_partkey 	and p_brand = 'Brand#23' 	and p_container = 'MED BOX' 	and l_quantity < ( 		select 			0.2 * avg(l_quantity) 		from 			lineitem 		where 			l_partkey = p_partkey 	);");
        
        //18
        queries.push_back("select 	c_name, 	c_custkey, 	o_orderkey, 	o_orderdate, 	o_totalprice, 	sum(l_quantity) from 	customer, 	orders, 	lineitem where 	o_orderkey in ( 		select 			l_orderkey 		from 			lineitem 		group by 			l_orderkey having 				sum(l_quantity) > 300 	) 	and c_custkey = o_custkey 	and o_orderkey = l_orderkey group by 	c_name, 	c_custkey, 	o_orderkey, 	o_orderdate, 	o_totalprice order by 	o_totalprice desc, 	o_orderdate;");
        
        //19
        queries.push_back("select 	sum(l_extendedprice* (1 - l_discount)) as revenue from 	lineitem, 	part where 	( 		p_partkey = l_partkey 		and p_brand = 'Brand#12' 		and p_container in ('SM CASE', 'SM BOX', 'SM PACK', 'SM PKG') 		and l_quantity >= 1 and l_quantity <= 1 + 10 		and p_size between 1 and 5 		and l_shipmode in ('AIR', 'AIR REG') 		and l_shipinstruct = 'DELIVER IN PERSON' 	) 	or 	( 		p_partkey = l_partkey 		and p_brand = 'Brand#23' 		and p_container in ('MED BAG', 'MED BOX', 'MED PKG', 'MED PACK') 		and l_quantity >= 10 and l_quantity <= 10 + 10 		and p_size between 1 and 10 		and l_shipmode in ('AIR', 'AIR REG') 		and l_shipinstruct = 'DELIVER IN PERSON' 	) 	or 	( 		p_partkey = l_partkey 		and p_brand = 'Brand#34' 		and p_container in ('LG CASE', 'LG BOX', 'LG PACK', 'LG PKG') 		and l_quantity >= 20 and l_quantity <= 20 + 10 		and p_size between 1 and 15 		and l_shipmode in ('AIR', 'AIR REG') 		and l_shipinstruct = 'DELIVER IN PERSON' 	);");
        
        //20
        queries.push_back("select 	s_name, 	s_address from 	supplier, 	nation where 	s_suppkey in ( 		select 			ps_suppkey 		from 			partsupp 		where 			ps_partkey in ( 				select 					p_partkey 				from 					part 				where 					p_name like 'forest%' 			) 			and ps_availqty > ( 				select 					0.5 * sum(l_quantity) 				from 					lineitem 				where 					l_partkey = ps_partkey 					and l_suppkey = ps_suppkey 					and l_shipdate >= date '1994-01-01' 					and l_shipdate < date '1994-01-01' + interval '1' year 			) 	) 	and s_nationkey = n_nationkey 	and n_name = 'CANADA' order by 	s_name;");

        //21
        //queries.push_back("select 	s_name, 	count(*) as numwait from 	supplier, 	lineitem l1, 	orders, 	nation where 	s_suppkey = l1.l_suppkey 	and o_orderkey = l1.l_orderkey 	and o_orderstatus = 'F' 	and l1.l_receiptdate > l1.l_commitdate 	and exists ( 		select 			* 		from 			lineitem l2 		where 			l2.l_orderkey = l1.l_orderkey 			and l2.l_suppkey <> l1.l_suppkey 	) 	and not exists ( 		select 			* 		from 			lineitem l3 		where 			l3.l_orderkey = l1.l_orderkey 			and l3.l_suppkey <> l1.l_suppkey 			and l3.l_receiptdate > l3.l_commitdate 	) 	and s_nationkey = n_nationkey 	and n_name = 'SAUDI ARABIA' group by 	s_name order by 	numwait desc, 	s_name; ");
    

        //queries.push_back("select count(*) from region, nation where r_regionkey = n_regionkey;");
        
        //1
        //queries.push_back("select  	l_returnflag,  	l_linestatus,  	sum(l_quantity) as sum_qty,  	sum(l_extendedprice) as sum_base_price,  	sum(l_extendedprice * (1 - l_discount)) as sum_disc_price,  	sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge,  	avg(l_quantity) as avg_qty,  	avg(l_extendedprice) as avg_price,  	avg(l_discount) as avg_disc,  	count(*) as count_order  from  	lineitem  where  	l_shipdate <= date '1998-12-01' - interval '90' day (3)  group by  	l_returnflag,  	l_linestatus  order by  	l_returnflag, 	l_linestatus;");

        //6
        //queries.push_back("select 	sum(l_extendedprice * l_discount) as revenue from 	lineitem where 	l_shipdate >= date '1994-01-01' 	and l_shipdate < date '1994-01-01' + interval '1' year 	and l_discount between .06 - 0.01 and .06 + 0.01 	and l_quantity < 24; ");
        /*queries.push_back("SELECT * FROM sbtest1;");
        queries.push_back("SELECT * from sbtest1 where k = 2;");
        queries.push_back("SELECT * from sbtest1 where k < 2;");
        queries.push_back("SELECT * from sbtest1 where k = 2 and id = 2;");
        queries.push_back("select * from sbtest1 where k = 3 and c is not null;");*/
    }
    vector<pair<float, long long int>> results;
    for(const char* query : queries) {
        pair<float, long long int> res = process_query(replicas[0], query);
        results.push_back(res);
    }
    for(auto par : results) {
        printf("%f, %lld\n", par.first / 1000.0, par.second/1000);
    }
    return 0;
}