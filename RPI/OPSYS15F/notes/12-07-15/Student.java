public class Student implements java.io.Serializable
{
  private String name;
  private String id;
  private double gpa; 
  private boolean goingToGraduate;

  public Student( String name, String id, double gpa )
  {
    this.name = name;
    this.id = id;
    this.gpa = gpa;
    this.goingToGraduate = true;
  }

  public String getName() {
    return name;
  }

  public String getId() {
    return id;
  }

  public double getGpa() {
    return gpa;
  }
}
