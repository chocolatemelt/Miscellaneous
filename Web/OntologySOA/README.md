Ontology Service
================

This is a small web application written using the MEAN stack, following the guidelines detailed in ```ProblemDefinition.md```. I will also be using SailsJS for the web framework.

## Installation
Getting started with the project:

    npm install -g sails jshint nodemon

Installing the project and running the server:

    npm install
    nodemon app.js

Alternatively:
    
    sails lift

## Troubleshooting
Sometimes node doesn't actually get the KILL signal. Then you get errors about the port being in use. We fix that by killing the node process.
For Windows (in the administrator CLI):

    C:\Windows\System32>taskkill /F /IM node.exe

For Linux:

    killall node
