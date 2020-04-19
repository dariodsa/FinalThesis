## Psql

### /var/lib/pgsql/data/pg_hba.conf
```
# "local" is for Unix domain socket connections only
local   all             all                                     password
# IPv4 local connections:
host    all             all             127.0.0.1/32            password
# IPv6 local connections:
host    all             all             ::1/128                 password
```

## TPC-H

[http://myfpgablog.blogspot.com/2016/08/tpc-h-queries-on-postgresql.html](Link)   
https://ankane.org/tpc-h



docker pull postgres

docker network create --gateway 192.50.0.1 --subnet 192.50.0.0/24 thesis

docker run -e POSTGRES_PASSWORD=12345 -e POSTGRES_USER=postgres -e POSTGRES_DB=tpch --net thesis -d postgres
