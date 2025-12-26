--4
select ssn,dno,salary
from employee
order by dno,salary;
--5
select dname
from department
where mgr_start_date>='1988-01-01' and mgr_start_date<='1988-12-31';
--6
select p.pname
from employee as e,works_on as w, project as p
where e.ssn=(select ssn from employee where fname='Joyce' and lname='English') and e.ssn=w.essn and p.pnumber=w.pno;
--7

--DEL
-- delete from works_on where essn='453453453';

--rollback; can retrieve deleted info
--truncate clears all cells in that table
--no ddl can be rolled back

--truncate and drop simlarity-> in both the entries vanish and cant be rolled back
--truncate and drop diff-> truncate leaves the table, drop deletes the table
--delete and trunc diff-> delete can be selective, truncate works on all cells, del can rollback

-- some commands: 
--select * from employee where dno in (1,4); for range based query
-- select * from employee where super_ssn IS NULL; 
-- select * from employee where super_ssn IS NOT NULL;
-- dont do = NULL since 'NULL'!=NULL in sql
-- special commands for null
-- select * from employee where salary between x and y;(can be used in between numeric columns and dates);
-- between is inclusive
-- select max(salary) from employee; works fro strings too(lexicographic order)

-- where lname LIKE 'S%'; (lname begins with s)
-- by default our database is case insensitive
-- sensitivity can be introduced by changing the encoding using collate
-- order by salary desc , orders by descending salary order
-- group by works on discrete values
-- select distinct dno from employee gives distinct dno
-- select dno,min(salary),max(salary)
-- -> from employee
-- -> group by dno;
-- always the group by attribute or aggregate functions only in the select statements


-- Q who has the highest salary?
-- A1-select fname,dno,salary
-- from employee
-- order by salary desc limit 1;
-- but 2 people can have same salary

-- A2-better solution
-- where salary = (select max(salary) from employee);


-- show unique project ids from works_on table:
-- select distinct pno from works_on;

-- show annual and monthly salary with appropriate headings
-- select fname, dno, salary 'Annual salary', salary/12 'Monthly salary' from employee;

--select *,salary/12 as 'Monthly salary' from employee order by 'Monthly salary'; doesnot work
-- use-> select *,salary/12 as monthly_salary from employee order by monthly_salary; (use a single word no quotes)

-- select count(super_ssn) from employee; count * selects all the tuples but if specified attribute it doesnot take count NULL attribs


--select dno,sex,avg(salary) from employee group by dno,sex; give av of dept of each gender
 
--see employees having greater salary than the avg of their department
-- select e1.fname,e1.dno,e1.salary from employee e1 where e1.salary >= (select avg(e2.salary) from employee e2 where e2.dno = e1.dno);
-- inner query works everytime for every tuple in e1


-- select dno which has the highest number of employees
-- COMMON TABLE EXPRESSION:(create virtual table) named query, table variable;
-- with dept_count as (select dno,count(ssn) dept_count from employee group by dno )
-- select dno,(dept_count) from dept_count where dept_count=(select max(d2.dept_count) from dept_count d2);


--SOME/ANY
--Q find staff whose salary is larger than salary of at least one member of staff at branch b003
-- select ssn,fname,lname,salary,dno from employee where salary>some(select salary from employee where dno=5); (SOME/ANY are synonymous)

--EXISTS
-- exists gives true if the subquery in it gives atleast one row or result
-- Q show employee who have atleast one dependent
-- select * from employee where exists(select * from dependent where essn=ssn);

--UNION(always in brackets)(ALWAYS SAME COLUMNS IN BOTH)
--union of dno1 and dno4

--EXCEPT is equivalent to minus



--COMMON TABLE EXPRESSIONS
