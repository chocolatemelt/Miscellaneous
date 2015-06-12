package com.sandbox.samplecli;

import java.io.File;

import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.DefaultParser;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

/**
 *
 * @author zhang
 */
public class ConfigMain {
    public static void main(String[] args) throws ParseException {
        Options options = new Options();
        
        // create some options
        options.addOption("l", false, "List files in current directory.");
        options.addOption("r", true, "Remove given file.");
        options.addOption("h", false, "See usage.");
        
        // parse the arguments
        CommandLineParser parser = new DefaultParser();
        try {
            CommandLine cmd = parser.parse(options, args);
            
            /* get the working directory */
            File dir = new File(".");
            
            if(cmd.hasOption("l")) {
                System.out.println("Performing UNIX style ls on working directory:");
                
                // list the current directory, folders first */
                File[] filesList = dir.listFiles();
                for (File file : filesList) {
                    if (!file.isFile()) {
                        System.out.println((char)27 + "[1;36m" + file.getName() + (char)27 + "[0m");
                    }
                }
                for (File file : filesList) {
                    if (file.isFile()) {
                        System.out.println(file.getName());
                    }
                }
            }
            else if(cmd.hasOption("r")) {
                String rmf = cmd.getOptionValue("r");
                System.out.println("Performing UNIX style rm on " + rmf + ":");
                if(rmf == null) System.out.println("rm requires an input.");
                else {
                    File rm;
                    Boolean found = false;
                    
                    // find the file
                    File[] filesList = dir.listFiles();
                    for(File file : filesList) {
                        if(file.getName().equals(rmf)) {
                            rm = file;
                            found = true;
                            
                            // delete the file
                            System.out.println("Deleted " + rmf + ".");
                            rm.delete();
                            
                            // leave
                            break;
                        }
                    }
                    
                    if(!found) System.out.println("File was not found.");
                }
            }
            else {
                help();
            }
            
        } catch(ParseException e) {
            help();
        }
    }
    
    public static void help() {
        System.out.println("Usage: java -cp <filename> com.sandbox.samplecli.ConfigMain " + "[-hlr]");
    }
}
