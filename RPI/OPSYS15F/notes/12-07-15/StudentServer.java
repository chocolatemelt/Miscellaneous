import java.io.*;
import java.net.*;
import java.util.*;

public class StudentServer
{
  private ObjectOutputStream outputToFile;
  private ObjectInputStream inputFromClient;

  public static void main( String[] args ) {
    new StudentServer();
  }

  public StudentServer()
  {
    try
    {
      // Create a server socket
      ServerSocket serverSocket = new ServerSocket( 8125 );
      System.out.println( "Server started at " + new Date() );

      // Create an object ouput stream to the s.dat file
      outputToFile =
        new ObjectOutputStream( new FileOutputStream( "s.dat", true ) );

      while ( true )
      {
        // Listen for a new connection request
        Socket socket = serverSocket.accept();

        // Create an input stream from the socket
        inputFromClient =
          new ObjectInputStream( socket.getInputStream() );

        // Read from input
        Object object = inputFromClient.readObject();  // BLOCK

        if ( object instanceof Student )
        {
          // Write to the file
          outputToFile.writeObject( object );
          System.out.println( "Wrote a new student object to the file" );
        }

        socket.close();
      }
    }
    catch( ClassNotFoundException ex ) {
      ex.printStackTrace();
    }
    catch( IOException ex ) {
      ex.printStackTrace();
    }
    finally {
      try {
        inputFromClient.close();
        outputToFile.close();
      }
      catch ( Exception ex ) {
        ex.printStackTrace();
      }
    }
  }
}
