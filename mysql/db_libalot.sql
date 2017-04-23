use DB_LIBALOT;

/*****
Drop every trigger
******/
drop trigger if exists TG_JOB_INSERT;
drop trigger if exists TG_JOB_UPDATE;
drop trigger if exists TG_JOB_DELETE;

/*****
Drop every view
******/
drop view if exists VW_JOB;

/*****
Drop every table
******/
drop table if exists TB_MODULE1;

drop table if exists TB_MODULE2;

drop table if exists TB_JOB_LOG;
drop table if exists TB_JOB;

/*****
The table of jobs
******/
create table TB_JOB (
	JOB_ID int(10) unsigned not null auto_increment,

	JOB_UUID binary(16) not null,

	JOB_CREATION timestamp not null default current_timestamp,

	primary key (JOB_ID),

	unique (JOB_UUID)
);

create table TB_JOB_LOG (
	JOB_LOG_ID int(10) unsigned not null auto_increment,
	JOB_LOG_OPERATION enum('INSERT','UPDATE','DELETE'),
	JOB_LOG_DATE timestamp not null default current_timestamp,

	JOB_LOG_JOB_ID int(10) unsigned not null,

	JOB_LOG_OLD_UUID binary(16) null,

	JOB_LOG_NEW_UUID binary(16) null,

	primary key (JOB_LOG_ID)
);

create trigger TG_JOB_INSERT after insert on TB_JOB
	for each row
		insert into TB_JOB_LOG (
			JOB_LOG_OPERATION,
			JOB_LOG_JOB_ID,
			JOB_LOG_OLD_UUID,
			JOB_LOG_NEW_UUID
		) values (
			'INSERT',
			new.JOB_ID,
			null,
			new.JOB_UUID
		);

create trigger TG_JOB_UPDATE after update on TB_JOB
	for each row
		insert into TB_JOB_LOG (
			JOB_LOG_OPERATION,
			JOB_LOG_JOB_ID,
			JOB_LOG_OLD_UUID,
			JOB_LOG_NEW_UUID
		) values (
			'UPDATE',
			new.JOB_ID,
			old.JOB_UUID,
			new.JOB_UUID
		);

create trigger TG_JOB_DELETE before delete on TB_JOB
	for each row
		insert into TB_JOB_LOG (
			JOB_LOG_OPERATION,
			JOB_LOG_JOB_ID,
			JOB_LOG_OLD_UUID,
			JOB_LOG_NEW_UUID
		) values (
			'DELETE',
			old.JOB_ID,
			old.JOB_UUID,
			null
		);

/*****
The table storing the results from Module1
*****/
create table TB_MODULE1(
	MODULE1_ID int(10) unsigned not null auto_increment,

	MODULE1_JOB_ID int(10) unsigned not null,

	MODULE1_RESULT_A text,
	MODULE1_RESULT_B double not null,

	MODULE1_CREATION timestamp not null default current_timestamp,

	primary key (MODULE1_ID),

	key KY_MODULE1_JOB_ID (MODULE1_JOB_ID),
	constraint FK_MODULE1_JOB_ID foreign key (MODULE1_JOB_ID) references TB_JOB (JOB_ID) on delete no action on update no action
);

/*****
The table storing the results from Module2
*****/
create table TB_MODULE2(
	MODULE2_ID int(10) unsigned not null auto_increment,

	MODULE2_JOB_ID int(10) unsigned not null,

	MODULE2_RESULT_A varchar(256),
	MODULE2_RESULT_B text,

	MODULE2_CREATION timestamp not null default current_timestamp,

	primary key (MODULE2_ID),

	key KY_MODULE2_JOB_ID (MODULE2_JOB_ID),
	constraint FK_MODULE2_JOB_ID foreign key (MODULE2_JOB_ID) references TB_JOB (JOB_ID) on delete no action on update no action
);

/*****
Viewing the details and results relating to the jobs
*****/
create view VW_JOB as
	select

		TB_JOB.JOB_ID as JOB_ID,
		hex( TB_JOB.JOB_UUID ) as JOB_UUID,
		TB_JOB.JOB_CREATION as JOB_CREATION,

/*****
joining with the results from Module1
******/
		TB_MODULE1.MODULE1_ID as MODULE1_ID,

		TB_MODULE1.MODULE1_RESULT_A as MODULE1_RESULT_A,
		TB_MODULE1.MODULE1_RESULT_B as MODULE1_RESULT_B,

		TB_MODULE1.MODULE1_CREATION as MODULE1_CREATION,

/*****
joining with the results from Module2
******/
		TB_MODULE2.MODULE2_ID as MODULE2_ID,

		TB_MODULE2.MODULE2_RESULT_A as MODULE2_RESULT_A,
		TB_MODULE2.MODULE2_RESULT_B as MODULE2_RESULT_B,

		TB_MODULE2.MODULE2_CREATION as MODULE2_CREATION

	from TB_JOB
		left join TB_MODULE1 on TB_JOB.JOB_ID = TB_MODULE1.MODULE1_JOB_ID
		left join TB_MODULE2 on TB_JOB.JOB_ID = TB_MODULE2.MODULE2_JOB_ID
;
