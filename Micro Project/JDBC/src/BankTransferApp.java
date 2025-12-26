//STEP 1. Import required packages


import java.sql.*;
import java.util.Scanner;
import java.util.Date;
import java.text.SimpleDateFormat;
public class BankTransferApp {

   // Set JDBC driver name and database URL
   static final String JDBC_DRIVER = "com.mysql.cj.jdbc.Driver";
   static final String DB_URL = "jdbc:mysql://localhost:3306/banktransferDB";

   // Database credentials
   static final String USER = "root";// add your user
   static final String PASSWORD = "3password6";// add password

   public static void main(String[] args) {
      Connection conn = null;
      Statement stmt = null;
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
         stmt = conn.createStatement();
         conn.setAutoCommit(false);
         System.out.println("Connected to bank system");
         //process scheduled transfers
         System.out.println("Performing scheduled transfers if any...");
         processScheduledTransfers(conn);

         while(true){
            System.out.println("Choose an option:");
            System.out.println("1. Transfer Funds");
            System.out.println("2. Schedule Transfer");
            System.out.println("3. Cancel Sechduled Transfer");
            System.out.println("4. Admin View");
            System.out.println("5. Exit");
            System.out.print("Enter choice: ");
            int choice = scanner.nextInt();
            if (choice == 1) {
               System.out.print("Enter your user ID: ");
               int userId = scanner.nextInt();
               System.out.print("Enter source account number: ");
               int sourceAccount = scanner.nextInt();
               System.out.print("Enter destination account number: ");
               int destinationAccount = scanner.nextInt();
               System.out.print("Enter amount to transfer: ");
               int amount = scanner.nextInt();
               if(!validate(conn, userId, sourceAccount, destinationAccount, amount)){
                  conn.rollback();
                  System.out.println("Transfer failed for id "+userId+". Rolled back changes.");
                  continue;
               }
               else if (performTransfer(conn, userId, sourceAccount, destinationAccount, amount)) {
                   conn.commit();
                   System.out.println("Transfer completed successfully!");
               } else {
                   conn.rollback();
                   System.out.println("Transfer failed for id "+userId+" Rolled back changes.");
               }
            }
            else if(choice==2){
               System.out.print("Enter your user ID: ");
               int userId = scanner.nextInt();
               System.out.print("Enter source account number: ");
               int sourceAccount = scanner.nextInt();
               System.out.print("Enter destination account number: ");
               int destinationAccount = scanner.nextInt();
               System.out.print("Enter amount to transfer: ");
               int amount = scanner.nextInt();
               scanner.nextLine();
               System.out.print("Enter scheduled date (yyyy-mm-dd): ");
               String scheduledForStr = scanner.nextLine();
               SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd");
               Date scheduledFor = formatter.parse(scheduledForStr);
               Date now = new Date();
               if(!scheduledFor.after(now)){
                  conn.rollback();
                  System.out.println("Schedules date is invalid, transaction failed.");
                  continue;
               }
               if(!validate(conn, userId, sourceAccount, destinationAccount, amount)){
                  conn.rollback(); 
                  System.out.println("Schedule transfer failed for id "+userId+". Rolled back changes.");
                  continue;
               }
               if (scheduleTransfer(conn, userId, sourceAccount, destinationAccount, amount, scheduledFor)) {
                  conn.commit();
                  System.out.println("Transfer scheduled successfully!");
               } else {
                  conn.rollback();
                  System.out.println("Failed to schedule transfer for "+userId +" Rolled back changes.");
               }
            }
            else if(choice==3){  
               System.out.print("Enter user ID: ");
               int userId = scanner.nextInt();
               System.out.print("Enter scheduled ID: ");
               int scheduledID = scanner.nextInt();
               cancelScheduledTransfer(conn, scheduledID, userId);
            }
            else if(choice==4){
               adminView(conn);
            }
            else if(choice==5){
               System.out.println("Thank you.");
               return;
            }
            else{
               System.out.println("Wrong input entered. Try again.");
               continue;
            }
         }
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
   }
   public static boolean validate(Connection conn, int userId, int source, int destination, int amount){
      PreparedStatement stmtCheckBalance = null;
      PreparedStatement stmtVerifyUser = null;
      ResultSet rs = null;
      try{
         // 1. Check if source has enough balance
         String sqlCheck = "SELECT balance FROM Accounts WHERE account_number = ?";
         stmtCheckBalance = conn.prepareStatement(sqlCheck);
         stmtCheckBalance.setInt(1, source);
         rs = stmtCheckBalance.executeQuery();

         if (!rs.next()) {
            System.out.println("Source account not found.");
            return false;
         }

         int balance = rs.getInt("balance");
         if (balance < amount) {
            System.out.println("Insufficient funds in source account.");
            return false;
         }
         
         // 2. Check if the userId owns the source account
         String sqlUserVerify = "SELECT user_id FROM Accounts WHERE account_number = ?";
         stmtVerifyUser = conn.prepareStatement(sqlUserVerify);
         stmtVerifyUser.setInt(1, source);
         rs = stmtVerifyUser.executeQuery();

         if (rs.next()) {
            int id = rs.getInt("user_id");
            if (userId != id) {
               System.out.println("Accessing account of different user.");
               return false;
            }
         } else {
            System.out.println("Source account does not exist.");
            return false;
         }

         // 3. validate destination id
         String destinationCheck = "SELECT account_number FROM Accounts WHERE account_number = ?";
         stmtVerifyUser = conn.prepareStatement(destinationCheck);
         stmtVerifyUser.setInt(1, destination);
         rs = stmtVerifyUser.executeQuery();

         if (!rs.next()) {
            System.out.println("Destination account not found.");
            return false;
         }
         return true;
      }catch (SQLException se) {
         se.printStackTrace();
         return false;
      } finally {
         //closing the resources
         try { if (rs != null) rs.close(); } catch (SQLException e) {}
         try { if (stmtCheckBalance != null) stmtCheckBalance.close(); } catch (SQLException e) {}
         try { if (stmtVerifyUser != null) stmtVerifyUser.close(); } catch (SQLException e) {}
      }
      
   }
   public static boolean performTransfer(Connection conn, int userId, int source, int destination, int amount) {
      
      PreparedStatement stmtUpdateSource = null;
      PreparedStatement stmtUpdateDest = null;
      PreparedStatement stmtInsertTransfer = null;
      ResultSet rs = null;

      try {
         // 1.Deduct from source
         String sqlUpdateSource = "UPDATE Accounts SET balance = balance - ? WHERE account_number = ?";
         stmtUpdateSource = conn.prepareStatement(sqlUpdateSource);
         stmtUpdateSource.setInt(1, amount);
         stmtUpdateSource.setInt(2, source);
         stmtUpdateSource.executeUpdate();

         // 2. Add to destination
         String sqlUpdateDest = "UPDATE Accounts SET balance = balance + ? WHERE account_number = ?";
         stmtUpdateDest = conn.prepareStatement(sqlUpdateDest);
         stmtUpdateDest.setInt(1, amount);
         stmtUpdateDest.setInt(2, destination);
         int rowsUpdated = stmtUpdateDest.executeUpdate();
         if(rowsUpdated<1){
            System.out.println("Error occured, transfer cancelled.");
            return false;
         }
         // 3. Insert into Transfers
         String sqlInsert = "INSERT INTO Transfers ( user_id, source_account_id, destination_account_id, amount) " +
                           "VALUES (?, ?, ?, ?)";
         stmtInsertTransfer = conn.prepareStatement(sqlInsert);
         stmtInsertTransfer.setInt(1, userId);
         stmtInsertTransfer.setInt(2, source);
         stmtInsertTransfer.setInt(3, destination);
         stmtInsertTransfer.setInt(4, amount);
         stmtInsertTransfer.executeUpdate();

         return true;

      } catch (SQLException se) {
         se.printStackTrace();
         return false;
      } finally {
         //closing the resources
         try { if (rs != null) rs.close(); } catch (SQLException e) {}
         try { if (stmtUpdateSource != null) stmtUpdateSource.close(); } catch (SQLException e) {}
         try { if (stmtUpdateDest != null) stmtUpdateDest.close(); } catch (SQLException e) {}
         try { if (stmtInsertTransfer != null) stmtInsertTransfer.close(); } catch (SQLException e) {}
      }
   }
   public static boolean scheduleTransfer(Connection conn, int userId, int source, int destination, int amount, Date scheduledFor) {
      PreparedStatement stmtInsertScheduled = null;
  
      try {
          // Insert into Scheduled_Transfers table
          String sqlInsert = "INSERT INTO Scheduled_Transfers " +
                  "(user_id, source_account_id, destination_account_id, amount, scheduled_for, status) " +
                  "VALUES ( ?, ?, ?, ?, ?, ?)";
  
          stmtInsertScheduled = conn.prepareStatement(sqlInsert);

          stmtInsertScheduled.setInt(1, userId);
          stmtInsertScheduled.setInt(2, source);
          stmtInsertScheduled.setInt(3, destination);
          stmtInsertScheduled.setInt(4, amount);
          stmtInsertScheduled.setDate(5, new java.sql.Date(scheduledFor.getTime()));
          stmtInsertScheduled.setString(6, "scheduled"); // Initial status is scheduled
  
          int rows = stmtInsertScheduled.executeUpdate();
          return rows > 0; // success if 1 row inserted
  
      } catch (SQLException se) {
          se.printStackTrace();
          return false;
      } finally {
          try { if (stmtInsertScheduled != null) stmtInsertScheduled.close(); } catch (SQLException e) {}
      }
   }
   public static void processScheduledTransfers(Connection conn) {
      PreparedStatement stmtFetch = null;
      PreparedStatement stmtUpdateSchedule = null;
      ResultSet rs = null;
  
      try {
          String sqlFetch = "SELECT * FROM Scheduled_Transfers WHERE scheduled_for <= CURDATE() AND status = 'scheduled'";
          stmtFetch = conn.prepareStatement(sqlFetch);
          rs = stmtFetch.executeQuery();
  
          while (rs.next()) {
              int scheduleId = rs.getInt("schedule_id");
              int userId = rs.getInt("user_id");
              int sourceAccount = rs.getInt("source_account_id");
              int destinationAccount = rs.getInt("destination_account_id");
              int amount = rs.getInt("amount");
  
              System.out.println("Processing scheduled transfer: " + scheduleId);
  
              boolean transferSuccess = performTransfer(conn, userId, sourceAccount, destinationAccount, amount);
  
              if (transferSuccess) {
                  // Update status to completed
                  String sqlUpdateSchedule = "UPDATE Scheduled_Transfers SET status = 'completed' WHERE schedule_id = ?";
                  stmtUpdateSchedule = conn.prepareStatement(sqlUpdateSchedule);
                  stmtUpdateSchedule.setInt(1, scheduleId);
                  stmtUpdateSchedule.executeUpdate();
  
                  conn.commit();
                  System.out.println("Scheduled transfer completed: " + scheduleId);
              } else {
                  conn.rollback();
                  System.out.println("Transfer failed for schedule: " + scheduleId);
              }
          }
      } catch (SQLException se) {
          se.printStackTrace();
          try { conn.rollback(); } catch (SQLException e) { e.printStackTrace(); }
      } finally {
          try { if (rs != null) rs.close(); } catch (SQLException se) {}
          try { if (stmtFetch != null) stmtFetch.close(); } catch (SQLException se) {}
          try { if (stmtUpdateSchedule != null) stmtUpdateSchedule.close(); } catch (SQLException se) {}
      }
   }
   public static boolean cancelScheduledTransfer(Connection conn, int scheduleId, int userId) {
      PreparedStatement stmtFetch = null;
      PreparedStatement stmtUpdate = null;
      ResultSet rs = null;

      try {
         // 1. Check if the schedule exists and belongs to the user and is still scheduled
         String sqlFetch = "SELECT * FROM Scheduled_Transfers WHERE schedule_id = ? AND user_id = ? AND status = 'scheduled'";
         stmtFetch = conn.prepareStatement(sqlFetch);
         stmtFetch.setInt(1, scheduleId);
         stmtFetch.setInt(2, userId);
         rs = stmtFetch.executeQuery();

         if (!rs.next()) {
               System.out.println("No such scheduled transfer found or already processed/cancelled.");
               return false;
         }

         // 2. Update status to cancelled
         String sqlUpdate = "UPDATE Scheduled_Transfers SET status = 'cancelled' WHERE schedule_id = ?";
         stmtUpdate = conn.prepareStatement(sqlUpdate);
         stmtUpdate.setInt(1, scheduleId);
         int updatedRows = stmtUpdate.executeUpdate();

         if (updatedRows > 0) {
               conn.commit();
               System.out.println("Scheduled transfer cancelled successfully!");
               return true;
         } else {
               conn.rollback();
               System.out.println("Failed to cancel scheduled transfer.");
               return false;
         }

      } catch (SQLException se) {
         se.printStackTrace();
         try { conn.rollback(); } catch (SQLException e) { e.printStackTrace(); }
         return false;
      } finally {
         try { if (rs != null) rs.close(); } catch (SQLException se) {}
         try { if (stmtFetch != null) stmtFetch.close(); } catch (SQLException se) {}
         try { if (stmtUpdate != null) stmtUpdate.close(); } catch (SQLException se) {}
      }
   }
   public static void adminView(Connection conn) {
      Scanner sc = new Scanner(System.in);
      int choice = -1;
  
      while (choice != 5) {
          System.out.println("\n=== Admin Panel ===");
          System.out.println("1. View All Users");
          System.out.println("2. View All Accounts");
          System.out.println("3. View All Transfers");
          System.out.println("4. View All Scheduled Transfers");
          System.out.println("5. Exit Admin Panel");
          System.out.print("Enter your choice: ");
          choice = sc.nextInt();
  
          switch (choice) {
              case 1:
                  viewAllUsers(conn);
                  break;
              case 2:
                  viewAllAccounts(conn);
                  break;
              case 3:
                  viewAllTransfers(conn);
                  break;
              case 4:
                  viewAllScheduledTransfers(conn);
                  break;
              case 5:
                  System.out.println("Exiting Admin Panel...");
                  break;
              default:
                  System.out.println("Invalid choice.");
          }
      }
  }
   public static void viewAllUsers(Connection conn) {
      try (Statement stmt = conn.createStatement()) {
         ResultSet rs = stmt.executeQuery("SELECT * FROM Users");

         System.out.printf("%-10s %-20s %-30s %-10s%n", "UserID", "Name", "Email", "Type");
         System.out.println("--------------------------------------------------------------------------");

         while (rs.next()) {
            System.out.printf("%-10d %-20s %-30s %-10s%n",
                     rs.getInt("user_id"),
                     rs.getString("name"),
                     rs.getString("email"),
                     rs.getString("user_type"));
         }
      } catch (SQLException e) {
         e.printStackTrace();
      }
   }

   public static void viewAllAccounts(Connection conn) {
      try (Statement stmt = conn.createStatement()) {
          ResultSet rs = stmt.executeQuery("SELECT * FROM Accounts");
  
          System.out.printf("%-10s %-10s %-15s %-10s%n", "AccNo", "UserID", "Type", "Balance");
          System.out.println("---------------------------------------------------");
  
          while (rs.next()) {
              System.out.printf("%-10d %-10d %-15s %-10d%n",
                      rs.getInt("account_number"),
                      rs.getInt("user_id"),
                      rs.getString("account_type"),
                      rs.getInt("balance"));
          }
      } catch (SQLException e) {
          e.printStackTrace();
      }
  }
  

   public static void viewAllTransfers(Connection conn) {
      try (Statement stmt = conn.createStatement()) {
         ResultSet rs = stmt.executeQuery("SELECT * FROM Transfers");

         System.out.printf("%-10s %-10s %-15s %-25s %-10s %-30s%n", 
                           "TransID", "UserID", "Source Account", "Destination Account", "Amount", "Completed At");
         System.out.println("-------------------------------------------------------------------------------------");

         while (rs.next()) {
            System.out.printf("%-10d %-10d %-15d %-25d %-10d %-30s%n",
                     rs.getInt("transfer_id"),
                     rs.getInt("user_id"),
                     rs.getInt("source_account_id"),
                     rs.getInt("destination_account_id"),
                     rs.getInt("amount"),
                     rs.getTimestamp("completed_at").toString());
         }
      } catch (SQLException e) {
         e.printStackTrace();
      }
   }
   public static void viewAllScheduledTransfers(Connection conn) {
      try (Statement stmt = conn.createStatement()) {
          ResultSet rs = stmt.executeQuery("SELECT * FROM Scheduled_Transfers");
  
          System.out.printf("%-10s %-10s %-15s %-25s %-10s %-15s %-15s%n",
                            "SchedID", "UserID", "Source Account", "Destination Account", "Amount", "Scheduled For", "Status");
          System.out.println("----------------------------------------------------------------------------------------------------");
  
          while (rs.next()) {
              System.out.printf("%-10d %-10d %-15d %-25d %-10d %-15s %-15s%n",
                      rs.getInt("schedule_id"),
                      rs.getInt("user_id"),
                      rs.getInt("source_account_id"),
                      rs.getInt("destination_account_id"),
                      rs.getInt("amount"),
                      rs.getDate("scheduled_for").toString(),
                      rs.getString("status"));
          }
      } catch (SQLException e) {
          e.printStackTrace();
      }
  }
  
  

  
  
}