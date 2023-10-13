package BFinterpreter;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.InputStreamReader;
import java.util.logging.FileHandler;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.logging.SimpleFormatter;

public class Main {

    public static final Logger logger = Logger.getLogger(Main.class.getName()); //--------------------------------------

    public static void main(String[] args) {

        FileHandler fileHandler = null;  //-----------------------------------------------------------------------------
        try {
            fileHandler = new FileHandler("ExecutionLog.log"); //-----------------------------------------------
        } catch (Exception e) {
            System.err.println(e.getMessage() + " (Unable to create logger for this programme)\n"); //------------------
            return;
        }
        fileHandler.setFormatter(new SimpleFormatter()); //-------------------------------------------------------------
        logger.addHandler(fileHandler); //------------------------------------------------------------------------------
        logger.setUseParentHandlers(false); //--------------------------------------------------------------------------

        String filename = args[0];

        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(System.in));
        String programme = null;
        logger.log(Level.INFO, "Trying to read BF programme from standard input stream..."); //-------------------
        try {
            programme = bufferedReader.readLine();
        } catch (Exception e) {
            System.err.println("Unable to read BF programme from standard input stream: " + e.getMessage() + "\n");
            logger.log(Level.SEVERE, "Unable to read BF programme from standard input stream"); //----------------
            fileHandler.close(); //-------------------------------------------------------------------------------------
            return;
        }
        logger.log(Level.INFO, "Successfully read BF programme"); //----------------------------------------------

        OkFactory factory = new OkFactory();
        logger.log(Level.INFO, "Trying to configure factory from configuration file..."); //----------------------
        try (FileReader file = new FileReader(filename);
             BufferedReader reader = new BufferedReader(file)) {
            factory.fillMap(reader);
        } catch (Exception e) {
            System.err.println("Unable to read configuration file: " + e.getMessage() + "\n");
            logger.log(Level.SEVERE, "Unable to configure factory due to: " + e.getMessage()); //-----------------
            fileHandler.close(); //-------------------------------------------------------------------------------------
            return;
        }
        logger.log(Level.INFO, "Successfully configured factory"); //---------------------------------------------

        ExecutionContext ec = new ExecutionContext(programme, factory);

        logger.log(Level.INFO, "Initiating main loop...");
        //read exec inc
        while (ec.getCommandIdx() < ec.getCommandIdxMax()) {
            char currentCommand = ec.getCommandAtIdx();
            OpInterface command = null;
            int commandIndex = ec.getCommandIdx();
            try {
                command = factory.getFromFactory(currentCommand);
            } catch (Exception e) {
                System.err.println("When trying to interpret command '" + currentCommand + "': " + e.getMessage() +
                                   " (happened on BF command number " + commandIndex + ")\n");
                logger.log(Level.SEVERE, "Unable to interpret command '" + currentCommand + "' due to: " +
                        e.getMessage() + " (happened on BF command number " + commandIndex + ")"); //-------------------
                fileHandler.close(); //---------------------------------------------------------------------------------
                return;
            }
            try {
                command.executeCommand(ec);
            }
            catch (Exception e) {
                System.err.println("When trying to execute command '" + currentCommand + "': " + e.getMessage() +
                                   " (happened on BF command number " + commandIndex + ")\n");
                logger.log(Level.SEVERE, "Unable to execute command '" + currentCommand + "' due to: " +
                        e.getMessage() + " (happened on BF command number " + commandIndex + ")"); //-------------------
                fileHandler.close(); //---------------------------------------------------------------------------------
                return;
            }
            ec.incCommandIdx();
        }
        logger.log(Level.INFO, "Execution complete"); //--------------------------------------------------------
    }

}
