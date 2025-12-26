--1
select distinct lname from employee e inner join dependent d on e.ssn=d.essn where d.sex='M' order by lname asc;
--2
select sum(hours),w.pno as 'pno' from works_on w inner join project p on w.pno=p.pnumber inner join department d on p.dnum=d.dnumber where d.dname='Research' GROUP BY w.pno order by w.pno asc;
--3
select year(current_date())-year(bdate) as age from employee order by age asc;
--4 
select essn,sum(hours) from works_on group by essn having avg(hours)>=10 order by essn asc;
--5
select fname from employee e1 where salary>(select avg(salary) from employee e2 where e1.dno=e2.dno) order by fname;
--6
select fname from employee where fname like '%s%' order by fname;
--7
select fname,lname,salary from employee where salary>=25000 order by lname;
--8
select distinct plocation from project order by plocation;
--9
select fname,minit,lname,ssn,bdate,address,sex,salary,super_ssn,dno from employee where sex='M' and bdate>'1950-01-01' order by salary asc;
--10
select fname,lname,bdate,salary from employee where super_ssn is NULL or salary<30000 order by bdate desc;
--11
select fname,minit,lname,ssn,bdate,address,sex,salary,super_ssn,dno from employee where salary between 30000 and 50000 and sex='F' order by dno;
--12
select distinct fname from employee left join dependent on ssn=essn where essn is null order by fname;
--13
select min(salary) from employee inner join department on dno=dnumber where dname='Research';
--14
select fname as 'First' from employee order by fname;
--15
select sex,sum(salary) from employee group by sex;
--16
select count(ssn) as 'count(*)' from employee  where dno=5 group by dno;
--17
/*Show the ssn and total number of hours worked on projects for all employees order by ssn.

Hint: Note that even if SSN is not working in any project, it is there in the output with NULL value

-----------+------------+
| ssn       | sum(hours) |
+-----------+------------+
| 123456789 |      40.00 |
| 333445555 |      40.00 |
| 453453453 |      40.00 |
| 666884444 |      40.00 |
| 888665555 |       NULL |
| 987654321 |      35.00 |
| 987987987 |      40.00 |
| 999887777 |      40.00 |
+-----------+------------+
8 rows in set (0.00 sec)*/

/*
Great question! Since you want all employees to be shown, even those not working on any project, this calls for a LEFT JOIN — from the employee table to the works_on table.

✅ Correct SQL Query:
sql
Copy
Edit
SELECT e.ssn, SUM(w.hours) AS total_hours
FROM employee e
LEFT JOIN works_on w ON e.ssn = w.essn
GROUP BY e.ssn
ORDER BY e.ssn;
🧠 Explanation:
LEFT JOIN ensures all employees are included, even if they have no matching records in works_on

SUM(w.hours) returns NULL if no hours exist (which is expected!)

GROUP BY e.ssn groups hours per employee

ORDER BY e.ssn sorts the results as requested

🧾 Sample Output:
ssn	total_hours
123456789	40.00
333445555	40.00
453453453	40.00
666884444	40.00
888665555	NULL
987654321	35.00
987987987	40.00
999887777	40.00
Let me know if you'd like to replace NULL with 0, or include employee names too!








*/
select e.ssn as ssn, sum(w.hours) from employee e left join works_on w on e.ssn=w.essn group by e.ssn order by e.ssn;

--18
select min(salary) from employee inner join department on dno=dnumber where dname='Administration' ;
--19
select dno,count(ssn) from employee where salary>30000 group by dno  order by dno;
--20
select essn from employee inner join dependent on ssn=essn group by essn having count(essn)>1 order by ssn;
--21
select essn,count(pno) from works_on group by essn having count(pno)>1 order by essn;
--22
select ssn,fname,dname from employee inner join department on dno=dnumber order by ssn;
--23
select ssn,fname from employee inner join dept_locations  on dno=dnumber where dlocation ='Houston' order by ssn;
--24 
select distinct lname from employee inner join dependent on ssn=essn where dependent.sex='F' order by lname;
