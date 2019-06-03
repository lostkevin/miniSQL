##表达式匹配及句法分析模块

###以下命令必须支持:
+create table tablename ( columnname type, etc., primary key (columnname));
+drop table tablename;
+create index indexname on tablename ( columnname );
+drop index indexname;
+select * from tablename [where conditions], conditions satisfy columnname op value and etc. op: = <> < > <= >=
+insert into tablename values (keys set);
+delete from tablename where condition;
+quit;
+execfile filename;


###句法结构
语句以';'结尾，中间有任意个' ', '\n'符号，表名、列名可能有``，值可能有''


###支持的token列表(由支持的命令归纳)
create
quit
execfile
drop
select
insert

table
index
primary key

where
from
and
on
into

( ) ; '' `` < <= > >= = <>(!=) *
