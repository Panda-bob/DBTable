# DBTable

## Function
* export the database table object ,this class object with function : CreateTable , Insert , Delete ,Select,SelectAll, 
* Update ,Replace and so on .
* Don't need write sql like this :  " create table IF NOT EXISTS tab_collect(`collect_id` bigint(64) NOT NULL  ,
* `collect_infos` blob NOT NULL  , PRIMARY KEY(`collect_id`)) ENGINE=INNODB DEFAULT CHARSET=utf8 ;" by yourself.
* 

## Compile
###  Unix-like Operating Systems
* this build with depend lib libmysqlclient.so , you need install mysql first.
	```
	mkdir build
	cd build && cmake ../
	make
	```

## Use DBTable
* mysql you need install , and before initialization mysql ,you should set mysql variable " lower_case_table_names = 1 " 
* in mysql config my.cnf .
* start mysql server and create the database and table that you will export .
	```
	mkdir files
	./frame/dbtable mysql_host_ip 3306 database_name mysql_user mysql_password ./files/
	```
* you can find the export table object(.cpp .h file) in the ./files/  
