//STEP 1. Import required packages


import java.sql.*;
import java.util.Scanner;

public class Demo {

   // Set JDBC driver name and database URL
   static final String JDBC_DRIVER = "com.mysql.cj.jdbc.Driver";
   static final String DB_URL = "jdbc:mysql://localhost:3306/companyDB";

   // Database credentials
   static final String USER = "root";// add your user
   static final String PASSWORD = "3password6";// add password

   public static void main(String[] args) {
      Connection conn = null;
      PreparedStatement stmt = null;
      PreparedStatement insert_employee = null;
      PreparedStatement insertWorksOn = null;
      Scanner scanner = new Scanner(System.in);
      // STEP 2. Connecting to the Database
      try {
         // STEP 2a: Register JDBC driver
         Class.forName(JDBC_DRIVER);
         // STEP 2b: Open a connection
         System.out.println("Connecting to database...");
         conn = DriverManager.getConnection(DB_URL, USER, PASSWORD);
         // STEP 2c: Execute a query
         System.out.println("Creating statement...");
         //auto commit off
         conn.setAutoCommit(false);
         String ssn;
         String inputString;
         int inputInt;
         // INSERT, UPDATE, DELETE
         String query = "INSERT INTO employee values (?,?,?,?,?,?,?,?,?,?)";
         insert_employee = conn.prepareStatement(query);
         System.out.print("Enter ID: ");
         ssn = scanner.next();
         insert_employee.setString(1, ssn);
         System.out.print("Enter first name: ");
         inputString = scanner.next();
         insert_employee.setString(2, inputString);
         System.out.print("Enter middle name: ");
         inputString = scanner.next();
         insert_employee.setString(3, inputString);
         System.out.print("Enter last name: ");
         inputString = scanner.next();
         insert_employee.setString(4, inputString);
         System.out.print("Enter birth date: ");
         inputString = scanner.next();
         insert_employee.setString(5, inputString);
         scanner.nextLine();
         System.out.print("Enter address: ");
         inputString = scanner.nextLine();
         insert_employee.setString(6, inputString);
         System.out.print("Enter gender: ");
         inputString = scanner.next();
         insert_employee.setString(7, inputString);
         System.out.print("Enter salary: ");
         inputInt = scanner.nextInt();
         insert_employee.setInt(8, inputInt);
         System.out.print("Enter supervisor ID: ");
         inputString = scanner.next();
         insert_employee.setString(9, inputString);
         System.out.print("Enter Department number: ");
         inputInt = scanner.nextInt();
         insert_employee.setInt(10, inputInt);
         int rowsInserted = insert_employee.executeUpdate();
         if (rowsInserted ==1) {
            System.out.println("Employee inserted successfully.");
         }


         //Insert into works_on

         System.out.print("Enter Project Number (pno): ");
         int pno = scanner.nextInt();

         System.out.print("Enter Hours to assign: ");
         float hours = scanner.nextFloat();

         String insertQuery = "INSERT INTO works_on (essn, pno, hours) VALUES (?, ?, ?)";
         insertWorksOn = conn.prepareStatement(insertQuery);
         insertWorksOn.setString(1, ssn);
         insertWorksOn.setInt(2, pno);
         insertWorksOn.setFloat(3, hours);

         int rows = insertWorksOn.executeUpdate();

         if (rows == 1) {
               System.out.println("Works_on table updated successfully.");
         } else {
               conn.rollback();
               System.out.println("Assignment failed. Changes rolled back.");
         }
         if(rows==1 && rowsInserted==1 && hours > 0){//if both successful
            conn.commit();
            System.out.println("Assignment successful and committed.");
         }
         else{
            System.out.println("Assignment failed due to hours<=0. Changes rolled back.");
            conn.rollback();
         }

         // STEP 3: Query to database
         PreparedStatement showNames = conn.prepareStatement("SELECT fname, lname FROM employee");
         ResultSet rs = showNames.executeQuery();

         System.out.println("\n--- Employee Names ---");
         while (rs.next()) {
            String fname = rs.getString("fname");
            String lname = rs.getString("lname");
            System.out.println("First Name: " + fname + ", Last Name: " + lname);
         }
         
        /*String query = """
            SELECT e.fname, e.lname, d.essn, d.dependent_name, d.relationship
            FROM employee e
            JOIN dependent d ON e.ssn = d.essn
            ORDER BY e.ssn, d.dependent_name
         """;

         stmt = conn.prepareStatement(query);
         ResultSet rs = stmt.executeQuery();

         String currentEssn = "";
         int depCount = 0;
         int ssnCount = 1 ;

         while (rs.next()) {
            String essn = rs.getString("essn");
            String fname = rs.getString("fname");
            String lname = rs.getString("lname");
            String depName = rs.getString("dependent_name");
            String relation = rs.getString("relationship");

            if (!essn.equals(currentEssn)) {
               if (!currentEssn.equals("")) {
                     System.out.println("* * * *");
               }
               System.out.println("Employee#"+ssnCount+" full name: " + fname + " " + lname);
               depCount = 1;
               currentEssn = essn;
               ssnCount++;
            }

            System.out.println("Dependent #" + depCount + " : " + depName + " (" + relation + ")");
            depCount++;
         }
         System.out.println("* * * *");*/
         // STEP 4: Clean-up environment
         rs.close();
         //stmt.close();
         conn.close();
      } catch (SQLException se) { // Handle errors for JDBC
         se.printStackTrace();
      } catch (Exception e) { // Handle errors for Class.forName
         e.printStackTrace();
      } finally { // finally block used to close resources regardless of whether an exception was
                  // thrown or not
         try {
            if (stmt != null)
               stmt.close();
         } catch (SQLException se2) {
         }
         try {
            if (conn != null)
               conn.close();
         } catch (SQLException se) {
            se.printStackTrace();
         } // end finally try
      } // end try
      System.out.println("End of Code");
   } // end main
} // end class

// Note : By default autocommit is on. you can set to false using
// con.setAutoCommit(false)
