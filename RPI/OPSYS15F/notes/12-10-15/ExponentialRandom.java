/*
 *  Instead of using a uniform distribution, we'll use
 *  an exponential distribution, which should better model
 *  the real-world system.
 *
 *  Independent events; autonomous processes
 *
 *  Exponential distribution: average arrival rate
 */
public class ExponentialRandom
{
  public static void main( String[] args )
  {
    double min = 0;     //       -ln(r)
    double max = 0;     //  x = --------
    double sum = 0;     //       lambda
    int n = 10000;     // <-- make this number very large
    for ( int i = 0 ; i < n ; i++ )
    {
      double lambda = 0.001;  // average will be 1000
      double r = Math.random();            // uniform dist [0,1)
      double x = -Math.log( r ) / lambda;  // log() is natural log
      if ( x > 8000 ) { i--; continue; }
      System.out.println( "x is " + x );
      sum += x;
      if ( i == 0 || x < min ) { min = x; }
      if ( i == 0 || x > max ) { max = x; }
    }
    double avg = sum / n;
    System.out.println( "MIN: " + min );
    System.out.println( "MAX: " + max );
    System.out.println( "AVG: " + avg );
  }
}

