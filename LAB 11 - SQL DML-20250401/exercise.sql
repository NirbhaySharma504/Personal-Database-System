-- 1
insert into employee(ssn,fname,lname,bdate,address,sex,salary,super_ssn,dno) values
('123456','Jane','Smith','1969-03-03','454 Fondren, Houston, TX','F',28000,NULL,5);
insert into dept_locations(dnumber,dlocation) values 
(5,'MG Road');
insert into project(Pname,Pnumber,Plocation, Dnum) values 
('NextGen Computing',4,'MG Road',5);
insert into works_on(essn,pno,hours) values
('123456',4,20);
insert into dependent(essn,dependent_name,sex,bdate,relationship) values 
('123456','Jake','M','1995-05-01','Son');
update employee
set super_ssn = '333445555'
where ssn='123456';
-- 2
insert into department(Dname,Dnumber,mgr_ssn,mgr_start_date) values 
('Finance',3,'66688444','1989-05-01');
insert into dept_locations(dnumber,dlocation) values
(3,'Electronics City');
insert into employee(ssn,fname,lname,bdate,address,sex,salary,super_ssn,dno) values 
('3343443','Peter','Pan','1968-05-30','343 Fondren, Houston, TX','M',29000,'666884444',3);
insert into project(pname,pnumber,plocation,dnum) values 
('Smart Card Implementation', 5 , 'Electronics City',3);
insert into works_on(essn,pno,hours) values ('3343443',5,30);
--3
insert into department(dname,dnumber,mgr_ssn,mgr_start_date) values
('IT Department',2,'453453453','1989-06-01');
insert into dept_locations(dnumber,dlocation) values (2,'Jayanagar');
insert into employee(ssn,fname,lname,bdate,address,sex,salary,super_ssn,dno) values
('97868565','Simon','Tar','1967-04-01','322 Electronics City, Blr','M',27500,'453453453',2);
insert into project(pname,pnumber,plocation,dnum) values ('HR Automation',6,'Jayanagar',2);
insert into works_on(essn,pno,hours) values ('97868565',6,40);
insert into dependent(essn,dependent_name,sex,bdate,relationship) values
('97868565','Jay','M','1988-11-30','Son'),
('97868565','Tina','F','1989-12-30','Daughter');




