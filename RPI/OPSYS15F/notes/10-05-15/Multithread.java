/**
 * Multithread.java example to create a few
 * Count and Echo objects and run them as
 * multiple threads
 */
public class Multithread
{
  public static void main( String[] args )
  {
    // Create the "task" objects
    Echo e1 = new Echo( 'R', 1000 );
    Echo e2 = new Echo( 'P', 1000 );
    Echo e3 = new Echo( 'I', 1000 );
    Count c1 = new Count( 1000 );

    // this is NOT multi-threaded execution...
//    e1.run();
//    e2.run();
//    e3.run();
//    c1.run();

    // Create the Thread objects
    Thread t1 = new Thread( e1 );
    Thread t2 = new Thread( e2 );
    Thread t3 = new Thread( e3 );
    Thread t4 = new Thread( c1 );

    System.out.println( "Priority of t4: " + t4.getPriority() );
    t4.setPriority( Thread.MAX_PRIORITY );
    System.out.println( "Priority of t4: " + t4.getPriority() );

    // Start the threads
    t4.start();
    t1.start();  // start() method here creates the
    t2.start();  //   thread, then calls the run() method
    t3.start();

    System.out.println( "Joining in child threads...." );

    try
    {
      while ( Thread.activeCount() > 1 )
      {
        if ( t1.isAlive() )
        {
          t1.join();         // BLOCKING CALL
        }
        System.out.println( "t1 thread terminated" );
        if ( t2.isAlive() )
        {
          t2.join();         // BLOCKING CALL
        }
        System.out.println( "t2 thread terminated" );
        if ( t3.isAlive() )
        {
          t3.join();         // BLOCKING CALL
        }
        System.out.println( "t3 thread terminated" );
        if ( t4.isAlive() )
        {
          t4.join();         // BLOCKING CALL
        }
        System.out.println( "t4 thread terminated" );
      }
    }
    catch ( InterruptedException ex )
    {
      System.err.println( "Rcvd InterruptedException" );
    }
  }
}





