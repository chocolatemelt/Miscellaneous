Ontology Validation Service
===========================

###Goal:
Design a and build a web service that could fit into a [Services Oriented Architecture](http://en.wikipedia.org/wiki/Service-oriented_architecture) which can manage an ontology as defined in the format specified below, and validate given objects against that ontology.  For the sake of this program, "ontology" simply means the set of entities that are defined to exist in a given domain, the attributes that they have, and their hierarchical relationship.

###Ontology format definition:
The base ontology to be used in this program is in the accompanying file `ontology.json`.  It contains a single JSON object which describes the objects, attributes, and relationship of various object types that we want to represent.  Beyond the requirements of [the JSON format](http://json.org/) itself, there are some additional restrictions of the format which describe the data:

All properties of a given JSON object must either have values of type string or object.  A property with a value of type string represents an attribute that that represented object has.  The value associated with that property defines the type of the attribute.  The types that are valid within this ontology are:

- `boolean` - true or false
- `int` - 32 bit signed integer
- `float` - 32 bit floating point number
- `string` - character array

For example, the object:

    ...
      "person" : {
        "name": "string"
      }
    ...

Means that the person object within the ontology contains a property called `name`, which should be of type `string`.  There are no additional constraints on the data, based on the ontological definition, so a name of `"42"` would still be valid.  Property names are required to be alphanumeric (matching regex `^[a-zA-Z0-9]+$`).

In addition, a property consisting of an asterisk followed by an existing property name denotes the allowed multiplicity of that property.  Valid multiplicities are:

- `%d` - Exactly %d (digit) values ("1" is a single-value, required field, for instance)
- `?` - Zero or one values (optional single-value field)
- `+` - One or more values (required multi-value field)
- `*` - Zero or more values (optional multi-value field)

For example, expanding our person object above, we may have:

    ...
      "person" : {
        "firstName": "string",
        "*firstName": "1",
        "nickName": "string",
        "*nickName": "*"
      }
    ...

Meaning that there is only one allowed first name, and it's a required field, but there are multiple allowed nickNames, and it's an optional field.  If no multiplicity is specified, `*`, meaning any number of that property is valid, is assumed.

A JSON property with an object value denotes a child class of the containing object.  A child class has all the properties defined in all parent classes, as well as any additional properties defined within the child object.  There is no attribute type overriding, meaning that a child class cannot contain a property already defined in a parent class.

Continuing with the example:

    ...
      "person" : {
        "firstName": "string"
        "*firstName": "1",
        "employee" : {
          "salary": "float",
          "*salary": "1"
        }
      }
    ...

So, an employee object has both a required name field as defined in it's parent object, person, as well as a required salary field of type int.

Lastly, the property name "type" is disallowed in the ontology, as it is reserved for an object instance to declare its type.  The type is the period-delimited string of class names that an object represents.  For example, a fully definied employee could be represented as:

	{
	  "type":"person.employee",
	  "firstname":"Tony",
	  "salary":36000.00
	}
	
###Validation Service
The primary goal of this project is to create a server application that can manage the ontology described above, and validate given objects against that ontology via network communication.  Calls will be done via HTTP requests, with the following endpoints and behaviors:

**POST /update**

 - Update the current ontology on the server to the one given in the request body.
 - Post body will contain the ontology to be used, in JSON format (the contents of the `ontology.json` file)
 - The expected status codes of the request are as follows:
 	- 200 OK: The ontology was successfully updated
 	- 400 Bad Request: The given ontology is invalid
 	- 500 Internal Server Error: Graceful failure if an error is encountered by the server.
 	
**GET /fetch**

 - Fetch the current ontology from the server
 - The response body should be the current ontology in JSON format.
 - The expected status codes of the request are as follows:
 	- 200 OK: The fetch was successful - the body should contain the ontology
 	- 500 Internal Server Error: Graceful failure if an error is encountered by the server.
 	
**POST /validate**

 - Validate the object given in the request body against the current ontology.
 - Return a simple JSON object with the property "valid", and with a boolean value denoting the validity of the object within the ontology.
 - The expected status codes of the request are as follows:
 	- 200 OK: The validation was completed successfully, and the response body shoud contain the object stating the object validity.  This should also be the response if the object is valid json, but not within the ontology.  If the JSON itself is invalid, the server should return a 400 status code.
 	- 400 Bad Request: The given object is invalid JSON.
 	- 500 Internal Server Error: Graceful failure if an error is encountered by the server.
 	
It should be noted that any of these endpoints, including `/update` can be called concurrently, so be sure to design the service with parallelism in mind.

###Test Cases:
Included with the ontology JSON file is a bundle of other JSON files, representing objects that may or may not be valid within the context of the ontology.
Test objects in files beginning with "v_" should be valid within the provided ontology.

Similarly, test objects in files beginning with "i_" should be invalid within the ontology defined in the ontology.json file.

In addition to tests for correctness, you should also write at least **one benchmark test** for your server (this will obviously be dependent on framework choice).

###Evaluation criteria:
Evaluation of the code will be based on correctness, efficiency, and code cleanliness and maintainability.  Clear instructions for installing/using the produced service should be included within a README file.  Also include unit tests covering the final product and checking for any edge cases you think of, in addition to the provided test cases.

Feel free to use whatever language you are most comfortable with, and any libraries that you are able to find online, so long as the instructions for building, running, and testing the program are clearly documented in the README.

###Bonus features:
Don't be afraid to go above and beyond!  If there are any features you think would make the project better in the end, feel free to implement them as well.  Just add to your README any extra features that you added and your reasoning for adding them.

Potential improvements include:

 - Additional data type support in the ontology, such as
   - Dates
   - Strings with format constraints
   - Linked/associated objects
 - Support for other serialization methods (XML, ProtoBuf, etc)
