import java.io.*;
import java.net.*;
import java.util.*;

public class StudentClient
{
  public static void main( String[] args )
  {
    new StudentClient();
  }

  public StudentClient()
  {
    try
    {
      // Establish connection with the server
      Socket socket = new Socket( "linux02.cs.rpi.edu", 8125 );

      // Create an output stream to the server
      ObjectOutputStream toServer =
          new ObjectOutputStream( socket.getOutputStream() );

      // Create a Student object and send to the server
      Student s = new Student( "John Smith", "123-45-6789", 3.5 );

      toServer.writeObject( s );
      System.out.println( "Sent student " + s.getId() + " to server" );

      socket.close();
    }
    catch ( IOException ex )
    {
      System.err.println( ex );
    }
  }
}
