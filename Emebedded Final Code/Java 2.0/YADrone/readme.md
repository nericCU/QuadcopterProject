Developed for Windows Eclipse Luna
Download and install Eclipse Luna
Importance Yadrone porject into Eclipse
Download RXTX
GO to Project -> Properties -> Java Build Path -> Libraries -> Add jar
add RXTXcomm.jar
GO to Yadrone ->src->serial test
Change line 18 "COMM3" to what every comm port you are using
Go to run -> run configurations
select project YaDrone
Select main class de.yadrone.apps.controlcenter.YADroneControlCenter
run -> run as
java application
select YADroneControlCenter
done
