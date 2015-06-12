sampleCLI
=========

This is a really, really, _really_ useless program. If you can run this program you might as well forgo it altogether - all it does is emulate ```ls``` and ```rm```. It doesn't even work as well as them or have any further flags for them.

This project was written solely to become more comfortable with the Apache Commons CLI library and the Maven build system (which is significantly easier to learn than CMake, IMO).

All that being said if you're really curious, you can run it yourself pretty easily:

```
$ mvn clean install
$ java -cp target/<executableJAR.jar> com.sandbox.samplecli.ConfigMain [-hlr]
```

