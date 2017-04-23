use DB_LIBALOT;

create user 'LibALotEngine'@'%' identified by 'other_password';

grant all privileges on * to 'LibALotEngine'@'%';

create user 'LibALotUser'@'%' identified by 'password';

grant select on `TB_MODULE1`            to 'LibALotUser'@'%';
grant select on `TB_MODULE2`            to 'LibALotUser'@'%';
grant select on `TB_JOB`                to 'LibALotUser'@'%';
grant select on `VW_JOB`                to 'LibALotUser'@'%';

flush privileges;
